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
void ReachMegaloOptionValueEntry::write(cobb::bitwriter& stream, const ReachMegaloOption& owner) noexcept {
   this->value.write(stream);
   if (!owner.isRange) {
      {  // Correct indices
         if (this->name) {
            this->nameIndex = this->name->index;
         } else
            this->nameIndex = 0;
         //
         if (this->desc) {
            this->descIndex = this->desc->index;
         } else
            this->descIndex = 0;
      }
      this->nameIndex.write(stream);
      this->descIndex.write(stream);
   }
}
/*static*/ uint32_t ReachMegaloOptionValueEntry::bitcount() noexcept {
   uint32_t bitcount = 0;
   bitcount += decltype(value)::max_bitcount;
   bitcount += decltype(nameIndex)::max_bitcount;
   bitcount += decltype(descIndex)::max_bitcount;
   return bitcount;
}

ReachMegaloOption::~ReachMegaloOption() {
   if (this->rangeDefault) {
      delete this->rangeDefault;
      this->rangeDefault = nullptr;
   }
   if (this->rangeMin) {
      delete this->rangeMin;
      this->rangeMin = nullptr;
   }
   if (this->rangeMax) {
      delete this->rangeMax;
      this->rangeMax = nullptr;
   }
}
void ReachMegaloOption::read(cobb::ibitreader& stream) noexcept {
   this->is_defined = true;
   //
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
void ReachMegaloOption::write(cobb::bitwriter& stream) noexcept {
   {  // Correct indices
      if (this->name) {
         this->nameIndex = this->name->index;
      } else
         this->nameIndex = 0;
      //
      if (this->desc) {
         this->descIndex = this->desc->index;
      } else
         this->descIndex = 0;
   }
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
ReachMegaloOptionValueEntry* ReachMegaloOption::add_value() noexcept {
   if (this->isRange)
      return nullptr;
   auto& list = this->values;
   if (list.size() >= Megalo::Limits::max_script_option_values)
      return nullptr;
   auto& value = *this->values.emplace_back(new ReachMegaloOptionValueEntry);
   return &value;
}
void ReachMegaloOption::delete_value(ReachMegaloOptionValueEntry* target) noexcept {
   if (!target)
      return;
   auto& list = this->values;
   if (list.size() == 1) // options must always have at least one value
      return;
   auto  it   = std::find(list.begin(), list.end(), target);
   if (it == list.end())
      return;
   auto dist = std::distance(list.begin(), it);
   list.erase(it);
   if (dist <= this->defaultValueIndex)
      this->defaultValueIndex -= 1;
   if (dist <= this->currentValueIndex)
      this->currentValueIndex -= 1;
}
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
   this->rangeCurrent = this->rangeDefault->value;
   this->isRange = true;
}
void ReachMegaloOption::swap_values(size_t a, size_t b) noexcept {
   auto& list = this->values;
   if (a >= list.size() || b >= list.size())
      return;
   list.swap_items(a, b);
   //
   if (this->defaultValueIndex == a)
      this->defaultValueIndex = b;
   else if (this->defaultValueIndex == b)
      this->defaultValueIndex = a;
   //
   if (this->currentValueIndex == a)
      this->currentValueIndex = b;
   else if (this->currentValueIndex == b)
      this->currentValueIndex = a;
}
bool ReachMegaloOption::uses_string(ReachString* str) const noexcept {
   if (this->name == str || this->desc == str)
      return true;
   for (auto* value : this->values)
      if (value->name == str || value->desc == str)
         return true;
   return false;
}
//
uint32_t ReachMegaloOption::bitcount() const noexcept {
   uint32_t bitcount = 0;
   bitcount += decltype(nameIndex)::max_bitcount;
   bitcount += decltype(descIndex)::max_bitcount;
   bitcount += decltype(isRange)::max_bitcount;
   if (this->isRange) {
      bitcount += ReachMegaloOptionValueEntry::bitcount() * 3; // range default, min, and max
      bitcount += decltype(rangeCurrent)::max_bitcount;
   } else {
      bitcount += decltype(defaultValueIndex)::max_bitcount;
      bitcount += 8;
      bitcount += ReachMegaloOptionValueEntry::bitcount() * this->values.size();
      bitcount += decltype(currentValueIndex)::max_bitcount;
   }
   return bitcount;
}