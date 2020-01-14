#pragma once
#include <cstdint>
#include "iterators/pointer_based.h"

namespace cobb {

   /*

      MANAGED POINTER LIST

      A vector of pointers with the following features:

       - If the pointed-to type has a member called "index," the list attempts to write 
         to that member, so that list items know where they are in the list.

       - Elements are accessed as references to the pointed-to objects, not raw pointers. 
         This is to prevent you from directly interacting with the list (which would by-
         pass the code to keep indices up to date). Consequently, nullptr elements are 
         not allowed.

          - I'm not sure that using functions to swap iterators will work, though...

   */

   class _managed_pointer_list_impl {
      public:
         using value_type = void*;
         using size_type  = size_t;
         //
      protected:
         void** _list     = nullptr;
         size_t _size     = 0;
         size_t _capacity = 0;
         //
         template<typename It> It _get_iterator_for_position(const int64_t i) const noexcept {
            if (this->empty())
               return It(nullptr);
            return It(this->_list[i]);
         }
         //
      protected:
         value_type* data() noexcept { return this->_list; }
         const value_type* data() const noexcept { return this->_list; }
         //
         void push_back(value_type i) noexcept {
            if (this->_size == this->_capacity)
               this->reserve(this->_capacity + 1);
            this->data()[this->_size] = i;
            ++this->_size;
         }
         //
      public:
         inline size_t capacity() const noexcept { return this->_capacity; }
         inline bool   empty() const noexcept { return this->_size == 0 || this->_list == nullptr; }
         inline size_t size() const noexcept { return this->_size; }
         //
         void clear() noexcept;
         void reserve(size_type c);
         void resize(size_type i);
         void shrink_to_fit() noexcept;
   };
   template<typename T> class managed_pointer_list : public _managed_pointer_list_impl {
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
         using _managed_pointer_list_impl::_managed_pointer_list_impl;
         static constexpr bool type_has_index_member = _type_has_index_member_v<T>;
         //
      protected:
         template<bool has_index> static void __set_element_index(T& item, size_t i) {}
         template<> static void __set_element_index<true>(T& item, size_t i) {
            item.index = i;
         }
         static void _set_element_index(T& item, size_t i) {
            __set_element_index<type_has_index_member>(item, i);
         }
         //
      public:
         using value_type = T*;
         using reference  = T&;
         using const_reference = const T&;
         using iterator       = ::cobb::iterators::pointer_based::iterator<value_type>;
         using const_iterator = ::cobb::iterators::pointer_based::const_iterator<value_type>;
         using reverse_iterator       = ::cobb::iterators::pointer_based::reverse_iterator<value_type>;
         using const_reverse_iterator = ::cobb::iterators::pointer_based::const_reverse_iterator<value_type>;
         //
         void push_back(value_type item) {
            if (!item)
               return;
            _managed_pointer_list_impl::push_back(item);
            _set_element_index(*item, this->_size - 1);
         }
         template<class... Args > reference emplace_back(Args&&... args) {
            auto item = new T(std::forward<Args>(args)...);
            this->push_back(item);
            return *item;
         }
         void swap_items(size_type a, size_type b) noexcept {
            auto x = (*this)[a];
            auto y = (*this)[b];
            (*this)[a] = y;
            (*this)[b] = x;
            _set_element_index(x, a);
            _set_element_index(y, b);
         }
         //
         inline value_type* data() noexcept { return (value_type*)_managed_pointer_list_impl::data(); }
         inline const value_type* data() const noexcept { return (const value_type*)_managed_pointer_list_impl::data(); }
         //
         inline reference operator[](size_type i) noexcept { return **(value_type*)this->data(); }
         inline const_reference operator[](size_type i) const noexcept { return **(value_type*)this->data(); }
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
   };
}