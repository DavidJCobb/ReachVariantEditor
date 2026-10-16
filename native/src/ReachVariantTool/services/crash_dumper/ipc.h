#pragma once
#include <string>
#include "helpers/win32/forward_declare_handles.h"

namespace rvt::subsystems::crash_dumper::ipc {
   extern void send_bytes(HANDLE stream, const void*, size_t);
   extern bool read_bytes(HANDLE stream, void*,       size_t);

   template<typename T>
   [[forceinline]] [[msvc::forceinline]] void send_value(HANDLE stream, const T& v) {
      send_bytes(stream, &v, sizeof(v));
   }

   template<typename T>
   [[forceinline]] [[msvc::forceinline]] bool read_value(HANDLE stream, T& v) {
      return read_bytes(stream, &v, sizeof(v));
   }

   extern void send_length_prefixed_string(HANDLE stream, const std::string_view);
   extern void send_length_prefixed_string(HANDLE stream, const std::wstring_view);
   extern bool read_length_prefixed_string(HANDLE stream, std::wstring&);
}