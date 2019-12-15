// ReachVariantTool.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "game_variants/base.h"
#include "helpers/files.h"

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
   TestingGameVariant("Stockpile",        L"slayer.bin"),
   TestingGameVariant("Territories",      L"slayer.bin"),
};

LPCWSTR testFile  = L"alphazombies.bin";
LPCWSTR testFile2 = L"tu_ctf_1flag.bin";

int main() {
   for (int i = 0; i < std::extent<decltype(g_tests)>::value; i++)
      g_tests[i].execute();
   //
   return 0;
}

