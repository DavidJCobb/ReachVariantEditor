#include "bitstreams.h"
#include "halo/util/indexed.h"
#include "./megalo/variant_data.h"

namespace halo::reach {
   #pragma region bitreader
      void bitreader::read(megalo::string_ref& ref) {
         string_index<false> index;
         base_type::read(index);
         if (this->main_string_table) {
            ref = this->main_string_table->string_by_index(index);
         } else {
            ref = nullptr;
            this->_pending_string_refs.required.push_back({ &ref, index });
         }
      }
      void bitreader::read(megalo::string_ref_optional& ref) {
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

      void bitreader::read(megalo::string_table& tbl) {
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
   #pragma endregion

   #pragma region bitwriter
      void bitwriter::set_game_variant_data(game_variant_data* v) {
         this->current_variant_data = v;
         this->main_string_table    = nullptr;
         if (auto* casted = dynamic_cast<megalo_variant_data*>(v)) {
            this->main_string_table = &casted->script.strings;
         }
      }

      void bitwriter::write(const megalo::string_ref& ref) {
         assert(ref != nullptr);
         string_index<false> index = static_cast<halo::util::indexed&>(*ref).index;
         base_type::write(index);
      }
      void bitwriter::write(const megalo::string_ref_optional& ref) {
         string_index<true> index = -1;
         if (ref != nullptr) {
            index = static_cast<halo::util::indexed&>(*ref).index;
         }
         base_type::write(index);
      }
   #pragma endregion
}