#pragma once
#include "../../helpers/bitnumber.h"
#include "../../helpers/bitwriter.h"
#include "../../formats/localized_string_table.h"
#include "../../formats/indexed_lists.h"
#include "../../helpers/refcounting.h"
#include "../../helpers/stream.h"

class GameVariantDataMultiplayer;

class ReachMegaloGameStat : public indexed_list_item {
   public:
      enum class Format : uint8_t {
         number,
         number_with_sign,
         percentage,
         time,
      };
      enum class Sort : int8_t {
         ascending = -1,
         ignored,
         descending,
         obsolete_2,
      };
      MegaloStringRef name;
      Format format      = Format::number;
      Sort   sortOrder   = Sort::ascending;
      bool   groupByTeam = false;
      //
      void read(cobb::ibitreader&, GameVariantDataMultiplayer&) noexcept;
      void write(cobb::bitwriter& stream) const noexcept;
      //
      inline static constexpr uint32_t bitcount() noexcept { // constexpr implies inline but let's be explicit
         return MegaloStringIndex::bitcount + 2 + 2 + 1;
      }
};