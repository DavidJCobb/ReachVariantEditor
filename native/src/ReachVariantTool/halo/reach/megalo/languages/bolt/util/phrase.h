#pragma once
#include <array>
#include <cstdint>
#include <type_traits>
#include <QLatin1String>

namespace halo::reach::megalo::bolt::util {

   // generic version of event_type_phrase; not sure I need this yet
   class phrase {
      protected:
         using size_type = uint8_t;
         struct word {
            size_type start = 0;
            size_type size  = 0;

            constexpr word() {}
            constexpr word(size_type a, size_type b) : start(a), size(b) {}
         };

         const char* string = nullptr;
         size_t      count = 0;
         std::array<word, 6> words;

      public:
         constexpr phrase(const char* str) {
            this->string = str;
            //
            size_t a = 0;
            size_t i = 0;
            while (true) {
               auto c = str[i];
               if (c == '\0') {
                  if (i)
                     this->words[a++] = word(str - this->string, i);
                  break;
               }
               if (c == ' ') {
                  #pragma warning(suppress: 28020) // IntelliSense gets confused by the postfix-increment on the array index.
                  this->words[a++] = word(str - this->string, i);
                  str += i + 1;
                  i = 0;
                  continue;
               }
               ++i;
            }
            this->count = a;
            for (; a < this->words.size(); ++a)
               this->words[a] = {};
         }

         constexpr const QLatin1String operator[](size_t i) const { return QLatin1String(this->string + this->words[i].start, this->words[i].size); }
         constexpr size_t size() const noexcept { return this->count; }
   };

}