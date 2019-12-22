#include "content_author.h"
#include "../helpers/bitstream.h"
#include "../helpers/bytereader.h"

bool ReachContentAuthor::read(cobb::bitreader& stream) noexcept {
   this->timestamp.read(stream);
   this->xuid.read(stream);
   stream.read_string(this->author, 16);
   this->isOnlineID.read(stream);
   return true;
}
bool ReachContentAuthor::read(cobb::bytereader& stream) noexcept {
   this->timestamp.read(stream);
   this->xuid.read(stream);
   stream.read_string(this->author, 16);
   this->isOnlineID.read(stream);
   stream.pad(3);
   return true;
}
void ReachContentAuthor::write(cobb::bitwriter& stream) const noexcept {
   this->timestamp.write(stream);
   this->xuid.write(stream);
   stream.write_string(this->author, 16);
   stream.write((bool)this->isOnlineID);
}
void ReachContentAuthor::write(cobb::bytewriter& stream) const noexcept {
   this->timestamp.write(stream);
   this->xuid.write(stream);
   stream.write_string(this->author, 16);
   stream.write(this->isOnlineID);
   stream.pad(3);
}