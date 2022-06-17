#pragma once
#include <bit>
#include <cstdint>
#include <vector>
#include "halo/bitreader.h"
#include "halo/bitwriter.h"
#include "./load_process.h"
#include "./bitstreams.fwd.h"
#include "./megalo/strings.h"

namespace halo::reach {
   class game_variant_data;
   class megalo_variant_data;

   namespace impl::streams {
      template<bool Optional> struct _derive_string_index {
         using value_type = int32_t;
         //
         static constexpr bool    optional = Optional;
         static constexpr int32_t offset   = optional ? 1 : 0;
         static constexpr auto params = bitnumber_params<int32_t>{
            .offset = offset,
         };
         //
         using type = bitnumber<
            std::bit_width(megalo::string_table::max_count - (optional ? 0 : 1)),
            int32_t,
            params
         >;
      };
      template<bool Optional> using string_index = typename _derive_string_index<Optional>::type;
   }

   class bitreader : public halo::bitreader<bitreader, load_process&> {
      public:
         using base_type::bitreader;
         using base_type::read;

      protected:
         template<typename T> struct _pending_string_ref {
            T* target = nullptr;
            int32_t index = -1;
         };

         game_variant_data* current_variant_data = nullptr;
         struct {
            std::vector<_pending_string_ref<megalo::string_ref>> required;
            std::vector<_pending_string_ref<megalo::string_ref_optional>> optional;
         } _pending_string_refs;
         megalo::string_table* main_string_table = nullptr;

         template<bool Optional> using string_index = impl::streams::string_index<Optional>;

      public:
         game_variant_data* get_game_variant_data() const { return this->current_variant_data; }
         void set_game_variant_data(game_variant_data* v) { this->current_variant_data = v; }

         void read(megalo::string_ref& ref);
         void read(megalo::string_ref_optional& ref);

         void read(megalo::string_table& tbl);
   };

   class bitwriter : public halo::bitwriter<bitwriter> {
      public:
         using base_type::bitwriter;
         using base_type::write;
         using base_type::write_bits;

      protected:
         game_variant_data*    current_variant_data = nullptr;
         megalo::string_table* main_string_table    = nullptr;

         template<bool Optional> using string_index = impl::streams::string_index<Optional>;

      public:
         game_variant_data* get_game_variant_data() const { return this->current_variant_data; }
         void set_game_variant_data(game_variant_data* v);

         void write(megalo::string_ref& ref);
         void write(megalo::string_ref_optional& ref);
   };
}