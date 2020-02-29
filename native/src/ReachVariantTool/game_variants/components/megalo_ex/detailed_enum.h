#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <QString>
#include "../../../helpers/bitwise.h"

struct DetailedEnumValueInfo {
   enum class info_type {
      none,
      bare_string,      // QString
      localized_string, // QString
      friendly_name, // QString
      description,   // QString
      image,         // bare_string // use for icon enums
      map_tag,       // signature and string   // use for MP object types, sounds, etc.
      subtitle,      // QString // for sounds consisting of dialogue
   };
   //
   info_type type = info_type::none;
   uint32_t  signature = 0;
   QString   string;
   //
   DetailedEnumValueInfo() {}
   DetailedEnumValueInfo(info_type t, uint32_t s) : type(t), signature(s) {}
   DetailedEnumValueInfo(info_type t, uint32_t s, const char* st) : type(t), signature(s), string(st) {}
   DetailedEnumValueInfo(info_type t, const char* s) : type(t), string(s) {}
   DetailedEnumValueInfo(info_type t, QString s) : type(t), string(s) {}
   //
   static DetailedEnumValueInfo make_description(QString v) {
      return DetailedEnumValueInfo(info_type::description, v);
   }
   static DetailedEnumValueInfo make_friendly_name(QString v) {
      return DetailedEnumValueInfo(info_type::friendly_name, v);
   }
   static DetailedEnumValueInfo make_map_tag(uint32_t sig, const char* path) {
      return DetailedEnumValueInfo(info_type::map_tag, sig, path);
   }
   static DetailedEnumValueInfo make_subtitle(QString v) {
      return DetailedEnumValueInfo(info_type::subtitle, v);
   }
};
struct DetailedEnumValue {
   uint32_t    index = 0;
   std::string name;
   DetailedEnumValueInfo infos[4];
   //
   DetailedEnumValue(const char* n) : name(n) {}
   DetailedEnumValue(const char* n, DetailedEnumValueInfo a) : name(n) {
      this->infos[0] = a;
   }
   DetailedEnumValue(const char* n, DetailedEnumValueInfo a, DetailedEnumValueInfo b) : name(n) {
      this->infos[0] = a;
      this->infos[1] = b;
   }
   DetailedEnumValue(const char* n, DetailedEnumValueInfo a, DetailedEnumValueInfo b, DetailedEnumValueInfo c) : name(n) {
      this->infos[0] = a;
      this->infos[1] = b;
      this->infos[2] = c;
   }
   DetailedEnumValue(const char* n, DetailedEnumValueInfo a, DetailedEnumValueInfo b, DetailedEnumValueInfo c, DetailedEnumValueInfo d) : name(n) {
      this->infos[0] = a;
      this->infos[1] = b;
      this->infos[2] = c;
      this->infos[3] = d;
   }
   //
   QString get_description() const noexcept;
   QString get_friendly_name() const noexcept;
};
struct DetailedEnum {
   std::vector<DetailedEnumValue> values;
   //
   inline size_t size() const noexcept { return this->values.size(); }
   inline DetailedEnumValue& operator[](int i) noexcept { return this->values[i]; }
   //
   DetailedEnum(std::initializer_list<DetailedEnumValue> values) : values(values) {
      size_t s = this->size();
      for (size_t i = 0; i < s; ++i)
         this->values[i].index = i;
   }
   constexpr inline int count_bits() const noexcept {
      auto s = this->size();
      if (!s)
         return 0;
      return cobb::bitcount(s);
   }
   constexpr inline int index_bits() const noexcept {
      auto s = this->size();
      if (!s)
         return 0;
      return cobb::bitcount(s - 1);
   }
   //
   const DetailedEnumValue* item(uint32_t i) const noexcept {
      if (i >= this->size())
         return nullptr;
      return &this->values[i];
   }
};