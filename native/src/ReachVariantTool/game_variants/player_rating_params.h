#pragma once
#include <array>
#include <cstdint>
#include "../helpers/bitstream.h"
#include "../helpers/bitnumber.h"

class ReachPlayerRatingParams {
   public:
      std::array<float, 15> unknown;
      cobb::bitbool flag;
      //
      void read(cobb::bitstream& stream) noexcept {
         for (float& f : this->unknown)
            stream.read(f);
         this->flag.read(stream);
      }
      void write(cobb::bitwriter& stream) const noexcept {
         for (float f : this->unknown)
            stream.write(f);
         this->flag.write(stream);
      }
};