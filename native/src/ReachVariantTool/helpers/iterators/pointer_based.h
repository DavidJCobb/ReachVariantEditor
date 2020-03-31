/*

This file is provided under the Creative Commons 0 License.
License: <https://creativecommons.org/publicdomain/zero/1.0/legalcode>
Summary: <https://creativecommons.org/publicdomain/zero/1.0/>

One-line summary: This file is public domain or the closest legal equivalent.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/
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
               iterator(void* v) : item((value_type*)v) {}
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
               const_iterator(void* v) : item((value_type*)v) {}
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
         //
         template<typename value_type> class ptr_ref_iterator {
            //
            // This and variants exist to facilitate cobb::indexed_list, which should act as a vector of pointers 
            // but have its iterators return references to the pointed-to objects. This requires having the iterators 
            // wrap pointers in the vector (i.e. they are pointers to pointers) and dereference them twice.
            //
            protected:
               value_type** item = nullptr;
               //
               using reference = value_type&;
               //
            public:
               ptr_ref_iterator() {}
               ptr_ref_iterator(std::nullptr_t) : item(nullptr) {}
               ptr_ref_iterator(value_type*& v) : item(&v) {}
               ptr_ref_iterator(value_type** v) : item(v) {}
               ptr_ref_iterator(void* v) : item((value_type**)v) {}
               value_type* operator->() const noexcept { return *item; }
               reference operator*() const noexcept { return **item; }
               reference operator[](size_t i) const noexcept { return **(item + i); }
               ptr_ref_iterator& operator+=(size_t i) noexcept { this->item += i; return *this; }
               ptr_ref_iterator& operator-=(size_t i) noexcept { this->item -= i; return *this; }
               ptr_ref_iterator operator+(size_t i) const noexcept {
                  iterator temp = *this;
                  return temp += i;
               }
               ptr_ref_iterator operator-(size_t i) const noexcept {
                  iterator temp = *this;
                  return temp -= i;
               }
               ptr_ref_iterator& operator++() noexcept { ++this->item; return *this; }
               ptr_ref_iterator& operator--() noexcept { --this->item; return *this; }
               //
               bool operator==(const ptr_ref_iterator& o) const noexcept { return o.item == this->item; }
               bool operator!=(const ptr_ref_iterator& o) const noexcept { return o.item != this->item; }
               bool operator>=(const ptr_ref_iterator& o) const noexcept { return o.item >= this->item; }
               bool operator<=(const ptr_ref_iterator& o) const noexcept { return o.item <= this->item; }
               bool operator> (const ptr_ref_iterator& o) const noexcept { return o.item >  this->item; }
               bool operator< (const ptr_ref_iterator& o) const noexcept { return o.item <  this->item; }
         };
         template<typename value_type> class ptr_ref_const_iterator {
            protected:
               const value_type** item = nullptr;
               //
               using reference = const value_type&;
               //
            public:
               ptr_ref_const_iterator() {}
               ptr_ref_const_iterator(std::nullptr_t) : item(nullptr) {}
               ptr_ref_const_iterator(value_type*& v) : item(&v) {}
               ptr_ref_const_iterator(value_type** v) : item(v) {}
               ptr_ref_const_iterator(void* v) : item((value_type*)v) {}
               const value_type* operator->() const noexcept { return *item; }
               reference operator*() const noexcept { return **item; }
               reference operator[](size_t i) const noexcept { return **(item + i); }
               ptr_ref_const_iterator& operator+=(size_t i) noexcept { this->item += i; return *this; }
               ptr_ref_const_iterator& operator-=(size_t i) noexcept { this->item -= i; return *this; }
               ptr_ref_const_iterator operator+(size_t i) const noexcept {
                  const_iterator temp = *this;
                  return temp += i;
               }
               ptr_ref_const_iterator operator-(size_t i) const noexcept {
                  const_iterator temp = *this;
                  return temp -= i;
               }
               ptr_ref_const_iterator& operator++() noexcept { ++this->item; return *this; }
               ptr_ref_const_iterator& operator--() noexcept { --this->item; return *this; }
               //
               bool operator==(const ptr_ref_const_iterator& o) const noexcept { return o.item == this->item; }
               bool operator!=(const ptr_ref_const_iterator& o) const noexcept { return o.item != this->item; }
               bool operator>=(const ptr_ref_const_iterator& o) const noexcept { return o.item >= this->item; }
               bool operator<=(const ptr_ref_const_iterator& o) const noexcept { return o.item <= this->item; }
               bool operator> (const ptr_ref_const_iterator& o) const noexcept { return o.item >  this->item; }
               bool operator< (const ptr_ref_const_iterator& o) const noexcept { return o.item <  this->item; }
         };
         template<typename value_type> class ptr_ref_reverse_iterator : public ptr_ref_iterator<value_type> {
            public:
               using ptr_ref_iterator::ptr_ref_iterator;
               //
               ptr_ref_reverse_iterator& operator+=(size_t i) noexcept { return iterator::operator-=(i); }
               ptr_ref_reverse_iterator& operator-=(size_t i) noexcept { return iterator::operator+=(i); }
               ptr_ref_reverse_iterator operator+(size_t i) const noexcept { return iterator::operator-(i); }
               ptr_ref_reverse_iterator operator-(size_t i) const noexcept { return iterator::operator+(i); }
               ptr_ref_reverse_iterator& operator++() noexcept { return iterator::operator--(); }
               ptr_ref_reverse_iterator& operator--() noexcept { return iterator::operator++(); }
         };
         template<typename value_type> class ptr_ref_const_reverse_iterator : public ptr_ref_const_iterator<value_type> {
            public:
               using ptr_ref_const_iterator::ptr_ref_const_iterator;
               //
               ptr_ref_const_reverse_iterator& operator+=(size_t i) noexcept { return const_iterator::operator-=(i); }
               ptr_ref_const_reverse_iterator& operator-=(size_t i) noexcept { return const_iterator::operator+=(i); }
               ptr_ref_const_reverse_iterator operator+(size_t i) const noexcept { return const_iterator::operator-(i); }
               ptr_ref_const_reverse_iterator operator-(size_t i) const noexcept { return const_iterator::operator+(i); }
               ptr_ref_const_reverse_iterator& operator++() noexcept { return const_iterator::operator--(); }
               ptr_ref_const_reverse_iterator& operator--() noexcept { return const_iterator::operator++(); }
         };
      }
   }
}