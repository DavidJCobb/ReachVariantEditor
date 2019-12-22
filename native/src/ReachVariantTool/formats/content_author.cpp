#include "content_author.h"
#include "../helpers/bitstream.h"
#include "../helpers/bytereader.h"

bool ReachContentAuthor::read(cobb::bitstream& stream) noexcept {
   this->timestamp.read(stream);
   this->xuid.read(stream);
   stream.read_string(this->author, 16);
   this->isOnlineID.read(stream);
   stream.pad(3);
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
void ReachContentAuthor::write_bits(cobb::bitwriter& stream) const noexcept {
   this->timestamp.write_bits(stream);
   this->xuid.write_bits(stream);
   stream.write_string(this->author, 16);
   stream.write((bool)this->isOnlineID);
}
void ReachContentAuthor::write_bytes(cobb::bitwriter& stream) const noexcept {
   this->timestamp.write_bytes(stream);
   this->xuid.write_bytes(stream);
   stream.write(this->author);
   stream.write(this->isOnlineID, 8);
   stream.pad_bytes(3);
}