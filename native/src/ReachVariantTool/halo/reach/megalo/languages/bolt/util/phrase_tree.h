#pragma once
#include <array>
#include <cstdint>
#include <limits>
#include "helpers/string/strlen.h"
#include "helpers/y_combinator.h"

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

   class phrase_tree {
      public:
         using size_type  = uint8_t;
         using index_type = uint8_t;
         static constexpr index_type none = std::numeric_limits<index_type>::max();
         
      public:
         struct word {
            size_type start = 0;
            size_type size  = 0;

            constexpr word() {}
            constexpr word(size_type a, size_type b) : start(a), size(b) {}
            constexpr bool operator==(const word&) const noexcept = default;
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
            word data;
            struct {
               index_type start = none;
               index_type count = none;
            } children;
         };

         std::string buffer;
         std::array<node,   12> all_nodes;
         std::array<size_t, 12> top_level_nodes;

         template<size_t Count> constexpr phrase_tree(const std::array<const char*, Count>& phrases) {
            for (const auto* phrase : phrases)
               buffer += phrase;
            
            //
            // First attempted implementation: build a heap-allocated node tree the conventional 
            // way; then construct an equivalent constexpr tree and delete the heap-allocated 
            // tree. Fails due to internal compiler bugs in MSVC which manifest as absurdly 
            // wrong error messages.
            //

            struct temp_node {
               word data = {};
               std::vector<temp_node*> children;
               
               constexpr temp_node() {}
               ~temp_node() { for(auto* item : children) delete item; }
            };
            std::vector<temp_node*> tops;
            
            size_t accum = 0;
            for (size_t i = 0; i < phrases.size(); ++i) {
               auto*  phrase  = phrases[i];
               auto   strlen  = cobb::strlen(phrase);

               auto   size = strlen + 1;
               size_t last = 0;
               temp_node* current = nullptr;
               for (size_t j = 0; j < size; ++j) {
                  if (phrase[j] == ' ' || phrase[j] == '\0') {
                     word here = word(accum + last, accum + j - last);
                     last = j + 1;

                     temp_node* next = nullptr;
                     if (current) {
                        bool exists = false;
                        for (auto* existing : current->children) {
                           if (existing->data == here) {
                              next   = existing;
                              exists = true;
                              break;
                           }
                        }
                        if (!exists) {
                           auto* child = next = new temp_node;
                           child->data = here;
                           current->children.push_back(child);
                        }
                     } else {
                        auto* child = next = new temp_node;
                        child->data = here;
                        tops.push_back(child);
                     }

                     current = next;
                     if (phrase[j] == '\0')
                        break;
                  }
               }
               accum += strlen;
            }
            
            size_t ni = 0;
            size_t ti = 0;
            for(temp_node* item : tops) {
               auto dst_i = ni;

               auto nested = [this, &ni](auto& self, const temp_node* src) mutable -> void {
                  this->all_nodes[ni++].data = src->data;
                  for (const temp_node* child : src->children) {
                     self(child);
                  }
               };
               auto wrapper = cobb::y_combinator{ nested };
               wrapper(item); // this fails: MSVC thinks the lambda's second argument is const int for some reason
               /*/
               auto lambda = cobb::y_combinator{[this, &ni](auto& self, const temp_node* src) mutable -> void { // this fails: MSVC thinks the lambda's second argument is const int for some reason
                  this->all_nodes[ni++].data = src->data;
                  for (const temp_node* child : src->children) {
                     self(child);
                  }
               }};
               lambda(item);
               //*/
               this->top_level_nodes[ti++] = dst_i;
            }
            
            for(auto* item : tops)
               delete item;
         }
   };

   // reference implementation (i.e. non-constexpr)
   /*
   class phrase_tree {
      public:
         using size_type  = uint8_t;
         using index_type = uint8_t;
         static constexpr index_type none = std::numeric_limits<index_type>::max();
         
      public:
         struct word {
            size_type start = 0;
            size_type size  = 0;
         };

         struct node {
            word data;
            struct {
               index_type start = none;
               index_type count = none;
            } children;
         };

         std::string buffer;
         std::array<node,   12> all_nodes;
         std::array<size_t, 12> top_level_nodes;

         template<size_t Count> constexpr phrase_tree(const std::array<const char*, Count>& phrases) {
            for (const auto* phrase : phrases)
               buffer += phrase;
            
            struct temp_node {
               word data;
               std::vector<temp_node*> children;
               
               ~temp_node() { for(auto* item : children) delete item; }
            };
            std::vector<temp_node*> tops;
            
            size_t accum = 0;
            for (size_t i = 0; i < phrases.size(); ++i) {
               auto* current = nullptr;
               auto* phrase  = phrases[i];
               auto  size    = strlen(phrase) + 1;
               size_t last = 0;
               for (size_t j = 0; j < size; ++j) {
                  if (phrase[j] == ' ' || phrase[j] == '\0') {
                     word here = word{ accum + last, accum + j - last };

                     temp_node* next = nullptr;
                     if (current) {
                        bool exists = false;
                        for (auto* existing : current->children) {
                           if (existing->data == here) {
                              next   = existing;
                              exists = true;
                              break;
                           }
                        }
                        if (!exists) {
                           auto* child = next = new temp_node;
                           child->data = here;
                           current->children.push_back(child);
                        }
                     } else {
                        auto* child = next = new temp_node;
                        child->data = here;
                        tops.push_back(child);
                     }

                     current = next;
                     if (phrase[j] == '\0')
                        break;
                  }
               }
               accum += strlen(phrase);
            }
            
            size_t ni = 0;
            size_t ti = 0;
            auto _traverse = [&ni, this](temp_node& src) {
               auto& dst = this->nodes[ni];
               dst.data = src.data;
               for(auto* child : src.children) {
                  _traverse(*child);
               }
            };
            for(auto* item : tops) {
               auto dst_i = ni;
               _traverse(*item);
               this->top_level_nodes[ti++] = dst_i;
               
            }
            
            for(auto* item : tops)
               delete item;
         }
   };
   */
}