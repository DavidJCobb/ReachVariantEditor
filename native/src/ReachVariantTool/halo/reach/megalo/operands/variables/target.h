#pragma once
#include <array>
#include <cstdint>
#include <optional>
#include <type_traits>
#include <variant>
#include "helpers/constexpr_for_functor.h"
#include "helpers/tuple_filter.h"
#include "helpers/tuple_prepend.h"
#include "helpers/tuple_transform.h"
#include "helpers/tuple_unpack.h"
#include "halo/util/refcount.h"
#include "halo/reach/megalo/variable_scope.h"

// for var base
#include "helpers/cs.h"

namespace halo::reach {
   class megalo_variant_data;
}

namespace halo::reach::megalo::operands {
   namespace variables {
      enum class target_type {
         immediate,    // e.g. integer constants
         variable,     // e.g. global.player[i], player[w].object[i]
         engine_data,  // engine-level data objects, e.g. game.betrayal_booting, game.grace_period_timer, object[w].spawn_sequence
         indexed_data, // indexed data defined in the game variant, e.g. script_option[i], player[w].script_stat[i]
      };

      struct target_metadata {
         public:
            struct flag {
               flag() = delete;
               enum {
                  readonly = 0x01, // implied by "immediate" type
               };
            };

         protected:
            struct scope_set {
               std::optional<variable_scope> outer;
               std::optional<variable_scope> inner;

               constexpr scope_set() {}
               constexpr scope_set(variable_scope o) : outer(o) {}
            };

         public:
            const char* name = "";
            target_type type;
            //
            size_t    bitcount = 0; // bitcount for index or for immediate value; used if non-zero and relevant to the register type
            uint32_t  flags    = 0;
            scope_set scopes   = {};

            static constexpr target_metadata engine_data_readonly(const target_metadata& base) {
               auto out = base;
               out.type   = target_type::engine_data;
               out.flags |= flag::readonly;
               return out;
            }

         public:
            constexpr bool has_which() const {
               return this->scopes.outer.has_value();
            }
            constexpr bool has_index() const {
               switch (this->type) {
                  using enum target_type;
                  case immediate:
                  case variable:
                  case indexed_data:
                     return true;
               }
               return false;
            }

            constexpr bool is_valid() const {
               if (this->type == target_type::immediate) {
                  if (this->bitcount == 0) // immediate values are stored via the index; bitcount must be known
                     return false;
                  if (this->scopes.outer.has_value()) // immediates cannot be scoped
                     return false;
                  if (this->scopes.inner.has_value()) // immediates cannot be scoped
                     return false;
               }
               if (this->type == target_type::engine_data) {
                  if (this->scopes.outer == variable_scope::global) // engine data should have no scope set when it is not a member of any object
                     return false;
               }
               if (this->scopes.inner.has_value()) {
                  if (!this->scopes.outer.has_value()) // inner scopes are only valid if an outer scope is present
                     return false;
                  if (this->scopes.inner.value() == variable_scope::global) // the "global" scope cannot be nested
                     return false;
               }
               if (this->has_index()) {
                  //
                  // Access to elements of an indexed collection requires that we be able to 
                  // know the index bitcount.
                  //
                  if (this->bitcount == 0) {
                     if (!this->scopes.inner.has_value()) // bitcount can be deduced from inner scope
                        return false;
                     if (this->type != target_type::variable) // bitcount can be deduced from variable type
                        return false;
                  }
               }
               return true;
            }
      };

      template<typename IndexedData> struct target_definition {
         public:
            using indexed_data_type = IndexedData;
         protected:
            struct _dummy {
               constexpr _dummy() {}
               constexpr _dummy(std::nullptr_t) {}
            };

         public:
            using accessor_type = std::conditional_t<
               std::is_same_v<indexed_data_type, void>,
               _dummy,
               indexed_data_type* (*)(megalo_variant_data&, size_t index)
            >;
         public:
            target_metadata metadata;
            accessor_type   accessor = nullptr;
      };

      namespace impl {
         template<typename T> concept is_target_definition = requires {
            typename T::indexed_data_type;
            requires std::is_same_v<std::decay_t<T>, target_definition<typename T::indexed_data_type>>;
         };
      }

      template<typename... Types> requires ((impl::is_target_definition<Types> || std::is_same_v<Types, target_metadata>) && ...)
      struct target_definition_list {
         public:
            using as_tuple = std::tuple<Types...>;
            template<size_t N> using nth_type = std::tuple_element_t<N, as_tuple>;

            static constexpr size_t size = sizeof...(Types);

         protected:
            template<typename T> struct _get_indexed_type {
               using type = void;
               using accessor_type = void;
            };
            template<typename T> requires impl::is_target_definition<T> struct _get_indexed_type<T> {
               using type = T::indexed_data_type;
               using accessor_type = T::accessor_type;
            };

         public:
            template<size_t N> using nth_indexed_type  = typename _get_indexed_type<nth_type<N>>::type;
            template<size_t N> using nth_accessor_type = typename _get_indexed_type<nth_type<N>>::accessor_type;

         protected:
            struct _indexed_type_variant {
               template<typename Current> struct transformer {
                  using type = util::refcount_ptr<Current>;
               };

               //
               // Iterate over all of the register set metadata types. Find any that 
               // refer to indexed data, and extract the types of indexed data they 
               // refer to. Create a std::variant of util::refcount_ptrs to those 
               // types, and give the variant std::monostate as its first type.
               //
               using type = cobb::tuple_unpack_t<
                  std::variant,
                  cobb::tuple_prepend<
                     std::monostate,
                     cobb::tuple_transform<
                        transformer,
                        cobb::tuple_filter_t<
                           []<typename Current>() {
                              return !std::is_same_v<Current, target_metadata>;
                           },
                           as_tuple
                        >
                     >
                  >
               >;
            };

         public:
            using indexed_data_variant = _indexed_type_variant::type;

         public:
            std::tuple<Types...> entries;

            template<size_t N> struct _extract_indexed_accessor {
               static void* access(megalo_variant_data&, size_t index) {
                  return nullptr;
               }
            };
            template<size_t N> requires (!std::is_same_v<void, nth_indexed_type<N>>) struct _extract_indexed_accessor<N> {
               static void* access(megalo_variant_data& v, size_t index) {
                  return (void*) (std::get<N>(this->entries)).accessor(v, index);
               }
            };

            using type_erased_accessor_type = void* (*)(megalo_variant_data&, size_t index);

            static constexpr auto type_erased_accessors = ([]() {
               static constexpr auto size = sizeof...(Types);
               //
               std::array<type_erased_accessor_type, size> out = {};
               cobb::constexpr_for<0, size, 1>([&out]<size_t I>() {
                  out[I] = &_extract_indexed_accessor<I>::access;
               });
               return out;
            })();

         public:
            constexpr target_definition_list(Types&&... items) : entries(items) {}

            const target_metadata& metadata_for(size_t index) const;

            indexed_data_variant pull_indexed_data(megalo_variant_data&, size_t index) const;
               static_assert(false, "TODO: Okay, but how do we pick an accessor by index when the types vary?");
                  //
                  // maybe generate a function table, indexable at run-time, which takes a variant reference 
                  // and the megalo_variant_data& and calls the relevant accessor for the table entry?
                  //
      };
      //
   }
}