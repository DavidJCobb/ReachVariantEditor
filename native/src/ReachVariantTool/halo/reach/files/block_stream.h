#pragma once
#include <cstdint>
#include "helpers/memory.h"
#include "helpers/stream.h"
#include "./blocks/base.h"
#include "../bytereader.h"

namespace halo::reach {
   class file_block_stream : public bytereader {
      protected:
         file_block_stream() {}
      public:
         file_block_stream(cobb::generic_buffer&& source) {
            this->decompressed = std::move(source);
            this->set_buffer(this->decompressed.data(), this->decompressed.size());
         };
         file_block_stream(cobb::generic_buffer& source) = delete; // use std::move on the buffer

         static file_block_stream create_invalid() {
            file_block_stream out;
            out.is_invalid = true;
            return out;
         }
         
         file_block_header header;
         bool    was_compressed     = false;
         uint8_t decompressed_unk00 = 0;
         
         inline bool is_valid() const { return !this->is_invalid; }
         inline operator bool() const { return this->is_valid(); }
         
      protected:
         cobb::generic_buffer decompressed;
         bool is_invalid = false;
   };

   extern file_block_stream read_next_file_block(bytereader&);
}
