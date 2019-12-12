#include "content_author.h"
#include "../helpers/bitstream.h"

bool ReachContentAuthor::read(cobb::bitstream& stream) noexcept {
   this->timestamp.read(stream);
   this->xuid.read(stream);
   stream.read_string(this->author, 16);
   this->isOnlineID.read(stream);
   stream.pad(3);
   return true;
}
bool ReachContentAuthor::read(cobb::bytestream& stream) noexcept {
   this->timestamp.read(stream);
   this->xuid.read(stream);
   stream.read_string(this->author, 16);
   this->isOnlineID.read(stream);
   stream.pad(3);
   return true;
}