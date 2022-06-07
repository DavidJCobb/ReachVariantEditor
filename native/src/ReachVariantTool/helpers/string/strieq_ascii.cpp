#include "strieq_ascii.h"
#include <intrin.h>
#include "../cpuinfo.h"

namespace cobb {
   extern int strieq_ascii(const std::string& a, const std::string& b) {
      size_t size = a.size();
      if (size != b.size())
         return false;
      //
      size_t i = 0;
      if (auto& cpu = cobb::cpuinfo::get(); cpu.extension_support.sse_2 && cpu.extension_support.sse_3) {
         auto mb_a = _mm_set1_epi8('A' - 1);
         auto mb_z = _mm_set1_epi8('Z' + 1);
         //
         auto _to_lower = [&mb_a, &mb_z](__m128i& data) {
            //
            // Goal: for each active byte in (mask_a), OR the byte in (data) by 0x20
            //
            auto mask_a = _mm_cmpgt_epi8(data, mb_a); // per byte: (a >= 'A') ? 0xFF : 0
            auto mask_z = _mm_cmplt_epi8(data, mb_z); // per byte: (a <= 'Z') ? 0xFF : 0
            mask_a = _mm_and_si128(mask_a, mask_z); // bitwise-AND
            mask_a = _mm_and_si128(_mm_set1_epi8(0x20), mask_a); // per byte: (a >= 'A' && a <= 'Z') ? 0x20 : 0
            data = _mm_or_si128(data, mask_a); // bitwise-OR
         };
         //
         for (; i + 15 < size; i += 16) {
            auto ma = _mm_loadu_si128((const __m128i*)(a.data() + i));
            auto mb = _mm_loadu_si128((const __m128i*)(b.data() + i));
            _to_lower(ma);
            _to_lower(mb);
            auto eq = _mm_movemask_epi8(_mm_cmpeq_epi8(ma, mb));
            if (eq != 0xFFFF) {
               return false;
            }
         }
         if (i + 7 < size) {
            auto ma = _mm_loadl_epi64((const __m128i*)(a.data() + i));
            auto mb = _mm_loadl_epi64((const __m128i*)(b.data() + i));
            _to_lower(ma);
            _to_lower(mb);
            auto eq = _mm_movemask_epi8(_mm_cmpeq_epi8(ma, mb));
            if (eq != 0xFFFF) {
               return false;
            }
            //
            i += 8;
         }
      }
      for (; i < size; ++i) {
         char x = a[i];
         char y = b[i];
         if (x >= 'A' && x <= 'Z')
            x |= 0x20;
         if (y >= 'A' && y <= 'Z')
            y |= 0x20;
         if (x != y)
            return false;
      }
      return true;
   }
}