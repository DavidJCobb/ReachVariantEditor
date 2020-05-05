#pragma once
#include "../formats/localized_string_table.h"
#include <QString>

class LocalizedStringLibrary {
   public:
      static LocalizedStringLibrary& get() {
         static LocalizedStringLibrary instance;
         return instance;
      }
      //
   public:
      struct Entry {
         struct permutation {
            uint32_t     integer; // applies to both signs of the number, i.e. integer 1 also applies to -1
            ReachString* content = nullptr; // must NOT be nullptr at run-time (would use a reference but std::vector can't cope with that)
            //
            permutation();
            ~permutation();
         };
         //
         Entry();
         ~Entry();
         //
         QString internal_name;
         QString friendly_name;
         QString description;
         QString category;
         QString source;
         QString source_type;
         std::vector<QString> tags;
         ReachString* content = nullptr; // must NOT be nullptr at run-time (would use a reference but std::vector can't cope with that)
         std::vector<permutation> permutations; // e.g. for "X points", the (content) would be "%1 points" and there'd be permutations for {0, "no points"} and {1, "%1 point"}.
         //
         void copy(reach::language, QString&);
         void apply_permutation(reach::language, QString&, int32_t value);
         //
         permutation* get_or_create_permutation(uint32_t v);
      };
      //
   protected:
      std::vector<Entry*> entries;
      //
   public:
      LocalizedStringLibrary();
      ~LocalizedStringLibrary();
      //
      inline const Entry* operator[](int i) const noexcept {
         if (i >= 0 && i <= this->size())
            return this->entries[i];
      }
      inline size_t size() const noexcept { return this->entries.size(); }
};