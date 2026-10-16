#pragma once
#include "./crash_info.h"
#include "helpers/variants/index_of_type.h"

namespace rvt::subsystems::crash_dumper {
   template<typename Error, typename... Args>
      requires (
         // either no data to send...
         (!impl::has_data_to_send<Error> && sizeof...(Args) == 0) ||
         // ...or sending the right data
         impl::can_send_args<Error, Args...>
      )
   static void crash_info::send(HANDLE stream, HANDLE process_to_send_to, Args&&... args) {
      _send_header(stream, process_to_send_to);
      {
         constexpr uint8_t crash_type = cobb::variants::index_of_type<decltype(data), Error>;
         ipc::send_value(stream, crash_type);
      }
      if constexpr (impl::has_data_to_send<Error>) {
         Error::send(stream, std::forward<Args>(args)...);
      }
   }
}