#include "detailed_flags.h"

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