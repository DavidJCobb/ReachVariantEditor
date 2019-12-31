#pragma once
#include <cstdint>
#include <vector>
#include "../../helpers/bitnumber.h"
#include "../../helpers/bitreader.h"
#include "../../helpers/bitwriter.h"

namespace reach {
   enum class map_permission_type : uint8_t {
      only_these_maps  = 0,
      never_these_maps = 1,
   };
}

class ReachMapPermissions {
   public:
      std::vector<uint16_t> mapIDs;
      cobb::bitnumber<1, reach::map_permission_type> type;
      //
      void read(cobb::bitreader& stream) noexcept {
         size_t count = stream.read_bits<size_t>(6);
         this->mapIDs.resize(count);
         for (size_t i = 0; i < count; i++)
            this->mapIDs[i] = stream.read_bits<uint16_t>(16);
         this->type.read(stream);
      }
      void write(cobb::bitwriter& stream) const noexcept {
         stream.write(this->mapIDs.size(), 6);
         for (uint16_t id : this->mapIDs)
            stream.write(id);
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
};