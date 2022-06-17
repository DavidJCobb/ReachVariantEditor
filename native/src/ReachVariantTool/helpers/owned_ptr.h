#pragma once
#include <type_traits>

namespace cobb {

   //
   // The STL smart pointers don't implicitly cast to bare pointers. There are reasons 
   // for this, but it also results in some heavy tradeoffs.
   // 
   //  - Math operations on smart pointers become possible, and yield bare pointers. 
   //    This can be a problem when the math operation was intended to target the 
   //    pointed-to object (i.e. forgotten operator*).
   // 
   //     - We disable math operations via deleted operator overloads.
   // 
   //  - One smart pointer can implicitly steal the bare pointer out from another smart 
   //    pointer, such that one will eventually delete the pointed-to object out from 
   //    under the other.
   // 
   //     - Deleted copy-constructors and copy-assignments prevent this.
   // 
   //  - The delete operator becomes usable on smart pointers, and deletes the pointed-
   //    to object out from under the smart pointer.
   // 
   //     - No mitigation possible when implicit casting is allowed.
   // 
   // With implicit casting enabled, we see some obvious benefits and some less obvious 
   // benefits.
   // 
   //  - It is now easier to pass smart pointers to code which should receive and work 
   //    with unowned pointers. 90% of the time, if you're passing the pointer around 
   //    at all, it will be to something which should not own it.
   // 
   //  - Smart pointers must be implicitly comparable with bare pointers in order to 
   //    allow e.g. using std::find on a container of smart pointers given a bare 
   //    pointer to search for.
   //


   // Deletes the pointed-to object on assignment to a new pointer, or on destruction.
   // No copy constructor allowed.
   template<typename T> class owned_ptr {
      public:
         using value_type = T;
         using pointer = value_type*;

      protected:
         pointer _target = nullptr;

      public:
         owned_ptr() {}
         owned_ptr(pointer v) : _target(v) {}
         template<typename Derived> requires std::is_base_of_v<value_type, Derived> owned_ptr(Derived* b) : _target(b) {}

         template<typename U> owned_ptr(const owned_ptr<U>&) = delete;
         owned_ptr(owned_ptr&& v) noexcept : _target(v.release()) {}

         template<typename U> owned_ptr& operator=(const owned_ptr<U>&) = delete;
         owned_ptr& operator=(owned_ptr&& v) {
            this->clear();
            this->_target = v.release();
            return *this;
         }

         ~owned_ptr() {
            this->clear();
         }

         void clear() {
            if (this->_target) {
               delete this->_target;
               this->_target = nullptr;
            }
         }

         constexpr value_type& operator*() const noexcept(noexcept(*std::declval<pointer>())) { return *this->_target; }
         constexpr pointer operator->() const noexcept { return this->_target; }

         explicit constexpr operator bool() const noexcept { return this->_target != nullptr; }
         //constexpr operator pointer() const noexcept { return this->_target; }
         template<typename Base> requires std::is_base_of_v<Base, value_type> constexpr operator Base*() const noexcept { return (Base*)this->_target; }

         template<typename Other> bool operator==(const Other* o) {
            return o == get();
         }

         constexpr pointer get() const {
            return this->_target;
         }
         constexpr pointer release() {
            auto* p = this->_target;
            this->_target = nullptr;
            return p;
         }

         // Don't allow pointer arithmetic:
         template<typename U> owned_ptr& operator+(U) const = delete;
         template<typename U> owned_ptr& operator-(U) const = delete;
   };

   // std::swap will invoke this via ADL when given two owned_ptrs of the same specialization directly
   template<typename value_type> void swap(owned_ptr<value_type>& a, owned_ptr<value_type>& b) {
      std::swap(a._target, b._target);
   }

   // Don't allow pointer arithmetic:
   template<typename T, typename U> T operator+(const T&, const owned_ptr<U>&) = delete;
   template<typename T, typename U> T operator-(const T&, const owned_ptr<U>&) = delete;
}
