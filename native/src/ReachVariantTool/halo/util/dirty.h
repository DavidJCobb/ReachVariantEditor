#pragma once
#include <concepts>

namespace halo::util {
   namespace impl::dirty {
      template<typename T> concept is_dirtiable = requires(T & x) {
         { x.dirty } -> std::same_as<bool>;
      };
      template<typename T> concept has_dirtiable_owner = requires(T& x) {
         { x.owner };
         { x.owner.dirty } -> std::same_as<bool>;
      };
   }

   template<typename T> requires impl::dirty::is_dirtiable<T> void set_dirty(T& object) {
      object.dirty = true;
      if constexpr (impl::dirty::has_dirtiable_owner<T>) {
         set_dirty(object.owner);
      }
   };

   // requires curiously recurring template pattern
   template<class Subclass> class dirtiable {
      //friend template<class Other> class dirtiable<Other>;
      protected:
         bool dirty = false;

         void set_dirty() {
            this->dirty = true;
            if constexpr (impl::dirty::has_dirtiable_owner<Subclass>) {
               this->owner.set_dirty();
            }
         }

         // Subclasses that contain dirtiable objects will need to define their own "clear dirty" function 
         // which loops over those and calls this.
         void _clear_dirty() {
            this->dirty = false;
         }

      public:
         inline bool is_dirty() const { return this->dirty; }
   };
}