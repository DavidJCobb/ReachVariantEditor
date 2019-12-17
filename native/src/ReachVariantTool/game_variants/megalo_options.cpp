#include "megalo_options.h"

void ReachMegaloOptionValueEntry::read(cobb::bitstream& stream, ReachMegaloOption& owner) noexcept {
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

void ReachMegaloOption::read(cobb::bitstream& stream) noexcept {
   this->nameIndex.read(stream);
   this->descIndex.read(stream);
   this->isRange.read(stream);
   if (this->isRange) {
      this->rangeDefault.read(stream, *this);
      this->rangeMin.read(stream, *this);
      this->rangeMax.read(stream, *this);
      this->rangeCurrent.read(stream);
   } else {
      this->defaultValueIndex.read(stream);
      ReachMegaloOptionValueCount count;
      count.read(stream);
      this->values.resize(count);
      for (size_t i = 0; i < count; i++)
         this->values[i].read(stream, *this);
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
   this->rangeDefault.postprocess_string_indices(table);
   for (auto& value : this->values)
      value.postprocess_string_indices(table);
}