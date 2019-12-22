#pragma once
#include <vector>
#include "../helpers/bitnumber.h"
#include "../helpers/bitreader.h"
#include "../helpers/bitwriter.h"
#include "../formats/localized_string_table.h"

using ReachMegaloOptionValue = cobb::bitnumber<10, int16_t>;
using ReachMegaloOptionValueIndex = cobb::bitnumber<cobb::bitcount(8 - 1), uint8_t>;
using ReachMegaloOptionValueCount = cobb::bitnumber<cobb::bitcount(8), uint8_t>;

class ReachBlockMPVR;

class ReachMegaloOption;

class ReachMegaloOptionValueEntry {
   public:
      ReachString* name = nullptr;
      ReachString* desc = nullptr;
      ReachMegaloOptionValue value;
      MegaloStringIndex nameIndex;
      MegaloStringIndex descIndex;
      //
      void read(cobb::bitreader&, ReachMegaloOption& owner) noexcept;
      void postprocess_string_indices(ReachStringTable& table) noexcept;
      void write(cobb::bitwriter& stream, const ReachMegaloOption& owner) const noexcept;
};

class ReachMegaloOption {
   public:
      ReachString* name = nullptr;
      ReachString* desc = nullptr;
      MegaloStringIndex nameIndex;
      MegaloStringIndex descIndex;
      cobb::bitbool isRange;
      ReachMegaloOptionValueEntry rangeDefault;
      ReachMegaloOptionValueEntry rangeMin;
      ReachMegaloOptionValueEntry rangeMax;
      std::vector<ReachMegaloOptionValueEntry> values;
      ReachMegaloOptionValueIndex defaultValueIndex;
      ReachMegaloOptionValue rangeCurrent;
      ReachMegaloOptionValueIndex currentValueIndex;
      //
      void read(cobb::bitreader&) noexcept;
      void postprocess_string_indices(ReachStringTable& table) noexcept;
      void write(cobb::bitwriter& stream) const noexcept;
};