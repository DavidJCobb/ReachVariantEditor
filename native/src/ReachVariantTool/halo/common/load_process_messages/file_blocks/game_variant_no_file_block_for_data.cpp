#include "game_variant_no_file_block_for_data.h"

namespace halo::common::load_process_messages {
   QString game_variant_no_file_block_for_data::to_string() const {
      return QString("This file does not contain a file block for game variant data (i.e. an 'mpvr' or 'gvar' block, possibly compressed into a '_cmp' block).");
   }
}