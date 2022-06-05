#include "cannot_allocate_buffer.h"
extern "C" {
   #include "../zlib/zlib.h" // interproject ref
}

namespace halo::common::load_process_messages {
   QString string_table_cannot_allocate_buffer::to_string() const {
      auto& info = this->info;
      //
      QString out;
      if (info.is_zlib) {
         switch (info.zlib_code) {
            case Z_MEM_ERROR:
               out = QString("Failed to decompress %1 bytes of compressed data into %2 bytes of uncompressed data. zlib failed to allocate memory as needed.");
               break;
            case Z_BUF_ERROR:
               out = QString("Failed to decompress %1 bytes of compressed data into %2 bytes of uncompressed data. According to zlib, the compressed data would decode to a size larger than expected.");
               break;
            case Z_DATA_ERROR:
               out = QString("Failed to decompress %1 bytes of compressed data into %2 bytes of uncompressed data. According to zlib, the compressed data may be corrupt.");
               break;
            default:
               out = QString("Failed to decompress %1 bytes of compressed data into %2 bytes of uncompressed data. zlib encountered an unknown error.");
               break;
         }
         out = out.arg(info.compressed_size).arg(info.uncompressed_size);
      } else {
         if (info.is_compressed) {
            out = QString("Failed to allocate %1 bytes of memory to hold the decompressed string table content. Compressed size is %2; max compressed size is %3.");
         } else {
            out = QString("Failed to allocate %1 bytes of memory to hold the uncompressed string table content. Max size is %3.");
         }
         out = out.arg(info.uncompressed_size).arg(info.compressed_size).arg(info.max_buffer_size);
      }
      return out;
   }
}