#include "megalo_options.h"
#include <cassert>

void ReachMegaloOptionValueEntry::read(cobb::ibitreader& stream, ReachMegaloOption& owner) noexcept {
   this->value.read(stream);
   if (!owner.isRange) {
      this->nameIndex.read(stream);
      this->descIndex.read(stream);
   }
}
void ReachMegaloOptionValueEntry::postprocess_string_indices(ReachStringTable& table) noexcept {
   this->name = table.get_entry(this->nameIndex);
   this->desc = table.get_entry(this->descIndex);
}
void ReachMegaloOptionValueEntry::write(cobb::bitwriter& stream, const ReachMegaloOption& owner) const noexcept {
   this->value.write(stream);
   if (!owner.isRange) {
      this->nameIndex.write(stream);
      this->descIndex.write(stream);
   }
}

void ReachMegaloOption::read(cobb::ibitreader& stream) noexcept {
   this->nameIndex.read(stream);
   this->descIndex.read(stream);
   this->isRange.read(stream);
   if (this->isRange) {
      this->rangeDefault = new ReachMegaloOptionValueEntry;
      this->rangeMin     = new ReachMegaloOptionValueEntry;
      this->rangeMax     = new ReachMegaloOptionValueEntry;
      //
      this->rangeDefault->read(stream, *this);
      this->rangeMin->read(stream, *this);
      this->rangeMax->read(stream, *this);
      this->rangeCurrent.read(stream);
   } else {
      this->defaultValueIndex.read(stream);
      ReachMegaloOptionValueCount count;
      count.read(stream);
      this->values.reserve(count);
      for (size_t i = 0; i < count; i++) {
         auto& value = *this->values.emplace_back(new ReachMegaloOptionValueEntry);
         value.read(stream, *this);
      }
      if (this->defaultValueIndex >= count)
         printf("WARNING: Megalo option has out-of-bounds default index!\n");
      this->currentValueIndex.read(stream);
      if (this->currentValueIndex >= this->values.size())
         printf("WARNING: Megalo option has out-of-bounds current index!\n");
   }
}
void ReachMegaloOption::postprocess_string_indices(ReachStringTable& table) noexcept {
   this->name = table.get_entry(this->nameIndex);
   this->desc = table.get_entry(this->descIndex);
   if (this->rangeDefault)
      this->rangeDefault->postprocess_string_indices(table);
   for (auto& value : this->values)
      value->postprocess_string_indices(table);
}
void ReachMegaloOption::write(cobb::bitwriter& stream) const noexcept {
   this->nameIndex.write(stream);
   this->descIndex.write(stream);
   this->isRange.write(stream);
   if (this->isRange) {
      assert(this->rangeDefault && "A range option is missing its default value!");
      assert(this->rangeMin     && "A range option is missing its minimum value!");
      assert(this->rangeMax     && "A range option is missing its maximum value!");
      this->rangeDefault->write(stream, *this);
      this->rangeMin->write(stream, *this);
      this->rangeMax->write(stream, *this);
      this->rangeCurrent.write(stream);
   } else {
      this->defaultValueIndex.write(stream);
      stream.write(this->values.size(), cobb::bitcount(8));
      for (auto& value : this->values)
         value->write(stream, *this);
      this->currentValueIndex.write(stream);
   }
}
//
void ReachMegaloOption::make_range() noexcept {
   int16_t min = 65535;
   int16_t max = 0;
   int16_t def = 0;
   if (!this->rangeMin || !this->rangeMax) {
      for (auto& v : this->values) {
         auto n = v->value;
         if (n < min)
            min = n;
         if (n > max)
            max = n;
      }
   }
   if (!this->rangeMin) {
      this->rangeMin = new ReachMegaloOptionValueEntry;
      auto other = this->rangeMax;
      if (other && min > other->value)
         min = other->value;
      this->rangeMin->value = min;
   }
   if (!this->rangeMax) {
      this->rangeMax = new ReachMegaloOptionValueEntry;
      auto other = this->rangeMin;
      if (other && max < other->value)
         max = other->value;
      this->rangeMin->value = max;
   }
   if (!this->rangeDefault) {
      this->rangeDefault = new ReachMegaloOptionValueEntry;
      auto di = this->defaultValueIndex;
      if (di >= 0 && di < this->values.size()) {
         auto& v = *this->values[di];
         if (v.value >= min && v.value <= max)
            this->rangeDefault->value = v.value;
         else
            this->rangeDefault->value = min;
      } else
         this->rangeDefault->value = min;
   }
   this->isRange = true;
}