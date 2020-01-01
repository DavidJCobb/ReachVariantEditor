#pragma once
#include <type_traits>
#include "../../../helpers/bitnumber.h"
#include "../../../helpers/stream.h"
#include "../../../helpers/bitwriter.h"
#include "../../../formats/localized_string_table.h"
#include "limits.h"
#include "limits_bitnumbers.h"

namespace Megalo {
   class ReachForgeLabel {
      public:
         enum class requirement_flags : uint8_t {
            objects_of_type = 0x01,
            assigned_team   = 0x02,
            number          = 0x04,
         };
         using requirement_int   = std::underlying_type_t<requirement_flags>;
         //
         ReachString* name = nullptr;
         MegaloStringIndexOptional nameIndex;
         cobb::bitnumber<3, requirement_int> requirements = 0;
         object_type_index_optional  requiredObjectType = -1;
         const_team_index            requiredTeam       = Megalo::const_team::none;
         cobb::bytenumber<int16_t>   requiredNumber     = 0;
         cobb::bitnumber<7, uint8_t> mapMustHaveAtLeast = 0;
         //
         void read(cobb::ibitreader&) noexcept;
         void postprocess_string_indices(ReachStringTable& table) noexcept;
         void write(cobb::bitwriter& stream) const noexcept;
         //
         inline bool requires_object_type()   const noexcept { return this->requirements & (requirement_int)requirement_flags::objects_of_type; }
         inline bool requires_assigned_team() const noexcept { return this->requirements & (requirement_int)requirement_flags::assigned_team; }
         inline bool requires_number()        const noexcept { return this->requirements & (requirement_int)requirement_flags::number; }
   };
}