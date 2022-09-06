#pragma once

// HACK: A template defined in a given namespace (including the unnamed global namespace) 
// can only be specialized from within an enclosing namespace or an enclosed namespace; a 
// "sibling" namespace or any descendant of one cannot be used. Ergo this has to be in the 
// global namespace in order to be able to specialize it from anywhere.
template<class Enumeration> struct cobb__reflex3__enumeration_data;

namespace cobb::reflex3 {
   //template<class Enumeration> struct enumeration_data;
   template<typename... Args> using enumeration_data = cobb__reflex3__enumeration_data<Args...>;
}