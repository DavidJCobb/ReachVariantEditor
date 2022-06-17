#include "map_permissions.h"
#include "halo/reach/bitstreams.h"

#include "halo/bad_data_on_write_exception.h"
#include "halo/reach/megalo/load_process_messages/map_permissions_too_many_ids.h"

namespace halo::reach {
   namespace {
      constexpr size_t count_bitcount = std::bit_width(map_permissions::max_count);
   }

   void map_permissions::read(bitreader& stream) {
      auto   pos   = stream.get_position();
      size_t count = stream.read_bits<size_t>(count_bitcount);
      if (count > max_count) {
         stream.emit_error_at<halo::reach::load_process_messages::megalo::map_permissions_too_many_ids>(
            {
               .count     = count,
               .max_count = 32,
            },
            pos
         );
      }
      this->map_ids.resize(count);
      for (size_t i = 0; i < count; i++)
         this->map_ids[i] = stream.read_bits<uint16_t>(16);
      this->type.read(stream);
   }
   void map_permissions::write(bitwriter& stream) const {
      if (this->map_ids.size() > max_count)
         throw bad_data_on_write_exception("Map ID count is too high.");
      //
      stream.write_bits(count_bitcount, this->map_ids.size());
      for (auto& id : this->map_ids)
         stream.write(id);
   }
}