#pragma once
#include <variant>
#include "helpers/win32/forward_declare_handles.h"
#include "./crash_types/cpp_exception.h"
#include "./crash_types/cpp_generic_terminate.h"
#include "./crash_types/cpp_generic_throw.h"
#include "./crash_types/structured_exception.h"
#include "./ipc.h"

namespace rvt::subsystems::crash_dumper {
   namespace impl {
      template<typename Error>
      concept has_data_to_send = requires {
         { &Error::send };
      };

      template<typename Error, typename... Args>
      concept can_send_args = requires (HANDLE stream, Args&&... args) {
         { Error::send(stream, std::forward<Args>(args)...) };
      };
   }

   struct crash_info {
      public:
         struct {
            HANDLE process   = NULL;
            DWORD  thread_id = NULL;
         } source;
         std::variant<
            std::monostate,
            crash_types::cpp_exception,
            crash_types::cpp_generic_terminate,
            crash_types::cpp_generic_throw,
            crash_types::structured_exception
         > data;
         
      protected:
         static void _send_header(HANDLE stream, HANDLE send_to_process);

      public:
         // `Error` should be a type in the `crash_types` namespace.
         // If that type has a static `send` function, `stream` and `Args` are forwarded to that.
         template<typename Error, typename... Args>
            requires (
               // either no data to send...
               (!impl::has_data_to_send<Error> && sizeof...(Args) == 0) ||
               // ...or sending the right data
               impl::can_send_args<Error, Args...>
            )
         static void send(
            HANDLE stream,
            HANDLE process_to_send_to,
            Args&&...
         );

         bool read(HANDLE stream);
   };
}

#include "./crash_info.inl"