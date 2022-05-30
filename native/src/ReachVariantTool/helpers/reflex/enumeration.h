#pragma once
#include <array>
#include <string>
#include <tuple>
#include <type_traits>
#include "helpers/numeric_min_max.h"
#include "helpers/tuple_index_of.h"
#include "helpers/unreachable.h"
#include "../cs.h"
#include "./member.h"
#include "./member_range.h"
#include "./nested_enum.h"
#include "enumeration/member_concepts.h"
#include "enumeration/constraints.h"

#include "enumeration/member_list.h"

namespace cobb::reflex {
   namespace impl::enumeration {
      template<typename First, typename... Types> concept members_are_valid =
         member_types_are_valid<First, Types...> &&
         member_names_are_unique<First, Types...> &&
         member_explicit_values_are_representable<First, Types...>;

      template<typename First, typename... Members> struct extract_parameters {
         //
         // This struct extracts a reflex enum's explicit underlying type, if one was provided, and also 
         // runs validation on the members.
         //

         static constexpr bool explicit_underlying_type = is_underlying_type<First>;

         using member_list = std::conditional_t<
            explicit_underlying_type,
            member_list<First, Members...>,
            member_list<default_underlying_enum_type, First, Members...>
         >;

         using underlying_type = std::conditional_t<
            explicit_underlying_type,
            First,
            default_underlying_enum_type
         >;
      };
   }

   #pragma region Helpers for enumeration<...>::underlying_value_of
   namespace impl::enumeration::underlying_value_of {
      enum class no_detail {};

      template<typename DetailType> concept detail_absent = requires {
         requires std::is_same_v<DetailType, no_detail>;
      };

      template<typename DetailType> concept detail_appropriate_for_range = std::is_convertible_v<DetailType, size_t>;
      template<typename Enumeration, cobb::cs Name, typename DetailType> concept detail_valid_for_range = requires {
         requires detail_appropriate_for_range<DetailType>;
         typename Enumeration::template member_type<Name>;
         requires is_member_range<typename Enumeration::template member_type<Name>>;
      };

      template<typename Enumeration, cobb::cs Name, typename DetailType> concept detail_valid_for_nested_enum = requires {
         requires is_cs<DetailType>;
         typename Enumeration::template member_type<Name>;
         requires is_nested_enum<typename Enumeration::template member_type<Name>>;
      };

      template<typename Enumeration, cobb::cs Name, typename DetailType> concept detail_is_valid =
         (detail_absent<DetailType> || detail_valid_for_range<Enumeration, Name, DetailType> || detail_valid_for_nested_enum<Enumeration, Name, DetailType>);

      template<typename T> struct unwrap_nested_enum {
         using type = void;
      };
      template<typename T> requires is_nested_enum<T> struct unwrap_nested_enum<T> {
         using type = typename T::enumeration;
      };
   }
   #pragma endregion

   template<typename... Parameters> requires impl::enumeration::members_are_valid<Parameters...>
   class enumeration {
      protected:
         using _extractor = impl::enumeration::extract_parameters<Parameters...>;
         static constexpr auto _index_of_none = std::numeric_limits<size_t>::max();

      public:
         using underlying_type = _extractor::underlying_type;

      protected:
         using member_list = _extractor::member_list;

         underlying_type _value = {};

      public:
         static constexpr bool explicit_underlying_type = _extractor::explicit_underlying_type;

      public:
         using metadata_type = member_list::metadata_type;

      protected:
         template<cs Name> static constexpr size_t index_of = member_list::template index_of<Name>();

      public:
         template<cs Name> static constexpr bool has = (index_of<Name> != _index_of_none);
         template<typename Subset> static constexpr bool has_subset = member_list::template index_of_subset<Subset>() != std::numeric_limits<size_t>::max();
         template<typename Subset> requires has_subset<Subset> static constexpr auto name_of_subset = member_list::template subset_name<Subset>();

         static constexpr auto all_names = member_list::all_names();
         static constexpr auto all_underlying_values = member_list::all_underlying_values();
         static constexpr auto all_metadata = member_list::all_metadata();

      public:
         constexpr enumeration() {}
         template<typename V> requires std::is_convertible_v<V, underlying_type> constexpr static enumeration from_int(V v) {
            enumeration out;
            out._value = v;
            return out;
         }
         template<typename V> requires has_subset<V> constexpr enumeration(const V& v) {
            constexpr auto base = underlying_value_of<name_of_subset<V>>;
            //
            this->_value = base + (underlying_type)(v.to_int() - V::min_underlying_value);
         }

         constexpr underlying_type to_int() const {
            return this->_value;
         }
         constexpr const char* to_c_str() const {
            auto i = member_list::underlying_value_to_type_index(this->_value);
            if (i == std::numeric_limits<size_t>::max())
               return nullptr;
            return all_names[i];
         }
         std::string to_string() const {
            auto p = to_c_str();
            return p ? p : std::string{};
         }

         constexpr const metadata_type* to_metadata() const {
            auto i = member_list::underlying_value_to_type_index(this->_value);
            if (i == std::numeric_limits<size_t>::max())
               return nullptr;
            return &all_metadata[i];
         }

         constexpr bool operator==(const enumeration&) const = default;
         constexpr bool operator!=(const enumeration&) const = default;
         template<typename V> requires has_subset<V> constexpr bool operator==(const V& v) const {
            return *this == enumeration(v);
         }
         template<typename V> requires has_subset<V> constexpr bool operator!=(const V& v) const {
            return *this != enumeration(v);
         }

      public:
         template<cs Name> requires has<Name> using member_type = std::tuple_element_t<index_of<Name>, typename member_list::as_tuple>;

         static constexpr underlying_type min_underlying_value = cobb::min(all_underlying_values);
         static constexpr underlying_type max_underlying_value = cobb::max(all_underlying_values);
         static constexpr enumeration min_value() { return from_int(min_underlying_value); }
         static constexpr enumeration max_value() { return from_int(max_underlying_value); }

         static constexpr underlying_type underlying_value_range = max_underlying_value - min_underlying_value;
         static constexpr size_t value_count = member_list::value_count();

         template<cs Name, auto Detail = impl::enumeration::underlying_value_of::no_detail{}>
         requires (has<Name> && impl::enumeration::underlying_value_of::detail_is_valid<enumeration, Name, std::decay_t<decltype(Detail)>>)
         static constexpr underlying_type underlying_value_of = [](){
            using detail_param_type = std::decay_t<decltype(Detail)>;
            using namespace impl::enumeration::underlying_value_of;

            if constexpr (detail_absent<detail_param_type>) {
               return member_list::template underlying_value_of<Name>();
            }
            if constexpr (detail_appropriate_for_range<detail_param_type>) {
               return underlying_value_of<Name> + (underlying_type)Detail;
            }
            if constexpr (is_cs<detail_param_type>) {
               using nested_type = unwrap_nested_enum<member_type<Name>>::type;
               if constexpr (!std::is_same_v<nested_type, void>) {
                  auto base  = underlying_value_of<Name>;
                  auto inner = nested_type::underlying_value_of<Detail> - nested_type::min_underlying_value;

                  return base + (underlying_type)inner;
               }
            }
            cobb::unreachable();
         }();

         template<cs Name, auto Detail = impl::enumeration::underlying_value_of::no_detail{} >
         requires (has<Name>&& impl::enumeration::underlying_value_of::detail_is_valid<enumeration, Name, std::decay_t<decltype(Detail)>>)
         static constexpr enumeration value_of = [](){
            return from_int(underlying_value_of<Name, Detail>);
         }();
   };

   // You can using-declare this namespace for quick access to the "cs" and "reflex_enum" templates.
   namespace quick::enumeration {
      template<size_t S> using cs = ::cobb::cs<S>;

      template<typename... Parameters> using reflex_enum = ::cobb::reflex::enumeration<Parameters...>;

      using reflex_enum_gap = member_gap;

      template<cobb::cs Name, auto Value = undefined, auto Metadata = no_member_metadata{}>
      using reflex_member = ::cobb::reflex::member<Name, Value, Metadata>;

      template<cobb::cs Name, size_t Count, auto BaseValue = undefined, auto Metadata = no_member_metadata{}>
      using reflex_member_range = member_range<Name, Count, BaseValue, Metadata>;

      template<cobb::cs Name, typename Enum>
      using reflex_nested_enum = nested_enum<Name, Enum>;
   }
}
