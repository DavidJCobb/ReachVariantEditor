// ReachVariantTool.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "game_variants/base.h"
#include "helpers/files.h"

LPCWSTR testFile = L"alphazombies.bin";

int main() {
   cobb::mapped_file file;
   file.open(testFile);
   //
   auto variant = new GameVariant();
   variant->read(file);
   //
   printf("Header name: %S\n", variant->contentHeader.data.title);
   printf("Header desc: %S\n", variant->contentHeader.data.description);
   printf("Embedded name: %S\n", variant->multiplayer.variantHeader.title);
   printf("Embedded desc: %S\n", variant->multiplayer.variantHeader.description);
   printf("Loadout camera time: %d\n", (int)variant->multiplayer.options.respawn.loadoutCamTime);


   return 0;
}

