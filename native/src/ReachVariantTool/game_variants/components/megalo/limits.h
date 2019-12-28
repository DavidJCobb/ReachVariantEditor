#pragma once

namespace Megalo {
   namespace Limits {
      constexpr int max_actions = 1024;
      constexpr int max_conditions = 512;
      constexpr int max_engine_sounds = 95;
      constexpr int max_incident_types = 1024;
      constexpr int max_object_types = 2048;
      constexpr int max_script_labels = 16;
      constexpr int max_script_options = 16;
      constexpr int max_script_stats = 4;
      constexpr int max_script_traits = 16;
      constexpr int max_script_widgets = 4;
      constexpr int max_string_ids = 256;
      constexpr int max_triggers = 320;
      constexpr int max_variant_strings = 112;
   }
   enum class const_team : int8_t {
      none = -1,
      team_1,
      team_2,
      team_3,
      team_4,
      team_5,
      team_6,
      team_7,
      team_8,
      neutral,
      //
      _count = 10,
   };
}