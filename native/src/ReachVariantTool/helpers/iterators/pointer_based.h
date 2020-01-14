#pragma once

namespace cobb {
   namespace iterators {
      namespace pointer_based {
         template<typename value_type> class iterator {
            protected:
               value_type* item = nullptr;
               //
               using reference = value_type&;
               //
            public:
               iterator() {}
               iterator(std::nullptr_t) : item(nullptr) {}
               iterator(value_type* v) : item(v) {}
               iterator(reference r) : item(&r) {}
               value_type operator->() const noexcept { return *item; }
               value_type operator*() const noexcept { return *item; }
               value_type operator[](size_t i) const noexcept { return *(item + i); }
               iterator& operator+=(size_t i) noexcept { this->item += i; return *this; }
               iterator& operator-=(size_t i) noexcept { this->item -= i; return *this; }
               iterator operator+(size_t i) const noexcept {
                  iterator temp = *this;
                  return temp += i;
               }
               iterator operator-(size_t i) const noexcept {
                  iterator temp = *this;
                  return temp -= i;
               }
               iterator& operator++() noexcept { ++this->item; return *this; }
               iterator& operator--() noexcept { --this->item; return *this; }
               //
               bool operator==(const iterator& o) const noexcept { return o.item == this->item; }
               bool operator!=(const iterator& o) const noexcept { return o.item != this->item; }
               bool operator>=(const iterator& o) const noexcept { return o.item >= this->item; }
               bool operator<=(const iterator& o) const noexcept { return o.item <= this->item; }
               bool operator> (const iterator& o) const noexcept { return o.item >  this->item; }
               bool operator< (const iterator& o) const noexcept { return o.item <  this->item; }
         };
         template<typename value_type> class const_iterator {
            protected:
               const value_type* item = nullptr;
               //
               using reference = value_type&;
               //
            public:
               const_iterator() {}
               const_iterator(std::nullptr_t) : item(nullptr) {}
               const_iterator(value_type* v) : item(v) {}
               const_iterator(reference r) : item(&r) {}
               const value_type operator->() const noexcept { return *item; }
               const value_type operator*() const noexcept { return *item; }
               const value_type operator[](size_t i) const noexcept { return *(item + i); }
               const_iterator& operator+=(size_t i) noexcept { this->item += i; return *this; }
               const_iterator& operator-=(size_t i) noexcept { this->item -= i; return *this; }
               const_iterator operator+(size_t i) const noexcept {
                  const_iterator temp = *this;
                  return temp += i;
               }
               const_iterator operator-(size_t i) const noexcept {
                  const_iterator temp = *this;
                  return temp -= i;
               }
               const_iterator& operator++() noexcept { ++this->item; return *this; }
               const_iterator& operator--() noexcept { --this->item; return *this; }
               //
               bool operator==(const const_iterator& o) const noexcept { return o.item == this->item; }
               bool operator!=(const const_iterator& o) const noexcept { return o.item != this->item; }
               bool operator>=(const const_iterator& o) const noexcept { return o.item >= this->item; }
               bool operator<=(const const_iterator& o) const noexcept { return o.item <= this->item; }
               bool operator> (const const_iterator& o) const noexcept { return o.item >  this->item; }
               bool operator< (const const_iterator& o) const noexcept { return o.item <  this->item; }
         };
         template<typename value_type> class reverse_iterator : public iterator<value_type> {
            public:
               using iterator::iterator;
               //
               reverse_iterator& operator+=(size_t i) noexcept { return iterator::operator-=(i); }
               reverse_iterator& operator-=(size_t i) noexcept { return iterator::operator+=(i); }
               reverse_iterator operator+(size_t i) const noexcept { return iterator::operator-(i); }
               reverse_iterator operator-(size_t i) const noexcept { return iterator::operator+(i); }
               reverse_iterator& operator++() noexcept { return iterator::operator--(); }
               reverse_iterator& operator--() noexcept { return iterator::operator++(); }
         };
         template<typename value_type> class const_reverse_iterator : public const_iterator<value_type> {
            public:
               using const_iterator::const_iterator;
               //
               const_reverse_iterator& operator+=(size_t i) noexcept { return const_iterator::operator-=(i); }
               const_reverse_iterator& operator-=(size_t i) noexcept { return const_iterator::operator+=(i); }
               const_reverse_iterator operator+(size_t i) const noexcept { return const_iterator::operator-(i); }
               const_reverse_iterator operator-(size_t i) const noexcept { return const_iterator::operator+(i); }
               const_reverse_iterator& operator++() noexcept { return const_iterator::operator--(); }
               const_reverse_iterator& operator--() noexcept { return const_iterator::operator++(); }
         };
      }
   }
}