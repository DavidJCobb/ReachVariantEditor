#pragma once
#include <cstdint>
#include "../../../helpers/bitnumber.h"
#include "../../../helpers/bitwriter.h"
#include "../../../helpers/refcounting.h"
#include "../../../helpers/stream.h"
#include "../../../formats/indexed_lists.h"

namespace Megalo {
   class HUDWidgetDeclaration : public indexed_list_item {
      public:
         cobb::bitnumber<4, uint8_t> position; // values over 11 are not valid and will cause MCC to fail to load/display the game variant
         //
         void read(cobb::ibitreader& stream) noexcept {
            this->is_defined = true;
            //
            this->position.read(stream);
         }
         void write(cobb::bitwriter& stream) const noexcept {
            this->position.write(stream);
         }
         //
         inline static constexpr uint32_t bitcount() noexcept {
            return decltype(position)::bitcount;
         }
   };
}
