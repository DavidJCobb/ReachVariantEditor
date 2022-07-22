#pragma once
#include <functional>
#include <optional>
#include <QString>
#include "./literal_number.h"
#include "./token.h"
#include "./token_pos.h"

namespace halo::reach::megalo::AST {
   class scanner {
      public:
         using scan_functor_t = std::function<bool(QChar)>;

      protected:
         QString   source;
         token_pos pos;

      public:
         scanner(const QString& s) : source(s) {}
         scanner(QString&& s) : source(s) {}

         static_assert(false, "TODO: Bring string_scanner stuff here.");

         token_pos backup_stream_state() const;
         void restore_stream_state(const token_pos&);

         void scan(scan_functor_t);

      protected:

         // Does not extract a number literal on its own; the caller needs to check for a sign and base.
         template<size_t Base = 10> requires (Base > 1 && Base <= 36)
         literal_number _try_extract_number_digits();

      public:
         std::optional<literal_number> try_extract_number_literal(bool immediate = false);
   };
}

#include "./scanner.inl"