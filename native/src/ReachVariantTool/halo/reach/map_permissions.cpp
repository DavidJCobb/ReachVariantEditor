#include "map_permissions.h"
#include "halo/reach/bitstreams.h"

namespace halo::reach {
   void map_permissions::read(bitreader& stream) {
      size_t count = stream.read_bits<size_t>(6);
      if constexpr (bitreader::has_load_process) {
         if (count > 32) {
            static_assert(false, "TODO: emit error through load process for too many map IDs");
         }
      }
      this->map_ids.resize(count);
      for (size_t i = 0; i < count; i++)
         this->map_ids[i] = stream.read_bits<uint16_t>(16);
      this->type.read(stream);
   }
}