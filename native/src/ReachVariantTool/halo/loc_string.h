#pragma once
#include "halo/util/dirty.h"
#include "halo/util/dummyable.h"
#include "halo/util/indexed.h"
#include "halo/util/refcount.h"
#include "./loc_string_base.h"

namespace halo {
   template<class Table> class loc_string;
   template<class Table> class loc_string : loc_string_base, public util::dirtiable<loc_string<Table>>, public util::passively_refcounted, public util::dummyable, public util::indexed {
      using owner_type = Table;
      friend owner_type;
      public:
         loc_string(Table& o) : owner(o) {}

         Table& owner;

         loc_string& operator=(const loc_string_base&);
         loc_string& operator=(loc_string_base&&);

         using loc_string_base::operator==;
         using loc_string_base::operator!=;

         const std::string& get_translation(localization_language) const;
         void set_translation(localization_language, const std::string&);
         void set_translation(localization_language, const char*);

         inline const decltype(translations)& get_all_translations() const { return this->translations; }
   };
}
