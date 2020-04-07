#pragma once
#include <cstdint>
#include <stdexcept>
#include <QString>
#include "../string_scanner.h"

namespace Megalo {
   class Compiler;
   class compile_exception : public std::exception {
      private:
         QString reason;
      public:
         compile_exception(const QString& reason) : std::exception(""), reason(reason) {} // TODO: try using a QString instead so we can support Unicode and so our code is cleaner
         //
         [[nodiscard]] virtual char const* what() const {
            return "this is a QString";
         }
         const QChar* why() const noexcept { return reason.constData(); }
         const QString& why_ref() const noexcept { return reason; }
   };
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