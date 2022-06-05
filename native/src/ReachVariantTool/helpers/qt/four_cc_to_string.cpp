#include "four_cc_to_string.h"

namespace cobb::qt {
   extern QString four_cc_to_string(uint32_t signature) {
      return QString("%1%2%3%4")
         .arg(QChar(signature >> 0x18))
         .arg(QChar((signature >> 0x10) & 0xFF))
         .arg(QChar((signature >> 0x08) & 0xFF))
         .arg(QChar(signature & 0xFF));
   }

   extern QString four_cc_to_string(const char(&signature)[4]) {
      QString out;
      out += signature[0];
      out += signature[1];
      out += signature[2];
      out += signature[3];
      return out;
   }
}