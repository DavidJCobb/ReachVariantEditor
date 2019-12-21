#pragma once
#include <cstdint>
#include "../../helpers/bitnumber.h"
#include "../../helpers/bitstream.h"

namespace Megalo {
   class HUDWidgetDeclaration {
      public:
         cobb::bitnumber<4, uint8_t> position;
         //
         void read(cobb::bitstream& stream) noexcept {
            this->position.read(stream);
         }
         void write(cobb::bitwriter& stream) const noexcept {
            this->position.write(stream);
         }
   };
}
