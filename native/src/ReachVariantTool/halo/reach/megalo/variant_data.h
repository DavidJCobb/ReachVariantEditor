#pragma once
#include <array>
#include "halo/util/indexed_list.h"
#include "halo/util/refcount.h"
#include "halo/loc_string_table.h"
#include "../game_variant.h"

#include "./strings.h"

#include "../arena_parameters.h"
#include "../map_permissions.h"
#include "../trait_set.h"

namespace halo::reach {
   namespace megalo {
      class forge_label;
      class game_option;
      class game_stat;
      class hud_widget;
      class player_trait_set;
   }

   class megalo_variant_data : public game_variant_data {
      public:

         //
         // Be aware that members here are not grouped or listed in the order they're read in.
         //

         struct {
            struct {
               uint32_t encoding;
               uint32_t engine;
            } version; // read before the UGC header??
            struct {
               loc_string_table<1, 0x180> name;
               loc_string_table<1, 0xC00> description;
               loc_string_table<1, 0x180> category;
            } localized_info;
            bitnumber<5, int8_t, bitnumber_params<int8_t>{ .offset = 1 }> engine_category = -1;
            bitnumber<5, int8_t, bitnumber_params<int8_t>{ .offset = 1 }> engine_icon     = -1;
            arena_parameters arena_parameters;
            map_permissions  map_permissions;
         } metadata;
         struct {
            megalo::string_table strings;
            megalo::string_ref   generic_name; // more specific than the category name but less specific than the name; could possibly also be a "fallback name"
            
            util::indexed_list<megalo::forge_label, 16>      forge_labels;
            util::indexed_list<megalo::game_option, 16>      options;
            util::indexed_list<megalo::game_stat, 4>         stats;
            util::indexed_list<megalo::player_trait_set, 16> traits;
            util::indexed_list<megalo::hud_widget, 4>        widgets;

            megalo::object_type_mask used_object_types;

            util::indexed_list<megalo::trigger, 320> triggers;
            megalo::trigger_entry_points entry_points;
            struct {
               Megalo::VariableDeclarationSet global = Megalo::VariableDeclarationSet(Megalo::variable_scope::global);
               Megalo::VariableDeclarationSet player = Megalo::VariableDeclarationSet(Megalo::variable_scope::player);
               Megalo::VariableDeclarationSet object = Megalo::VariableDeclarationSet(Megalo::variable_scope::object);
               Megalo::VariableDeclarationSet team = Megalo::VariableDeclarationSet(Megalo::variable_scope::team);
            } variables;
         } script;
         struct {
            bitbool fireteams_enabled;
            bitbool symmetric;
            bytenumber<uint16_t> score_to_win;
         } options_mp;
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
         struct {
            tu1_data tu1;
         } title_update_data;
         struct {
            bitnumber<2, uint8_t> flags          = 0;
            bitnumber<2, uint8_t> edit_mode_type = 0;
            bitnumber<6, uint8_t> respawn_time   = 0; // if this is greater than 60, it gets reset to 5 on load
            trait_set editorTraits;
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
   };
}
