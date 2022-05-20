#include "load_process.h"

namespace halo::reach {
   void load_process::emit_notice(const notice& item) {
      auto& list = this->contents.notices;
      list.push_back(item);
      if (std::holds_alternative<std::monostate>(item.where)) { // "where" not set by the caller?
         list.back().where = this->where;
      }
   }
   void load_process::emit_warning(const warning& item) {
      auto& list = this->contents.warnings;
      list.push_back(item);
      if (std::holds_alternative<std::monostate>(item.where)) { // "where" not set by the caller?
         list.back().where = this->where;
      }
   }
   void load_process::emit_error(const error& item) {
      auto& list = this->contents.errors;
      list.push_back(item);
      if (std::holds_alternative<std::monostate>(item.where)) { // "where" not set by the caller?
         list.back().where = this->where;
      }
   }
   void load_process::throw_fatal(const fatal& item) {
      if (this->contents.fatal_error.has_value())
         return;
      auto& dst = this->contents.fatal_error;
      dst = item;
      if (std::holds_alternative<std::monostate>(dst.value().where)) { // "where" not set by the caller?
         dst.value().where = this->where;
      }
      //
      // TODO: Throw exception?
      //
   }

   void load_process::import_from(const load_process& other) {
      auto _append = []<typename List>(List& a, const List& b) {
         auto i    = a.size();
         auto size = i + b.size();
         a.resize(size);
         for (; i < size; ++i)
            a[i] = b[i];
      };
      //
      _append(this->contents.notices,  other.notices());
      _append(this->contents.warnings, other.warnings());
      _append(this->contents.errors,   other.errors());
      if (other.has_fatal()) {
         this->contents.fatal_error = other.get_fatal();
      }
      this->where = other.where;
   }
}