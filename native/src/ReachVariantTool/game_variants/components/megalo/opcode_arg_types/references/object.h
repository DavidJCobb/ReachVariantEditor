#pragma once
#include "./impl/explicit_object_handle.h"
#include "./impl/reference_kind_info.h"
#include "./handle_reference.h"

namespace Megalo {
   namespace references {
      enum class object_reference_kind {
         non_member,
         member_of_player,
         member_of_object,
         member_of_team,

         biped_property_on_non_member_player,
         biped_property_on_player_player,
         biped_property_on_object_player,
         biped_property_on_team_player,

         __enum_end//
      };
      
      template<> struct kind_info_set_for<object_reference_kind> {
         using enumeration = object_reference_kind;
         //
         static constexpr const reference_type type = reference_type::object;

         static constexpr const auto data = []() {
            kind_info_set<enumeration> out = {};

            out[enumeration::non_member] = { .top_level_type = type };
            out[enumeration::member_of_object] = { .top_level_type = reference_type::object, .member_type = type };
            out[enumeration::member_of_player] = { .top_level_type = reference_type::player, .member_type = type };
            out[enumeration::member_of_team]   = { .top_level_type = reference_type::team,   .member_type = type };

            {
               const char* name = "biped";
               const auto  property_is_member_of = reference_type::player;

               out[enumeration::biped_property_on_object_player] = {
                  .top_level_type = reference_type::object,
                  .member_type    = property_is_member_of,
                  .property_name  = name,
               };
               out[enumeration::biped_property_on_player_player] = {
                  .top_level_type = reference_type::player,
                  .member_type    = property_is_member_of,
                  .property_name  = name,
               };
               out[enumeration::biped_property_on_team_player] = {
                  .top_level_type = reference_type::team,
                  .member_type    = property_is_member_of,
                  .property_name  = name,
               };
            }

            return out;
         }();

         static constexpr const kind_info* first = &data[0];
         static constexpr const size_t count = data.size();
      };
   }

}