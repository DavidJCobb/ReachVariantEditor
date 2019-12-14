#pragma once
#include <vector>
#include "../helpers/bitnumber.h"
#include "../helpers/bitstream.h"
#include "../formats/localized_string_table.h"

using ReachMegaloOptionValue = cobb::bitnumber<10, int16_t>;
using ReachMegaloOptionValueIndex = cobb::bitnumber<cobb::bitcount(8 - 1), uint8_t>;
using ReachMegaloOptionValueCount = cobb::bitnumber<cobb::bitcount(8), uint8_t>;

class ReachMegaloOption;

class ReachMegaloOptionValueEntry {
   public:
      ReachMegaloOptionValue value;
      MegaloStringIndex nameIndex;
      MegaloStringIndex descIndex;
      //
      void read(cobb::bitstream&, ReachMegaloOption& owner) noexcept;
};

class ReachMegaloOption {
   public:
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
      void read(cobb::bitstream&) noexcept;
};