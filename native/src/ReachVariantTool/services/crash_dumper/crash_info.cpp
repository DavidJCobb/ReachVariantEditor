#include "./crash_info.h"
#include <array>
#include "helpers/variants/index_of_type.h"

namespace rvt::subsystems::crash_dumper {
   namespace {
      using crash_type_variant = decltype(crash_info::data);

      template<typename Error>
      bool read_typed_data(crash_info& dst, HANDLE stream) {
         Error& info = dst.data.emplace<Error>();
         if constexpr (impl::has_data_to_send<Error>) {
            return info.read(stream);
         } else {
            return true;
         }
      }

      using typed_data_reader = bool(*)(crash_info&, HANDLE);

      template<typename Variant>
      struct reader_table_template;

      template<typename... VariantElements>
      struct reader_table_template<std::variant<VariantElements...>> {
         static constexpr const std::array<typed_data_reader, sizeof...(VariantElements)> value = std::array{
            &read_typed_data<VariantElements>...
         };
      };
      
      constexpr const auto& reader_table = reader_table_template<crash_type_variant>::value;
   }

   /*static*/ void crash_info::_send_header(HANDLE stream, HANDLE send_to_process) {
      HANDLE process;
      DuplicateHandle(
         GetCurrentProcess(), // sender process
         GetCurrentProcess(), // handle to duplicate
         send_to_process,     // recipient process
         &process,            // out: duplicated handle
         0,
         TRUE,
         DUPLICATE_SAME_ACCESS
      );
      DWORD thread_id = GetCurrentThreadId();
      ipc::send_value(stream, process);
      ipc::send_value(stream, thread_id);
   }

   bool crash_info::read(HANDLE stream) {
      if (!ipc::read_value(stream, this->source.process)) {
         //
         // This read may fail under normal circumstances. For an ordinary program exit, this read 
         // will fail because the main process will exit (closing the pipe) without ever having 
         // sent anything to us.
         //
         return false;
      }
      if (!ipc::read_value(stream, this->source.thread_id))
         return false;

      uint8_t crash_type = cobb::variants::index_of_type<decltype(data), std::monostate>;
      if (!ipc::read_value(stream, crash_type))
         return false;
      if (crash_type >= reader_table.size())
         return false;

      return reader_table[crash_type](*this, stream);
   }
}