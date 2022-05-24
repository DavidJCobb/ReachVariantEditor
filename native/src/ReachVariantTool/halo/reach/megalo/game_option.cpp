#include "game_option.h"
#include "halo/reach/bitstreams.h"

namespace halo::reach::megalo {
   void _validate_option_value(bitreader& stream, game_option::value_type& v) {
      if (v < -500) {
         if constexpr (bitreader::has_load_process) {
            static_assert(false, "report error");
         }
         v = -500;
         return;
      }
      if (v > 500) {
         if constexpr (bitreader::has_load_process) {
            static_assert(false, "report error");
         }
         v = 500;
         return;
      }
   }
   template<typename... Types> void _validate_option_values(bitreader& stream, Types&... values) {
      (_validate_option_value(stream, values), ...);
   }

   void game_option::enum_value::read(bitreader& stream) {
      stream.read(
         value,
         name,
         desc
      );
   }

   void game_option::read(bitreader& stream) {
      stream.read(
         name,
         desc,
         is_range
      );
      if (this->is_range) {
         stream.read(
            range.initial,
            range.min,
            range.max,
            range.current
         );
         _validate_option_values(stream,
            //
            this->range.initial,
            this->range.min,
            this->range.max,
            this->range.current
         );
      } else {
         value_index initial_index;
         stream.read(initial_index);

         bitnumber<std::bit_width(limits::script_option_values), uint8_t> count;
         stream.read(count);

         this->enumeration.values.resize(count);
         for (size_t i = 0; i < count; ++i) {
            stream.read(this->enumeration.values[i]);
         }

         value_index current_index;
         stream.read(current_index);

         if (initial_index < count) {
            this->enumeration.initial = this->enumeration.values[initial_index];
         } else {
            if constexpr (bitreader::has_load_process) {
               static_assert(false, "report error");
            }
         }
         if (current_index < count) {
            this->enumeration.current = this->enumeration.values[current_index];
         } else {
            if constexpr (bitreader::has_load_process) {
               // TODO: did the game ever actually validate this?
               static_assert(false, "report error");
            }
         }

         for (auto& item : this->enumeration.values)
            _validate_option_value(stream, item.value);
      }
   }
}