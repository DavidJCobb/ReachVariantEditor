#pragma once
#include <bit>
#include "helpers/reflex/enumeration.h"
#include "halo/reach/bitstreams.fwd.h"
#include "../operand_typeinfo.h"
#include "../operand.h"

#include "halo/reach/megalo/limits.h"

namespace halo::reach::megalo::operands {
   class sound : public operand {
      public:
         static constexpr operand_typeinfo typeinfo = {
            .internal_name = "sound",
         };

      public:
         using value_type = cobb::reflex::enumeration<
            cobb::reflex::member<cobb::cs("announce_slayer")>,
            cobb::reflex::member<cobb::cs("announce_ctf")>,
            cobb::reflex::member<cobb::cs("announce_ctf_captured")>,
            cobb::reflex::member<cobb::cs("announce_ctf_dropped")>,
            cobb::reflex::member<cobb::cs("announce_ctf_recovered")>,
            cobb::reflex::member<cobb::cs("announce_ctf_reset")>,
            cobb::reflex::member<cobb::cs("announce_ctf_stolen")>,
            cobb::reflex::member<cobb::cs("announce_ctf_taken")>,
            cobb::reflex::member<cobb::cs("announce_vip")>,
            cobb::reflex::member<cobb::cs("announce_vip_new")>,
            cobb::reflex::member<cobb::cs("announce_vip_killed")>,
            cobb::reflex::member<cobb::cs("announce_juggernaut")>,
            cobb::reflex::member<cobb::cs("announce_juggernaut_new")>,
            cobb::reflex::member<cobb::cs("announce_territories")>,
            cobb::reflex::member<cobb::cs("announce_territories_captured")>,
            cobb::reflex::member<cobb::cs("announce_territories_lost")>,
            cobb::reflex::member<cobb::cs("announce_assault")>,
            cobb::reflex::member<cobb::cs("announce_assault_armed")>,
            cobb::reflex::member<cobb::cs("announce_assault_detonated")>,
            cobb::reflex::member<cobb::cs("announce_assault_disarmed")>,
            cobb::reflex::member<cobb::cs("announce_assault_dropped")>,
            cobb::reflex::member<cobb::cs("announce_assault_reset")>,
            cobb::reflex::member<cobb::cs("announce_assault_returned")>,
            cobb::reflex::member<cobb::cs("announce_assault_taken")>,
            cobb::reflex::member<cobb::cs("announce_infection")>,
            cobb::reflex::member<cobb::cs("announce_infection_infected")>,
            cobb::reflex::member<cobb::cs("announce_infection_last_man")>,
            cobb::reflex::member<cobb::cs("announce_infection_new_zombie")>,
            cobb::reflex::member<cobb::cs("announce_oddball")>,
            cobb::reflex::member<cobb::cs("announce_oddball_play")>,
            cobb::reflex::member<cobb::cs("announce_oddball_taken")>,
            cobb::reflex::member<cobb::cs("announce_oddball_dropped")>,
            cobb::reflex::member<cobb::cs("announce_oddball_reset")>,
            cobb::reflex::member<cobb::cs("announce_koth")>,
            cobb::reflex::member<cobb::cs("announce_koth_controlled")>,
            cobb::reflex::member<cobb::cs("announce_koth_contested")>,
            cobb::reflex::member<cobb::cs("announce_koth_moved")>,
            cobb::reflex::member<cobb::cs("announce_headhunter")>,
            cobb::reflex::member<cobb::cs("announce_stockpile")>,
            cobb::reflex::member<cobb::cs("announce_race")>,
            cobb::reflex::member<cobb::cs("announce_defense")>,
            cobb::reflex::member<cobb::cs("announce_offense")>,
            cobb::reflex::member<cobb::cs("announce_destination_moved")>,
            cobb::reflex::member<cobb::cs("announce_assault_armed_2")>,
            cobb::reflex::member<cobb::cs("announce_assault_armed_3")>,
            cobb::reflex::member<cobb::cs("announce_assault_disarmed_2")>,
            cobb::reflex::member<cobb::cs("announce_assault_disarmed_3")>,
            cobb::reflex::member<cobb::cs("announce_sudden_death")>,
            cobb::reflex::member<cobb::cs("announce_game_over")>,
            cobb::reflex::member<cobb::cs("inv_boneyard_vo_covenant_p3_loss")>,
            cobb::reflex::member<cobb::cs("inv_boneyard_vo_covenant_p1_loss")>,
            cobb::reflex::member<cobb::cs("inv_boneyard_vo_covenant_p1_intro")>,
            cobb::reflex::member<cobb::cs("inv_boneyard_vo_covenant_p1_win")>,
            cobb::reflex::member<cobb::cs("inv_boneyard_vo_covenant_p2_loss")>,
            cobb::reflex::member<cobb::cs("inv_boneyard_vo_covenant_p2_win")>,
            cobb::reflex::member<cobb::cs("inv_boneyard_vo_covenant_p3_win")>,
            cobb::reflex::member<cobb::cs("inv_boneyard_vo_covenant_win")>,
            cobb::reflex::member<cobb::cs("inv_boneyard_vo_spartan_p3_loss")>,
            cobb::reflex::member<cobb::cs("inv_boneyard_vo_spartan_p1_intro")>,
            cobb::reflex::member<cobb::cs("inv_boneyard_vo_spartan_p1_win")>,
            cobb::reflex::member<cobb::cs("inv_boneyard_vo_spartan_p2_intro")>,
            cobb::reflex::member<cobb::cs("inv_boneyard_vo_spartan_p2_win")>,
            cobb::reflex::member<cobb::cs("inv_boneyard_vo_spartan_p3_intro")>,
            cobb::reflex::member<cobb::cs("inv_boneyard_vo_spartan_p3_win")>,
            cobb::reflex::member<cobb::cs("inv_spire_vo_covenant_loss")>,
            cobb::reflex::member<cobb::cs("inv_spire_vo_covenant_p1_win")>,
            cobb::reflex::member<cobb::cs("inv_spire_vo_covenant_p1_intro")>,
            cobb::reflex::member<cobb::cs("inv_spire_vo_covenant_p2_intro")>,
            cobb::reflex::member<cobb::cs("inv_spire_vo_covenant_p2_win")>,
            cobb::reflex::member<cobb::cs("inv_spire_vo_covenant_p3_intro")>,
            cobb::reflex::member<cobb::cs("inv_spire_vo_covenant_p3_win")>,
            cobb::reflex::member<cobb::cs("inv_spire_vo_spartan_p3_loss")>,
            cobb::reflex::member<cobb::cs("inv_spire_vo_spartan_p1_loss")>,
            cobb::reflex::member<cobb::cs("inv_spire_vo_spartan_p1_extra")>,
            cobb::reflex::member<cobb::cs("inv_spire_vo_spartan_p1_intro")>,
            cobb::reflex::member<cobb::cs("inv_spire_vo_spartan_p1_win")>,
            cobb::reflex::member<cobb::cs("inv_spire_vo_spartan_p2_loss")>,
            cobb::reflex::member<cobb::cs("inv_spire_vo_spartan_p2_win")>,
            cobb::reflex::member<cobb::cs("inv_spire_vo_spartan_p3_win")>,
            cobb::reflex::member<cobb::cs("inv_spire_vo_spartan_win")>,
            cobb::reflex::member<cobb::cs("inv_boneyard_vo_spartan_p4_intro")>,
            cobb::reflex::member<cobb::cs("inv_spire_vo_covenant_p4_intro")>,
            cobb::reflex::member<cobb::cs("inv_cue_covenant_win_big")>,
            cobb::reflex::member<cobb::cs("inv_cue_covenant_win_1")>,
            cobb::reflex::member<cobb::cs("inv_cue_covenant_win_2")>,
            cobb::reflex::member<cobb::cs("unused_87")>,
            cobb::reflex::member<cobb::cs("inv_cue_spartan_win_big")>,
            cobb::reflex::member<cobb::cs("inv_cue_spartan_win_1")>,
            cobb::reflex::member<cobb::cs("inv_cue_spartan_win_2")>,
            cobb::reflex::member<cobb::cs("boneyard_generator_power_down")>,
            cobb::reflex::member<cobb::cs("firefight_lives_added")>,
            cobb::reflex::member<cobb::cs("timer_beep")>,
            cobb::reflex::member<cobb::cs("announce_a_under_attack")>,
            cobb::reflex::member<cobb::cs("announce_b_under_attack")>,
            cobb::reflex::member<cobb::cs("announce_c_under_attack")>//,
         >;

         using index_type = bitnumber<
            std::bit_width(limits::engine_sounds),
            value_type,
            bitnumber_params<value_type>{
               .initial  = -1,
               .offset   = 1,
            }
         >;

      public:
         index_type value;

         virtual void read(bitreader& stream) override;
   };
}

