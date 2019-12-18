#pragma once
#include "type_traits.h"
#include "templating.h"

namespace cobb {
   #pragma optimize("", off)
   //
   // A really dumb function to try and keep Visual Studio's debugger from hiding certain 
   // values if they're locals that aren't used in the current block / aren't going to be 
   // used again / etc.. Each of the locals you want to preserve must be passed as an 
   // argument to ONE SINGLE CALL of this function; consecutive calls won't work. Add 
   // templated overloads for more args if you need to preserve more variables than the 
   // current code allows.
   //
   // The idea is to force the compiler to "use" these variables at the point at which 
   // you insert a call to this function, such that they must be kept around until the 
   // call and will likely remain around shortly after it. That's why consecutive calls 
   // to this function don't work: the variables in earlier calls can be discarded.
   //
   template<typename T> void try_to_keep_visible_in_debugger(T& x) noexcept {
      #pragma warning(suppress: 4474 4477) // turn off printf arg checking
      printf("\0%u", x);
   }
   template<typename T, typename U> void try_to_keep_visible_in_debugger(T& x, U y) noexcept {
      #pragma warning(suppress: 4474 4477) // turn off printf arg checking
      printf("\0%u%u", x, y);
   }
   #pragma optimize("", on)
}