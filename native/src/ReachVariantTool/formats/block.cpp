#include "block.h"
#include "../helpers/bitstream.h"

bool ReachFileBlock::read(cobb::bitstream& stream) noexcept {
   stream.read(this->found.signature); // bswap?
   stream.read(this->found.size);
   stream.read(this->found.version); // bswap?
   stream.read(this->found.flags); // bswap?
   //
   if (this->found.signature != this->expected.signature)
      return false;
   if (this->found.size != this->expected.size) {
      auto s = _byteswap_ulong(this->found.size);
      if (s != this->expected.size)
         return false;
   }
   return true;
}
bool ReachFileBlock::read(cobb::bytestream& stream) noexcept {
   stream.read(this->found.signature); // bswap?
   this->found.signature = _byteswap_ulong(this->found.signature);
   stream.read(this->found.size);
   stream.read(this->found.version); // bswap?
   stream.read(this->found.flags); // bswap?
   //
   if (this->found.signature != this->expected.signature)
      return false;
   if (this->found.size != this->expected.size) {
      auto s = _byteswap_ulong(this->found.size);
      if (s != this->expected.size)
         return false;
   }
   return true;
}
