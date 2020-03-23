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

   using  OpcodeArgValueFactory = OpcodeArgValue*(*)(cobb::ibitreader& stream);
   extern OpcodeArgValue* OpcodeArgAnyVariableFactory(cobb::ibitreader& stream);
   extern OpcodeArgValue* OpcodeArgTeamOrPlayerVariableFactory(cobb::ibitreader& stream);

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
            };
         };
         using flags_type = std::underlying_type_t<flags::type>;
         using factory_t = std::function<OpcodeArgValueFactory>;
         //
         template<typename T> static OpcodeArgValue* default_factory(cobb::ibitreader&) { return new T; }
         //
         // TODO: This virtual class should hold metadata about each OpcodeArgValue subclass, including whether the 
         // subclass represents an enum or flags mask; this class should also provide the factory function. Each 
         // OpcodeArgValue subclass should have a static member that is an instance of this class.
         //
         // We need this because...
         //
         //  - The parser and compiler need to be able to get a list of all values that are valid for a given enum 
         //    or flags-mask type, in order to know what values a script author is actually referencing.
         //
         //     - We need this in order to be able to alias enum and flag values. If we handle things as mentioned 
         //       below (a "compile" member function on OpcodeArgValue), then that's all we need this for.
         //
         //  - The "modify grenade count" opcode will be implemented as a (property_set) mapping with no name, set 
         //    so that the grenade type provides the property name (i.e. current_player.plasma_grenades += 3). In 
         //    order to decompile to that output, we need to be able to access the names of the values in the 
         //    "grenade type" enum.
         //
         //     - I am increasingly coming to believe that we should just compile opcodes by creating all of the 
         //       the arguments and giving them a "compile" member function, though I'm not sure what that member 
         //       function should take (e.g. raw string, etc.).
         //
         //        - I mean, we kinda need that for any multi-part function arguments, which first-pass parsing 
         //          needs to load as a string; but enums, variables, and numbers can be handled by the compiler 
         //          without help from typeinfos.
         //
         // Once this is ready, we'll have opcode bases refer to it instead of having them refer directly to each 
         // factory function, in their argument lists.
         //
         // I anticipate that this will also be where we end up storing the functions to parse undifferentiated 
         // multi-part function arguments (which we will use for flags masks, printfs, vector3s, shapes, and so 
         // on).
         //
      public:
         typeinfo_type            type    = typeinfo_type::default;
         flags_type               flags   = 0;
         std::vector<const char*> elements; // unscoped words that the compiler should be aware of, e.g. flag/enum value names
         OpcodeArgValueFactory    factory = nullptr;
         // TODO: other fields from the JavaScript parser implementation's MScriptTypename
         //
         OpcodeArgTypeinfo() {}
         OpcodeArgTypeinfo(typeinfo_type t, flags_type f, OpcodeArgValueFactory fac) : type(t), flags(f), factory(fac) {}
         OpcodeArgTypeinfo(typeinfo_type t, flags_type f, std::initializer_list<const char*> e, OpcodeArgValueFactory fac) : type(t), flags(f), elements(e), factory(fac) {}
   };
   
   class OpcodeArgValue {
      public:
         virtual bool read(cobb::ibitreader&) noexcept = 0;
         virtual void write(cobb::bitwriter& stream) const noexcept = 0;
         virtual void to_string(std::string& out) const noexcept = 0;
         virtual void configure_with_base(const OpcodeArgBase&) noexcept {}; // used for bool options so they can stringify intelligently
         virtual void decompile(Decompiler& out, uint64_t flags = 0) noexcept { // override me!
            out.write(u8"ARG");
         };
         //
         static OpcodeArgValue* factory(cobb::ibitreader& stream) {
            assert(false && "OpcodeArgValue::factory should never be called; any subclasses that can actually appear in a file should override it.");
            return nullptr;
         }
         virtual variable_type get_variable_type() const noexcept {
            return variable_type::not_a_variable;
         }
         //
         virtual void postprocess(GameVariantDataMultiplayer* newlyLoaded) noexcept {
            //
            // OpcodeArgValue subclasses should override this as necessary in order to 
            // access data that is only reliably available after the full variant has 
            // loaded. For example, the OpcodeArgValue subclass for a Forge label may 
            // use this to exchange its numeric label index for a pointer to the Forge 
            // label data.
            //
         }
   };
   //
   class OpcodeArgBase {
      public:
         const char* name = "";
         OpcodeArgValueFactory factory = nullptr;
         bool is_out_variable = false;
         OpcodeArgTypeinfo* typeinfo = nullptr;
         //
         const char* text_true  = "true";
         const char* text_false = "false";
         //
         OpcodeArgBase(const char* n, OpcodeArgValueFactory f, bool io = false) : name(n), factory(f), is_out_variable(io) {};
         OpcodeArgBase(const char* n, OpcodeArgValueFactory f, const char* tt, const char* tf) : name(n), factory(f), text_true(tt), text_false(tf) {}
         OpcodeArgBase(const char* n, OpcodeArgTypeinfo& f, bool io = false) : name(n), typeinfo(&f), is_out_variable(io) {};
   };
   //
   class OpcodeArgValueBaseEnum : public OpcodeArgValue {
      public:
         OpcodeArgValueBaseEnum(const SmartEnum& base, uint32_t offset = 0) : 
            baseEnum(base),
            baseOffset(offset)
         {}
         //
         const SmartEnum& baseEnum;
         uint32_t baseOffset = 0; // not currently applied
         //
         uint32_t value = 0; // loaded value
         //
         virtual bool read(cobb::ibitreader& stream) noexcept override {
            this->value = stream.read_bits(this->baseEnum.index_bits_with_offset(this->baseOffset)) - this->baseOffset;
            return true;
         }
         virtual void write(cobb::bitwriter& stream) const noexcept override {
            stream.write(this->value + this->baseOffset, this->baseEnum.index_bits_with_offset(this->baseOffset));
         }
         virtual void to_string(std::string& out) const noexcept override {
            this->baseEnum.to_string(out, this->value + this->baseOffset);
         }
   };
   class OpcodeArgValueBaseFlags : public OpcodeArgValue {
      public:
         OpcodeArgValueBaseFlags(const SmartFlags& base) : base(base) {}
         //
         const SmartFlags& base;
         uint32_t value = 0; // loaded value
         //
         virtual bool read(cobb::ibitreader& stream) noexcept override {
            this->value = stream.read_bits(this->base.bits());
            return true;
         }
         virtual void write(cobb::bitwriter& stream) const noexcept override {
            stream.write(this->value, this->base.bits());
         }
         virtual void to_string(std::string& out) const noexcept override {
            this->base.to_string(out, this->value);
         }
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
         virtual bool read(cobb::ibitreader& stream) noexcept override {
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
   };

   class OpcodeArgValueAllPlayers : public OpcodeArgValue { // one of the possibilities for team-or-player-vars.
      public:
         OpcodeArgValueAllPlayers() {};
         //
         static OpcodeArgValue* factory(cobb::ibitreader&) {
            return new OpcodeArgValueAllPlayers();
         }
         virtual bool read(cobb::ibitreader& stream) noexcept override {
            return true;
         }
         virtual void write(cobb::bitwriter& stream) const noexcept override {
            return;
         }
         virtual void to_string(std::string& out) const noexcept override {
            out = "all players";
         }
         virtual variable_type get_variable_type() const noexcept {
            return variable_type::not_a_variable;
         }
   };
};