// ReachVariantTool.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "game_variants/base.h"
#include "helpers/endianness.h"
#include "helpers/files.h"
#include "helpers/stream.h"
#include "formats/sha1.h"

//
// TODO:
//
//  - Bitnumber: remove the (swap) template parameter. (Requires adjustment 
//    of all bitnumber instances and typedefs that use the advanced params.)
//
//  - Investigate Firefight, Campaign, and Forge variants. Consider splitting 
//    MPVR up into multiple classes in order to allow us to load the other 
//    types.
//
//  - Change how we load blocks: we should loop over all blocks in the file, 
//    instead of assuming that CHDR and MPVR must be at the start. I think 
//    the game itself makes that assumption but I'm not sure (wouldn't be 
//    hard to hex-edit a dummy block between them and find out; maybe I 
//    should).
//
//     - We could have GameVariant store a std::vector of ReachFileBlock 
//       instances, and each instance could have a pointer to a "data" 
//       object with virtual read/save methods. You wouldn't call those 
//       directly; rather, you'd call read/save on the block, and it would 
//       call read/save on its data object (and this in turn would allow 
//       the block to do things like block length fixup after writing).
//
// ==========================================================================
//
//  - Begin testing to identify further unknown information in Reach.
//
//     - Unknown values for Shield Regen Rate
//
//        - Presumed to be "decay."
//
//     - Unknown values for Health Regen Rate
//
//        - Presumed to be "decay."
//
//           - Does it take effect when your shields are up?
//
//           - Does it prevent shield regen?
//
//           - Can it kill?
//
//     - Shield Delay
//
//     - Shield Vampirism
//
//        - Requires someone to test with.
//
//     - Three settings governing Ability Usage
//
//        - Wouldn't be surprised if it was one or two settings with 
//          different values. I should look at the bits for each UI-exposed 
//          value.
//
//     - Unknown spawn AA indices
//
//     - Maximum jump height (game clamps or validates values; 420% did not 
//       work in-game despite being supported by the format).
//
//     - Unknown Active Camo option value
//
//        - Presumed to be "perfect camo."
//
//        - Remember: you can use mounted turrets to get a third-person view 
//          of your player model.
//
//     - Appearance: Aura
//
//        - Remember: you can use mounted turrets to get a third-person view 
//          of your player model.
//
//     - Sensors: Directional Damage Indicator
//
//        - Probably a bool trait. Could try setting off explosives near 
//          oneself to see.
//
//        - May require someone to test with.
//
//     - Unknown radar range values
//
//        - May require someone to test with.
//
//     - Test all illegal-but-storable values for all player trait enums
//
//     - Test all engine option toggles
//
//  = KNOWN INFO:
//
//     - The Grenade Regeneration trait is a trait-bool. If enabled for a 
//       player, that player will receive one frag and one plasma on every 
//       fifth second of the round.
//

#define REACH_GAME_VARIANTS_TESTING_RESAVE 1

struct TestingGameVariant {
   LPCWSTR     path = L"";
   const char* name = "";
   //
   TestingGameVariant(const char* n, LPCWSTR p) : path(p), name(n) {};
   //
   void execute() {
      cobb::mapped_file file;
      file.open(this->path);
      //
      printf("----------------------------------------------------------------------\n");
      printf("   Loading: %s <%S>\n", this->name, this->path);
      printf("----------------------------------------------------------------------\n");
      //
      auto variant = new GameVariant();
      variant->read(file);
      //
      printf("Header name: %S\n", variant->contentHeader.data.title);
      printf("Header desc: %S\n", variant->contentHeader.data.description);
      printf("Embedded name: %S\n", variant->multiplayer.variantHeader.title);
      printf("Embedded desc: %S\n", variant->multiplayer.variantHeader.description);
      printf("Loadout camera time: %d\n", (int)variant->multiplayer.options.respawn.loadoutCamTime);
      #if _DEBUG
         __debugbreak();
      #endif
      printf("\n");
   }
};

TestingGameVariant g_tests[] = {
   //
   // MCC files:
   //
   TestingGameVariant("Alpha Zombies",    L"alphazombies.bin"),
   TestingGameVariant("TU One Flag",      L"tu_ctf_1flag.bin"),
   TestingGameVariant("Assault",          L"assault.bin"),
   //TestingGameVariant("Halo Chess",       L"halo_chess.bin"), // Halo Chess is not valid; it uses a _cmp block instead of an mpvr block
   TestingGameVariant("Headhunter",       L"headhunter.bin"),
   TestingGameVariant("TU Headhunter",    L"headhunter_tu.bin"),
   TestingGameVariant("Juggernaut",       L"juggernaut.bin"),
   TestingGameVariant("King of the Hill", L"koth.bin"),
   TestingGameVariant("Oddball",          L"oddball.bin"),
   TestingGameVariant("Race",             L"race.bin"),
   TestingGameVariant("Rally",            L"rally.bin"),
   TestingGameVariant("Skeeball",         L"skeeball.bin"),
   TestingGameVariant("Slayer",           L"slayer.bin"),
   TestingGameVariant("Stockpile",        L"stockpile.bin"),
   TestingGameVariant("Territories",      L"territories.bin"),
   TestingGameVariant("Invasion (Boneyard)",   L"invasion_boneyard.bin"),
   TestingGameVariant("Invasion (Breakpoint)", L"invasion_breakpoint.bin"),
};

void test_create_hacked_variant() {
   const wchar_t* path = L"hr_4v4_team_slayer_dmr_ar_50points_tu.bin";
   //
   printf("Attempting to create hacked TU Slayer variant...\n");
   cobb::mapped_file file;
   file.open(path);
   auto variant = new GameVariant();
   if (!variant->read(file)) {
      printf("Failed to read base variant.\n");
      return;
   }
   printf("Variant read.\n");
   //
   auto& mp = variant->multiplayer;
   mp.options.map.baseTraits.movement.jumpHeight  = 69;
   mp.options.map.baseTraits.offense.grenadeRegen = (uint8_t)reach::bool_trait::enabled;
   mp.options.map.baseTraits.offense.weaponPrimary   = (uint8_t)reach::weapon::none;
   mp.options.map.baseTraits.offense.weaponSecondary = (uint8_t)reach::weapon::none;
   mp.titleUpdateData.flags |= (uint8_t)ReachTU1Flags::enable_automatic_magnum;
   mp.titleUpdateData.precisionBloom  = 10.0F; // 2x bloom
   mp.titleUpdateData.magnumFireDelay =  0.0F; // max fire rate
   mp.titleUpdateData.magnumDamage    =  1.0F;
   //
   auto& chdr = variant->contentHeader;
   chdr.data.set_title(L"Cursed Slayer");
   mp.variantHeader.set_title(L"Cursed Slayer");
   chdr.data.set_description(L"2x bloom; 69% jump height; grenade regen; unarmed starts; max Magnum fire rate.");
   mp.variantHeader.set_description(L"2x bloom; 69% jump height; grenade regen; unarmed starts; max Magnum fire rate.");
   //
   mp.variantHeader.unk08 = 0;
   mp.variantHeader.unk10 = 0;
   mp.variantHeader.unk18 = 0;
   mp.variantHeader.unk20 = 0;
   chdr.data.unk08 = 0;
   chdr.data.unk10 = 0;
   chdr.data.unk18 = 0;
   chdr.data.unk20 = 0;
   //
   printf("Writing modified data to buffer...\n");
   cobb::bit_or_byte_writer writer;
   variant->write(writer);
   writer.dump_to_console();
   //
   FILE*   out   = nullptr;
   errno_t error = fopen_s(&out, "test_output_cursed_slayer.bin", "wb");
   if (out) {
      printf("Writing modified data to output file...\n");
      writer.save_to(out);
      fclose(out);
      printf("Game variant saved.\n");
   } else
      printf("Failed to open output file for game variant.\n");
}

int main() {
   if (cobb::endian::native == cobb::endian::big) {
      printf("Current processor is big-endian.\n");
      printf("'ABCD' swapped to little-endian: %08X -> %08X\n", 'ABCD', cobb::to_little_endian(uint32_t('ABCD')));
   } else {
      printf("Current processor is little-endian.\n");
      printf("'ABCD' swapped to big-endian: %08X -> %08X\n", 'ABCD', cobb::to_big_endian(uint32_t('ABCD')));
   }
   test_create_hacked_variant();
   //
   #if REACH_GAME_VARIANTS_TESTING_RESAVE == 1
      {
         auto test = g_tests[0];
         cobb::mapped_file file;
         file.open(test.path);
         //
         printf("----------------------------------------------------------------------\n");
         printf("   Loading: %s <%S>\n", test.name, test.path);
         printf("----------------------------------------------------------------------\n");
         //
         auto variant = new GameVariant();
         variant->read(file);
         //
         printf("Header name: %S\n", variant->contentHeader.data.title);
         printf("Header desc: %S\n", variant->contentHeader.data.description);
         printf("Embedded name: %S\n", variant->multiplayer.variantHeader.title);
         printf("Embedded desc: %S\n", variant->multiplayer.variantHeader.description);
         printf("Loadout camera time: %d\n", (int)variant->multiplayer.options.respawn.loadoutCamTime);
         #if _DEBUG
            __debugbreak();
         #endif
         printf("\n");
         printf("Resaving...\n");
         cobb::bit_or_byte_writer writer;
         variant->write(writer);
         writer.dump_to_console();
         #if _DEBUG
            __debugbreak();
         #endif
      }
   #endif
   //
   for (int i = 0; i < std::extent<decltype(g_tests)>::value; i++)
      g_tests[i].execute();
   //
   return 0;
}

