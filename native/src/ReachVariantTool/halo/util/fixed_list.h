#pragma once
#include <array>
#include "../../helpers/type_traits/set_const.h"
#include "../../helpers/bitnumber.h"

namespace halo::util {
   namespace impl::fixed_list {
      template<typename T> concept has_index = requires (T& x) {
         { x.index };
      };
      template<typename T> concept is_default_constructible;
   }

   template<typename T, size_t M> class fixed_list {
      //
      // - Does not allow the list to exceed a maximum size
      // 
      // - Heap-allocates all list items, so that changes to the list order do not 
      //   invalidate inbound pointers; the list, then, is a pointer list
      // 
      // - Under the hood, iterators refer to slots in the pointer list; iterators 
      //   are not invalidated if the list is rearranged
      // 
      // - If the value type has an "index" member, updates that member to refer 
      //   to each value's position in the list
      //
      public:
         using value_type = T;
         using entry_type = value_type*;
         using size_type  = size_t;
         using reference  = value_type&;
         using const_reference = const value_type&;

         static constexpr size_t max_count = M;

      protected:
         static constexpr bool _is_indexed = impl::fixed_list::has_index<T>;

      protected:
         std::array<entry_type, max_count> _data = {};
         size_type _size = 0;

         static void _update_index(value_type& item, size_type index) {
            if constexpr (_is_indexed) {
               item.index = index;
            }
         }

      protected:
         template<bool is_const> class _iterator_base {
            protected:
               using access_type = cobb::set_const<value_type**, is_const>;
               using pointer     = access_type*;
               using reference   = access_type&;

               pointer item = nullptr;

            public:
               _iterator_base() {}
               _iterator_base(std::nullptr_t) : item(nullptr) {}
               _iterator_base(pointer v) : item(v) {}
               _iterator_base(void* v) : item((value_type*)v) {}
               _iterator_base(reference r) : item(&r) {}

               reference operator->() const noexcept { return *this->item; }
               reference operator*() const noexcept { return **this->item; }
               reference operator[](size_t i) const noexcept { return **(this->item + i); }

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
               
               auto operator<=>(const iterator&) const = default;
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
            return It(&this->_data[i]);
         }

      public:
         fixed_list() {}

         const std::array<entry_type, max_count>& data() const { return _data; }
         inline bool empty() const { return _size == 0; }
         inline bool full() const { return _size >= max_count; }

         inline reference operator[](size_type i) noexcept { return *this->_data[i]; }
         inline const_reference operator[](size_type i) const noexcept { return *this->_data[i]; }

         reference back()  noexcept { return operator[](this->_size - 1); }
         reference front() noexcept { return operator[](0); }

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
            _update_index(*item, this->_size);
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
            dst = src;
            _update_index(*dst, this->_size);
            ++this->_size;
            return true;
         }
         void swap_items(size_type a, size_type b) {
            auto& x = this->_data[a];
            auto& y = this->_data[b];
            std::swap(x, y);
            if constexpr (_is_indexed) {
               std::swap(x->index, y->index);
            }
         }
         entry_type take(size_type i) { // Remove an item from the list, and give ownership to the caller.
            entry_type item = this->_data[i];
            if (i + 1 < this->_size) {
               //
               // Items past the one we've removed need to be shifted back.
               //
               for (size_type j = i; j < this->_size - 1; ++j) {
                  this->_data[j] = this->_data[j + 1];
                  _update_index(*this->_data[j], j);
               }
               this->_data.back() = nullptr;
            }
            --this->_size;
            return item;
         }

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
   };
}