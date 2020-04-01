#pragma once
#include <cassert>
#include <functional>
#include <string>
#include "../../../helpers/bitwriter.h"
#include "../../../helpers/refcounting.h"
#include "../../../helpers/stream.h"
#include "../../../helpers/strings.h"
#include "enums.h"
#include "variables_and_scopes.h"
#include "decompiler/decompiler.h"
#include "compiler/types.h"

//
// Here, an "opcode" is an instruction that can appear in a Megalo trigger, i.e. a 
// condition or an action. An "opcode base" is a kind of opcode. The appropriate 
// comparison for x86 would be to say that MOV is an opcode base, while a specific 
// MOV instruction in a block of code is an opcode. Opcodes can have arguments.
//
// A loaded opcode argument is stored as an instance of a subclass of OpcodeArgValue. 
// There are subclasses for every possible argument type, and each subclass has a 
// factory function which constructs an instance. Opcode bases specify their argument 
// types via a list of factory functions; at run-time, opcodes will go over the list 
// and construct instances of OpcodeArgValue subclasses to hold loaded arguments.
//
// Factory functions can receive a cobb::bitstream reference. This is used in cases 
// where an argument type can vary. For example, the "Compare" condition can compare 
// any two variables (or game state values or constant integers), and so its opcode 
// base has two pointers to the (OpcodeArgAnyVariableFactory) factory function. That 
// factory function reads bits from the file which indicate what variable type is 
// present, and then constructs an instance of the appropriate subclass. Factory 
// functions should not use the bitstream for any other purpose; it is the opcode's 
// responsibility to direct an OpcodeArgValue received from a factory to load its 
// data.
//
// The class OpcodeArgBase is the means through which opcode bases list their 
// arguments. An OpcodeArgBase contains, as of this writing, a factory function 
// pointer, a name (for UI purposes), and flags indicating things such as whether 
// the argument is an out-variable (in which case read-only values would not be 
// allowed, e.g. a trigger action cannot modify the value of a constant integer).
//

class GameVariantDataMultiplayer;

namespace Megalo {
   class OpcodeArgBase;
   class OpcodeArgValue;

   using OpcodeArgValueFactory = OpcodeArgValue*(*)(cobb::ibitreader& stream);

   class OpcodeArgTypeinfo {
      public:
         enum class typeinfo_type {
            default,
            enumeration,
            flags_mask,
         };
         struct flags {
            flags() = delete;
            enum type : uint32_t {
               is_variable        = 0x00000001, // number, object, player, team, timer
               can_hold_variables = 0x00000002, // object, player, team
               always_read_only   = 0x00000004, // only relevant for (is_variable); only variables and properties can appear in assign statements, and properties have their own "is read only" attribute
               can_be_multiple    = 0x00000008, // this type represents one argument internally but multiple arguments in script code
               can_be_static      = 0x00000010, // this type can be indexed directly, e.g. player[1] and team[3]
            };
         };
         using flags_type = std::underlying_type_t<flags::type>;
         using factory_t = std::function<OpcodeArgValueFactory>;
         //
         template<typename T> static OpcodeArgValue* default_factory(cobb::ibitreader&) { return new T; }
         //
      public:
         typeinfo_type            type    = typeinfo_type::default;
         flags_type               flags   = 0;
         std::vector<const char*> elements; // unscoped words that the compiler should be aware of, e.g. flag/enum value names
         OpcodeArgValueFactory    factory = nullptr;
         std::vector<Script::Property> properties; // for compiler
         uint8_t static_count = 0; // e.g. 8 for player[7]
         //
         OpcodeArgTypeinfo() {}
         OpcodeArgTypeinfo(typeinfo_type t, flags_type f, OpcodeArgValueFactory fac) : type(t), flags(f), factory(fac) {}
         OpcodeArgTypeinfo(typeinfo_type t, flags_type f, std::initializer_list<const char*> e, OpcodeArgValueFactory fac) : type(t), flags(f), elements(e), factory(fac) {}
         OpcodeArgTypeinfo(typeinfo_type t, flags_type f, OpcodeArgValueFactory fac, std::initializer_list<Script::Property> pr, uint8_t sc) : type(t), flags(f), factory(fac), properties(pr), static_count(sc) {}
   };
   
   class OpcodeArgValue {
      public:
         virtual bool read(cobb::ibitreader&, GameVariantDataMultiplayer& mp) noexcept = 0;
         virtual void write(cobb::bitwriter& stream) const noexcept = 0;
         virtual void to_string(std::string& out) const noexcept = 0;
         virtual void configure_with_base(const OpcodeArgBase&) noexcept {}; // used for bool options so they can stringify intelligently
         virtual void decompile(Decompiler& out, uint64_t flags = 0) noexcept = 0;
         //
         virtual variable_type get_variable_type() const noexcept {
            return variable_type::not_a_variable;
         }
   };
   //
   class OpcodeArgBase {
      public:
         const char* name = "";
         bool is_out_variable = false;
         OpcodeArgTypeinfo& typeinfo;
         //
         const char* text_true  = "true";
         const char* text_false = "false";
         //
         OpcodeArgBase(const char* n, OpcodeArgTypeinfo& f, const char* tt, const char* tf) : name(n), typeinfo(f), text_true(tt), text_false(tf) {}
         OpcodeArgBase(const char* n, OpcodeArgTypeinfo& f, bool io = false) : name(n), typeinfo(f), is_out_variable(io) {};
   };

   enum class index_quirk {
      none,
      presence, // index value is preceded by an "is none" bit
      reference,
      offset,
      word,
   };
   class OpcodeArgValueBaseIndex : public OpcodeArgValue {
      public:
         static constexpr int32_t none = -1;
      public:
         OpcodeArgValueBaseIndex(const char* name, uint32_t max, index_quirk quirk = index_quirk::none) : 
            name(name), max(max), quirk(quirk)
         {};

         const char* name;
         uint32_t    max;
         index_quirk quirk;
         int32_t     value = 0; // loaded value
         //
         virtual bool read(cobb::ibitreader& stream, GameVariantDataMultiplayer& mp) noexcept override {
            if (this->quirk == index_quirk::presence) {
               bool absence = stream.read_bits(1) != 0;
               if (absence) {
                  this->value = OpcodeArgValueBaseIndex::none;
                  return true;
               }
            }
            this->value = stream.read_bits(cobb::bitcount(this->max - 1));
            if (this->quirk == index_quirk::offset)
               --this->value;
            return true;
         }
         virtual void write(cobb::bitwriter& stream) const noexcept override {
            if (this->quirk == index_quirk::presence) {
               if (this->value == OpcodeArgValueBaseIndex::none) {
                  stream.write(1, 1);
                  return;
               }
               stream.write(0, 1);
            }
            auto value = this->value;
            if (this->quirk == index_quirk::offset)
               ++value;
            stream.write(value, cobb::bitcount(this->max - 1));
         }
         virtual void to_string(std::string& out) const noexcept override {
            if (this->value == OpcodeArgValueBaseIndex::none) {
               cobb::sprintf(out, "No %s", this->name);
               return;
            }
            cobb::sprintf(out, "%s #%d", this->name, this->value);
         }
         virtual void decompile(Decompiler& out, uint64_t flags = 0) noexcept override {
            if (this->value == OpcodeArgValueBaseIndex::none || this->value < 0) {
               out.write(u8"none");
               return;
            }
            std::string temp;
            cobb::sprintf(temp, "%u", this->value);
            out.write(temp);
         }
   };
};