#pragma once
#include <cstdint>
#include "helpers/stream.h"
#include "halo/util/four_cc.h"
#include "halo/reach/bytestreams.fwd.h"

namespace halo::reach {
   extern bool file_block_signature_is_suspicious(util::four_cc);

   struct file_block_header {
      uint32_t signature = 0;
      uint32_t size      = 0; // size in bytes, including the block header
      uint16_t version   = 0;
      uint16_t flags     = 0;

      void read(bytereader&);
      void write(bytewriter&) const;
   };

   template<util::four_cc signature, size_t expected_size = 0> class file_block {
      public:
         file_block_header header;

         static bool header_is_as_expected(const file_block_header& header) {
            if constexpr (signature != 0)
               if (header.signature != signature)
                  return false;
            if constexpr (expected_size != 0)
               if (header.size != expected_size)
                  return false;
            return true;
         }
         bool is_as_expected() const {
            return header_is_as_expected(this->header);
         }

         mutable struct {
            struct {
               size_t pos = 0; // location of the start of the header in bytes, not bits
            } load;
            struct {
               size_t pos = 0; // location of the start of the header in bytes, not bits
            } save;
         } state;

      protected:
         void _error_if_eof(bytereader&);

         void _write_header(bytewriter&) const;
         void _write_finalize(bytewriter&) const;

      public:
         void read(bytereader&);

         /*//
         void write(cobb::bytewriter&) const;
         void write_postprocess(cobb::bytewriter&) const; // rewrites block size, etc.; must be called immediately after the block is done writing
         //*/
         
         inline uint32_t end() const {
            return this->state.load.pos + expected_size;
         }
         inline uint32_t write_end() const {
            return this->state.save.pos + expected_size;
         }
   };
};

#include "base.inl"