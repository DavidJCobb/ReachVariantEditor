#pragma once
#include <type_traits>
#include "../../helpers/bitnumber.h"
#include "../../helpers/bitstream.h"
#include "../../formats/localized_string_table.h"
#include "limits.h"

namespace Megalo {
   class ReachForgeLabel {
      public:
         enum class requirement_flags : uint8_t {
            objects_of_type = 0x01,
            assigned_team   = 0x02,
            number          = 0x04,
         };
         using requirement_int = std::underlying_type_t<requirement_flags>;
         //
         ReachString* name = nullptr;
         MegaloStringIndexOptional nameIndex;
         requirement_int    requirements       = 0;
         int32_t            requiredObjectType = -1;
         Megalo::const_team requiredTeam       = Megalo::const_team::none;
         int32_t            requiredNumber     = 0;
         uint8_t            mapMustHaveAtLeast = 0;
         //
         void read(cobb::bitstream&) noexcept;
         void postprocess_string_indices(ReachStringTable& table) noexcept;
         void write(cobb::bitwriter& stream) const noexcept;
         //
         inline bool requires_object_type()   const noexcept { return this->requirements & (requirement_int)requirement_flags::objects_of_type; }
         inline bool requires_assigned_team() const noexcept { return this->requirements & (requirement_int)requirement_flags::assigned_team; }
         inline bool requires_number()        const noexcept { return this->requirements & (requirement_int)requirement_flags::number; }
   };
}