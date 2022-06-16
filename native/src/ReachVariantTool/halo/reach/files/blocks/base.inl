#pragma once
#include "base.h"
#include "halo/reach/bytestreams.h"
#include "halo/common/load_process_messages/file_blocks/invalid_file_block_header.h"
#include "halo/common/load_process_messages/file_blocks/unexpected_end.h"

#define CLASS_TEMPLATE_PARAMS template<util::four_cc signature, size_t expected_size>
#define CLASS_NAME file_block<signature, expected_size>
//
namespace halo::reach {
   CLASS_TEMPLATE_PARAMS void CLASS_NAME::_error_if_eof(bytereader& stream) {
      if (stream.get_overshoot() == 0)
         return;
      stream.emit_error<halo::common::load_process_messages::file_block_unexpected_end>({
         .block = {
            .signature = this->header.signature,
            .size      = this->header.size 
         },
         .overshoot = stream.get_overshoot(),
      });
   }

   CLASS_TEMPLATE_PARAMS void CLASS_NAME::read(bytereader& stream) {
      this->read_state.pos = stream.get_position();
      stream.read(this->header);
      //
      bool valid = true;
      if constexpr (signature != util::four_cc(0)) {
         if (this->header.signature != signature)
            valid = false;
      }
      if constexpr (expected_size) {
         if (this->header.size != expected_size)
            valid = false;
      }
      if (!valid) {
         stream.emit_error<halo::common::load_process_messages::invalid_file_block_header>({
            .expected = { .signature = signature, .size = expected_size },
            .found    = { .signature = this->header.signature, .size = this->header.size },
         });
      }
   }
}
//
#undef CLASS_TEMPLATE_PARAMS
#undef CLASS_NAME