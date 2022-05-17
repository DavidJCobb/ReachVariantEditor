#pragma once
#include <cstdint>
#include <type_traits>

namespace halo::util {
   // Objects inheriting from this class keep track of whether they're refcounted, but aren't 
   // automatically deleted when the refcount reaches zero. If you want automatic deletion, 
   // just use std::shared_ptr.
   class passively_refcounted {
      template<typename T> requires std::is_base_of_v<passively_refcounted, T> friend class refcount_ptr;
      protected:
         mutable uint32_t refcount = 0;
         
      public:
         inline bool is_referenced() const { return refcount > 0; }
   };

   template<typename T> requires std::is_base_of_v<passively_refcounted, T>
   class refcount_ptr {
      public:
         using value_type   = T;
         using pointer_type = T*;

      protected:
         value_type* target = nullptr;
         
         inline void _inc() { if (this->target) ++this->target->refcount; }
         inline void _dec() { if (this->target) --this->target->refcount; }
         
      public:
         refcount_ptr() {}
         refcount_ptr(pointer_type v) {
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
         refcount_ptr& operator=(pointer_type v) noexcept {
            this->_dec();
            this->target = v;
            this->_inc();
            return *this;
         }
         refcount_ptr& operator=(value_type& v) noexcept {
            this->_dec();
            this->target = &v;
            this->_inc();
            return *this;
         }
         refcount_ptr& operator=(const refcount_ptr& other) noexcept {
            this->_dec();
            this->target = other.target;
            this->_inc();
            return *this;
         }
         operator pointer_type() const noexcept { return this->target; } // implicitly provides comparison operators
         
         value_type* operator->() const noexcept { return this->target; }
         value_type& operator*() const noexcept { return *this->target; }
         
         template<typename C> C* pointer_cast() const noexcept { return (C*)this->target; }
   };
}
