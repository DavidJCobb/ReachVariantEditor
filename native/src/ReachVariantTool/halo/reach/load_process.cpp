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
      auto& dst = this->contents.fatal_error;
      if (dst.has_value())
         return;
      dst.emplace(item);
      if (std::holds_alternative<std::monostate>(dst.value().where)) { // "where" not set by the caller?
         dst.value().where = this->where;
      }
      //
      // TODO: Throw exception?
      //
   }
}