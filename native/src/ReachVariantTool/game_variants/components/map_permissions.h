#pragma once
#include <cstdint>
#include <vector>
#include "../../helpers/bitnumber.h"
#include "../../helpers/stream.h"
#include "../../helpers/bitwriter.h"

namespace reach {
   enum class map_permission_type : uint8_t {
      only_these_maps  = 0,
      never_these_maps = 1,
   };
}

class ReachMapPermissions {
   public:
      static constexpr int max_count = 32;
      //
   public:
      std::vector<int16_t> mapIDs;
      cobb::bitnumber<1, reach::map_permission_type> type;
      //
      void read(cobb::ibitreader& stream) noexcept {
         size_t count = stream.read_bits<size_t>(6);
         this->mapIDs.resize(count);
         for (size_t i = 0; i < count; i++)
            this->mapIDs[i] = (int16_t) stream.read_bits<uint16_t>(16);
         this->type.read(stream);
      }
      void write(cobb::bitwriter& stream) const noexcept {
         stream.write(this->mapIDs.size(), 6);
         for (int16_t id : this->mapIDs)
            stream.write((uint16_t)id);
         this->type.write(stream);
      }
      //
      #if __cplusplus <= 201703L
      bool operator==(const ReachMapPermissions& other) const noexcept {
         if (this->type != other.type)
            return false;
         return this->mapIDs == other.mapIDs;
      }
      bool operator!=(const ReachMapPermissions& other) const noexcept { return !(*this == other); }
      #else
      bool operator==(const ReachMapPermissions&) const noexcept = default;
      bool operator!=(const ReachMapPermissions&) const noexcept = default;
      #endif
      //
      inline uint32_t bitcount() const noexcept {
         return 6 + (cobb::bits_in<uint16_t> * this->mapIDs.size()) + decltype(type)::bitcount; // list count; list contents; type
      }
};