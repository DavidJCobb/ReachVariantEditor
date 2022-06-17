#pragma once
#include <array>
#include "halo/util/indexed_list.h"
#include "halo/util/refcount.h"
#include "halo/loc_string_table.h"
#include "../game_variant.h"

#include "./limits.h"

#include "./forge_label.h"
#include "./game_option.h"
#include "./game_stat.h"
#include "./hud_widget.h"
#include "./object_type_mask.h"
#include "./option_toggles.h"
#include "./player_trait_set.h"
#include "./strings.h"
#include "./trigger.h"
#include "./trigger_entry_points.h"
#include "./tu1_data.h"
#include "./variable_declarations.h"

#include "../arena_parameters.h"
#include "../map_permissions.h"
#include "../trait_set.h"

#include "./opcodes/action.h"
#include "./opcodes/condition.h"

namespace halo::reach {
   class megalo_variant_data : public game_variant_data {
      public:

         //
         // Be aware that members here are not grouped or listed in the order they're read in.
         //

         struct {
            struct {
               bytenumber<uint32_t> encoding;
               bytenumber<uint32_t> engine;
            } version; // read before the UGC header??
            struct {
               loc_string_table<1, 0x180> name;
               loc_string_table<1, 0xC00> description;
               loc_string_table<1, 0x180> category;
            } localized_info;
            bitnumber<5, int8_t, bitnumber_params<int8_t>{ .offset = 1 }> engine_category = -1;
            bitnumber<5, int8_t, bitnumber_params<int8_t>{ .offset = 1 }> engine_icon     = -1;
         } metadata;
         //
         arena_parameters arena_parameters;
         map_permissions  map_permissions;
         struct {
            bitbool fireteams_enabled;
            bitbool symmetric;
            bytenumber<uint16_t> score_to_win;
         } options_mp;
         //
         struct {
            megalo::string_table strings;
            megalo::string_ref   generic_name; // more specific than the category name but less specific than the name; could possibly also be a "fallback name"
            
            util::indexed_list<megalo::forge_label,      megalo::limits::forge_labels>   forge_labels;
            util::indexed_list<megalo::game_option,      megalo::limits::script_options> options;
            util::indexed_list<megalo::game_stat,        megalo::limits::script_stats>   stats;
            util::indexed_list<megalo::player_trait_set, megalo::limits::script_traits>  traits;
            util::indexed_list<megalo::hud_widget,       megalo::limits::script_widgets> widgets;

            megalo::object_type_mask used_object_types;

            util::indexed_list<megalo::trigger, megalo::limits::triggers> triggers;
            megalo::trigger_entry_points entry_points;
            struct {
               megalo::variable_declaration_set<megalo::variable_scope::global> global;
               megalo::variable_declaration_set<megalo::variable_scope::player> player;
               megalo::variable_declaration_set<megalo::variable_scope::object> object;
               megalo::variable_declaration_set<megalo::variable_scope::team>   team;
            } variables;
         } script;
         struct {
            struct {
               engine_option_toggles disabled;
               engine_option_toggles hidden;
            } engine;
            struct {
               megalo_option_toggles disabled;
               megalo_option_toggles hidden;
            } megalo;
         } option_toggles;
         //
         struct {
            tu1_data tu1;
         } title_update_data;
         //
         struct {
            bitnumber<2, uint8_t> flags          = 0;
            bitnumber<2, uint8_t> edit_mode_type = 0;
            bitnumber<6, uint8_t> respawn_time   = 0; // if this is greater than 60, it gets reset to 5 on load
            trait_set editor_traits;
         } forge_data;

      protected:
         struct {
            //
            // Raw data pulled while loading; it will be transferred to its proper place when 
            // possible. It's retained as a member to aid in debugging, should a load process 
            // error be raised.
            //
            std::vector<megalo::condition> conditions;
            std::vector<megalo::action>    actions;
         } raw;

      public:
         virtual void read(bitreader&) override;
         virtual void write(bitwriter&) const override;
   };
}
