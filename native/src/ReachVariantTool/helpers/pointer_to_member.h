#pragma once

namespace cobb {
   //
   // Helpers for getting a pointer-to-member of a nested member, i.e.
   //
   // bool MyStruct::* foo = NESTED_MEMBER_POINTER(MyStruct, field.boolField);
   // // (foo) accesses (someStruct->field.boolField)
   //
   // <https://stackoverflow.com/a/40483540>
   template<typename C, typename T, size_t offset> union _member_pointer_impl final {
      template<typename U> struct Helper     { using type = U C::*; };
      template<typename U> struct Helper<U&> { using type = U C::*; };
      //
      using member_pointer = typename Helper<T>::type;
      //
      // union members:
      member_pointer o;
      size_t i = offset;
      //
      static_assert(sizeof(i) == sizeof(o));
   };
   /*//#define NESTED_MEMBER_POINTER(C, M) \
     ((::cobb::_member_pointer_impl<__typeof__(C), \
         decltype(((__typeof__(C)*)nullptr)->M),  \
         __builtin_offsetof(__typeof__(C), M)     \
     >{ }).o)
   //*/
   #define NESTED_MEMBER_POINTER(C, M) \
     ((::cobb::_member_pointer_impl<C, \
         decltype(((C*)nullptr)->M),  \
         offsetof(C, M)     \
     >{ }).o)
}