#pragma once
#include <cstdint>

namespace cobb {
   //
   // The content in this class is intended for tracking the number of outside references to an object 
   // that has a definite owner, i.e. this is a refcount system that DOES NOT automatically manage the 
   // lifetimes of objects it is used for. If you need automatic lifetime management, then use something 
   // like std::shared_ptr.
   //
   template<typename T> class refcount_ptr {
      protected:
         T* target = nullptr;
         //
         inline void _inc() { if (this->target) ++this->target->refcount; }
         inline void _dec() { if (this->target) --this->target->refcount; }
         //
      public:
         using value_type = T;
         using pointer_type = T*;
         //
         refcount_ptr() {}
         refcount_ptr(T* v) {
            this->target = v;
            this->_inc();
         }
         ~refcount_ptr() {
            this->_dec();
            this->target = nullptr;
         }
         refcount_ptr& operator=(std::nullptr_t) noexcept {
            this->_dec();
            this->target = nullptr;
            return *this;
         }
         refcount_ptr& operator=(T* v) noexcept {
            this->_dec();
            this->target = v;
            this->_inc();
            return *this;
         }
         refcount_ptr& operator=(T& v) noexcept {
            this->_dec();
            this->target = &v;
            this->_inc();
            return *this;
         }
         refcount_ptr& operator=(const T* v) noexcept {
            return this->operator=(const_cast<T*>(v));
         }
         refcount_ptr& operator=(const refcount_ptr& other) noexcept {
            this->_dec();
            this->target = other.target;
            this->_inc();
            return *this;
         }
         operator T*() const noexcept { return this->target; } // implicitly provides comparison operators
         //
         T* operator->() const noexcept { return this->target; }
         T& operator*() const noexcept { return *this->target; }
         //
         template<typename C> C* pointer_cast() const noexcept { return (C*)this->target; }
   };
   class refcountable {
      template<typename T> friend class refcount_ptr;
      protected:
         mutable uint32_t refcount = 0;
         //
      public:
         inline uint32_t get_refcount() const noexcept { return this->refcount; }
   };
   //
   class indexed_refcountable : public refcountable {
      public:
         mutable int32_t index = -1;
   };
}