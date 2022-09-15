
//
// All tests use nested-name-specifiers 
// when specializing a template.
//


//
// Test 1: Specialize a template from a 
// child/descendant namespace, when the 
// original declaration is inside of the 
// global namespace.
//
// Just for fun, we'll compare it to a 
// non-templated class declaration and 
// definition as well.
//
// MSVC:  Yes
// Clang: No
// GCC:   No [fails on principle; no P1787]
//
#pragma region Test 1
   template<typename T> struct foo;
   class hey;

   namespace A {
      // Works, per P1787 changes to:
      //  - [class.pre]3
      //     - The scope of the definition must be the same as, or an ancestor 
      //       of, the scope of the declaration only for non-template class 
      //       definitions (i.e. those that use an identifier rather than a 
      //       simple-template-id).
      //  - [temp.spec.partial.general]
      //     - Formerly referred to [namespace.memdef]
      //     - Now refers only to:
      //        - [dcl.meaning] for function template specialization
      //        - [class.mem] for class member templates
      //        - [temp.mem] for member templates generally
      template<> struct ::foo<int> {};

      // Fails, per limits set in P1787 [class.pre]3
      class ::hey {};

      namespace B {
         // works, per P1787 changes
         template<> struct ::foo<float> {};
      }
   }
#pragma endregion

//
// Test 2: Specialize a template from a 
// child/descendant namespace, when the 
// original definition is not inside of 
// the global namespace. Also try from 
// a parent/ancestor namespace.
//
// MSVC:  Mixed (Parent only)
// Clang: No
// GCC:   No [fails on principle; no P1787]
//
#pragma region Test 2
   namespace B {
      template<typename T> struct bar;

      namespace C {
         // Does not work despite P1787 changes.
         template<> struct ::B::bar<int> {};
      }
   }
   template<> struct B::bar<float> {};
#pragma endregion

//
// Test 3: Specialize a template from a 
// namespace that is not an ancestor or 
// descendant of the namespace that the 
// template was declared in.
//
// MSVC:  No
// Clang: No
// GCC:   No [fails on principle; no P1787]
//
#pragma region Test 3
   namespace X {
      template<typename T> struct baz;
   }
   namespace Y {
      // Does not work despite P1787 changes.
      template<> struct ::X::baz<int> {};
   }
#pragma endregion