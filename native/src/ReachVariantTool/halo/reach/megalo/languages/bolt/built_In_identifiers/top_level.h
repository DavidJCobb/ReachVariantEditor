#pragma once
#include "helpers/constexpr_tree.h"
#include "../built_in_identifier.h"

namespace halo::reach::megalo::bolt::built_in_identifiers {

   //
   // For now, this is defined the way that it is for testing purposes. In the future, 
   // we'll have constexpr objects that describe the language's various types, and 
   // opcode-specific enums will be imported into here from there. We may also use 
   // type definitions to auto-generate things like the "global" namespace, and the 
   // "static" values e.g. `team[0]`.
   //

   inline constexpr const auto top_level = cobb::constexpr_tree(
      cobb::constexpr_tree_node(built_in_identifier("current_player")),
      cobb::constexpr_tree_node(built_in_identifier("current_object")),
      cobb::constexpr_tree_node(built_in_identifier("current_team")),
      cobb::constexpr_tree_node(built_in_identifier("no_player")),
      cobb::constexpr_tree_node(built_in_identifier("no_object")),
      cobb::constexpr_tree_node(built_in_identifier("no_team")),
      cobb::constexpr_tree_node(built_in_identifier("no_widget")),
      //
      // Top-level enums:
      //
      cobb::constexpr_tree_node(built_in_identifier("orientation"),
         std::make_tuple(
            cobb::constexpr_tree_node(built_in_identifier("up_is_up")),
            cobb::constexpr_tree_node(built_in_identifier("upright")),
            cobb::constexpr_tree_node(built_in_identifier("right_is_up")),
            cobb::constexpr_tree_node(built_in_identifier("backward_is_up")),
            cobb::constexpr_tree_node(built_in_identifier("nose_down")),
            cobb::constexpr_tree_node(built_in_identifier("forward_is_up")),
            cobb::constexpr_tree_node(built_in_identifier("nose_up")),
            cobb::constexpr_tree_node(built_in_identifier("left_is_up")),
            cobb::constexpr_tree_node(built_in_identifier("down_is_up")),
            cobb::constexpr_tree_node(built_in_identifier("upside_down"))//,
         )
      ),
      //
      // Top-level namespaces:
      //
      cobb::constexpr_tree_node(built_in_identifier("game"),
         std::make_tuple(
            cobb::constexpr_tree_node(built_in_identifier("betrayal_booting")),
            cobb::constexpr_tree_node(built_in_identifier("betrayal_penalty")),
            cobb::constexpr_tree_node(built_in_identifier("current_round")),
            cobb::constexpr_tree_node(built_in_identifier("grace_period_time")),
            cobb::constexpr_tree_node(built_in_identifier("grace_period_timer")),
            cobb::constexpr_tree_node(built_in_identifier("round_time_limit")),
            cobb::constexpr_tree_node(built_in_identifier("round_timer")),
            cobb::constexpr_tree_node(built_in_identifier("sudden_death_timer"))
         )
      ),
      //
      // Opcode-specific enums:
      //
      cobb::constexpr_tree_node(built_in_identifier("none")),
      
      // Attachment reference frame
      cobb::constexpr_tree_node(built_in_identifier("absolute")),
      cobb::constexpr_tree_node(built_in_identifier("relative")),

      // Shape
      cobb::constexpr_tree_node(built_in_identifier("none")),
      cobb::constexpr_tree_node(built_in_identifier("box")),
      cobb::constexpr_tree_node(built_in_identifier("cylinder")),
      cobb::constexpr_tree_node(built_in_identifier("sphere")),

      // Waypoint priority
      cobb::constexpr_tree_node(built_in_identifier("blink")),
      cobb::constexpr_tree_node(built_in_identifier("high")),
      cobb::constexpr_tree_node(built_in_identifier("low")),
      cobb::constexpr_tree_node(built_in_identifier("normal")),

      // Weapon slot (read)
      cobb::constexpr_tree_node(built_in_identifier("primary")),
      cobb::constexpr_tree_node(built_in_identifier("secondary")),

      // Weapon slot (write)
      cobb::constexpr_tree_node(built_in_identifier("primary")),
      cobb::constexpr_tree_node(built_in_identifier("secondary")),
      cobb::constexpr_tree_node(built_in_identifier("force"))//,
   );
}