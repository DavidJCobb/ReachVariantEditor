#pragma once
#include <variant>
#include "helpers/cs.h"
#include "helpers/tuple_filter.h"
#include "helpers/tuple_prepend.h"
#include "helpers/tuple_transform.h"
#include "helpers/tuple_unpack.h"
#include "halo/util/refcount.h"
#include "../../operand.h"
#include "../../operand_typeinfo.h"

#include "./target.h"

namespace halo::reach::megalo::operands::variables {
   namespace impl {
      class base : public operand {
         protected:
            size_t  target_id = 0;
         public:
            uint8_t which = 0;
            int16_t index = 0;
            
         protected:
            void read_target_id(bitreader&, size_t bitcount);
            void read_which(bitreader&, size_t bitcount);
            void read_index(bitreader&, size_t bitcount);
      };
   }
   
   template<
      cobb::cs Name,
      variable_type Type,
      const auto& DefinitionList
   > class base : public impl::base {
      public:
         static constexpr variable_type type = Type;
         static constexpr operand_typeinfo typeinfo = {
            .internal_name = Name.c_str(),
         };

         static constexpr auto& definition_list = DefinitionList;
         using definition_list_type = std::decay_t<decltype(definition_list)>;
         using indexed_data_variant = typename definition_list_type::indexed_data_variant;

      #pragma region Functor table for indexed data accessors
      protected:
         template<size_t N> struct _extract_indexed_accessor {
            static void access(megalo_variant_data&, indexed_data_variant& v, size_t index) {
               v = std::monostate{};
            }
         };
         template<size_t N> requires (!std::is_same_v<void, typename definition_list_type::nth_indexed_type<N>>) struct _extract_indexed_accessor<N> {
            static void access(megalo_variant_data& v, indexed_data_variant& v, size_t index) {
               v = ((std::get<N>(definition_list.entries)).accessor)(v, index);
            }
         };

      public:
         using type_erased_accessor_type = void* (*)(megalo_variant_data&, size_t index);
         //
         static constexpr auto type_erased_accessors = [](){
            std::array<type_erased_accessor_type, definition_list_type::size> out = {};
            cobb::constexpr_for<0, definition_list_type::size, 1>([&out]<size_t I>() {
               out[I] = &_extract_indexed_accessor<I>::access;
            });
            return out;
         }();
      #pragma endregion
      #pragma region List of target metadata
      public:
         static constexpr auto all_target_metadata = [](){
            std::array<target_metadata, definition_list_type::size> out = {};
            cobb::constexpr_for<0, definition_list_type::size, 1>([&out]<size_t I>() {
               if constexpr (std::is_same_v<definition_list_type::nth_type<N>, target_metadata>) {
                  out[I] = std::get<N>(definition_list.entries);
               } else {
                  out[I] = std::get<N>(definition_list.entries).metadata;
               }
            });
            return out;
         }();
      #pragma endregion

         static_assert(
            [](){
               for (const auto& item : all_target_metadata)
                  if (!item.is_valid())
                     return false;
               return true;
            }(),
            "One or more of the target_metadata for this variable type is invalid."
         );

      #pragma region List of bitcounts
      protected:
         struct target_bitcount_data {
            size_t which = 0;
            size_t index = 0;
         };

         static constexpr auto all_target_bitcounts = [](){
            std::array<target_bitcount_data, definition_list_type::size> out = {};
            for (size_t i = 0; i < definition_list_type::size; ++i) {
               const auto& md = all_target_metadata[i];
               //
               // Get wihch:
               //
               if (md.scopes.outer.has_value()) {
                  auto& s = variable_scope_metadata_from_enum(md.scopes.outer.value());
                  out[i].which = std::bit_width(s.maximum_of_type<Type>());
                     static_assert(false, "TODO: This doesn't work for the global scope, because static variables e.g. killer_player are held there too.");
                     static_assert(false, "TODO: Actually, this doesn't work at all...");
                        //
                        // We need to define a list of "top-level values" of each type, which will include things like 
                        // hud_player and killed_object. We'd then want to query that here.
                        //
               }
               //
               // Get index:
               //
               if (md.bitcount) {
                  out[i].index = md.bitcount;
               } else if (md.has_index()) {
                  if (md.type == target_type::variable) {

                  } else if (md.scopes.inner.has_value()) {
                     if (!md.scopes.outer.has_value())
                        cobb::unreachable();
                     auto outer = md.scopes.outer.value();
                     auto inner = md.scopes.inner.value();
                     auto& s = variable_scope_metadata_from_enum(outer);
                     out[i].index = std::bit_width(s.maximum_of_type<inner>()); static_assert(false, "TODO: fails here: variable as template parameter");
                  } else {
                     cobb::unreachable();
                  }
               }
               //
               // Done with this item.
            }
            return out;
         }();

      #pragma endregion

      public:
         indexed_data_variant indexed_data = std::monostate{};

         virtual void read(bitreader& stream) override {
            impl::base::read_target_id(stream, static_assert(false, "TODO: target-index bitcount here"));
            static_asserT(false, "TODO: validate the target ID; if it's out of range, that should be a fatal error.");
            //
            const target_metadata& metadata = all_target_metadata[this->target_id];
            if (metadata.has_which()) {
               impl::base::read_which(stream, static_assert(false, "TODO: Get the bitcount for the which-value somehow."));
            }
            if (metadata.has_index()) {
               //
               static_assert(false, "TODO: Bitcount can be computed implicitly in specific cases, e.g. object[w].player[i].biped, and in those cases it'll be 0 on the metadata object. We need to figure out how to do that.");
               // - maybe a constexpr list akin to all_target_metadata?
               //
               impl::base::read_index(stream, metadata.bitcount);
            }
            //
            static_assert(false, "TODO: code to validate the target ID, which, and index");
         }
   };
}