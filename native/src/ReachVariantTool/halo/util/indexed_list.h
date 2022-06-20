#pragma once
#include <array>
#include <cassert>
#include <stdexcept>
#include <type_traits>
#include <vector>
#include "../../helpers/type_traits/set_const.h"
#include "../../helpers/bitnumber.h"
#include "dummyable.h"
#include "indexed.h"
#include "refcount.h"

namespace halo::util {
   template<typename T, size_t M> requires std::is_base_of_v<indexed, T>
   class indexed_list {
      //
      // - Does not allow the list to exceed a maximum size
      // 
      // - Heap-allocates all list items, so that changes to the list order do not 
      //   invalidate inbound pointers; the list, then, is a pointer list
      // 
      // - Under the hood, iterators refer to slots in the pointer list; iterators 
      //   are not invalidated if the list is rearranged
      // 
      // - Updates items' "index" members to refer to each item's position in the 
      //   list
      // 
      // - Owns the list, and frees items when destroyed
      //
      public:
         using value_type = T;
         using entry_type = value_type*;
         using size_type  = size_t;
         using reference  = value_type&;
         using const_reference = const value_type&;

         static constexpr size_type max_count = M;

      protected:
         std::array<entry_type, max_count> _data = {};
         size_type _size = 0;

      protected:
         template<bool is_const> class _iterator_base {
            friend class indexed_list;
            protected:
               using access_type = std::add_pointer_t<cobb::set_const<value_type*, is_const>>;

               access_type item = nullptr;

            public:
               _iterator_base() {}
               _iterator_base(std::nullptr_t) : item(nullptr) {}
               _iterator_base(value_type** r) : item(r) {}
               _iterator_base(access_type r) requires !std::is_same_v<access_type, value_type**> : item(r) {}

               value_type* operator->() const noexcept { return *this->item; }
               value_type& operator*() const noexcept { return **this->item; }
               value_type& operator[](size_t i) const noexcept { return **(this->item + i); }

               _iterator_base& operator+=(size_t i) noexcept { this->item += i; return *this; }
               _iterator_base& operator-=(size_t i) noexcept { this->item -= i; return *this; }
               _iterator_base operator+(size_t i) const noexcept {
                  return (_iterator_base{ *this }) += i;
               }
               _iterator_base operator-(size_t i) const noexcept {
                  return (_iterator_base{ *this }) -= i;
               }
               _iterator_base& operator++() noexcept { ++this->item; return *this; }
               _iterator_base& operator--() noexcept { --this->item; return *this; }
               
               auto operator<=>(const _iterator_base&) const = default;
         };
         template<bool is_const> class _reverse_iterator_base : public _iterator_base<is_const> {
            public:
               using _iterator_base<is_const>::iterator;
            
               _reverse_iterator_base& operator+=(size_t i) noexcept { return _iterator_base::operator-=(i); }
               _reverse_iterator_base& operator-=(size_t i) noexcept { return _iterator_base::operator+=(i); }
               _reverse_iterator_base operator+(size_t i) const noexcept { return _iterator_base::operator-(i); }
               _reverse_iterator_base operator-(size_t i) const noexcept { return _iterator_base::operator+(i); }
               _reverse_iterator_base& operator++() noexcept { return _iterator_base::operator--(); }
               _reverse_iterator_base& operator--() noexcept { return _iterator_base::operator++(); }
         };
      public:
         using iterator       = _iterator_base<false>;
         using const_iterator = _iterator_base<true>;
         using reverse_iterator       = _reverse_iterator_base<false>;
         using const_reverse_iterator = _reverse_iterator_base<true>;

      protected:
         template<typename It> It _iterator_at(const size_type i) const noexcept {
            if (this->empty())
               return It(nullptr);
            auto v = const_cast<value_type**>(&this->_data[0] + i);
            return It(v);
         }

      public:
         indexed_list() {}
         ~indexed_list() {
            for (auto& item : this->_data) {
               delete item;
               item = nullptr;
            }
            this->_size = 0;
         }

         constexpr const std::array<entry_type, max_count>& data() const { return _data; }
         constexpr bool empty() const { return _size == 0; }
         constexpr bool full() const { return _size >= max_count; }
         constexpr size_type size() const { return _size; }

         constexpr reference operator[](size_type i) noexcept { return *this->_data[i]; }
         constexpr const_reference operator[](size_type i) const noexcept { return *this->_data[i]; }

         reference back()  noexcept { return operator[](this->_size - 1); }
         reference front() noexcept { return operator[](0); }

         #pragma region Helpers for halo::util::dummyable
         template<typename... Args> void set_up_dummies(Args&&... args) requires std::is_base_of_v<dummyable, value_type> {
            for (size_type i = 0; i < max_count; i++) {
               auto* dummy = this->emplace_back(std::forward<Args>(args)...);
               dummy->is_defined = false;
            }
         }

         // Returns true if any referenced dummies exist. Accepts an optional list 
         // which will be filled with the indices of any referenced dummies.
         bool tear_down_dummies(std::vector<size_type>* indices = nullptr, size_t* last_defined_index = nullptr) requires std::is_base_of_v<dummyable, value_type> {
            assert(this->_size == max_count); // dummies should've been created
            bool referenced_dummies = false;
            if (indices) {
               indices->clear();
            }
            for (size_type i = 0; i < max_count; ++i) {
               auto*& iptr = this->_data[max_count - i - 1];
               auto & item = *iptr;
               if (item.is_defined) {
                  if (last_defined_index)
                     *last_defined_index = i;
                  break;
               }
               if constexpr (std::is_base_of_v<passively_refcounted, T>) {
                  if (item.is_referenced()) {
                     referenced_dummies = true;
                     item.is_defined = true;
                     if (indices)
                        indices->push_back(max_count - i - 1);
                     continue;
                  }
               }
               if (!referenced_dummies) {
                  delete iptr;
                  iptr = nullptr;
                  --this->_size;
               }
            }
            return referenced_dummies;
         }

         template<typename... Args> void clear_all_dummy_flags(Args&&... args) requires std::is_base_of_v<dummyable, value_type> {
            for (size_type i = 0; i < this->_size; ++i) {
               auto*& iptr = this->_data[max_count - i - 1];
               auto&  item = *iptr;
               item.is_defined = true;
            }
         }
         #pragma endregion

         void clear() {
            for (auto& item : _data) {
               delete item;
               item = nullptr;
            }
            this->_size = 0;
         }
         template<typename... Args> entry_type emplace_back(Args&&... args) {
            if (this->full())
               return nullptr;
            entry_type item = new value_type(std::forward<Args>(args)...);
            this->_data[this->_size] = item;
            item->index = this->_size;
            if constexpr (std::is_base_of_v<dummyable, value_type>) {
               item->is_defined = true;
            }
            ++this->_size;
            return item;
         }
         void erase(size_type i) {
            delete this->take(i);
         }
         bool push_back(const_reference src) {
            if (this->full())
               return false;
            auto& dst = this->_data[this->_size];
            if (dst) {
               delete dst;
               dst = nullptr;
            }
            dst = new value_type(src);
            if constexpr (std::is_base_of_v<dummyable, value_type>) {
               dst->is_defined = true;
            }
            dst->index = this->_size;
            ++this->_size;
            return true;
         }
         void resize(size_type i) {
            if (i == size())
               return;
            if (i < size()) {
               for (size_t n = i; n < size(); ++n) {
                  delete this->_data[n];
                  this->_data[n] = nullptr;
               }
               this->_size = i;
            }
            if (i > size()) {
               if (i > max_count)
                  throw std::domain_error("desired size exceeded max allowed count");
               for (size_t n = size(); n < i; ++n) {
                  auto* dst = this->_data[n] = new value_type;
                  if constexpr (std::is_base_of_v<dummyable, value_type>) {
                     dst->is_defined = true;
                  }
                  dst->index = n;
               }
               this->_size = i;
            }
         }
         void swap_items(size_type a, size_type b) {
            auto& x = this->_data[a];
            auto& y = this->_data[b];
            std::swap(x, y);
            std::swap(x->index, y->index);
         }
         entry_type take(size_type i) { // Remove an item from the list, and give ownership to the caller.
            entry_type item = this->_data[i];
            if (i + 1 < this->_size) {
               //
               // Items past the one we've removed need to be shifted back.
               //
               for (size_type j = i; j < this->_size - 1; ++j) {
                  this->_data[j] = this->_data[j + 1];
                  this->_data[j]->index = j;
               }
               this->_data.back() = nullptr;
            }
            --this->_size;
            return item;
         }

         #pragma region iterators
         iterator begin() noexcept { return this->_iterator_at<iterator>(0); }
         iterator end()   noexcept { return this->_iterator_at<iterator>(this->_size); }
         const_iterator cbegin() const noexcept { return this->_iterator_at<const_iterator>(0); }
         const_iterator cend()   const noexcept { return this->_iterator_at<const_iterator>(this->_size); }
         reverse_iterator rbegin() noexcept { return this->_iterator_at<reverse_iterator>(this->_size - 1); }
         reverse_iterator rend()   noexcept { return this->_iterator_at<reverse_iterator>(-1); }
         const_reverse_iterator rcbegin() const noexcept { return this->_iterator_at<const_reverse_iterator>(this->_size - 1); }
         const_reverse_iterator rcend()   const noexcept { return this->_iterator_at<const_reverse_iterator>(-1); }
         inline const_iterator begin() const noexcept { return this->cbegin(); }
         inline const_iterator end()   const noexcept { return this->cend(); }
         inline const_reverse_iterator rbegin() const noexcept { return this->rcbegin(); }
         inline const_reverse_iterator rend()   const noexcept { return this->rcend(); }
         #pragma endregion
   };

   template<typename T> concept is_indexed_list = requires {
      typename T::value_type;
      { T::max_count };
      requires std::is_same_v<
         std::decay_t<T>,
         indexed_list<typename T::value_type, T::max_count>
      >;
   };

   template<typename... Types> requires (is_indexed_list<Types> && ...) void set_up_indexed_dummies(Types&... lists) {
      (lists.set_up_dummies(), ...);
   }

   // discouraged; do lists one by one so you can report warnings more meaningfully:
   template<typename... Types> requires (is_indexed_list<Types> && ...) void tear_down_indexed_dummies(Types&... lists) {
      (lists.tear_down_dummies(), ...);
   }

   template<typename... Types> requires (is_indexed_list<Types> && ...) void clear_all_indexed_dummy_flags(Types&... lists) {
      (lists.clear_all_dummy_flags(), ...);
   }
}