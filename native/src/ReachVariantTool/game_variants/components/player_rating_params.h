#pragma once
#include <array>
#include <cstdint>
#include "../../helpers/bitnumber.h"
#include "../../helpers/bitreader.h"
#include "../../helpers/bitwriter.h"

class ReachPlayerRatingParams {
   public:
      std::array<float, 15> unknown;
      cobb::bitbool flag;
      //
      void read(cobb::bitreader& stream) noexcept {
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