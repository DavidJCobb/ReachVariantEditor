#pragma once
#include <tuple>
#include <type_traits>
#include "helpers/integral_type_for_flag_count.h"
#include "helpers/tuple_unpack.h"
#include "../cs.h"
#include "./member.h"
#include "./member_range.h"

#include "member_concepts/names_are_unique.h"
#include "flags_mask/constraints.h"
#include "flags_mask/member_concepts.h"

#include "flags_mask/member_list.h"

namespace cobb::reflex {
   namespace impl::flags_mask {
      template<typename First, typename... Types> concept members_are_valid =
         member_types_are_valid<First, Types...> &&
         member_concepts::names_are_unique<First, Types...> &&
         member_explicit_values_are_representable<First, Types...>;

      template<typename First, typename... Members> struct extract_parameters {
         //
         // This struct extracts a reflex enum's explicit underlying type, if one was provided, and also 
         // runs validation on the members.
         //

         static constexpr bool explicit_underlying_type = is_underlying_type<First>;

         using member_list = std::conditional_t<
            explicit_underlying_type,
            member_list<Members...>,
            member_list<First, Members...>
         >;
         static constexpr size_t max_flag_index = member_list::highest_flag_index();

         using underlying_type = std::conditional_t<
            explicit_underlying_type,
            First,
            integral_type_for_flag_count<max_flag_index + 1>
         >;
         static_assert(
            [](){
               if constexpr (std::is_same_v<underlying_type, void>) {
                  return false;
               } else {
                  for (auto& d : member_list::all_value_details()) {
                     auto v = uintmax_t(1) << d.flag_index;
                     if (v > std::numeric_limits<underlying_type>::max())
                        return false;
                  }
               }
               return true;
            }(),
            "The specified underlying type cannot hold all of these flags."
         );
      };
   }

   #pragma region Helpers for flags_mask<...>::underlying_value_of
   namespace impl::flags_mask::underlying_value_of {
      enum class no_detail {};

      template<typename DetailType> concept detail_absent = requires {
         requires std::is_same_v<DetailType, no_detail>;
      };

      template<typename DetailType> concept detail_appropriate_for_range = std::is_convertible_v<DetailType, size_t>;
      template<typename FlagsMask, cobb::cs Name, typename DetailType> concept detail_valid_for_range = requires {
         requires detail_appropriate_for_range<DetailType>;
         typename FlagsMask::template member_type<Name>;
         requires is_member_range<typename FlagsMask::template member_type<Name>>;
      };

      template<typename FlagsMask, cobb::cs Name, typename DetailType> concept detail_is_valid =
         (detail_absent<DetailType> || detail_valid_for_range<FlagsMask, Name, DetailType>);
   }
   #pragma endregion

   template<typename... Parameters> requires impl::flags_mask::members_are_valid<Parameters...>
   class flags_mask {
      protected:
         using _extractor = impl::flags_mask::extract_parameters<Parameters...>;
         static constexpr auto _index_of_none = std::numeric_limits<size_t>::max();

      public:
         using original_parameters = std::tuple<Parameters...>;
         using underlying_type     = _extractor::underlying_type;

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

         static constexpr auto all_names = member_list::all_names();
         static constexpr auto all_indices = [](){
            constexpr auto meta = member_list::all_entry_details();
            //
            std::array<size_t, meta.size()> out = {};
            for (size_t i = 0; i < meta.size(); ++i)
               out[i] = meta[i].flag_index;
            return out;
         }();
         static constexpr auto all_underlying_values = [](){
            std::array<underlying_type, all_indices.size()> out = {};
            for (size_t i = 0; i < all_indices.size(); ++i)
               out[i] = underlying_type(1) << all_indices[i];
            return out;
         }();

         static constexpr size_t flag_count = member_list::flag_count();

         template<cs Name> static constexpr size_t flag_index = member_list::template base_flag_index_of<Name>();

         static constexpr size_t min_flag_index = member_list::lowest_flag_index();
         static constexpr size_t max_flag_index = member_list::highest_flag_index();

      public:
         static constexpr auto metadata_per_bit = []() {
            constexpr size_t size = max_flag_index + 1;
            //
            std::array<metadata_type, size> out = {};
            if constexpr (member_list::uniform_metadata_types) {
               constexpr auto meta = member_list::all_metadata();
               for (const auto& item : member_list::all_entry_details()) {
                  //out[item.flag_index] = meta[item.type_index];
                  // 
                  // IntelliSense thinks the array element is const, yet also doesn't fail static assertions 
                  // on it being non-const, so to avoid false-positive errors in the IDE, we have to do this 
                  // unnecessary garbage:
                  //
                  std::construct_at(&out[item.flag_index], meta[item.type_index]);
               }
            }
            return out;
         }();
         static constexpr underlying_type metadata_presence_per_bit = [](){
            underlying_type out = {};
            if constexpr (member_list::uniform_metadata_types) {
               for (const auto& item : member_list::all_entry_details())
                  if (item.has_metadata)
                     out |= underlying_type(1) << item.flag_index;
            }
            return out;
         }();

      public:
         constexpr flags_mask() {}
         explicit constexpr flags_mask(underlying_type v) { return from_int(v); }
         template<typename V> requires std::is_convertible_v<V, underlying_type> constexpr static flags_mask from_int(V v) {
            flags_mask out;
            out._value = v;
            return out;
         }

         constexpr underlying_type to_int() const {
            return this->_value;
         }

         template<cs Name> requires has<Name> constexpr bool flag() const {
            return (this->_value & underlying_value_of<Name>) != 0;
         }
         template<auto... Names> requires (is_cs<decltype(Names)> && ...) constexpr bool any_set() const {
            constexpr auto all = (underlying_value_of<Names> | ...);
            return (this->_value & all) != 0;
         }
         template<auto... Names> requires (is_cs<decltype(Names)> && ...) constexpr bool all_set() const {
            constexpr auto all = (underlying_value_of<Names> | ...);
            return (this->_value & all) == all;
         }

         template<cs Name> requires has<Name> constexpr void modify_flag(bool v) {
            if (v)
               this->_value |= underlying_value_of<Name>;
            else
               this->_value &= ~underlying_value_of<Name>;
         }
         template<auto... Names> requires (is_cs<decltype(Names)> && ...) constexpr void modify_flags(bool v) {
            constexpr auto all = (underlying_value_of<Names> | ...);
            if (v)
               this->_value |= all;
            else
               this->_value &= ~all;
         }

         explicit operator underlying_type() const { return this->_value; }

         constexpr bool operator==(const flags_mask&) const = default;
         constexpr bool operator!=(const flags_mask&) const = default;

         constexpr flags_mask& operator|=(const flags_mask& o) {
            this->_value |= o._value;
            return *this;
         }
         constexpr flags_mask& operator&=(const flags_mask& o) {
            this->_value &= o._value;
            return *this;
         }
         constexpr flags_mask operator|(const flags_mask& o) const {
            auto n = *this;
            return (n |= o);
         }
         constexpr flags_mask operator&(const flags_mask& o) const {
            auto n = *this;
            return (n &= o);
         }

         template<typename Functor> requires (member_list::uniform_metadata_types) constexpr void for_each_metadata(Functor&& f) const {
            for (size_t i = min_flag_index; i <= max_flag_index; ++i) {
               auto bit = (underlying_type(1) << i);
               if (((this->_value | metadata_presence_per_bit) & bit) == 0) // bit either is not set or has no metadata
                  continue;
               (f)(metadata_per_bit[i]);
            }
         }

      public:
         template<cs Name> requires has<Name> using member_type = std::tuple_element_t<index_of<Name>, typename member_list::as_tuple>;

         static constexpr underlying_type all_set_underlying = [](){ // all defined flags are set
            underlying_type v = 0;
            for (auto item : all_underlying_values)
               v |= item;
            return v;
         }();
         static constexpr underlying_type maxed_underlying = [](){ // all bits in the range are set, including gaps
            underlying_type v = 0;
            for (size_t i = min_flag_index; i <= max_flag_index; ++i)
               v |= underlying_type(1) << i;
            return v;
         }();
         static constexpr flags_mask all_set() { return from_int(all_set_underlying); } // all defined flags are set
         static constexpr flags_mask maxed()   { return from_int(maxed_underlying);   } // all bits in the range are set, including gaps

         template<cs Name, auto Detail = impl::flags_mask::underlying_value_of::no_detail{}>
         requires (has<Name> && impl::flags_mask::underlying_value_of::detail_is_valid<flags_mask, Name, std::decay_t<decltype(Detail)>>)
         static constexpr underlying_type underlying_value_of = [](){
            using detail_param_type = std::decay_t<decltype(Detail)>;
            using namespace impl::flags_mask::underlying_value_of;

            constexpr auto base_index = member_list::template base_flag_index_of<Name>();
            if constexpr (detail_absent<detail_param_type>) {
               return underlying_type(1) << base_index;
            }
            if constexpr (detail_appropriate_for_range<detail_param_type>) {
               return underlying_type(1) << (base_index + (size_t)Detail);
            }
            cobb::unreachable();
         }();

         template<cs Name, auto Detail = impl::flags_mask::underlying_value_of::no_detail{} >
         requires (has<Name>&& impl::flags_mask::underlying_value_of::detail_is_valid<flags_mask, Name, std::decay_t<decltype(Detail)>>)
         static constexpr flags_mask value_of = [](){
            return from_int(underlying_value_of<Name, Detail>);
         }();

         template<auto... Names> requires (is_cs<decltype(Names)> && ...)
         static constexpr flags_mask from = []() {
            flags_mask out;
            out._value |= (underlying_value_of<Names> | ...);
            return out;
         }();
   };

   template<typename T> concept is_flags_mask = requires {
      typename T::original_parameters;
      requires std::is_same_v<
         std::decay_t<T>,
         cobb::tuple_unpack_t<
            flags_mask,
            typename T::original_parameters
         >
      >;
   };

   // You can using-declare this namespace for quick access to the "cs" and "reflex_flags" templates.
   namespace quick::flags_mask {
      template<size_t S> using cs = ::cobb::cs<S>;

      template<typename... Parameters> using reflex_flags = ::cobb::reflex::flags_mask<Parameters...>;

      using reflex_flags_gap = member_gap;

      template<cobb::cs Name, auto Value = undefined, auto Metadata = no_member_metadata{}>
      using reflex_member = ::cobb::reflex::member<Name, Value, Metadata>;

      template<cobb::cs Name, size_t Count, auto BaseValue = undefined, auto Metadata = no_member_metadata{}>
      using reflex_member_range = member_range<Name, Count, BaseValue, Metadata>;
   }
}

