#include "opcode_arg.h"
#include "variables_and_scopes.h"
#include "opcode_arg_types/variables/all_core.h"
#include "../../errors.h"
#include "../../../helpers/qt/string.h"
#include "../../../formats/detailed_enum.h"
#include "../../../formats/detailed_flags.h"

namespace Megalo {
   bool OpcodeArgTypeinfo::has_imported_name(const QString& name) const noexcept {
      for (auto p : this->imported_names.bare_values)
         if (name.compare(p, Qt::CaseInsensitive) == 0)
            return true;
      if (auto e = this->imported_names.enum_values) {
         auto i = e->lookup(name);
         if (i >= 0)
            return true;
      }
      if (auto f = this->imported_names.flag_values) {
         auto i = f->lookup(name);
         if (i >= 0)
            return true;
      }
      return false;
   }

   const Script::Property* OpcodeArgTypeinfo::get_property_by_name(QString name) const {
      name = name.toLower();
      for (auto& prop : this->properties)
         if (cobb::qt::stricmp(name, prop.name) == 0)
            return &prop;
      return nullptr;
   }
   bool OpcodeArgTypeinfo::has_accessor_proxy_type(const OpcodeArgTypeinfo& type) const noexcept {
      for (auto t : this->accessor_proxy_types)
         if (t == &type)
            return true;
      return false;
   }

   /*static*/ arg_compile_result arg_compile_result::failure(bool irresolvable) {
      return arg_compile_result(irresolvable ? code_t::failure_irresolvable : code_t::failure);
   }
   /*static*/ arg_compile_result arg_compile_result::failure(const char* error, bool irresolvable) {
      return arg_compile_result::failure(QString(error), irresolvable);
   }
   /*static*/ arg_compile_result arg_compile_result::failure(QString error, bool irresolvable) {
      auto result = arg_compile_result(irresolvable ? code_t::failure_irresolvable : code_t::failure);
      result.error = error;
      return result;
   }
   /*static*/ arg_compile_result arg_compile_result::success() {
      return arg_compile_result(code_t::success);
   }
   /*static*/ arg_compile_result arg_compile_result::unresolved_string(QString content) {
      auto result = arg_compile_result(code_t::unresolved_string);
      result.error = content;
      return result;
   }
   /*static*/ arg_compile_result arg_compile_result::unresolved_label(QString content) {
      auto result = arg_compile_result(code_t::unresolved_label);
      result.error = content;
      return result;
   }

   OpcodeArgValue* OpcodeArgValue::clone() const noexcept {
      auto arg = this->create_of_this_type();
      arg->copy(this);
      return arg;
   }
}