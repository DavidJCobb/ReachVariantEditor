#pragma once
#include "halo/game.h"

namespace halo::common::load_errors {
   struct file_is_for_the_wrong_game {
      game expected;
      game found;
   };
}
