#pragma once
#include <cstdint>
#include "../../../helpers/bitnumber.h"
#include "../../../helpers/bitwriter.h"
#include "../../../helpers/reference_tracked_object.h"
#include "../../../helpers/stream.h"

namespace Megalo {
   class HUDWidgetDeclaration : public cobb::reference_tracked_object {
      public:
         cobb::bitnumber<4, uint8_t> position; // values over 11 are not valid and will cause MCC to fail to load/display the game variant
         uint16_t index = 0;
         //
         void read(cobb::ibitreader& stream) noexcept {
            this->position.read(stream);
         }
         void write(cobb::bitwriter& stream) const noexcept {
            this->position.write(stream);
         }
   };
}
