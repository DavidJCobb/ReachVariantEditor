#pragma once
#include <cstdint>
#include <memory>
#include <vector>
#include "halo/util/refcount.h"
#include "halo/reach/bitstreams.fwd.h"
#include "./opcodes/action.h"
#include "./opcodes/condition.h"
#include "./forge_label.h"

namespace halo::reach::megalo {
   enum class trigger_block_type : uint8_t {
      normal, // not a loop, but trigger can still access loop iterators from containing triggers?
      for_each_player,
      for_each_player_randomly, // iterates over all players in a random order; use for things like randomly picking initial zombies in infection
      for_each_team,
      for_each_object, // every MP object?
      for_each_object_with_label, // always sorted by ascending spawn sequence
   };
   enum class trigger_entry_type : uint8_t {
      normal,
      subroutine, // preserves iterator values from outer loops
      on_init,
      on_local_init, // unverified; not used in Bungie gametypes
      on_host_migration, // host migrations and double host migrations
      on_object_death,
      local,
      pregame,
      // Halo 4: incident
   };

   class trigger {
      protected:
         struct {
            //
            // Raw data loaded from a game variant file. Reach uses a struct-of-arrays approach to 
            // serialize trigger data, writing all conditions followed by all actions and then headers 
            // for triggers, with each header identifying the start index and count of each opcode type. 
            // This represents raw struct-of-arrays data; the (opcodes) vector is generated post-load by 
            // the (extract_opcodes) member function.
            //
            // The "raw" data is generally only meaningful during the load and save processes, and should 
            // not be checked at any other time.
            //
            condition_index condition_start = -1;
            condition_count condition_count =  0;
            action_index    action_start    = -1;
            action_count    action_count    =  0;
         } load_state;
         
      public:
         bitnumber<3, trigger_block_type> block_type = trigger_block_type::normal;
         bitnumber<3, trigger_entry_type> entry_type = trigger_entry_type::normal;
         util::refcount_ptr<forge_label> loop_forge_label;
         //
         std::vector<std::unique_ptr<opcode>> opcodes;
         
         void read(bitreader&);
         void extract_opcodes(bitreader&, const std::vector<condition>&, const std::vector<action>&);
   };
}