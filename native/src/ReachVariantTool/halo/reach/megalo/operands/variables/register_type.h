#pragma once
#include <cstdint>
#include <optional>
#include <type_traits>
#include "helpers/template_parameters/optional.h"
#include "halo/util/fixed_string.h"
#include "../../variable_scope.h"
#include "../../variable_type.h"

namespace halo::reach {
   class megalo_variant_data;
}

namespace halo::reach::megalo::operands {
   namespace variables {
      enum class register_type {
         immediate,    // e.g. integer constants
         variable,     // e.g. global.player[i], player[w].object[i]
         engine_data,  // engine-level data objects, e.g. game.betrayal_booting, game.grace_period_timer, object[w].spawn_sequence
         indexed_data, // indexed data defined in the game variant, e.g. script_option[i], player[w].script_stat[i]
      };

      struct register_set_flag {
         register_set_flag() = delete;
         enum {
            readonly = 0x01, // implied by "immediate" type
         };
      };

      // Each variable type should have a list of register_metadata. Refer to the readme for further details.
      template<typename IndexedData = void> struct register_set_definition {
         public: // a TeMpLaTe PaRaMeTeR oF cLaSs TyPe MuSt Be Of StRuCtUrAl ClAsS tYpE
            struct _dummy {
               constexpr _dummy() {}
               constexpr _dummy(std::nullptr_t) {}
            };

            using indexed_data_type = IndexedData;

            template<typename T> using optional = cobb::template_parameters::optional<T>;

         public:
            using accessor_type = std::conditional_t<
               std::is_same_v<indexed_data_type, void>,
               _dummy,
               indexed_data_type* (*)(megalo_variant_data&, size_t index)
            >;

         public:
            util::fixed_string<char, 128> name = ""; // currently for debugging only; fixed string used to allow use of struct as template parameter
            register_type type = (register_type)-1;
            //
            accessor_type accessor = nullptr;
            size_t   bitcount = 0; // bitcount for index or for immediate value; used if non-zero and relevant to the register type
            uint32_t flags    = 0;
            optional<variable_scope> scope;
            optional<variable_type>  scope_inner;

            static constexpr register_set_definition<void> engine_data_readonly(const register_set_definition<void>& base) {
               auto out = base;
               out.type   = register_type::engine_data;
               out.flags |= register_set_flag::readonly;
               return out;
            }
      };
   }
}
