// ReachVariantTool.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "game_variants/base.h"
#include "helpers/bitwriter.h"
#include "helpers/endianness.h"
#include "helpers/files.h"
#include "formats/sha1.h"

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
   mp.options.map.baseTraits.movement.jumpHeight = 420;
   mp.titleUpdateData.precisionBloom = 10.0F;
   mp.titleUpdateData.magnumFireRate = 10.0F;
   mp.titleUpdateData.magnumDamage   =  0.65F;
   //
   auto& chdr = variant->contentHeader;
   chdr.data.set_title(L"Cursed Slayer");
   mp.variantHeader.set_title(L"Cursed Slayer");
   chdr.data.set_description(L"2x bloom; 420% jump height; 10x Magnum fire rate, full clip + headshot to kill.");
   mp.variantHeader.set_description(L"2x bloom; 420% jump height; 10x Magnum fire rate, full clip + headshot to kill.");
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
   cobb::bitwriter writer;
   variant->write(writer);
   //
   FILE*   out   = nullptr;
   errno_t error = fopen_s(&out, "test_output_game_variant.bin", "wb");
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
   {
      const char* text = "This is a test";
      auto hasher = InProgressSHA1();
      hasher.transform((const uint8_t*)text, strlen(text));
      printf("SHA-1 hash of: %s\n", text);
      for (size_t i = 0; i < 5; i++)
         printf("   %08X\n", hasher.hash[i]);
      printf("\n");
   }
   {  // Test hash of a 360 file
      cobb::mapped_file file;
      file.open(L"SvE Mythic Infection.bin");
      GameVariant::test_mpvr_hash(file);
   }
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
         cobb::bitwriter writer;
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

