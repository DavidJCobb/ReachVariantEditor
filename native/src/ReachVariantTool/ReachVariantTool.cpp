// ReachVariantTool.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "game_variants/base.h"
#include "helpers/bitwriter.h"
#include "helpers/endianness.h"
#include "helpers/files.h"

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

void _cobb_invalid_parameter(
   const wchar_t* expression,
   const wchar_t* function,
   const wchar_t* file,
   unsigned int line,
   uintptr_t pReserved
) {
   printf("CRT Invalid Parameter Error\nFile: %S\nLine: %u\nFunc: %S\nExpr: %S\n", file, line, function, expression);
   assert(false && "Invalid parameter");
}

int main() {
   #if _DEBUG
      _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_DEBUG | _CRTDBG_MODE_WNDW); // make asserts log to the console and pop a window
      _CrtSetReportMode(_CRT_ERROR,  _CRTDBG_MODE_DEBUG | _CRTDBG_MODE_WNDW);
      _set_invalid_parameter_handler(_cobb_invalid_parameter);
   #endif
   if (cobb::endian::native == cobb::endian::big) {
      printf("Current processor is big-endian.\n");
      printf("'ABCD' swapped to little-endian: %08X -> %08X\n", 'ABCD', cobb::to_little_endian(uint32_t('ABCD')));
   } else {
      printf("Current processor is little-endian.\n");
      printf("'ABCD' swapped to big-endian: %08X -> %08X\n", 'ABCD', cobb::to_big_endian(uint32_t('ABCD')));
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

