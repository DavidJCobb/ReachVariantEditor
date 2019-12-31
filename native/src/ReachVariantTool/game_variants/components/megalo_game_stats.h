#pragma once
#include "../../helpers/bitnumber.h"
#include "../../helpers/bitreader.h"
#include "../../helpers/bitwriter.h"
#include "../../formats/localized_string_table.h"

class ReachMegaloGameStat {
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
      ReachString* name = nullptr;
      MegaloStringIndex nameIndex;
      Format format      = Format::number;
      Sort   sortOrder   = Sort::ascending;
      bool   groupByTeam = false;
      //
      void read(cobb::bitreader&) noexcept;
      void postprocess_string_indices(ReachStringTable& table) noexcept;
      void write(cobb::bitwriter& stream) const noexcept;
      //
      #if __cplusplus <= 201703L
      bool operator==(const ReachMegaloGameStat&) const noexcept;
      bool operator!=(const ReachMegaloGameStat& other) const noexcept { return !(*this == other); }
      #else
      bool operator==(const ReachMegaloGameStat&) const noexcept = default;
      bool operator!=(const ReachMegaloGameStat&) const noexcept = default;
      #endif
};