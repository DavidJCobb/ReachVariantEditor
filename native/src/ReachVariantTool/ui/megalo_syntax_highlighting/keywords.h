#pragma once
#include <array>
#include <cstdint>
#include <string_view>
#include <QLatin1String>
#include <QStringView>

namespace ui::megalo_syntax_highlighting {
   struct keyword {
      using keyword_index_type = uint8_t;
      static constexpr const keyword_index_type index_of_none = (keyword_index_type)-1;

      std::string_view                  word;
      bool                              is_subkeyword = false;
      bool                              is_wildcard   = false;
      keyword_index_type                index;
      std::array<keyword_index_type, 3> branches_to;
   };

   inline constexpr const auto all_keywords = []() consteval {
      std::array<keyword, 29> list = {};
      size_t i = 0;

      auto _add_keyword = [&list, &i](std::string_view name) constexpr {
         if (i >= list.size())
            throw;
         size_t at    = i;
         auto&  added = list[i++];
         added.word = name;
         if (added.word == "*")
            added.is_wildcard = true;
         for (auto& n : added.branches_to)
            n = keyword::index_of_none;
         return at;
      };
      auto _add_subkeyword = [&list, &i, &_add_keyword](size_t of, std::string_view name) constexpr {
         size_t at = _add_keyword(name);
         list[at].is_subkeyword = true;

         auto& indices = list[of].branches_to;
         bool  added   = false;
         for (size_t j = 0; j < indices.size(); ++j) {
            if (indices[j] == keyword::index_of_none) {
               indices[j] = at;
               added      = true;
               break;
            }
         }
         if (!added)
            throw; // failed: `keyword::branches_to` isn't long enough

         return at;
      };
      auto _add_phrase = [&_add_subkeyword]<size_t Size>(size_t index_of_root, const std::array<const char*, Size>& words) constexpr {
         size_t last = index_of_root;
         for (auto& word : words) {
            last = _add_subkeyword(last, word);
         }
         return last;
      };

      _add_keyword("alias");
      _add_keyword("alt");
      _add_keyword("altif");
      _add_keyword("and");
      size_t i_declare = _add_keyword("declare");
      _add_keyword("do");
      _add_keyword("end");
      _add_keyword("enum");
      size_t i_for = _add_keyword("for");
      _add_keyword("function");
      _add_keyword("if");
      _add_keyword("not");
      _add_keyword("on");
      _add_keyword("or");
      _add_keyword("then");

      {  // declare * with network priority (low|high|local)
         size_t j = _add_phrase(i_declare, std::array{ "*", "with", "network", "priority" });
         _add_subkeyword(j, "low");
         _add_subkeyword(j, "high");
         _add_subkeyword(j, "local");
      }
      {  // for each (player [randomly]|object [with label]|team)
         size_t i_each = _add_subkeyword(i_for, "each");
         _add_phrase(i_each, std::array{ "player", "randomly" });
         _add_phrase(i_each, std::array{ "object", "with", "label" });
         _add_subkeyword(i_each, "team");
      }

      for (size_t i = 0; i < list.size(); ++i)
         list[i].index = i;

      return list;
   }();
   
   #pragma region Compile-time sanity checks
      static_assert([]() {
         for (size_t i = 0; i < all_keywords.size(); ++i) {
            auto& item = all_keywords[i];
            if (item.index != i)
               return false;
         }
         return true;
      }(), "each keyword should know its own index within the list");
      static_assert([]() {
         std::array<bool, all_keywords.size()> subkeywords = {};
         for (size_t i = 0; i < all_keywords.size(); ++i) {
            auto& item = all_keywords[i];
            if (item.is_wildcard && !item.is_subkeyword)
               return false;
         }
         return true;
      }(), "a top-level keyword should not be a wildcard, as that'll make every word test as a keyword");
      static_assert([]() {
         for (size_t i = 0; i < all_keywords.size(); ++i) {
            auto& item = all_keywords[i];
            bool  none_seen = false;
            for (auto v : item.branches_to) {
               if (v == keyword::index_of_none) {
                  none_seen = true;
               } else {
                  if (none_seen)
                     return false;
               }
            }
         }
         return true;
      }(), "a keyword's list of next-keywords should not have a non-none index after a none index");
      static_assert([]() {
         for (size_t i = 0; i < all_keywords.size(); ++i) {
            auto& item = all_keywords[i];
            for (auto v : item.branches_to) {
               if (v != keyword::index_of_none && v >= all_keywords.size())
                  return false;
            }
         }
         return true;
      }(), "a keyword's list of next-keywords should all be in-bounds");
   #pragma endregion

   constexpr const keyword* lookup_keyword_by_name(std::string_view view) {
      for (const auto& k : all_keywords)
         if (k.word == view)
            return &k;
      return nullptr;
   }
   inline const keyword* lookup_keyword_by_name(QStringView view) {
      for (const auto& k : all_keywords) {
         const auto k_view = QLatin1String(k.word.data(), k.word.size());
         if (view == k_view)
            return &k;
      }
      return nullptr;
   }

   constexpr const keyword* lookup_subkeyword_by_name(const keyword& parent, std::string_view view) {
      for (const auto k_idx : parent.branches_to) {
         if (k_idx == keyword::index_of_none)
            break;
         const auto& k = all_keywords[k_idx];
         if (k.is_wildcard)
            return &k;
         if (view == k.word)
            return &k;
      }
      return nullptr;
   }
   inline const keyword* lookup_subkeyword_by_name(const keyword& parent, QStringView view) {
      for (const auto k_idx : parent.branches_to) {
         if (k_idx == keyword::index_of_none)
            break;
         const auto& k = all_keywords[k_idx];
         if (k.is_wildcard)
            return &k;
         const auto k_view = QLatin1String(k.word.data(), k.word.size());
         if (view == k_view)
            return &k;
      }
      return nullptr;
   }
}