#pragma once
#include <cstdint>
#include <optional>

namespace cobb::win32 {
   extern std::optional<uint32_t> get_parent_process_id();
}