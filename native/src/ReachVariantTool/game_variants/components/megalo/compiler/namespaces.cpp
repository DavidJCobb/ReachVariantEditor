#include "namespaces.h"
#include "enums.h"
#include "../opcode_arg_types/variables/all_core.h"
#include "../opcode_arg_types/variables/player_or_group.h"
#include "../opcode_arg_types/widget_related.h"
#include "../../../helpers/qt/string.h"

namespace Megalo {
   namespace Script {
      /*static*/ NamespaceMember NamespaceMember::make_enum_member(Namespace& ns, Enum& e) {
         auto member = make_bare_member(e.name.toLatin1(), OpcodeArgValueScalar::typeinfo);
         member.enumeration = &e;
         return member;
      }
      bool NamespaceMember::has_index() const noexcept {
         if (this->scope)
            return this->scope->has_index();
         return false;
      }
      bool NamespaceMember::is_read_only() const noexcept {
         if (this->scope) {
            return this->scope->is_readonly();
         }
         if (this->which) {
            return this->which->is_read_only();
         }
         return true; // bare members are always read-only
      }
      //
      const NamespaceMember* Namespace::get_member(const QString& name) const noexcept {
         for (auto& member : this->members)
            if (cobb::qt::stricmp(name, member.name) == 0)
               return &member;
         return nullptr;
      }

      namespace namespaces {
         std::array<Namespace*, 4> list = { &unnamed, &global, &game, &enums };
         Namespace* get_by_name(const QString& name) {
            for (auto& ns : namespaces::list)
               if (cobb::qt::stricmp(name, ns->name) == 0)
                  return ns;
            return nullptr;
         }
         //
         Namespace unnamed = Namespace("", true, false, {
            NamespaceMember::make_which_member("no_object",         OpcodeArgValueObject::typeinfo, variable_which_values::object::no_object),
            NamespaceMember::make_which_member("no_player",         OpcodeArgValuePlayer::typeinfo, variable_which_values::player::no_player),
            NamespaceMember::make_which_member("no_team",           OpcodeArgValueTeam::typeinfo,   variable_which_values::team::no_team),
            NamespaceMember::make_which_member("current_object",    OpcodeArgValueObject::typeinfo, variable_which_values::object::current),
            NamespaceMember::make_which_member("current_player",    OpcodeArgValuePlayer::typeinfo, variable_which_values::player::current),
            NamespaceMember::make_which_member("current_team",      OpcodeArgValueTeam::typeinfo,   variable_which_values::team::current),
            NamespaceMember::make_which_member("neutral_team",      OpcodeArgValueTeam::typeinfo,   variable_which_values::team::neutral_team),
            NamespaceMember::make_which_member("hud_target_object", OpcodeArgValueObject::typeinfo, variable_which_values::object::hud_target),
            NamespaceMember::make_which_member("hud_target_player", OpcodeArgValuePlayer::typeinfo, variable_which_values::player::hud_target),
            NamespaceMember::make_which_member("hud_player",        OpcodeArgValuePlayer::typeinfo, variable_which_values::player::hud),
            NamespaceMember::make_which_member("killed_object",     OpcodeArgValueObject::typeinfo, variable_which_values::object::killed),
            NamespaceMember::make_which_member("killer_object",     OpcodeArgValueObject::typeinfo, variable_which_values::object::killer),
            NamespaceMember::make_which_member("killer_player",     OpcodeArgValuePlayer::typeinfo, variable_which_values::player::killer),
            NamespaceMember::make_which_member("unk_14_team",       OpcodeArgValueTeam::typeinfo,   variable_which_values::team::unk_14),
            NamespaceMember::make_which_member("unk_15_team",       OpcodeArgValueTeam::typeinfo,   variable_which_values::team::unk_15),
            //
            NamespaceMember::make_scope_member("script_option", OpcodeArgValueScalar::typeinfo, Megalo::variable_scope_indicators::number::option),
            //
            NamespaceMember::make_bare_member("all_players", OpcodeArgValuePlayerOrGroup::typeinfo),
            //
            NamespaceMember::make_bare_member("no_widget", OpcodeArgValueWidget::typeinfo),
         });
         Namespace global = Namespace("global", false, true);
         Namespace game = Namespace("game", false, false, {
            NamespaceMember::make_scope_member("betrayal_booting",        OpcodeArgValueScalar::typeinfo, Megalo::variable_scope_indicators::number::betrayal_booting),
            NamespaceMember::make_scope_member("betrayal_penalty",        OpcodeArgValueScalar::typeinfo, Megalo::variable_scope_indicators::number::betrayal_penalty),
            NamespaceMember::make_scope_member("current_round",           OpcodeArgValueScalar::typeinfo, Megalo::variable_scope_indicators::number::current_round),
            NamespaceMember::make_scope_member("death_event_damage_type", OpcodeArgValueScalar::typeinfo, Megalo::variable_scope_indicators::number::death_event_damage_type),
            NamespaceMember::make_scope_member("friendly_fire",           OpcodeArgValueScalar::typeinfo, Megalo::variable_scope_indicators::number::friendly_fire),
            NamespaceMember::make_scope_member("grace_period",            OpcodeArgValueScalar::typeinfo, Megalo::variable_scope_indicators::number::grace_period),
            NamespaceMember::make_scope_member("grenades_on_map",         OpcodeArgValueScalar::typeinfo, Megalo::variable_scope_indicators::number::grenades_on_map),
            NamespaceMember::make_scope_member("indestructible_vehicles", OpcodeArgValueScalar::typeinfo, Megalo::variable_scope_indicators::number::indestructible_vehicles),
            NamespaceMember::make_scope_member("lives_per_round",         OpcodeArgValueScalar::typeinfo, Megalo::variable_scope_indicators::number::lives_per_round),
            NamespaceMember::make_scope_member("loadout_cam_time",        OpcodeArgValueScalar::typeinfo, Megalo::variable_scope_indicators::number::loadout_cam_time),
            NamespaceMember::make_scope_member("perfection_enabled",      OpcodeArgValueScalar::typeinfo, Megalo::variable_scope_indicators::number::perfection_enabled),
            NamespaceMember::make_scope_member("powerup_duration_red",    OpcodeArgValueScalar::typeinfo, Megalo::variable_scope_indicators::number::powerup_duration_r),
            NamespaceMember::make_scope_member("powerup_duration_blue",   OpcodeArgValueScalar::typeinfo, Megalo::variable_scope_indicators::number::powerup_duration_b),
            NamespaceMember::make_scope_member("powerup_duration_yellow", OpcodeArgValueScalar::typeinfo, Megalo::variable_scope_indicators::number::powerup_duration_y),
            NamespaceMember::make_scope_member("respawn_growth",          OpcodeArgValueScalar::typeinfo, Megalo::variable_scope_indicators::number::respawn_growth),
            NamespaceMember::make_scope_member("respawn_time",            OpcodeArgValueScalar::typeinfo, Megalo::variable_scope_indicators::number::respawn_time),
            NamespaceMember::make_scope_member("respawn_traits_duration", OpcodeArgValueScalar::typeinfo, Megalo::variable_scope_indicators::number::respawn_traits_time),
            NamespaceMember::make_scope_member("round_limit",             OpcodeArgValueScalar::typeinfo, Megalo::variable_scope_indicators::number::round_limit),
            NamespaceMember::make_scope_member("round_time_limit",        OpcodeArgValueScalar::typeinfo, Megalo::variable_scope_indicators::number::round_time_limit),
            NamespaceMember::make_scope_member("rounds_to_win",           OpcodeArgValueScalar::typeinfo, Megalo::variable_scope_indicators::number::rounds_to_win),
            NamespaceMember::make_scope_member("score_to_win",            OpcodeArgValueScalar::typeinfo, Megalo::variable_scope_indicators::number::score_to_win),
            NamespaceMember::make_scope_member("proximity_voice",         OpcodeArgValueScalar::typeinfo, Megalo::variable_scope_indicators::number::proximity_voice),
            NamespaceMember::make_scope_member("dont_team_restrict_chat", OpcodeArgValueScalar::typeinfo, Megalo::variable_scope_indicators::number::dont_team_restrict_chat),
            NamespaceMember::make_scope_member("dead_players_can_talk",   OpcodeArgValueScalar::typeinfo, Megalo::variable_scope_indicators::number::dead_players_can_talk),
            NamespaceMember::make_scope_member("sudden_death_time",       OpcodeArgValueScalar::typeinfo, Megalo::variable_scope_indicators::number::sudden_death_time),
            NamespaceMember::make_scope_member("suicide_penalty",         OpcodeArgValueScalar::typeinfo, Megalo::variable_scope_indicators::number::suicide_penalty),
            NamespaceMember::make_scope_member("symmetry",                OpcodeArgValueScalar::typeinfo, Megalo::variable_scope_indicators::number::symmetry),
            NamespaceMember::make_scope_member("symmetry_getter",         OpcodeArgValueScalar::typeinfo, Megalo::variable_scope_indicators::number::symmetry_get),
            NamespaceMember::make_scope_member("team_lives_per_round",    OpcodeArgValueScalar::typeinfo, Megalo::variable_scope_indicators::number::team_lives_per_round),
            NamespaceMember::make_scope_member("teams_enabled",           OpcodeArgValueScalar::typeinfo, Megalo::variable_scope_indicators::number::teams_enabled),
            NamespaceMember::make_scope_member("fireteams_enabled",       OpcodeArgValueScalar::typeinfo, Megalo::variable_scope_indicators::number::fireteams_enabled),
            //
            NamespaceMember::make_scope_member("round_timer",             OpcodeArgValueTimer::typeinfo,  Megalo::variable_scope_indicators::timer::round_timer),
            NamespaceMember::make_scope_member("sudden_death_timer",      OpcodeArgValueTimer::typeinfo,  Megalo::variable_scope_indicators::timer::sudden_death_timer),
            NamespaceMember::make_scope_member("grace_period_timer",      OpcodeArgValueTimer::typeinfo,  Megalo::variable_scope_indicators::timer::grace_period_timer),
         });
         //
         namespace {
            namespace _enum_definitions {
               Enum damage_reporting_modifier = Enum("damage_reporting_modifier", {
                  "none",
                  "pummel",
                  "assassination",
                  "splatter",
                  "sticky",
                  "headshot",
               });
               Enum damage_reporting_type = Enum("damage_reporting_type", {
                  "unknown",    // UI labels this "The Guardians"
                  "guardians",  // UI labels this "The Guardians"
                  "script",     // UI labels this "The Guardians"
                  "ai_suicide", // map file doesn't appear to have a string for this
                  "magnum",
                  "assault_rifle", // 5
                  "dmr", // "marksman rifle" internally
                  "shotgun",
                  "sniper_rifle",
                  "rocket_launcher",
                  "spartan_laser", // 10
                  "frag_grenade",
                  "grenade_launcher",
                  "plasma_pistol",
                  "needler",
                  "plasma_rifle", // 15
                  "plasma_repeater",
                  "needle_rifle",
                  "spiker",
                  "plasma_launcher",
                  "gravity_hammer", // 20 // includes golf club damage
                  "energy_sword",
                  "plasma_grenade",
                  "concussion_rifle",
                  "ghost", // cannons or splatters
                  "revenant", // mortar or splatters
                  "revenant_gunner", // passengers
                  "wraith", // mortar or splatters
                  "wraith_anti_infantry",
                  "banshee", // cannons or splatters
                  "banshee_bomb", // 30
                  "seraph",
                  "mongoose", // splatters
                  "warthog", // splatters
                  "warthog_turret_chaingun",
                  "warthog_turret_gauss", // 35
                  "warthog_turret_rocket",
                  "scorpion", // cannon or splatters
                  "scorpion_anti_infantry",
                  "falcon", // front chaingun (if any) or splatters
                  "falcon_gunner", // 40 // passengers
                  "fall_damage",
                  "collision_damage", // any collision damage not attributable to a specific cause
                  "melee_generic",
                  "explosion_generic",
                  "explosion_birthday_party", // 45
                  "melee_flag",
                  "melee_bomb",
                  "explosion_bomb",
                  "melee_skull",
                  "teleporter", // 50 // ?
                  "shifted_blame", // "transfer_damage" internally; no one seems to know what it actually is
                  "armor_lock_crush",
                  "target_locator",
                  "machine_gun_turret", // mounted or non-mounted
                  "plasma_cannon", // mounted or non-mounted
                  "plasma_mortar", // pre-Halo-3 leftover?
                  "plasma_turret", // this is NOT for the mounted, detachable plasma cannon
                  "shade_turret",
                  "sabre",        // "excavator" internally, though i believe Halo 3 used that name internally for the prowler
                  "smg",          // 60 // leftover
                  "carbine",      // leftover
                  "battle_rifle", // leftover
                  "focus_rifle",  // "beam_rifle" internally // monitors' default "beam" weapon would count as this, if you managed to damage someone with it
                  "fuel_rod_gun",
                  "missile_pod",  // 65 // leftover
                  "brute_shot",   // leftover
                  "flamethrower", // leftover
                  "sentinel_gun", // how does this differ from "sentinel_beam"?
                  "spike_grenade",    // leftover // "claymore_grenade" internally; there are tutorial strings in the map files which identify it as the spike grenade
                  "firebomb_grenade", // leftover
                  "elephant_turret",  // leftover
                  "spectre_driver",   // leftover? // there are no strings in the map files with any relation to "spectre"
                  "spectre_gunner",   // leftover?
                  "tank",    // unknown
                  "chopper", // leftover
                  "hornet",  // leftover
                  "mantis",  // scrapped in Reach; implemented in Halo 4?
                  "prowler", // leftover
                  "sentinel_beam", // based on list placement, this is likely the Halo 3 weapon, except that Halo 3 internally called it "sentinel_gun" which is another value in this list
                  "sentinel_rpg",  // ?
                  "tripmine", // ?
               });
            }
         }
         Namespace enums = Namespace("enums", false, false, {
            NamespaceMember::make_enum_member(enums, _enum_definitions::damage_reporting_modifier),
            NamespaceMember::make_enum_member(enums, _enum_definitions::damage_reporting_type),
         });
      }
   }
}