#include "detailed_flags.h"
#include "../helpers/qt/string.h"

QString DetailedFlagsValue::get_description() const noexcept {
   for (size_t i = 0; i < std::extent<decltype(this->infos)>::value; i++) {
      auto& info = this->infos[i];
      if (info.type == DetailedFlagsValueInfo::info_type::description)
         return info.string;
   }
   return "";
}
QString DetailedFlagsValue::get_friendly_name() const noexcept {
   for (size_t i = 0; i < std::extent<decltype(this->infos)>::value; i++) {
      auto& info = this->infos[i];
      if (info.type == DetailedFlagsValueInfo::info_type::friendly_name)
         return info.string;
   }
   return "";
}

int DetailedFlags::lookup(const char* name) const noexcept {
   size_t s = this->size();
   for (size_t i = 0; i < s; ++i) {
      const auto& item = this->values[i];
      if (item.name == name)
         return i;
   }
   return -1;
}
int DetailedFlags::lookup(const QString& name) const noexcept {
   size_t s = this->size();
   for (size_t i = 0; i < s; ++i) {
      const auto& item = this->values[i];
      if (cobb::qt::stricmp(name, item.name) == 0)
         return i;
   }
   return -1;
}