#pragma once
#include <variant>
#include <vector>
#include "halo/bitreader.h"
#include "./load_process.h"

namespace halo::reach {
   using bitreader = halo::bitreader<load_process>;
}