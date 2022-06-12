#include "load_process_base.h"
#include "load_process_fatal_error.h"

namespace halo {
   void load_process_base::emit_notice(notice&& item) {
      auto& list = this->contents.notices;
      list.push_back(std::move(item));
      this->location.copy_to(list.back().location);
   }
   void load_process_base::emit_warning(warning&& item) {
      auto& list = this->contents.warnings;
      list.push_back(std::move(item));
      this->location.copy_to(list.back().location);
   }
   void load_process_base::emit_error(error&& item) {
      auto& list = this->contents.errors;
      list.push_back(std::move(item));
      this->location.copy_to(list.back().location);
   }
   void load_process_base::throw_fatal(fatal&& item) {
      auto& dst = this->contents.fatal_error;
      if (dst.has_value())
         return;
      dst.emplace(std::move(item));
      this->location.copy_to(dst.value().location);
      throw load_process_fatal_error{};
   }

   void load_process_base::emit_notice(const notice& item) {
      auto& list = this->contents.notices;
      list.push_back(item);
      this->location.copy_to(list.back().location);
   }
   void load_process_base::emit_warning(const warning& item) {
      auto& list = this->contents.warnings;
      list.push_back(item);
      this->location.copy_to(list.back().location);
   }
   void load_process_base::emit_error(const error& item) {
      auto& list = this->contents.errors;
      list.push_back(item);
      this->location.copy_to(list.back().location);
   }
   void load_process_base::throw_fatal(const fatal& item) {
      auto& dst = this->contents.fatal_error;
      if (dst.has_value())
         return;
      dst.emplace(item);
      this->location.copy_to(dst.value().location);
      throw load_process_fatal_error{};
   }

   void load_process_base::set_stream_offsets(size_t bytes, uint8_t bits) {
      this->stream_offsets.bytes = bytes;
      this->stream_offsets.bits  = bits;
   }
   void load_process_base::enter_location(const load_process_location& loc) {
      this->location.push_back(loc);
   }
   void load_process_base::exit_location() {
      this->location.pop_back();
   }
}