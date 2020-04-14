#pragma once
#include <cassert>
#include <functional>
#include <string>
#include <vector>
#include "../../../helpers/bitwriter.h"
#include "../../../helpers/refcounting.h"
#include "../../../helpers/stream.h"
#include "../../../helpers/strings.h"
#include "enums.h"
#include "variables_and_scopes.h"
#include "compiler/string_scanner.h"
#include "decompiler/decompiler.h"
#include "compiler/types.h"
#include <QString>

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

class DetailedEnum;
class DetailedFlags;
class GameVariantDataMultiplayer;

namespace Megalo {
   class Compiler;
   class OpcodeArgBase;
   class OpcodeArgValue;
   class OpcodeArgTypeinfo;
   namespace Script {
      class VariableReference;
   }

   class OpcodeArgTypeRegistry {
      public:
         using type_list_t = std::vector<const OpcodeArgTypeinfo*>;
         //
      public:
         static OpcodeArgTypeRegistry& get() {
            static OpcodeArgTypeRegistry instance;
            return instance;
         }
         //
         std::vector<const OpcodeArgTypeinfo*> types;
         //
         void register_type(const OpcodeArgTypeinfo& type);
         const OpcodeArgTypeinfo* get_by_internal_name(const QString& name) const;
         const OpcodeArgTypeinfo* get_static_indexable_type(const QString& name) const;
         const OpcodeArgTypeinfo* get_variable_type(const QString& name) const;
         void lookup_imported_name(const QString& name, type_list_t& out) const;
   };

   class OpcodeArgTypeinfo {
      public:
         struct flags {
            flags() = delete;
            enum type : uint32_t {
               none = 0,
               //
               is_variable        = 0x00000001, // number, object, player, team, timer
               can_hold_variables = 0x00000002, // object, player, team
            };
         };
         using flags_type = std::underlying_type_t<flags::type>;
         using factory_t  = OpcodeArgValue*(*)();
         //
         template<typename T> static OpcodeArgValue* default_factory() { return new T; }
         //
         static constexpr std::initializer_list<Script::Property> no_properties = {};
         //
      public:
         std::string internal_name;
         QString     friendly_name;
         QString     description;
         flags_type  flags = 0;
         struct {
            const DetailedEnum*  enum_values = nullptr;
            const DetailedFlags* flag_values = nullptr;
         } imported_names;
         std::vector<const char*> elements; // unscoped words that the compiler should be aware of, e.g. flag/enum value names
         factory_t                factory = nullptr;
         std::vector<Script::Property> properties; // for the compiler; do not list abstract properties here
         uint8_t static_count      = 0; // e.g. 8 for player[7]
         uint32_t which_sig_static = 0; // e.g. for the (player) type, this would be the signature corresponding to "player[0]" in megalo_players (TODO: move this to the generic Variable class)
         uint32_t which_sig_global = 0; // e.g. for the (player) type, this would be the signature corresponding to "global.player[0]" in megalo_players (TODO: move this to the generic Variable class)
         //
         OpcodeArgTypeinfo() {
            OpcodeArgTypeRegistry::get().register_type(*this);
         }
         OpcodeArgTypeinfo(const char* in, QString fn, QString desc, flags_type f, factory_t fac) : internal_name(in), friendly_name(fn), description(desc), flags(f), factory(fac) {
            OpcodeArgTypeRegistry::get().register_type(*this);
         }
         OpcodeArgTypeinfo(const char* in, QString fn, QString desc, flags_type f, factory_t fac, const DetailedEnum&  names_to_import) : internal_name(in), friendly_name(fn), description(desc), flags(f), factory(fac) {
            this->imported_names.enum_values = &names_to_import;
            OpcodeArgTypeRegistry::get().register_type(*this);
         }
         OpcodeArgTypeinfo(const char* in, QString fn, QString desc, flags_type f, factory_t fac, const DetailedFlags& names_to_import) : internal_name(in), friendly_name(fn), description(desc), flags(f), factory(fac) {
            this->imported_names.flag_values = &names_to_import;
            OpcodeArgTypeRegistry::get().register_type(*this);
         }
         OpcodeArgTypeinfo(
            const char* in,
            QString fn,
            QString desc,
            flags_type f,
            factory_t fac,
            std::initializer_list<Script::Property> pr,
            uint32_t wsg = 0,
            uint32_t wss = 0,
            uint8_t sc = 0
         ) :
            internal_name(in),
            friendly_name(fn),
            description(desc),
            flags(f),
            factory(fac),
            properties(pr),
            static_count(sc),
            which_sig_static(wss),
            which_sig_global(wsg)
         {
            OpcodeArgTypeRegistry::get().register_type(*this);
         }
         //
         inline bool can_be_static() const noexcept {
            return this->static_count > 0;
         }
         inline bool is_variable() const noexcept {
            return (this->flags) & flags::is_variable;
         }
         inline bool can_have_variables() const noexcept {
            return this->flags & flags::can_hold_variables;
         }
         const Script::Property* get_property_by_name(QString) const;
   };

   enum class arg_compile_result {
      failure,
      success,
      needs_another,
      can_take_another,
   };
   
   class OpcodeArgValue {
      public:
         virtual bool read(cobb::ibitreader&, GameVariantDataMultiplayer& mp) noexcept = 0;
         virtual void write(cobb::bitwriter& stream) const noexcept = 0;
         virtual void to_string(std::string& out) const noexcept = 0;
         virtual void configure_with_base(const OpcodeArgBase&) noexcept {}; // used for bool options so they can stringify intelligently
         virtual void decompile(Decompiler& out, uint64_t flags = 0) noexcept = 0;
         virtual arg_compile_result compile(Compiler&, Script::string_scanner&, uint8_t part) noexcept { return arg_compile_result::failure; };
         virtual arg_compile_result compile(Compiler&, Script::VariableReference&, uint8_t part) noexcept { return arg_compile_result::failure; };
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