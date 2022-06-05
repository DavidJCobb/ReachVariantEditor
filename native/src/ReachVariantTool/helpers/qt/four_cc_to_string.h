#pragma once
#include <cstdint>
#include <QString>

namespace cobb::qt {
   extern QString four_cc_to_string(uint32_t);
   extern QString four_cc_to_string(const char(&)[4]);
}