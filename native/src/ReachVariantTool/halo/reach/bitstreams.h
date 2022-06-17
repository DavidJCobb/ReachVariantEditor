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

         static constexpr size_t string_ref_bitcount = std::bit_width(megalo::string_table::max_count - 1);
         static constexpr size_t string_ref_bitcount_optional = std::bit_width(megalo::string_table::max_count);

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

      public:
         game_variant_data* get_game_variant_data() const { return this->current_variant_data; }
         void set_game_variant_data(game_variant_data* v) { this->current_variant_data = v; }

         void read(megalo::string_ref& ref) {
            string_index<false> index;
            base_type::read(index);
            if (this->main_string_table) {
               ref = this->main_string_table->string_by_index(index);
            } else {
               ref = nullptr;
               this->_pending_string_refs.required.push_back({ &ref, index });
            }
         }
         void read(megalo::string_ref_optional& ref) {
            string_index<true> index;
            base_type::read(index);
            if (this->main_string_table) {
               if (index < 0) {
                  ref = nullptr;
                  return;
               }
               ref = this->main_string_table->string_by_index(index);
            } else {
               ref = nullptr;
               this->_pending_string_refs.optional.push_back({ &ref, index });
            }
         }

         void read(megalo::string_table& tbl) {
            base_type::read(tbl);
            if (!this->main_string_table) {
               this->main_string_table = &tbl;
               //
               auto& list_req = this->_pending_string_refs.required;
               auto& list_opt = this->_pending_string_refs.optional;
               for (auto& item : list_req) {
                  *item.target = tbl.string_by_index(item.index); // TODO: validate index
               }
               for (auto& item : list_opt) {
                  if (item.index == -1)
                     *item.target = nullptr;
                  else
                     *item.target = tbl.string_by_index(item.index); // TODO: validate index
               }
               this->_pending_string_refs = {}; // fast clear
            }
         }
   };

   class bitwriter : public halo::bitwriter<bitwriter> {
      public:
         using base_type::bitwriter;
         using base_type::write;
         using base_type::write_bits;
   };
}