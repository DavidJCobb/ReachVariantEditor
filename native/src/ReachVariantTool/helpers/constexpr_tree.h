#pragma once
#include <array>
#include <cstdint>
#include <tuple>
#include <type_traits>
#include "./tuple_for_each_value.h"
#include "./tuple_prepend.h"
#include "./tuple_unpack.h"

/*
   CONSTEXPR TREES

   A set of helper templates to allow you to create constexpr tree structures 
   without the need for heap allocation. Here's an example:

      struct userdata { int id; };

      constexpr auto example_tree = cobb::constexpr_tree(
         cobb::constexpr_tree_node(
            userdata{ 1 },
            std::make_tuple(
               cobb::constexpr_tree_node(userdata{ 2 }),
               cobb::constexpr_tree_node(userdata{ 3 }),
               cobb::constexpr_tree_node(userdata{ 4 }),
               cobb::constexpr_tree_node(userdata{ 5 },
                  std::make_tuple(
                     cobb::constexpr_tree_node(userdata{ 6 }),
                     cobb::constexpr_tree_node(userdata{ 7 })
                  )
               )
            )
         )
      );

   The way it works is that you define your nodes as cobb::constexpr_tree_node, 
   and pass the top-level nodes as the arguments to the cobb::constexpr_tree 
   constructor. We use class template argument deduction to specialize the tree 
   class based on the total number of nodes in the tree data.

   The cobb::constexpr_tree_node structures are not retained; they are only used 
   for initial setup. The final tree structure uses a flat array of "stored node" 
   structures, which contain each node's data along with information on where the 
   node's children are located within the array. However, all access to nodes in 
   the tree is via wrapper objects which emulate the tree structure based on the 
   underlying data:

      constexpr auto sum_of_ids = [](){
         int id = 0;
         for (const auto child_node : example_tree[0].children()) {
            id += child_node.data().id;
         }
         return id;
      }();

   Within the underlying storage, all child nodes of some given parent node are 
   arranged contiguously. This avoids the need for variable-length arrays of 
   child node indices -- a necessary measure since variable-length anything is 
   impossible in constexpr data definitions. Ergo, the following tree:

    - A
       - Ax
       - Ay
    - B
       - Bx
    - C
    - D
    - E
       - Ex
       - Ey
          - Ey1
          - Ey2
       - Ez

   has its nodes stored in the following order:

      A B C D E Ax Ay Bx Ex Ey Ez Ey1 Ey2

*/

namespace cobb {
   // Definition for a node in a constexpr tree. If you wish to add child nodes, 
   // pass a tuple of node definitions (via std::make_tuple) as the second argument.
   template<typename T, typename... ChildTypes> struct constexpr_tree_node {
      using tuple_type = std::tuple<ChildTypes...>;
      using value_type = T;

      static constexpr size_t child_count = sizeof...(ChildTypes);

      tuple_type children = {};
      value_type data;

      consteval constexpr_tree_node(const value_type& v) requires (sizeof...(ChildTypes) == 0) : data(v) {}
      consteval constexpr_tree_node(const value_type& v, const tuple_type& m) requires (sizeof...(ChildTypes) > 0) : data(v), children(m) {}
   };
   template<size_t N> constexpr_tree_node(const char(&)[N]) -> constexpr_tree_node<const char*>; // CTAD for string literals as node data

   template<typename T> concept is_constexpr_tree_node_definition = requires {
      typename std::decay_t<T>::tuple_type;
      typename std::decay_t<T>::value_type;
      requires std::is_same_v<
         std::decay_t<T>,
         cobb::tuple_unpack_t<
            constexpr_tree_node,
            cobb::tuple_prepend<typename std::decay_t<T>::value_type, typename std::decay_t<T>::tuple_type>
         >
      >;
   };

   namespace impl::constexpr_tree {
      template<typename Definition> struct descendant_count;
      template<typename T> struct descendant_count<constexpr_tree_node<T>> {
         static constexpr size_t value = 0;
      };
      template<typename T, typename... ChildTypes> struct descendant_count<constexpr_tree_node<T, ChildTypes...>> {
         using definition_type = constexpr_tree_node<T, ChildTypes...>;

         template<typename T> static consteval size_t _worker() {
            if constexpr (is_constexpr_tree_node_definition<T>) {
               return descendant_count<T>::value;
            } else {
               return 0;
            }
         }

         static constexpr size_t value = sizeof...(ChildTypes) + (_worker<std::decay_t<ChildTypes>>() + ...);
      };

      template<typename T> constexpr size_t descendant_count_v = descendant_count<T>::value;
   }

   template<typename T, size_t Count, size_t TopLevelCount> struct constexpr_tree {
      public:
         using value_type = T;

      protected:
         // Use the smallest possible numeric type to hold node indices. This will 
         // reduce the size of the constexpr node storage.
         using index_type = std::conditional_t<
            std::bit_width(Count) <= 8,
            uint8_t,
            std::conditional_t<
               std::bit_width(Count) <= 16,
               uint16_t,
               std::conditional_t<
                  std::bit_width(Count) <= 32,
                  uint32_t,
                  size_t
               >
            >
         >;

         struct stored_node {
            T data = {};
            struct {
               index_type start = 0;
               index_type count = 0;
            } children;
         };

         std::array<stored_node, Count> nodes = {};

      protected:
         struct _initialization_state {
            size_t count_stored = 0;
         };

         template<typename Node> constexpr size_t append_children_of(stored_node& stored, const Node& src, _initialization_state& state) {
            static_assert(Node::child_count == std::tuple_size_v<std::decay_t<decltype(Node::children)>>);

            stored.children.start = state.count_stored;
            stored.children.count = Node::child_count;
            cobb::tuple_for_each_value(src.children, [this, &state]<typename ChildNodeType>(const ChildNodeType& child) {
               auto& dst = this->nodes[state.count_stored++];
               dst.data = child.data;
            });

            size_t i = stored.children.start;
            cobb::tuple_for_each_value(src.children, [this, &i, &state]<typename ChildNodeType>(const ChildNodeType& child) {
               if constexpr (ChildNodeType::child_count > 0) {
                  auto& dst = this->nodes[i];
                  this->append_children_of(dst, child, state);
               }
               ++i;
            });

            return i;
         }

         template<typename Node> constexpr void append_top_level_node(const Node& src, _initialization_state& state) {
            auto& dst = this->nodes[state.count_stored++];
            dst.data = src.data;
         }

      public:
         template<typename... Nodes> consteval constexpr_tree(const Nodes&... nodes) {
            _initialization_state state;
            (this->append_top_level_node(nodes, state), ...);

            size_t i = 0;
            (this->append_children_of(this->nodes[i++], nodes, state), ...);
         }

      public:
         class node {
            friend class constexpr_tree;
            protected:
               const constexpr_tree& tree;
               const stored_node&    wrapped;

               constexpr node(const constexpr_tree& tree, const stored_node& wrapped) : tree(tree), wrapped(wrapped) {}

               class node_child_list {
                  friend class node;
                  protected:
                     const constexpr_tree& tree;
                     const stored_node&    wrapped;

                     constexpr node_child_list(const constexpr_tree& tree, const stored_node& wrapped) : tree(tree), wrapped(wrapped) {}

                     struct iterator {
                        const constexpr_tree& tree;
                        const stored_node&    wrapped;
                        //
                        index_type index = 0;

                        constexpr bool operator==(const iterator& other) const {
                           if (&this->tree != &other.tree)
                              return false;
                           if (&this->wrapped != &other.wrapped)
                              return false;
                           return this->index == other.index;
                        };

                        constexpr iterator& operator+=(size_t i) { index += i; return *this; }
                        constexpr iterator& operator-=(size_t i) { index -= i; return *this; }
                        constexpr iterator& operator++() { return *this += 1; }
                        constexpr iterator& operator--() { return *this -= 1; }
                        constexpr iterator operator+(size_t i) const { return (iterator(*this) += i); }
                        constexpr iterator operator-(size_t i) const { return (iterator(*this) -= i); }
                        constexpr iterator operator++(int) const { return (iterator(*this) += 1); }
                        constexpr iterator operator--(int) const { return (iterator(*this) -= 1); }

                        constexpr const node operator*() const {
                           return tree._stored_node_at(wrapped.children.start + index);
                        }
                     };

                  public:
                     constexpr size_t size() const noexcept { return wrapped.children.count; }
                     constexpr bool empty() const noexcept { return size() == 0; }

                     constexpr const node at(size_t i) const {
                        if (i >= size())
                           throw;
                        return tree._stored_node_at(wrapped.children.start + i);
                     }
                     constexpr const node operator[](size_t i) const noexcept {
                        if (std::is_constant_evaluated()) {
                           if (i >= size())
                              #pragma warning(suppress: 4297) // Function is marked noexcept but has a throw statement. (We only throw when constant-evaluated.)
                              throw;
                        }
                        return tree._stored_node_at(wrapped.children.start + i);
                     }

                     constexpr const node front() const { return (*this)[0]; }
                     constexpr const node back() const { return (*this)[size() - 1]; }

                     constexpr iterator begin() const { return iterator{ tree, wrapped, 0 }; }
                     constexpr iterator end() const { return iterator{ tree, wrapped, wrapped.children.count }; }
                     constexpr iterator cbegin() const { return begin(); }
                     constexpr iterator cend() const { return end(); }
               };

            public:
               constexpr const value_type& data() const { return wrapped.data; }
               constexpr node_child_list children() const {
                  return node_child_list{ tree, wrapped };
               }
         };

      protected:
         constexpr const node _stored_node_at(size_t i) const {
            return node(*this, this->nodes[i]);
         }
         
         struct top_level_node_list_iterator {
            const constexpr_tree& tree;
            index_type index = 0;

            constexpr bool operator==(const top_level_node_list_iterator& other) const {
               if (&this->tree != &other.tree)
                  return false;
               return this->index == other.index;
            };

            constexpr top_level_node_list_iterator& operator+=(size_t i) { index += i; return *this; }
            constexpr top_level_node_list_iterator& operator-=(size_t i) { index -= i; return *this; }
            constexpr top_level_node_list_iterator& operator++() { return *this += 1; }
            constexpr top_level_node_list_iterator& operator--() { return *this -= 1; }
            constexpr top_level_node_list_iterator operator+(size_t i) const { return (top_level_node_list_iterator(*this) += i); }
            constexpr top_level_node_list_iterator operator-(size_t i) const { return (top_level_node_list_iterator(*this) -= i); }
            constexpr top_level_node_list_iterator operator++(int) const { return (top_level_node_list_iterator(*this) += 1); }
            constexpr top_level_node_list_iterator operator--(int) const { return (top_level_node_list_iterator(*this) -= 1); }

            constexpr const node operator*() const {
               return tree._stored_node_at(index);
            }
         };

      public:
         constexpr size_t size() const noexcept { return TopLevelCount; }

         constexpr node operator[](size_t i) const {
            if (i >= size())
               throw;
            return _stored_node_at(i);
         }

         constexpr const node front() const { return (*this)[0]; }
         constexpr const node back() const { return (*this)[size() - 1]; }

         constexpr top_level_node_list_iterator begin() const { return top_level_node_list_iterator{ tree, 0 }; }
         constexpr top_level_node_list_iterator end() const { return top_level_node_list_iterator{ tree, tree.size() }; }
         constexpr top_level_node_list_iterator cbegin() const { return begin(); }
         constexpr top_level_node_list_iterator cend() const { return end(); }
   };
   template<typename... Nodes> constexpr_tree(const Nodes&... nodes) -> constexpr_tree<
      typename std::tuple_element_t<
         0,
         std::tuple<Nodes...>
      >::value_type,
      sizeof...(Nodes) + (impl::constexpr_tree::descendant_count<Nodes>::value + ...),
      sizeof...(Nodes)
   >;
}