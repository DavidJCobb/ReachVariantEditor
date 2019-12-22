#include "forge_label.h"
//
namespace Megalo {
   void ReachForgeLabel::read(cobb::bitreader& stream) noexcept {
      this->nameIndex.read(stream);
      this->requirements.read(stream);
      if (this->requires_object_type())
         this->requiredObjectType.read(stream);
      if (this->requires_assigned_team())
         this->requiredTeam.read(stream);
      if (this->requires_number())
         this->requiredNumber.read(stream);
      this->mapMustHaveAtLeast.read(stream);
   }
   void ReachForgeLabel::postprocess_string_indices(ReachStringTable& table) noexcept {
      this->name = table.get_entry(this->nameIndex);
   }
   void ReachForgeLabel::write(cobb::bitwriter& stream) const noexcept {
      this->nameIndex.write(stream);
      this->requirements.write(stream);
      if (this->requires_object_type())
         this->requiredObjectType.write(stream);
      if (this->requires_assigned_team())
         this->requiredTeam.write(stream);
      if (this->requires_number())
         this->requiredNumber.write(stream);
      this->mapMustHaveAtLeast.write(stream);
   }
}