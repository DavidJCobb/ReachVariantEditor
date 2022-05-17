#include "loc_string_table.h"
extern "C" {
   #include "../../zlib/zlib.h" // interproject ref
}
#include "../game_variants/errors.h"
#include "../game_variants/warnings.h"

namespace halo {
   namespace impl::loc_string_table {
      extern void* load_buffer(size_t size, bool is_compressed, size_t uncompressed_size, cobb::ibitreader& stream) {
         void* buffer = malloc(size);
         for (uint32_t i = 0; i < size; i++)
            *(uint8_t*)((std::uintptr_t)buffer + i) = stream.read_bits<uint8_t>(8);
         if (is_compressed) {
            //
            // The buffer has four bytes indicating the length of the decompressed data, which is 
            // redundant. We'll read those four bytes just as a sanity check, but we need to skip 
            // them -- zlib needs to receive (buffer + 4) as its input.
            //
            uint32_t uncompressed_size_2 = *(uint32_t*)(buffer);
            if (uncompressed_size_2 != uncompressed_size)
               if (_byteswap_ulong(uncompressed_size_2) != uncompressed_size)
                  printf("WARNING: String table sizes don't match: Bungie 0x%08X versus zlib 0x%08X.", uncompressed_size, uncompressed_size_2);
            void* final = malloc(uncompressed_size);
            //
            // It's normally pointless to check that an allocation succeeded because if it didn't, 
            // that means you're out of memory and there's basically nothing you can do about that 
            // anyway. However, if we screwed up and misread the size, then we may try to allocate 
            // a stupidly huge amount of memory. In that case, allocation will (hopefully) fail and 
            // (final) will be nullptr, and that's worth handling.
            //
            if (final) {
               Bytef* input = (Bytef*)((std::uintptr_t)buffer + sizeof(uncompressed_size_2));
               int resultCode = uncompress((Bytef*)final, (uLongf*)&uncompressed_size_2, input, size - sizeof(uncompressed_size_2));
               if (Z_OK != resultCode) {
                  free(final);
                  final = nullptr;
                  auto& error_report = GameEngineVariantLoadError::get();
                  error_report.state         = GameEngineVariantLoadError::load_state::failure;
                  error_report.reason        = GameEngineVariantLoadError::load_failure_reason::zlib_decompress_error;
                  error_report.failure_point = GameEngineVariantLoadError::load_failure_point::string_table;
                  error_report.extra[0]      = resultCode;
               }
            }
            free(buffer);
            buffer = final;
         }
         return buffer;
      }
      extern void warn_on_loading_excess_strings(size_t claimed, size_t maximum) {
         #if _DEBUG
            __debugbreak();
         #endif
         GameEngineVariantLoadWarningLog::get().push_back(QString("String table's maximum count is %1 strings; table claims to have %2 strings.").arg(maximum).arg(claimed));
      }
   }
}