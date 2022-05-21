#pragma once
#include "loc_string.h"

#define CLASS_TEMPLATE_PARAMS template<class Table>
#define CLASS_NAME loc_string<Table>
//
namespace halo {
   CLASS_TEMPLATE_PARAMS CLASS_NAME& CLASS_NAME::operator=(const loc_string_base& o) {
      this->translations = o.translations;
      this->set_dirty();
   }
   CLASS_TEMPLATE_PARAMS CLASS_NAME& CLASS_NAME::operator=(loc_string_base&& o) {
      this->translations = o.translations;
      this->set_dirty();
   }

   CLASS_TEMPLATE_PARAMS const std::string& CLASS_NAME::get_translation(localization_language lang) const {
      return this->translations[(size_t)lang];
   }
   CLASS_TEMPLATE_PARAMS void CLASS_NAME::set_translation(localization_language lang, const std::string& v) {
      this->translations[(size_t)lang] = v;
      this->set_dirty();
   }
   CLASS_TEMPLATE_PARAMS void CLASS_NAME::set_translation(localization_language lang, const char* v) {
      this->translations[(size_t)lang] = v;
      this->set_dirty();
   }
}
//
#undef CLASS_TEMPLATE_PARAMS
#undef CLASS_NAME