#include "forge_label.h"
#include "halo/reach/bitstreams.h"

namespace halo::reach::megalo {
   void forge_label::read(bitreader& stream) {
      stream.read(
         name,
         requirements.flags
      );
      //
      auto& req = this->requirements;
      if (req.flags & requirement_flag::objects_of_type)
         stream.read(req.object_type);
      if (req.flags & requirement_flag::assigned_team)
         stream.read(req.team);
      if (req.flags & requirement_flag::number)
         stream.read(req.number);
      stream.read(req.map_must_have_at_least);
   }
   void forge_label::write(bitwriter& stream) const {
      stream.write(
         name,
         requirements.flags
      );
      //
      auto& req = this->requirements;
      if (req.flags & requirement_flag::objects_of_type)
         stream.write(req.object_type);
      if (req.flags & requirement_flag::assigned_team)
         stream.write(req.team);
      if (req.flags & requirement_flag::number)
         stream.write(req.number);
      stream.write(req.map_must_have_at_least);
   }
}