#pragma once
#include "../formats/localized_string_table.h"
#include <QString>

class LocalizedStringLibrary {
   public:
      static LocalizedStringLibrary& get() {
         static LocalizedStringLibrary instance;
         return instance;
      }
      LocalizedStringLibrary& operator=(const LocalizedStringLibrary& other) = delete; // no copy-assign
      //
      enum class token_type {
         none,
         integer,
      };
      //
   public:
      struct Entry {
         struct permutation {
            uint32_t     integer; // applies to both signs of the number, i.e. integer 1 also applies to -1
            ReachString* content = nullptr; // must NOT be nullptr at run-time (would use a reference but std::vector can't cope with that)
            //
            permutation();
            ~permutation();
            permutation& operator=(permutation&&) noexcept; // move-assign
            permutation& operator=(const permutation&) noexcept; // copy-assign
            permutation(permutation&& other) { *this = other; } // heaven forbid the compiler auto-generate these
            permutation(const permutation& other) { *this = other; }
         };
         //
         Entry();
         ~Entry();
         //
         QString    internal_name;
         QString    friendly_name;
         QString    description;
         QString    category;
         QString    source;
         QString    source_type;
         token_type token = token_type::none;
         std::vector<QString> tags;
         ReachString* content = nullptr; // must NOT be nullptr at run-time (would use a reference but std::vector can't cope with that)
         std::vector<permutation> permutations; // e.g. for "X points", the (content) would be "%1 points" and there'd be permutations for {0, "no points"} and {1, "%1 point"}.
         //
         void copy(reach::language, QString&);
         void apply_permutation(reach::language, QString&, int32_t value);
         //
         inline bool has_token() const noexcept { return this->token != token_type::none; }
         bool matches(const QString& search) const noexcept;
         //
         Entry& operator=(Entry&&) noexcept; // move-assign
         Entry& operator=(const Entry&) noexcept; // copy-assign
         Entry(Entry&& other) { *this = other; }
         Entry(const Entry& other) { *this = other; }
         //
         permutation* get_or_create_permutation(uint32_t v);
      };
      //
   protected:
      std::vector<Entry*> entries;
      LocalizedStringLibrary();
      //
   public:
      ~LocalizedStringLibrary();
      //
      inline const Entry* operator[](int i) const noexcept {
         if (i >= 0 && i <= this->size())
            return this->entries[i];
         return nullptr;
      }
      inline size_t size() const noexcept { return this->entries.size(); }
};