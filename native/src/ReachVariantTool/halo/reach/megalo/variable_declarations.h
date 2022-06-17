#pragma once
#include <array>
#include <bitset>
#include "halo/reach/bitstreams.h"
#include "./static_team.h"
#include "./variable_scope.h"
#include "./variable_type.h"

#include "halo/reach/megalo/operands/variables/base.h"

namespace halo::reach::megalo {
   namespace impl {
      class variable_declaration {
         public:
            enum class network_priority_type {
               none, // offline
               low,  // basic
               high, // usable in UI
            };

            bitnumber<2, network_priority_type> network_priority = network_priority_type::low;
            struct {
               operands::variables::unknown_type* value = nullptr; // can't use number type directly due to circular include issues
               static_team_index team = static_team::none;
            } initial;

            void read(variable_type, bitreader&);
            void write(variable_type, bitwriter&) const;
      };
   }

   template<variable_type VT> class variable_declaration : public impl::variable_declaration {
      public:
         static constexpr variable_type type = VT;

         void read(bitreader& stream) {
            impl::variable_declaration::read(type, stream);
         }
   };

   template<variable_scope S> class variable_declaration_set {
      public:
         static constexpr variable_scope scope = S;

      protected:
         static constexpr const variable_scope_metadata scope_metadata = variable_scope_metadata_from_enum(scope);
         // using a reference type breaks MSVC's handling of constant epxressions. Not using a 
         // reference type means that
         // 
         //    &varialbe_declaration_set<S>::scope_metadata != variable_scope_metadata_from_enum(S)
         //
         // which is undesirable, so just make this private. *sigh*

      protected:
         template<variable_type V> static constexpr size_t bit_offset() {
            size_t result = 0;
            //
            if constexpr (V == variable_type::number)
               return result;
            result += scope_metadata.maximum_of_type(variable_type::number);
            //
            if constexpr (V == variable_type::object)
               return result;
            result += scope_metadata.maximum_of_type(variable_type::object);
            //
            if constexpr (V == variable_type::player)
               return result;
            result += scope_metadata.maximum_of_type(variable_type::player);
            //
            if constexpr (V == variable_type::team)
               return result;
            result += scope_metadata.maximum_of_type(variable_type::team);
            //
            if constexpr (V == variable_type::timer)
               return result;
            //
            cobb::unreachable();
         }

      public:
         template<variable_type V> using variable_list = std::array<variable_declaration<V>, scope_metadata.maximum_of_type<V>()>;

      protected:
         template<variable_type V> variable_list<V>& _list_by_type() {
            //
            // Can't use switch-cases here because they're not constexpr; even if we 
            // never follow a case, it returning a value inconsistent with our return 
            // type rather understandably causes the compiler to choke.
            //
            using enum variable_type;
            if constexpr (V == number)
               return numbers;
            else if constexpr (V == object)
               return objects;
            else if constexpr (V == player)
               return players;
            else if constexpr (V == team)
               return teams;
            else if constexpr (V == timer)
               return timers;
            else
               cobb::unreachable();
         }

         template<variable_type V> void _read_list(bitreader& stream) {
            size_t count = stream.read_bits(std::bit_width(scope_metadata.maximum_of_type(V)));
            for (size_t i = 0; i < count; ++i) {
               this->set_variable_is_defined<V>(i, true);
            }
            auto& list = this->_list_by_type<V>();
            for (size_t i = 0; i < count; ++i) {
               list[i].read(stream);
            }
         }

      protected:
         std::bitset<scope_metadata.total()> presence;
         //
         variable_list<variable_type::number> numbers;
         variable_list<variable_type::object> objects;
         variable_list<variable_type::player> players;
         variable_list<variable_type::team>   teams;
         variable_list<variable_type::timer>  timers;

      public:
         template<variable_type V> bool variable_is_defined(size_t index) const {
            return this->presence.test(bit_offset<V>() + index);
         }
         template<variable_type V> void set_variable_is_defined(size_t index, bool v) {
            auto& bs = this->presence;
            auto  bi = bit_offset<V>() + index;
            if (v)
               bs.set(bi);
            else
               bs.reset(bi);
         }

         template<variable_type V> variable_declaration<V>* get_variable_declaration(size_t index) {
            if (variable_is_defined<V>(index))
               return &this->_list_by_type<V>()[index];
            return nullptr;
         }
         template<variable_type V> const variable_declaration<V>* get_variable_declaration(size_t index) const {
            if (variable_is_defined<V>(index))
               return &this->_list_by_type<V>()[index];
            return nullptr;
         }

         void read(bitreader& stream) {
            _read_list<variable_type::number>(stream);
            _read_list<variable_type::timer>(stream);
            _read_list<variable_type::team>(stream);
            _read_list<variable_type::player>(stream);
            _read_list<variable_type::object>(stream);
         }
   };
}