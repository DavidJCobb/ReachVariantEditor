#include "./ipc.h"
#include <bit>
#include <cstdint>
#include <limits>
#include <sstream> // for narrow -> wide conversion
#include <windows.h>

namespace {
   using string_length_prefix_type = uint16_t;

   constexpr const string_length_prefix_type string_wide_flag_mask = 1 << (std::bit_width((std::numeric_limits<string_length_prefix_type>::max)()) - 1);
   constexpr const string_length_prefix_type max_string_length     = string_wide_flag_mask - 1;
}

namespace rvt::subsystems::crash_dumper::ipc {
   extern void send_bytes(HANDLE stream, const void* src, size_t size) {
      WriteFile(stream, src, size, nullptr, nullptr);
   }
   extern bool read_bytes(HANDLE stream, void* dst, size_t size) {
      return ReadFile(stream, dst, size, nullptr, nullptr);
   }

   extern void send_length_prefixed_string(HANDLE stream, const std::string_view src) {
      size_t size = src.size();
      if (size > max_string_length)
         size = max_string_length;

      string_length_prefix_type prefix = size;
      send_value(stream, prefix);
      if (size)
         send_bytes(stream, src.data(), size);
   }
   extern void send_length_prefixed_string(HANDLE stream, const std::wstring_view src) {
      size_t size = src.size();
      if (size > max_string_length)
         size = max_string_length;

      string_length_prefix_type prefix = size | string_wide_flag_mask;
      send_value(stream, prefix);
      if (size)
         send_bytes(stream, src.data(), size);
   }
   extern bool read_length_prefixed_string(HANDLE stream, std::wstring& dst) {
      uint16_t prefix = 0;
      if (!read_value(stream, prefix)) {
         dst.clear();
         return false;
      }
      uint16_t size = prefix & ~string_wide_flag_mask;
      if (size == 0)
         return true;
      if (prefix & string_wide_flag_mask) {
         dst.resize(size);
         if (!read_bytes(stream, dst.data(), size * sizeof(wchar_t))) {
            dst.clear();
            return false;
         }
      } else {
         std::string narrow;
         narrow.resize(size);
         if (!read_bytes(stream, narrow.data(), size)) {
            dst.clear();
            return false;
         }

         std::wostringstream converter;
         converter << narrow.c_str();
         dst = converter.str();
      }
      return true;
   }
}