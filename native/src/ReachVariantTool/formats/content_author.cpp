#include "content_author.h"
#include "../helpers/bitreader.h"
#include "../helpers/bitwriter.h"
#include "../helpers/bytereader.h"
#include "../helpers/bytewriter.h"

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
void ReachContentAuthor::set_author_name(const char* s) noexcept {
   uint8_t i = 0;
   for (; i < 16; i++) {
      char c = s[i];
      if (!c)
         break;
      this->author[i] = c;
   }
   for (; i < 16; i++)
      this->author[i] = '\0';
}
bool ReachContentAuthor::has_xuid() const noexcept {
   return this->xuid != 0;
}
void ReachContentAuthor::erase_xuid() noexcept {
   this->xuid = 0;
   this->isOnlineID = false;
}
void ReachContentAuthor::set_datetime(uint64_t seconds_since_jan_1_1970) noexcept {
   this->timestamp = seconds_since_jan_1_1970;
}