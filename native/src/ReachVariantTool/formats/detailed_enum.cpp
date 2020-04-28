#include "detailed_enum.h"
#include "../helpers/qt/string.h"

QString DetailedEnumValue::get_description() const noexcept {
   for (size_t i = 0; i < std::extent<decltype(this->infos)>::value; i++) {
      auto& info = this->infos[i];
      if (info.type == DetailedEnumValueInfo::info_type::description)
         return info.string;
   }
   return "";
}
QString DetailedEnumValue::get_friendly_name() const noexcept {
   for (size_t i = 0; i < std::extent<decltype(this->infos)>::value; i++) {
      auto& info = this->infos[i];
      if (info.type == DetailedEnumValueInfo::info_type::friendly_name)
         return info.string;
   }
   return "";
}

int DetailedEnum::lookup(const char* name) const noexcept {
   size_t s = this->size();
   for (size_t i = 0; i < s; ++i) {
      const auto& item = this->values[i];
      if (item.name == name)
         return i;
   }
   return -1;
}
int DetailedEnum::lookup(const QString& name) const noexcept {
   size_t s = this->size();
   for (size_t i = 0; i < s; ++i) {
      const auto& item = this->values[i];
      if (cobb::qt::stricmp(name, item.name) == 0)
         return i;
   }
   return -1;
}