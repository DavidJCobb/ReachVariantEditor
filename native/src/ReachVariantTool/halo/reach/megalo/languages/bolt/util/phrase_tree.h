#pragma once
#include <array>
#include <cstdint>
#include <limits>
#include <optional>
#include <string_view>
#include "helpers/string/strlen.h"
#include "helpers/bitmask_t.h"
#include "helpers/cs.h"

#include <string>
#include <vector>

namespace halo::reach::megalo::bolt::util {

   //
   // The goal of the phrase_tree class is to create a compile-time node tree of 
   // words in a list of phrases, such that it's possible to generate if/else trees 
   // to check a phrase against the tree.
   //
   // For example, given the following list of phrases:
   // 
   //    each object
   //    each object of type
   //    each object with label
   //    each player
   //    each player randomly
   //    each team
   // 
   // We should generate a node tree that looks like this:
   // 
   //    each
   //     + object // 0
   //     |  + of
   //     |  |  - type // 1
   //     |  + with
   //     |     - label // 2
   //     + player // 3
   //     |  - randomly // 4
   //     + team // 5
   // 
   // It should be possible to automatically generate code like this:
   // 
   //    // given const std::vector<QString>& words:
   // 
   //    size_t phrase = -1;
   //    
   //    if (words.size() > 0 && words[0] == "each") {
   //       if (words.size() > 1) {
   //          if (words[1] == "object") {
   //             if (words.size() == 2) {
   //                phrase = 0;
   //             } else {
   //                if (words[2] == "of") {
   //                   if (words[3] == "type" && words.size() == 4) {
   //                      phrase = 1;
   //                   }
   //                } else if (words[2] == "with") {
   //                   if (words[3] == "label" && words.size() == 4) {
   //                      phrase = 2;
   //                   }
   //                }
   //             }
   //          } else if (words[1] == "player") {
   //             if (words.size() == 2) {
   //                phrase = 3;
   //             } else {
   //                if (words[2] == "randomly" && words.size() == 3) {
   //                   phrase = 4;
   //                }
   //             }
   //          } else if (words[1] == "team") {
   //             if (words.size() == 2) {
   //                phrase = 5;
   //             }
   //          }
   //       }
   //    }
   //

   template<cobb::cs... Phrases> class phrase_tree {
      public:
         using size_type  = uint8_t;
         using index_type = uint8_t;
         static constexpr index_type none = std::numeric_limits<index_type>::max();

      public:
         static constexpr size_t phrase_count = sizeof...(Phrases);
         static constexpr auto phrases = std::array{ Phrases.c_str()... };

         static constexpr auto phrase_blob = (Phrases + ...);
         static constexpr auto phrase_starts = []() -> std::array<size_t, phrase_count> {
            std::array<size_t, phrase_count> out = {};
            size_t i    = 0;
            size_t size = 0;
            ((out[i++] = size, size += Phrases.size()), ...);
            return out;
         }();
         
      public:
         struct word {
            size_type start = 0;
            size_type size  = 0;

            constexpr word() {}
            constexpr word(size_type a, size_type b) : start(a), size(b) {}
            constexpr bool operator==(const word&) const noexcept = default;

            constexpr bool empty() const noexcept { return size == 0; }
         };

         //
         // Constexpr code cannot heap-allocate objects, so (outside of trickier metaprogramming) 
         // a constexpr data structure cannot be variable-length. This means that our node tree 
         // must be backed by a flat list (with a fixed capacity), and each node must identify 
         // its children using a fixed number of indices. In practice, that works out to a start 
         // index and a count, which in turn means that a node's children must be contiguous in 
         // "memory."
         //
         struct node {
            word data = {};
            struct {
               index_type start = none;
               index_type count = none;
            } children;
            bool terminator = false;
         };

         const char* buffer = phrase_blob.c_str();
         std::array<node,   12> all_nodes       = {};
         std::array<size_t, 12> top_level_nodes = {};

         constexpr phrase_tree() {
            for (auto& item : top_level_nodes)
               item = none;

            //using similarity_mask_t = cobb::bitmask_t<phrase_count>;
            using similarity_mask_t = uint32_t; // MSVC is broken
            
            struct phrase_state {
               size_t            word_index        = 0;
               uint32_t similarity_mask   = (uint32_t(1) << (phrase_count + 1)) - 1;
               index_type        last_created_node = none;
            };
            struct word_info {
               word data = {};
               std::string_view view;

               constexpr bool empty() const noexcept { return data.empty(); }
            };

            std::array<phrase_state, phrase_count> phrase_states = {};

            auto _get_nth_word = [](const char* phrase, size_t word_index) -> word_info {
               word_info out;
               out.view = std::string_view(phrase);

               size_t start = 0;
               size_t size  = 0;

               size_t w    = 0;
               size_t last = 0;
               while (w <= word_index) {
                  auto i = out.view.find_first_of(' ', last);
                  if (w == word_index) {
                     if (i == std::string_view::npos) {
                        start = last;
                        size  = out.view.size() - last;
                        break;
                     }
                     start = last;
                     size  = i - last;
                     break;
                  }
                  if (i < std::string_view::npos)
                     break;
                  last = i + 1;
                  ++w;
               }

               out.data = word(start, size);
               if (size)
                  out.view = out.view.substr(start, size);
               return out;
            };
            auto _get_word_content = [this](const word_info& info) -> std::string_view {
               return std::string_view(this->buffer).substr(info.data.start, info.data.size);
            };

            size_t node_index = 0;
            auto _create_node = [this, &node_index]() -> node& {
               auto& n = this->all_nodes[node_index];
               ++node_index;
               return n;
            };

            size_t tlnl_index = 0; // top-level node list index
            for (size_t focus_index = 0; focus_index < phrase_count; ++focus_index) {
               auto& focus_state  = phrase_states[focus_index];
               auto* focus_phrase = phrases[focus_index];

               auto word_index = focus_state.word_index;

               auto focus_word = _get_nth_word(focus_phrase, word_index);
               focus_word.data.start += phrase_starts[focus_index];
               if (focus_word.empty()) {
                  if (focus_state.last_created_node != none) {
                     this->all_nodes[focus_state.last_created_node].terminator = true;
                  }
                  continue;
               }

               auto& focus_node = _create_node();
               focus_node.data = focus_word.data;
               if (focus_state.last_created_node != none) {
                  this->all_nodes[focus_state.last_created_node].children.start = node_index - 1;
                  this->all_nodes[focus_state.last_created_node].children.count = 1;
               } else {
                  this->top_level_nodes[tlnl_index++] = node_index - 1;
               }
               focus_state.last_created_node = node_index - 1;
               ++focus_state.word_index;

               similarity_mask_t ruled_out_this_time = 0;
               for (size_t other_index = 0; other_index < phrase_count; ++other_index) {
                  if (other_index == focus_index)
                     continue;
                  auto& other_state  = phrase_states[other_index];
                  auto* other_phrase = phrases[other_index];

                  if ((other_state.similarity_mask & (1 << focus_index)) == 0) {
                     //
                     // We've already determined that this phrase doesn't overlap the subject phrase.
                     //
                     continue;
                  }
                  //
                  // Remaining phrases are the "phrases under consideration." See if any diverge from 
                  // the subject, and if so, clear their similarity bit with the subject and track 
                  // them.
                  //
                  auto other_word = _get_nth_word(other_phrase, word_index);
                  other_word.data.start += phrase_starts[other_index];
                  if (!other_word.empty()) {
                     if (_get_word_content(other_word) == _get_word_content(focus_word)) {
                        other_state.last_created_node = focus_state.last_created_node;
                        ++other_state.word_index;
                        continue;
                     }
                     ruled_out_this_time |= (1 << other_index);
                  } else {
                     if (other_state.last_created_node != none)
                        this->all_nodes[other_state.last_created_node].terminator = true;
                  }
                  focus_state.similarity_mask &= ~(1 << other_index);
                  other_state.similarity_mask &= ~(1 << focus_index);
               }
               if (ruled_out_this_time != 0) {
                  //
                  // We found some phrases that diverge from the subject at this word index. We should 
                  // now create the nodes for their current word, and test them against each other as 
                  // we do so.
                  //
                  this->all_nodes[focus_state.last_created_node].children.start = node_index;
                  for (size_t other_index = 0; other_index < phrase_count; ++other_index) {
                     if ((ruled_out_this_time & (1 << other_index)) == 0)
                        continue;
                     auto& other_state  = phrase_states[other_index];
                     auto* other_phrase = phrases[other_index];

                     auto  other_word = _get_nth_word(other_phrase, word_index);
                     other_word.data.start += phrase_starts[other_index];

                     //
                     // Loop over all ruled-out-this-time phrases that come before this one in the list. 
                     // See if we've already created a node for this word.
                     //
                     bool  already = false;
                     for (size_t prior_index = 0; prior_index < other_index; ++prior_index) {
                        if ((ruled_out_this_time & (1 << prior_index)) == 0)
                           continue;
                        auto& prior_state  = phrase_states[prior_index];
                        auto* prior_phrase = phrases[prior_index];
                        auto  prior_word   = _get_nth_word(prior_phrase, word_index);
                        prior_word.data.start += phrase_starts[prior_index];
                        if (!prior_word.empty() && _get_word_content(prior_word) == _get_word_content(other_word)) {
                           already = true;

                           other_state.last_created_node = prior_state.last_created_node;
                           other_state.word_index        = prior_state.word_index;
                        } else {
                           other_state.similarity_mask &= ~(1 << prior_index);
                           prior_state.similarity_mask &= ~(1 << other_index);
                        }
                     }
                     if (!already) {
                        auto& other_node = _create_node();
                        other_node.data = word();
                        if (other_state.last_created_node != none) {
                           this->all_nodes[other_state.last_created_node].children.start = node_index - 1;
                           this->all_nodes[other_state.last_created_node].children.count = 1;
                        } else {
                           this->top_level_nodes[tlnl_index++] = node_index - 1;
                        }
                        other_state.last_created_node = node_index - 1;
                        ++other_state.word_index;
                        ++this->all_nodes[focus_state.last_created_node].children.count;
                     }
                  }
               }
               // Done.
            }
         }
   };
}