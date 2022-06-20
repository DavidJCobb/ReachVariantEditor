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
         mutable bool dirty = false;

         // Here's where things get a bit ugly: we want an object to be able to clear 
         // its dirty flag during "const" code, e.g. because some const-qualified 
         // write process is underway and will result in the object no longer being 
         // dirty. Only way to accomplish that is to make all our member functions 
         // here const, and make the flag mutable.

         void set_dirty() const {
            this->dirty = true;
            if constexpr (impl::dirty::has_dirtiable_owner<Subclass>) {
               this->owner.set_dirty();
            }
         }

         // Subclasses that contain dirtiable objects will need to define their own "clear dirty" function 
         // which loops over those and calls this.
         void _clear_dirty() const {
            this->dirty = false;
         }

      public:
         inline bool is_dirty() const { return this->dirty; }
   };
}