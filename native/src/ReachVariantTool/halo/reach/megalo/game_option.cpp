#include "game_option.h"
#include "halo/reach/bitstreams.h"

#include "halo/reach/megalo/load_process_messages/game_option/current_index_out_of_bounds.h"
#include "halo/reach/megalo/load_process_messages/game_option/initial_index_out_of_bounds.h"
#include "halo/reach/megalo/load_process_messages/game_option/value_out_of_bounds.h"

namespace halo::reach::megalo {
   void _validate_option_value(bitreader& stream, game_option::value_type& v) {
      if (v < -500) {
         if constexpr (bitreader::has_load_process) {
            stream.load_process().emit_error<halo::reach::load_process_messages::megalo::game_option_value_out_of_bounds>({
               .value = v,
            });
         }
         v = -500;
         return;
      }
      if (v > 500) {
         if constexpr (bitreader::has_load_process) {
            stream.load_process().emit_error<halo::reach::load_process_messages::megalo::game_option_value_out_of_bounds>({
               .value = v,
            });
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
         value_index current_index;
         stream.read(
            initial_index,
            this->enumeration.values,
            current_index
         );
         auto count = this->enumeration.values.size();

         if (initial_index < count) {
            this->enumeration.initial = &this->enumeration.values[initial_index];
         } else {
            if constexpr (bitreader::has_load_process) {
               stream.load_process().emit_error<halo::reach::load_process_messages::megalo::game_option_initial_index_out_of_bounds>({
                  .index = initial_index,
                  .count = count,
               });
            }
         }
         if (current_index < count) {
            this->enumeration.current = &this->enumeration.values[current_index];
         } else {
            if constexpr (bitreader::has_load_process) {
               // TODO: did the game ever actually validate this?
               stream.load_process().emit_error<halo::reach::load_process_messages::megalo::game_option_current_index_out_of_bounds>({
                  .index = current_index,
                  .count = count,
               });
            }
         }

         for (auto& item : this->enumeration.values)
            _validate_option_value(stream, item.value);
      }
   }
}