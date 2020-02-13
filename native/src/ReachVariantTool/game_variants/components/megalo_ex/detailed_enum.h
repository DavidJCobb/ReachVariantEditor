#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <QString>

struct DetailedEnumValueInfo {
   enum class info_type {
      none,
      bare_string,      // std::string
      localized_string, // QString
      friendly_name, // QString
      description,   // QString
      image,         // bare_string // use for icon enums
      map_tag_type,  // signature   // use for MP object types
      map_tag_path,  // bare_string // use for MP object types
   };
   //
   info_type type = info_type::none;
   union {
      uint32_t    signature = 0;
      QString     string;
      std::string bare_string;
   };
   //
   uint32_t get_signature() {
      if (this->type == info_type::map_tag_type)
         return this->signature;
      return 0;
   }
   //
   // ... etc.
   //
   DetailedEnumValueInfo() {}
   DetailedEnumValueInfo(info_type t, uint32_t s) : type(t), signature(s) {}
   //
   static DetailedEnumValueInfo make_map_tag_type(uint32_t signature) {
      return DetailedEnumValueInfo(info_type::map_tag_type, signature);
   }
   //
   // ... etc.
   //
};
struct DetailedEnumValue {
   std::string name;
   DetailedEnumValueInfo infos[4];
};
struct DetailedEnum {
   std::vector<DetailedEnumValue> values;
};