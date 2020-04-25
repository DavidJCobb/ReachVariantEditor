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
            std::vector<const char*> bare_values;
         } imported_names;
         std::vector<const char*> elements; // unscoped words that the compiler should be aware of, e.g. flag/enum value names
         factory_t                factory = nullptr;
         std::vector<Script::Property> properties; // for the compiler; do not list abstract properties here
         uint8_t static_count = 0; // e.g. 8 for player[7]
         const VariableScopeWhichValue* first_global = nullptr;
         const VariableScopeWhichValue* first_static = nullptr;
         std::vector<const OpcodeArgTypeinfo*> accessor_proxy_types; // the listed types can use this type's accessors and member functions; needed for OpcodeArgValuePlayerOrGroup, etc.
         //
         OpcodeArgTypeinfo() {}
         OpcodeArgTypeinfo(const char* in, QString fn, QString desc, flags_type f, factory_t fac) : internal_name(in), friendly_name(fn), description(desc), flags(f), factory(fac) {}
         OpcodeArgTypeinfo(const char* in, QString fn, QString desc, flags_type f, factory_t fac, const DetailedEnum&  names_to_import) : internal_name(in), friendly_name(fn), description(desc), flags(f), factory(fac) {
            this->imported_names.enum_values = &names_to_import;
         }
         OpcodeArgTypeinfo(const char* in, QString fn, QString desc, flags_type f, factory_t fac, const DetailedFlags& names_to_import) : internal_name(in), friendly_name(fn), description(desc), flags(f), factory(fac) {
            this->imported_names.flag_values = &names_to_import;
         }
         OpcodeArgTypeinfo(
            const char* in,
            QString fn,
            QString desc,
            flags_type f,
            factory_t fac,
            std::initializer_list<Script::Property> pr,
            uint8_t sc = 0
         ) :
            internal_name(in),
            friendly_name(fn),
            description(desc),
            flags(f),
            factory(fac),
            properties(pr),
            static_count(sc)
         {}
         //
         #pragma region Decorator methods
            //
            // These methods should only be called immediately after constructing an instance; they should not be 
            // called at any later point. They exist in an attempt to alleviate the problems that result from C++ 
            // not supporting named arguments.
            //
            // These methods return the instance they're called on, allowing you to write variable definitions 
            // such as:
            //
            //    OpcodeArgTypeinfo some_type = OpcodeArgTypeinfo(
            //       config_arguments
            //    ).decorator(
            //       more_config_arguments
            //    );
            //
            OpcodeArgTypeinfo& import_names(const DetailedEnum& e) {
               auto& p = this->imported_names.enum_values;
               if (p != &e) {
                  assert(!p && "Cannot import names from multiple enums.");
                  p = &e;
               }
               return *this;
            }
            OpcodeArgTypeinfo& import_names(const DetailedFlags& e) {
               auto& p = this->imported_names.flag_values;
               if (p != &e) {
                  assert(!p && "Cannot import names from multiple flags lists.");
                  p = &e;
               }
               return *this;
            }
            OpcodeArgTypeinfo& import_names(std::initializer_list<const char*> types) {
               auto& list = this->imported_names.bare_values;
               if (list.empty())
                  list = types;
               else {
                  list.reserve(list.size() + types.size());
                  for (auto p : types)
                     list.push_back(p);
               }
               return *this;
            }
            OpcodeArgTypeinfo& set_accessor_proxy_types(std::initializer_list<const OpcodeArgTypeinfo*> types) {
               this->accessor_proxy_types = types;
               return *this;
            }
            OpcodeArgTypeinfo& set_variable_which_values(const VariableScopeWhichValue* first_global, const VariableScopeWhichValue* first_static = nullptr) {
               this->first_global = first_global;
               this->first_static = first_static;
               return *this;
            }
         #pragma endregion
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
         bool has_accessor_proxy_type(const OpcodeArgTypeinfo& type) const noexcept;
         bool has_imported_name(const QString& name) const noexcept;
   };

   struct arg_compile_result {
      //
      // This class indicates information about the results of an attempt to compile an OpcodeArgValue. 
      // It signals whether the OpcodeArgValue needs to (or simply can) consume another script argument, 
      // along with information on whether the value failed to parse and whether this failure is recover-
      // able. A recoverable failure is one that wouldn't prevent the parsing of any remaining arguments 
      // in the function call.
      //
      // As an example: if an OpcodeArgValueShape receives an invalid shape type, that is an irresolvable 
      // failure, because the shape type is needed in order to know how many more script arguments to 
      // consume. However, if an OpcodeArgValueVector3 receives an invalid coordinate, we still know how 
      // many more coordinates to consume and how to consume them, and so this invalid coordinate is a 
      // resolvable failure.
      //
      enum class code_t : uint8_t {
         failure,
         failure_irresolvable, // it is impossible to attempt to parse the remaining function call arguments. e.g. a bad shape type means we don't know what arguments come next
         success,
         unresolved_string, // implies success
         base_class_is_expecting_override_behavior, // used by Variable to signal to subclasses that they should run their own logic
      };
      enum class more_t : uint8_t {
         no,       // we're good
         needed,   // we require another script argument
         optional, // we can take another script argument
      };
      //
      QString error; // if the (code) indicates an unresolved string, then this is the string content, not an error
      code_t  code = code_t::failure;
      more_t  more = more_t::no;
      //
      static arg_compile_result failure(bool irresolvable = false);
      static arg_compile_result failure(const char*, bool irresolvable = false); // overload needed because const char* implicitly casts to bool, not QString
      static arg_compile_result failure(QString, bool irresolvable = false);
      static arg_compile_result success();
      static arg_compile_result unresolved_string(QString);
      //
      arg_compile_result() {}
      arg_compile_result(code_t c) : code(c) {}
      //
      [[nodiscard]] inline bool is_failure() const noexcept { return this->code == code_t::failure || this->code == code_t::failure_irresolvable || this->code == code_t::base_class_is_expecting_override_behavior; }
      [[nodiscard]] inline bool is_irresolvable_failure() const noexcept { return this->code == code_t::failure_irresolvable; }
      [[nodiscard]] inline bool is_success() const noexcept { return this->code == code_t::success || this->code == code_t::unresolved_string; }
      [[nodiscard]] inline bool is_unresolved_string() const noexcept { return this->code == code_t::unresolved_string; }
      [[nodiscard]] inline bool needs_another() const noexcept { return this->more == more_t::needed; }
      [[nodiscard]] inline bool can_take_another() const noexcept { return this->more == more_t::optional; }
      //
      inline arg_compile_result& set_more(more_t more) noexcept { this->more = more; return *this; }
      inline arg_compile_result& set_needs_more(bool yes) noexcept { this->more = yes ? more_t::needed : more_t::no; return *this; }
      //
      inline QString get_unresolved_string() const noexcept {
         if (this->is_unresolved_string())
            return this->error;
         return "";
      }
   };
   
   class OpcodeArgValue {
      public:
         virtual ~OpcodeArgValue() {}
         virtual bool read(cobb::ibitreader&, GameVariantDataMultiplayer& mp) noexcept = 0;
         virtual void write(cobb::bitwriter& stream) const noexcept = 0;
         virtual void to_string(std::string& out) const noexcept = 0;
         virtual void configure_with_base(const OpcodeArgBase&) noexcept {}; // used for bool options so they can stringify intelligently
         virtual void decompile(Decompiler& out, uint64_t flags = 0) noexcept = 0;
         virtual arg_compile_result compile(Compiler&, Script::string_scanner&,    uint8_t part) noexcept { return arg_compile_result::failure("OpcodeArgValue::compile overload not defined for this type."); }; // used if the OpcodeArgValue was received as a script argument
         virtual arg_compile_result compile(Compiler&, Script::VariableReference&, uint8_t part) noexcept { return arg_compile_result::failure("OpcodeArgValue::compile overload not defined for this type."); }; // used if the OpcodeArgValue was received as the lefthand or righthand side of a statement, or the context of a function call
         virtual OpcodeArgValue* create_of_this_type() const noexcept = 0;
         virtual void copy(const OpcodeArgValue*) noexcept = 0;
         virtual OpcodeArgValue* clone() const noexcept;
         //
         virtual variable_type get_variable_type() const noexcept {
            return variable_type::not_a_variable;
         }
   };
   #define megalo_opcode_arg_value_make_create_override virtual OpcodeArgValue* create_of_this_type() const noexcept override { return (typeinfo.factory)(); }
   //
   class OpcodeArgBase {
      public:
         const char* name = "";
         bool is_out_variable = false;
         const OpcodeArgTypeinfo& typeinfo;
         //
         const char* text_true  = "true";
         const char* text_false = "false";
         //
         OpcodeArgBase(const char* n, OpcodeArgTypeinfo& f, const char* tt, const char* tf) : name(n), typeinfo(f), text_true(tt), text_false(tf) {}
         OpcodeArgBase(const char* n, OpcodeArgTypeinfo& f, bool io = false) : name(n), typeinfo(f), is_out_variable(io) {};
   };

};