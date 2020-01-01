#include "mp_object_names.h"
#include <stdexcept>

namespace {
   MPObjectNameDefinition _instances[] = {
      MPObjectNameDefinition("mp_boneyard_a_idle_start", "mp_boneyard_a_idle_start"),
      MPObjectNameDefinition("mp_boneyard_a_fly_in", "mp_boneyard_a_fly_in"),
      MPObjectNameDefinition("mp_boneyard_a_idle_mid", "mp_boneyard_a_idle_mid"),
      MPObjectNameDefinition("mp_boneyard_a_fly_out", "mp_boneyard_a_fly_out"),
      MPObjectNameDefinition("mp_boneyard_b_fly_in", "mp_boneyard_b_fly_in"),
      MPObjectNameDefinition("mp_boneyard_b_idle_mid", "mp_boneyard_b_idle_mid"),
      MPObjectNameDefinition("mp_boneyard_b_fly_out", "mp_boneyard_b_fly_out"),
      MPObjectNameDefinition("mp_boneyard_b_idle_start", "mp_boneyard_b_idle_start"),
      MPObjectNameDefinition("mp_boneyard_a_leave1", "mp_boneyard_a_leave1"),
      MPObjectNameDefinition("mp_boneyard_b_leave1", "mp_boneyard_b_leave1"),
      MPObjectNameDefinition("mp_boneyard_b_pickup", "mp_boneyard_b_pickup"),
      MPObjectNameDefinition("mp_boneyard_b_idle_pickup", "mp_boneyard_b_idle_pickup"),
      MPObjectNameDefinition("mp_boneyard_a", "mp_boneyard_a"),
      MPObjectNameDefinition("mp_boneyard_b", "mp_boneyard_b"),
      MPObjectNameDefinition("Default",          "default"),
      MPObjectNameDefinition("Spartan (Carter)", "carter"),
      MPObjectNameDefinition("Spartan (Jun)",    "jun"),
      MPObjectNameDefinition("Spartan (Female)", "female"),
      MPObjectNameDefinition("Spartan (Male)",   "male"),
      MPObjectNameDefinition("Spartan (Emile)",  "emile"),
      MPObjectNameDefinition("player_skull", "player_skull"),
      MPObjectNameDefinition("Spartan (Kat)",    "kat"),
      MPObjectNameDefinition("Elite (Minor)",    "minor"),
      MPObjectNameDefinition("Elite (Officer)",  "officer"),
      MPObjectNameDefinition("Elite (Ultra)",    "ultra"),
      MPObjectNameDefinition("Elite (Space)",    "space"),
      MPObjectNameDefinition("Elite (Spec Ops)", "spec_ops"),
      MPObjectNameDefinition("Elite (General)",  "general"),
      MPObjectNameDefinition("Elite (Zealot)",   "zealot"),
      MPObjectNameDefinition("mp", "mp"),
      MPObjectNameDefinition("jetpack", "jetpack"),
      MPObjectNameDefinition("Warthog (Gauss)",           "gauss"),
      MPObjectNameDefinition("Warthog (Troop Transport)", "troop"),
      MPObjectNameDefinition("Warthog (Rocket)",          "rocket"),
      MPObjectNameDefinition("fr", "fr"),
      MPObjectNameDefinition("pl", "pl"),
      MPObjectNameDefinition("35_spire_fp", "35_spire_fp"),
      MPObjectNameDefinition("mp_spire_fp", "mp_spire_fp"),
   };
}
MPObjectNameList::MPObjectNameList() {
   this->list  = _instances;
   this->count = std::extent<decltype(_instances)>::value;
}
const MPObjectNameDefinition& MPObjectNameList::operator[](int i) const {
   if (i < 0 || i >= this->size())
      throw std::out_of_range("");
   return this->list[i];
}
