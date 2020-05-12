#include "forge_label.h"
#include "../../types/multiplayer.h"
//
namespace Megalo {
   void ReachForgeLabel::read(cobb::ibitreader& stream, GameVariantDataMultiplayer& mp) noexcept {
      this->is_defined = true;
   //
      MegaloStringIndexOptional index;
      index.read(stream);
      this->name = mp.scriptData.strings.get_entry(index);
      //
      this->requirements.read(stream);
      if (this->requires_object_type())
         this->requiredObjectType.read(stream);
      if (this->requires_assigned_team())
         this->requiredTeam.read(stream);
      if (this->requires_number())
         this->requiredNumber.read(stream);
      this->mapMustHaveAtLeast.read(stream);
   }
   void ReachForgeLabel::write(cobb::bitwriter& stream) const noexcept {
      MegaloStringIndexOptional index = -1;
      if (this->name)
         index = this->name->index;
      index.write(stream);
      //
      this->requirements.write(stream);
      if (this->requires_object_type())
         this->requiredObjectType.write(stream);
      if (this->requires_assigned_team())
         this->requiredTeam.write(stream);
      if (this->requires_number())
         this->requiredNumber.write(stream);
      this->mapMustHaveAtLeast.write(stream);
   }
   uint32_t ReachForgeLabel::bitcount() const noexcept {
      uint32_t bitcount = 0;
      bitcount += MegaloStringIndexOptional::max_bitcount;
      bitcount += decltype(requirements)::max_bitcount;
      if (this->requires_object_type())
         bitcount += decltype(requiredObjectType)::max_bitcount;
      if (this->requires_assigned_team())
         bitcount += decltype(requiredTeam)::max_bitcount;
      if (this->requires_number())
         bitcount += decltype(requiredNumber)::max_bitcount;
      bitcount += decltype(mapMustHaveAtLeast)::max_bitcount;
      return bitcount;
   }
}