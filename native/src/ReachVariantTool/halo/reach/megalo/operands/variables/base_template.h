#pragma once
#include "base.h"
#include <variant>
#include "helpers/cs.h"
#include "helpers/tuple_filter.h"
#include "helpers/tuple_prepend.h"
#include "helpers/tuple_transform.h"
#include "helpers/tuple_unpack.h"
#include "helpers/unreachable.h"
#include "halo/util/refcount.h"
#include "halo/reach/bitstreams.h"

#include "./target.h"
#include "./top_level_values.h"

namespace halo::reach::megalo::operands::variables {
   template<
      cobb::cs Name,
      variable_type Type,
      const auto& DefinitionList
   > class base : public unknown_type {
      public:
         static constexpr variable_type type = Type;
         inline static constexpr operand_typeinfo typeinfo = {
            .internal_name = Name.c_str(),
         };

         static constexpr auto& definition_list = DefinitionList;
         using definition_list_type = std::decay_t<decltype(definition_list)>;
         using indexed_data_variant = typename definition_list_type::indexed_data_variant;

         static constexpr size_t target_count = definition_list_type::size;

      protected:
         template<size_t I> struct _nth_indexed_type {
            using type = void;
         };
         template<size_t I> requires requires {
            typename std::tuple_element_t<I, definition_list_type>::indexed_data_type;
         } struct _nth_indexed_type<I> {
            using type = typename definition_list_type::template nth_type<I>::indexed_data_type;
         };
      public:
         template<size_t I> using nth_indexed_type = _nth_indexed_type<I>::type;

      #pragma region Functor table for indexed data accessors
      protected:
         template<size_t N> struct _extract_indexed_accessor {
            static void access(megalo_variant_data&, indexed_data_variant& v, size_t index) {
               v = std::monostate{};
            }
         };
         template<size_t N> requires (!std::is_same_v<void, nth_indexed_type<N>>) struct _extract_indexed_accessor<N> {
            static void access(megalo_variant_data& gv, indexed_data_variant& data, size_t index) {
               data = ((std::get<N>(definition_list.entries)).accessor)(gv, index);
            }
         };

      public:
         using indexed_accessors_type = void (*)(megalo_variant_data&, indexed_data_variant&, size_t index);
         //
         static constexpr auto indexed_accessors = [](){
            std::array<indexed_accessors_type, definition_list_type::size> out = {};
            cobb::constexpr_for<0, definition_list_type::size, 1>([&out]<size_t I>() {
               out[I] = &_extract_indexed_accessor<I>::access;
            });
            return out;
         }();
      #pragma endregion
      #pragma region List of target metadata
      public:
         static constexpr auto all_target_metadata = [](){
            std::array<target_metadata, target_count> out = {};
            cobb::constexpr_for<0, target_count, 1>([&out]<size_t I>() {
               if constexpr (std::is_same_v<typename definition_list_type::template nth_type<I>, target_metadata>) {
                  out[I] = std::get<I>(definition_list.entries);
               } else {
                  out[I] = std::get<I>(definition_list.entries).metadata;
               }
            });
            return out;
         }();
      #pragma endregion

         static_assert(
            definition_list.index_of_first_invalid() == size_t(-1),
            "One or more of the target_metadata for this variable type is invalid."
         );

      protected:
         static constexpr auto all_target_index_bitcounts = [](){
            std::array<size_t, target_count> out = {};
            for (size_t i = 0; i < target_count; ++i) {
               const auto& src = all_target_metadata[i];
               auto& dst = out[i];
               dst = 0;
               //
               if (src.bitcount) {
                  dst = src.bitcount;
                  continue;
               }
               if (src.has_index()) {
                  if (src.type == target_type::variable) {
                     if (!src.scopes.outer.has_value())
                        cobb::unreachable();
                     auto scope_id = src.scopes.outer.value();
                     if (scope_id == variable_scope::global) {
                        //
                        // All top-level values, including global variables, share a target ID.
                        //
                        dst = std::bit_width(top_level_values::max_of_type(Type) - 1);
                     } else {
                        auto& s = variable_scope_metadata_from_enum(scope_id);
                        dst = std::bit_width(s.maximum_of_type(Type) - 1);
                     }
                     continue;
                  }
                  //
                  if (src.scopes.inner.has_value()) {
                     if (!src.scopes.outer.has_value())
                        cobb::unreachable();
                     auto  outer = src.scopes.outer.value();
                     auto  inner = src.scopes.inner.value();
                     auto& s     = variable_scope_metadata_from_enum(outer);
                     dst = std::bit_width(s.maximum_of_type(variable_type_for_scope(inner)) - 1);
                     continue;
                  }
                  cobb::unreachable();
               }
            }
            return out;
         }();

      public:
         indexed_data_variant indexed_data = std::monostate{};

         virtual void read(bitreader& stream) override {
            impl::base::read_target_id(stream, target_count);
            //
            const target_metadata& metadata = all_target_metadata[this->target_id];
            if (metadata.has_which()) {
               impl::base::read_which(stream, metadata.scopes.outer.value());
            }
            if (metadata.has_index()) {
               auto index_bc = all_target_index_bitcounts[this->target_id];
               if (metadata.type == target_type::immediate) {
                  impl::base::read_immediate(stream, index_bc);
               } else {
                  impl::base::read_index(stream, index_bc);
               }
               //
               // TODO: We can't validate indices if we're only keeping bitcounts. We should keep 
               //       maximums instead, falling back to bitcounts for immediates only.
               //
               //
               // TODO: Using stream.get_game_variant_data here requires that the full bitreader 
               //       definition be included; can we move that somewhere else?
               //
               indexed_accessors[this->target_id](*(megalo_variant_data*)stream.get_game_variant_data(), this->indexed_data, this->index);
            }
         }

         virtual variable_type get_type() const override {
            return Type;
         }
   };
}