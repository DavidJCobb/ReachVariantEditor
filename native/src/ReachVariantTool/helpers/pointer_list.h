#pragma once
#include <vector>

namespace cobb {
   template<typename T> class pointer_list : public std::vector<T*> {
      //
      // A vector of pointers which can optionally be set to own the pointed-to objects, 
      // destroying them when the vector is destroyed. The main advantage of this over a 
      // vector of unique pointers is that you can pass (references to) a pointer_list 
      // around without recipients having to care whether it holds normal or unique 
      // pointers.
      //
      protected:
         bool _owner = false;
         //
      public:
         pointer_list() {}
         pointer_list(const pointer_list& o) {  this->operator=(o); }
         pointer_list(pointer_list&& o) { this->operator=(o); }
         pointer_list(bool isOwner) : _owner(isOwner) {}
         ~pointer_list() {
            if (this->is_owner())
               for (auto& ptr : *this)
                  delete ptr;
         }
         //
         inline bool is_owner() const noexcept { return this->_owner; }
         void set_is_owner(bool o) noexcept {
            this->_owner = o;
         }
         //
         void swap_items(size_t a, size_t b) noexcept {
            auto x = (*this)[a];
            auto y = (*this)[b];
            (*this)[a] = y;
            (*this)[b] = x;
         }
         //
         pointer_list& operator=(const pointer_list& other) noexcept {
            std::vector::operator=(other); // call super
            // DON'T copy ownership state
            return *this;
         }
         pointer_list& operator=(pointer_list&& other) noexcept {
            std::vector::swap(other);
            this->_owner = other._owner;
            return *this;
         }
   };
}