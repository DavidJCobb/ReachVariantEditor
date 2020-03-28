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
}