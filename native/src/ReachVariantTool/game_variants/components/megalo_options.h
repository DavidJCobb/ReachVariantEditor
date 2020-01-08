#pragma once
#include <vector>
#include "../../helpers/bitnumber.h"
#include "../../helpers/bitwriter.h"
#include "../../formats/localized_string_table.h"
#include "../../helpers/reference_tracked_object.h"
#include "../../helpers/stream.h"

using ReachMegaloOptionValue = cobb::bitnumber<10, int16_t>;
using ReachMegaloOptionValueIndex = cobb::bitnumber<cobb::bitcount(8 - 1), uint8_t>;
using ReachMegaloOptionValueCount = cobb::bitnumber<cobb::bitcount(8), uint8_t>;

class ReachBlockMPVR;

class ReachMegaloOption;

class ReachMegaloOptionValueEntry : public cobb::reference_tracked_object {
   public:
      MegaloStringRef name = MegaloStringRef::make(*this);
      MegaloStringRef desc = MegaloStringRef::make(*this);
      ReachMegaloOptionValue value;
      MegaloStringIndex nameIndex;
      MegaloStringIndex descIndex;
      //
      void read(cobb::ibitreader&, ReachMegaloOption& owner) noexcept;
      void postprocess_string_indices(ReachStringTable& table) noexcept;
      void write(cobb::bitwriter& stream, const ReachMegaloOption& owner) const noexcept;
};

class ReachMegaloOption : public cobb::reference_tracked_object {
   public:
      MegaloStringRef name = MegaloStringRef::make(*this);
      MegaloStringRef desc = MegaloStringRef::make(*this);
      MegaloStringIndex nameIndex;
      MegaloStringIndex descIndex;
      cobb::bitbool isRange;
      ReachMegaloOptionValueEntry* rangeDefault = nullptr;
      ReachMegaloOptionValueEntry* rangeMin = nullptr;
      ReachMegaloOptionValueEntry* rangeMax = nullptr;
      std::vector<ReachMegaloOptionValueEntry*> values;
      ReachMegaloOptionValueIndex defaultValueIndex;
      ReachMegaloOptionValue rangeCurrent;
      ReachMegaloOptionValueIndex currentValueIndex;
      //
      void read(cobb::ibitreader&) noexcept;
      void postprocess_string_indices(ReachStringTable& table) noexcept;
      void write(cobb::bitwriter& stream) const noexcept;
};