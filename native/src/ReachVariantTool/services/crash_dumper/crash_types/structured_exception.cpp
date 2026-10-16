#include "./structured_exception.h"
#include "../ipc.h"

namespace rvt::subsystems::crash_dumper::crash_types {
   /*static*/ void structured_exception::send(HANDLE stream, const EXCEPTION_POINTERS& ep) {
      uint8_t record_count_and_flags = 0;
      for(auto* p = ep.ExceptionRecord; p; p = p->ExceptionRecord) {
         ++record_count_and_flags;
         if (record_count_and_flags == 0x7F)
            break;
      }
      if (ep.ContextRecord) {
         record_count_and_flags |= 0x80;
      }

      ipc::send_value(stream, record_count_and_flags);
      if (ep.ContextRecord) {
         ipc::send_value(stream, *ep.ContextRecord);
      }
      record_count_and_flags &= ~0x80;
      if (record_count_and_flags) {
         const auto* current_record = ep.ExceptionRecord;
         for (uint8_t i = 0; i < record_count_and_flags; ++i) {
            ipc::send_value(stream, *current_record);
            current_record = current_record->ExceptionRecord;
         }
      }
   }
   bool structured_exception::read(HANDLE stream) {
      uint8_t record_count_and_flags = 0;
      if (!ipc::read_value(stream, record_count_and_flags))
         return false;

      if (record_count_and_flags & 0x80) {
         if (!ipc::read_value(stream, this->context.emplace()))
            return false;
         record_count_and_flags &= ~0x80;
      }

      this->records.reserve(record_count_and_flags);
      for (uint8_t i = 0; i < record_count_and_flags; ++i) {
         auto& item = this->records.emplace_back();
         if (!ipc::read_value(stream, item))
            return false;
         item.ExceptionRecord = nullptr;
         if (i > 0) {
            this->records[i - 1].ExceptionRecord = &item;
         }
      }

      return true;
   }
}