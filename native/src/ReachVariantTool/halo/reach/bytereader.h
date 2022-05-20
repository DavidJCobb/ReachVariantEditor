#pragma once
#include <variant>
#include <vector>
#include "halo/bytereader.h"
#include "./load_process.h"

namespace halo::reach {
   using bytereader = halo::bytereader<load_process>;
}