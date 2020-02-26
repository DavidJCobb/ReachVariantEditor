#include "detailed_enum.h"

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