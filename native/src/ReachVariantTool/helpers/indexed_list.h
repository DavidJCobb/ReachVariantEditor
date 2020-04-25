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
#include <cstdint>
#include <cstdlib>
#include <limits>
#include <utility>
#include <vector>
#include "iterators/pointer_based.h"

namespace cobb {

   /*

      INDEXED LIST

      A vector of pointers with the following features:

       - If the pointed-to type has a member called "index," the list attempts to write 
         to that member, so that list items know where they are in the list.

       - Elements are accessed as references to the pointed-to objects, not raw pointers. 
         This is to prevent you from directly interacting with the list (which would by-
         pass the code to keep indices up to date). Consequently, nullptr elements are 
         not allowed.

          - I'm not sure that using functions to swap iterators will work, though...

       - The vector owns all contained pointers and its destructor will delete the pointed-
         to objects.

       - One of the template parameters is a maximum count; the vector will reject attempts 
         to insert elements if that would push the current size past that maximum.

   */
   template<typename T, size_t max_count = std::numeric_limits<size_t>::max()> class indexed_list {
      protected:
         // Code to check if a type has an (index) member. Changes made to detect and avoid 
         // breaking on non-class/non-struct types.
         // source: https://stackoverflow.com/a/1007175
         // source: https://cpptalk.wordpress.com/2009/09/12/substitution-failure-is-not-an-error-2/
         template<typename X, typename test = void> struct _type_has_index_member { 
             static bool const value = false;
         };
         template<typename X> struct _type_has_index_member<X, typename std::enable_if<std::is_class_v<X>>::type> {
            struct Fallback { int index; }; // introduce member name
            struct Derived : X, Fallback {};
            //
            template<typename C, C> struct ChT;
            template<typename C> static char(&f(ChT<int Fallback::*, &C::index>*))[1];
            template<typename C> static char(&f(...))[2];
            //
            static bool const value = sizeof(f<Derived>(0)) == 2;
         };
         template<typename T> static constexpr bool _type_has_index_member_v = _type_has_index_member<T>::value;
         //
      public:
         static constexpr bool   type_has_index_member = _type_has_index_member_v<T>;
         static constexpr size_t max_count = max_count;
         using value_type = T;
         using entry_type = T*;
         using size_type  = size_t;
         using reference  = T&;
         using const_reference = const T&;
         using iterator       = ::cobb::iterators::pointer_based::ptr_ref_iterator<value_type>;
         using const_iterator = ::cobb::iterators::pointer_based::ptr_ref_const_iterator<value_type>;
         using reverse_iterator = ::cobb::iterators::pointer_based::ptr_ref_reverse_iterator<value_type>;
         using const_reverse_iterator = ::cobb::iterators::pointer_based::ptr_ref_const_reverse_iterator<value_type>;
         //
      protected:
         template<bool has_index> static void __set_element_index(T& item, size_t i) {}
         template<> static void __set_element_index<true>(T& item, size_t i) {
            item.index = i;
         }
         static void _set_element_index(T& item, size_t i) {
            __set_element_index<type_has_index_member>(item, i);
         }
         template<typename It> It _get_iterator_for_position(const int64_t i) const noexcept {
            if (this->empty())
               return It(nullptr);
            return It(&this->_list[i]);
         }
         //
         entry_type* _list     = nullptr;
         size_t      _size     = 0;
         size_t      _capacity = 0;
         //
      public:
         ~indexed_list() {
            for (size_type i = 0; i < this->_size; i++)
               delete this->data()[i];
            this->_size = 0;
            free(this->_list);
            this->_list = nullptr;
            this->_capacity = 0;
         }
         //
         inline size_t capacity() const noexcept { return this->_capacity; }
         inline bool   empty() const noexcept { return this->_size == 0 || this->_list == nullptr; }
         inline size_t size() const noexcept { return this->_size; }
         //
         void clear() noexcept {
            for (size_type i = 0; i < this->_size; i++)
               delete this->data()[i];
            this->_size = 0;
            if (this->_capacity > 50) { // arbitrary threshold for freeing the array
               free(this->_list);
               this->_list     = nullptr;
               this->_capacity = 0;
            }
         }
         void reserve(size_type c) {
            if (this->_capacity >= c)
               return;
            auto old = this->_list;
            auto next = (entry_type*)malloc(c * sizeof(entry_type));
            for (size_type i = 0; i < c; i++) {
               if (i < this->_size) {
                  next[i] = this->_list[i];
               } else {
                  next[i] = entry_type();
               }
            }
            this->_capacity = c;
            this->_list     = next;
            if (old)
               free(old);
         }
         void resize(size_type newSize) {
            if (this->_size == newSize)
               return;
            if (this->_capacity < newSize) {
               this->reserve(newSize);
               for (size_type i = this->_size; i < newSize; i++)
                  this->_list[i] = entry_type();
               this->_size = newSize;
            } else {
               this->_size = newSize;
               if (this->_capacity / 2 > newSize)
                  this->shrink_to_fit();
            }
         }
         void shrink_to_fit() noexcept {
            auto old = this->_list;
            auto next = (entry_type*)malloc(this->_size * sizeof(entry_type));
            for (size_type i = 0; i < this->_size; i++)
               if (i < this->_size)
                  next[i] = this->_list[i];
            this->_capacity = this->_size;
            this->_list     = next;
            if (old)
               free(old);
         }
         //
         void* take(size_type i) noexcept {
            auto item = this->data()[i];
            std::intptr_t size = (this->_size - i - 1) * sizeof(void*);
            if (size) {
               memcpy(this->_list + i, this->_list + i + 1, size);
               this->data()[this->_size - 1] = nullptr;
               //
            }
            --this->_size;
            for (size_t i = 0; i < this->_size; ++i)
               _set_element_index(*this->data()[i], i);
            return item;
         }
         //
         void erase(size_type i) noexcept {
            delete this->take(i);
         }
         bool full() const noexcept {
            if (this->_size >= max_count)
               return true;
            return false;
         }
         int32_t index_of(reference item) const noexcept {
            for (size_type i = 0; i < this->_size; i++)
               if (this->data()[i] == &item)
                  return i;
            return -1;
         }
         int32_t index_of(entry_type item) const noexcept {
            for (size_type i = 0; i < this->_size; i++)
               if (this->data()[i] == item)
                  return i;
            return -1;
         }
         entry_type push_back(entry_type item) {
            if (!item || this->full())
               return nullptr;
            if (this->_size >= this->_capacity)
               this->reserve(this->_size + 1);
            this->data()[this->_size] = item;
            _set_element_index(*item, this->_size);
            ++this->_size;
            return item;
         }
         entry_type emplace_back() { // TODO: take and forward constructor args
            if (this->full())
               return nullptr;
            entry_type item = new T;
            this->push_back(item);
            return item;
         }
         void swap_items(size_type a, size_type b) noexcept {
            auto& x = this->data()[a];
            auto& y = this->data()[b];
            auto item = x;
            x = y;
            y = item;
            _set_element_index(*x, a);
            _set_element_index(*y, b);
         }
         //
         inline entry_type* data() noexcept { return this->_list; }
         inline const entry_type* data() const noexcept { return this->_list; }
         //
         inline reference operator[](size_type i) noexcept { return *this->data()[i]; }
         inline const_reference operator[](size_type i) const noexcept { return *this->data()[i]; }
         reference at(size_type i) noexcept {
            if (i >= this->_size)
               throw std::out_of_range("");
            return operator[](i);
         }
         const_reference at(size_type i) const noexcept {
            if (i >= this->_size)
               throw std::out_of_range("");
            return operator[](i);
         }
         reference back()  noexcept { return operator[](this->_size - 1); }
         reference front() noexcept { return operator[](0); }
         //
         iterator begin() noexcept { return this->_get_iterator_for_position<iterator>(0); }
         iterator end()   noexcept { return this->_get_iterator_for_position<iterator>(this->_size); }
         const_iterator cbegin() const noexcept { return this->_get_iterator_for_position<const_iterator>(0); }
         const_iterator cend()   const noexcept { return this->_get_iterator_for_position<const_iterator>(this->_size); }
         reverse_iterator rbegin() noexcept { return this->_get_iterator_for_position<reverse_iterator>(this->_size - 1); }
         reverse_iterator rend()   noexcept { return this->_get_iterator_for_position<reverse_iterator>(-1); }
         const_reverse_iterator rcbegin() const noexcept { return this->_get_iterator_for_position<const_reverse_iterator>(this->_size - 1); }
         const_reverse_iterator rcend()   const noexcept { return this->_get_iterator_for_position<const_reverse_iterator>(-1); }
         inline const_iterator begin() const noexcept { return this->cbegin(); }
         inline const_iterator end()   const noexcept { return this->cend(); }
         inline const_reverse_iterator rbegin() const noexcept { return this->rcbegin(); }
         inline const_reverse_iterator rend()   const noexcept { return this->rcend(); }
         //
         operator std::vector<entry_type>() const noexcept {
            auto data = this->data();
            std::vector<entry_type> result;
            result.reserve(this->size());
            for (size_type i = 0; i < this->_size; i++)
               result.push_back(data[i]);
            return result;
         }
   };
}