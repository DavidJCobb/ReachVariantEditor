#include "invalid_file_block_header.h"
#include "helpers/qt/four_cc_to_string.h"

namespace halo::common::load_process_messages {
   QString invalid_file_block_header::to_string() const {
      QString out;
      if (this->info.expected.size) {
         out = QString("Expected a %1 block with %2 bytes of data; found a %3 block with %4 bytes of data instead.");
      } else {
         out = QString("Expected a %1 block; found a %3 block with %4 bytes of data instead.");
      }
      out = out
         .arg(cobb::qt::four_cc_to_string(this->info.expected.signature.bytes))
         .arg(this->info.expected.size)
         .arg(cobb::qt::four_cc_to_string(this->info.found.signature.bytes))
         .arg(this->info.found.size);
      return out;
   }
}