#pragma once
#include <array>
#include "helpers/list_items_are_unique.h"
#include "./token_type.h"

namespace halo::reach::megalo::bolt {
   namespace impl {
      struct keyword_to_token {
         const char* keyword;
         token_type  token;

         bool operator==(const keyword_to_token&) const = default;
      };
   };
   constexpr auto keywords_to_token_types = std::array{
      impl::keyword_to_token{ "alias",    token_type::keyword_alias },
      impl::keyword_to_token{ "alt",      token_type::keyword_alt },
      impl::keyword_to_token{ "altif",    token_type::keyword_altif },
      impl::keyword_to_token{ "and",      token_type::keyword_and },
      impl::keyword_to_token{ "declare",  token_type::keyword_declare },
      impl::keyword_to_token{ "do",       token_type::keyword_do },
      impl::keyword_to_token{ "end",      token_type::keyword_end },
      impl::keyword_to_token{ "enum",     token_type::keyword_enum },
      impl::keyword_to_token{ "for",      token_type::keyword_for },
      impl::keyword_to_token{ "function", token_type::keyword_function },
      impl::keyword_to_token{ "if",       token_type::keyword_if },
      impl::keyword_to_token{ "on",       token_type::keyword_on },
      impl::keyword_to_token{ "or",       token_type::keyword_or },
      impl::keyword_to_token{ "pragma",   token_type::keyword_pragma },
      impl::keyword_to_token{ "then",     token_type::keyword_then },
   };
   static_assert(cobb::list_items_are_unique(keywords_to_token_types));
}