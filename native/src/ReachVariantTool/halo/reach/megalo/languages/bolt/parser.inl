#pragma once
#include "helpers/bitmask_t.h"
#include "./parser.h"

namespace halo::reach::megalo::bolt {
   template<token_type... TokenTypes>
   bool parser::_check_token_type_sequence_ahead() const {
      auto& list = this->scanner.tokens;
      auto  i    = this->next_token;
      if (i + sizeof...(TokenTypes) >= list.size())
         return false;
      bool result = true;
      (
         (result ? (result = list[i++].type == TokenTypes) : false),
         ...
      );
      return result;
   }

   template<token_type... TokenTypes>
   size_t parser::_find_any_token_of_types() const {
      auto& list = this->scanner.tokens;
      auto  i    = this->next_token;
      for (; i < list.size(); ++i) {
         bool match = ((match ? match : list[i].type == TokenTypes), ...);
         if (match)
            return i;
      }
      return size_t{ -1 };
   }

   template<token_type Type>
   bool parser::_consume_token_if_present() {
      if (this->_check_next_token(Type)) {
         this->_pull_next_token();
         return true;
      }
      return false;
   }

   template<token_type... TokenTypes>
   bool parser::_consume_any_token_of_types() {
      bool result = false;
      (
         (!result ?
            (this->_check_next_token(TokenTypes) ? (result = true) : false)
         :
            false // no-op
         ),
         ...
      );
      if (result)
         ++this->next_token;
      return result;
   }
   template<auto TokenTypes> requires cobb::is_std_array_of_type<std::decay_t<decltype(TokenTypes)>, token_type>
   bool parser::_consume_any_token_of_types() {
      for (auto t : TokenTypes) {
         if (t == token_type::none)
            break;
         if (this->_check_next_token(t)) {
            ++this->next_token;
            return true;
         }
      }
      return false;
   }

   template<size_t Size>
   void parser::_extract_and_consume_phrase(std::array<QString, Size>& out_phrase, size_t& count_extracted) {
      for (auto& item : out_phrase)
         item.clear();
      //
      size_t i = 0;
      while (this->_peek_next_token().type == token_type::identifier_or_word) {
         auto& token = this->_pull_next_token();

         assert(std::holds_alternative<literal_data_identifier_or_word>(token.literal.value));
         QString word = std::get<literal_data_identifier_or_word>(token.literal.value).content.toLower();

         out_phrase[i] = word;
         if (++i >= Size) {
            break;
         }
      }
      count_extracted = i;
   }

   template<const auto& List> requires cobb::is_std_array_of_type<std::decay_t<decltype(List)>, util::phrase>
   size_t parser::_extract_longest_matching_phrase_of() {
      using possibility_mask_type = cobb::bitmask_t<List.size()>;

      constexpr size_t longest_phrase_length = [](){
         size_t longest = 0;
         for (const auto& phrase : List) {
            if (phrase.size() > longest)
               longest = phrase.size();
         }
         return longest;
      }();

      auto possibility_mask = []() {
         possibility_mask_type mask = 0;
         for (size_t i = 0; i < List.size(); ++i)
            mask |= (1 << i);
         return mask;
      }();

      auto token_count = this->scanner.tokens.size();
      auto token_index = this->next_token;
      auto max_count   = token_count - token_index;
      if (max_count > longest_phrase_length)
         max_count = longest_phrase_length;

      size_t word_count = 0;
      for (; word_count < max_count; ++word_count) {
         const auto& token = this->scanner.tokens[token_index + word_count];
         if (token.type != token_type::identifier_or_word)
            break;

         assert(std::holds_alternative<literal_data_identifier_or_word>(token.literal.value));
         QString word = std::get<literal_data_identifier_or_word>(token.literal.value).content.toLower();

         for (size_t j = 0; j < List.size(); ++j) {
            auto mask = possibility_mask_type{ 1 } << j;
            if ((possibility_mask & mask) == 0)
               continue;

            const auto& phrase = List[j];
            if (word_count >= phrase.size()) {
               possibility_mask &= ~mask;
               continue;
            }
            if (phrase[word_count] != word) {
               possibility_mask &= ~mask;
               continue;
            }
         }
      }

      if (!possibility_mask)
         return npos;
      for (size_t i = 0; i < List.size(); ++i) {
         auto mask = possibility_mask_type{ 1 } << i;
         if ((possibility_mask & mask) == 0)
            continue;

         const auto& phrase = List[i];
         if (phrase.size() == word_count)
            return i;
      }
      return npos;
   }

   template<size_t TierIndex> expression* parser::_try_rule_expression_binary() {
      constexpr auto& tier = binary_operator_tiers[TierIndex];

      auto _get_side = [this]() {
         if constexpr (TierIndex + 1 < binary_operator_tiers.size())
            return this->_try_rule_expression_binary<TierIndex + 1>();
         else
            return this->_try_rule_expression_unary();
      };

      auto* expr = _get_side();
      while (this->_consume_any_token_of_types<tier.operators>()) {
         auto  op  = this->_previous_token();
         auto* rhs = _get_side();
         expr = expression::alloc_binary(expr, op->type, rhs);
      }
      return expr;
   };
}