#pragma once
#include <array>
#include <string>
#include <type_traits>
#include <QLatin1String>

namespace halo::reach::megalo::bolt {
   enum class event_type {
      init,
      local_init,
      host_migration,
      double_host_migration,
      object_death,
      local,
      pregame,
   };

   // NOTE: These won't preview in IntelliSense the way you expect. Instead of 
   // showing the words distinctly, it'll display the raw QLatin1String data, 
   // i.e. "{ length, original_string_literal + offset }".
   struct event_type_phrase {
      size_t count = 0;
      std::array<QLatin1String, 3> words;

      constexpr event_type_phrase(const char* str) {
         size_t a = 0;
         size_t i = 0;
         while (true) {
            auto c = str[i];
            if (c == '\0') {
               if (i)
                  this->words[a++] = QLatin1String(str, i);
               break;
            }
            if (c == ' ') {
               #pragma warning(suppress: 28020) // IntelliSense gets confused by the postfix-increment on the array index.
               this->words[a++] = QLatin1String(str, i);
               str += i + 1;
               i = 0;
               continue;
            }
            ++i;
         }
         this->count = a;
         for (; a < this->words.size(); ++a)
            this->words[a] = QLatin1String("", 0);
      }

      constexpr const QLatin1String& operator[](size_t i) const { return this->words[i]; }
      constexpr size_t size() const noexcept { return this->count; }
   };

   constexpr auto event_type_phrases = std::array{
      event_type_phrase("init"),
      event_type_phrase("local init"),
      event_type_phrase("host migration"),
      event_type_phrase("double host migration"),
      event_type_phrase("object death"),
      event_type_phrase("local"),
      event_type_phrase("pregame"),
   };
}