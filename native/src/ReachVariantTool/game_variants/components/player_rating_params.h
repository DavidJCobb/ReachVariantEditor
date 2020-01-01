#pragma once
#include <array>
#include <cstdint>
#include "../../helpers/bitnumber.h"
#include "../../helpers/stream.h"
#include "../../helpers/bitwriter.h"

class ReachPlayerRatingParams {
   public:
      std::array<float, 15> unknown;
      cobb::bitbool flag;
      //
      void read(cobb::ibitreader& stream) noexcept {
         for (float& f : this->unknown)
            stream.read(f);
         this->flag.read(stream);
      }
      void write(cobb::bitwriter& stream) const noexcept {
         for (float f : this->unknown)
            stream.write(f);
         this->flag.write(stream);
      }
      //
      #if __cplusplus <= 201703L
      bool operator==(const ReachPlayerRatingParams& other) const noexcept {
         if (this->flag != other.flag)
            return false;
         return this->unknown == other.unknown;
      }
      bool operator!=(const ReachPlayerRatingParams& other) const noexcept { return !(*this == other); }
      #else
      bool operator==(const ReachPlayerRatingParams&) const noexcept = default;
      bool operator!=(const ReachPlayerRatingParams&) const noexcept = default;
      #endif
};