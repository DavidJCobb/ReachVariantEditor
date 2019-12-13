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
         for (size_t i = 0; i < this->unknown.size(); i++)
            stream.read(this->unknown[i]);
         this->flag.read(stream);
      }
};