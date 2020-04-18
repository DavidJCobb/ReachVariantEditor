#pragma once
#include <array>
#include <cstdint>
#include "../../helpers/bitnumber.h"
#include "../../helpers/stream.h"
#include "../../helpers/bitwriter.h"

class ReachPlayerRatingParams {
   public:
      struct indices {
         indices() = delete;
         enum {
            rating_scale,
            kill_weight,
            assist_weight,
            betrayal_weight,
            death_weight,
            normalize_by_max_kills,
            base,
            range,
            loss_scalar,
            custom_stat_0,
            custom_stat_1,
            custom_stat_2,
            custom_stat_3,
            expansion_0,
            expansion_1,
         };
      };
      //
   public:
      std::array<float, 15> values;
      cobb::bitbool showInScoreboard;
      //
      void read(cobb::ibitreader& stream) noexcept {
         for (float& f : this->unknown)
            stream.read(f);
         this->showInScoreboard.read(stream);
      }
      void write(cobb::bitwriter& stream) const noexcept {
         for (float f : this->unknown)
            stream.write(f);
         this->showInScoreboard.write(stream);
      }
      //
      void reset() noexcept {
         this->values[indices::rating_scale] = 1.0F;
         this->values[indices::kill_weight] = 1.0F;
         this->values[indices::assist_weight] = 1.0F;
         this->values[indices::betrayal_weight] = 1.0F;
         this->values[indices::death_weight] = 0.33F;
         this->values[indices::normalize_by_max_kills] = 1.0F;
         this->values[indices::base] = 1000.0F;
         this->values[indices::range] = 1000.0F;
         this->values[indices::loss_scalar] = 0.96F;
      }
      //
      #if __cplusplus <= 201703L
      bool operator==(const ReachPlayerRatingParams& other) const noexcept {
         if (this->showInScoreboard != other.showInScoreboard)
            return false;
         return this->unknown == other.unknown;
      }
      bool operator!=(const ReachPlayerRatingParams& other) const noexcept { return !(*this == other); }
      #else
      bool operator==(const ReachPlayerRatingParams&) const noexcept = default;
      bool operator!=(const ReachPlayerRatingParams&) const noexcept = default;
      #endif
      //
      inline static constexpr uint32_t bitcount() noexcept { // constexpr implies inline but let's be explicit
         return (sizeof(float) * 8 * 15) + decltype(showInScoreboard)::bitcount;
      }
};