#pragma once
#include <cstdint>
#include "helpers/stream.h"
#include "halo/util/four_cc.h"

namespace halo::reach {
   struct file_block_header {
      uint32_t signature = 0;
      uint32_t size      = 0; // size in bytes, including the block header
      uint16_t version   = 0;
      uint16_t flags     = 0;
   };

   template<util::four_cc signature, size_t expected_size = 0> class file_block {
      public:
         file_block_header header;

         struct {
            struct {
               uint32_t pos = 0; // location of the start of the header in bytes, not bits
            } load;
            struct {
               uint32_t pos = 0; // location of the start of the header in bytes, not bits
            } save;
         } state;

         bool read(cobb::ibitreader&);
         bool read(cobb::ibytereader&);
         void write(cobb::bytewriter&) const;
         void write_postprocess(cobb::bytewriter&) const; // rewrites block size, etc.; must be called immediately after the block is done writing
         
         inline uint32_t end() const {
            return this->state.load.pos + expected_size;
         }
         inline uint32_t write_end() const {
            return this->state.save.pos + expected_size;
         }
         
         static bool signature_is_suspicious(uint32_t signature) noexcept;
   };
};
