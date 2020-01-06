#pragma once
#include <cstdint>
#include <type_traits>
#include <vector>

namespace cobb {
   /*

      An interface intended for use as a superclass. A subclass of (reference_tracked_object) can, 
      instead of having pointers to other reference_tracked_object instances, contain instances of 
      (reference_tracked_object::ref) which act as smart pointers; a reference_tracked_object keeps  
      track of inbound and outbound references from and to other reference_tracked_objects, but 
      only when those references exist as (reference_tracked_object::ref)s.

      USAGE EXAMPLE:

      class some_data : public reference_tracked_object {}
      class some_user : public reference_tracked_object {
         public:
            ref<some_data> myValue = decltype(myValue)::make(*this);
      }

      NOTES:

      Instances of reference_tracked_object cannot exist within vectors or other contains that can 
      unpredictably rearrange their contents. Instances keep track of each other via pointers (and 
      an instance's contained reference_tracked_object::ref members keep track of their owning 
      instance -- see below), so moving a reference_tracked_object instance will leave dangling 
      pointers everywhere. Always heap-allocate a reference_tracked_object.

      Given a reference_tracked_object (foo) which refers to other reference_tracked_objects: when 
      (foo) is destroyed, its destructor calls reference_tracked_object::sever_from_outbound. This 
      member function severs the outbound connections from (foo) to other reference_tracked_objects. 
      However, it is not possible at this time to sever any inbound connections.

      For now, I have chosen not to implement the severing of inbound connections on destroy for 
      performance reasons. We'd need to be able to instruct any given reference_tracked_object to 
      sever all outbound references to some target (the object being destroyed). There are two ways 
      to implement this:

       - SEAMLESS: Give each reference_tracked_object a std::vector<reference_tracked_object::ref*> 
         and have the reference_tracked_object::ref class add itself to that vector, so that the 
         object can loop over its own outbound references (rather than simply knowing how many such 
         references it has). That's a simplification; I would actually have to create a non-templated 
         superclass of reference_tracked_object::ref so that I can even store all instances in the 
         same vector.
      
       - MANUAL: Make reference_tracked_object an abstract class with an abstract virtual function 
         that, when called, should sever outbound references to a particular target. Subclasses 
         would be required to define this function and in it, check each of their refs manually. 
         This would be more efficient but less maintainable (and this class is already not perfectly 
         efficient anyway.

      THIS CLASS IS SELF-REFERENTIAL:

      This class isn't perfectly efficient. Among other things, each reference_tracked_object::ref 
      member has to be aware of its containing reference_tracked_object instance, and C++ doesn't 
      offer any built-in facilities for this (it would require being able to have a nested struct 
      definition that can only be instantiated as part of its containing struct or a subclass there-
      of). This means that each reference_tracked_object::ref must contain a reference_tracked_object& 
      pointed at its containing object. In other words, a reference_tracked_object instance contains 
      (by virtue of its reference_tracked_object::ref members) several pointers to itself. (This, 
      incidentally, is also why the syntax for declaring a reference_tracked_object::ref is so odd.)

   */
   class reference_tracked_object {
      protected:
         struct known_reference {
            reference_tracked_object* target = nullptr;
            uint32_t count = 0;
            //
            known_reference() {};
            known_reference(reference_tracked_object* o, uint32_t c) : target(o), count(c) {}
         };
         using list_type = std::vector<known_reference>;
         //
         list_type outbound;
         list_type inbound;
         //
         void _add_outbound(reference_tracked_object& target); // adds (target) to our outbound list, and then adds ourself to (target)'s inbound list
         void _add_inbound(reference_tracked_object& subject);
         void _remove_outbound(reference_tracked_object& target); // removes (target) from our outbound list, and then removes ourself from (target)'s inbound list
         void _remove_inbound(reference_tracked_object& subject);
         //
      public:
         const list_type& get_outbound() const noexcept { return this->outbound; }
         const list_type& get_inbound()  const noexcept { return this->inbound; }
         void  sever_from_outbound() noexcept; // sever all outbound references (i.e. removes all inbound references from this object to others)
         //
         reference_tracked_object() {};
         reference_tracked_object(const reference_tracked_object& source) = default; // copy-construct
         reference_tracked_object(reference_tracked_object&& source) = delete; // no move-construct
         reference_tracked_object& operator=(const reference_tracked_object& source) = default; // copy-assign
         reference_tracked_object& operator=(reference_tracked_object&& source) = delete; // disallow move-assign (see above: "THIS CLASS IS SELF-REFERENTIAL"; move-assignment isn't possible unless we can sever/replace inbound references to (source))
         //
      protected:
         //
         // Making the destructor protected means that instances of this class can only be heap-allocated. 
         // We need this (see above: "THIS CLASS IS SELF-REFERENTIAL").
         //
         ~reference_tracked_object() {
            this->sever_from_outbound();
         }
         //
      protected:
         template<class C> class ref final {
            private:
               reference_tracked_object& owner;
               C* target = nullptr;
               //
               static_assert(std::is_base_of_v<reference_tracked_object, C>, "reference_tracked_object::ref can only track reference_tracked_objects!");
               //
            public:
               ref(reference_tracked_object& o) : owner(o) {}
               //
               ref& operator=(C* other) noexcept {
                  if (this->target) {
                     this->owner._remove_outbound(*this->target);
                  }
                  this->target = other;
                  this->owner._add_outbound(*other);
                  return *this;
               }
               C* operator->() const noexcept { return this->target; }
               C& operator*() const noexcept { return *this->target; }
               operator C*() const noexcept { return this->target; } // implicitly provides comparison operators
               //
               ref& operator=(const ref& other) noexcept { return (*this = other.target); } // copy-assign
               ref& operator=(ref&& source) noexcept { // move-assign
                  auto t = source.target;
                  source = nullptr;
                  return (*this = t);
               }
               //
               inline static ref make(reference_tracked_object& o) {
                  return ref(o);
               }
         };
   };
}
