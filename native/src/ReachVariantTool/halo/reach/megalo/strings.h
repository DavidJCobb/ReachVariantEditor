#pragma once
#include "halo/loc_string_table.h"
#include "halo/util/refcount.h"

namespace halo::reach::megalo {
   using string_table = loc_string_table<0x70, 0x4C00>;

   // These are subclasses and not using-declarations so that they end up being distinct types; 
   // some of our metaprogramming will rely on this:

   struct string_ref : public util::refcount_ptr<string_table::entry_type> {
      using refcount_ptr::refcount_ptr;
   };
   struct string_ref_optional : public util::refcount_ptr<string_table::entry_type> {
      using refcount_ptr::refcount_ptr;
   };
}
