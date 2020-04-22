#pragma once
#include <cstdint>
#include <QString>
#include "../string_scanner.h"

namespace Megalo {
   class Compiler;
   //
   namespace Script {
      class ParsedItem {
         public:
            virtual ~ParsedItem() {} // need virtual methods to allow dynamic_cast
            //
            ParsedItem* parent = nullptr;
            ParsedItem* owner  = nullptr; // for conditions
            int32_t line = -1;
            int32_t col  = -1;
            struct {
               int32_t start = -1;
               int32_t end   = -1;
            } range;
            //
            void set_start(string_scanner::pos&);
            void set_end(string_scanner::pos&);
      };
   }
}