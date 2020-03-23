#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <QString>
#include "../../../helpers/bitwise.h"

struct DetailedFlagsValueInfo{
   enum class info_type {
      none,
      bare_string,      // QString
      localized_string, // QString
      friendly_name, // QString
      description,   // QString
   };
   //
   info_type type = info_type::none;
   QString   string;
   //
   DetailedFlagsValueInfo() {}
   DetailedFlagsValueInfo(info_type t, const char* s) : type(t), string(s) {}
   DetailedFlagsValueInfo(info_type t, QString s) : type(t), string(s) {}
   //
   static DetailedFlagsValueInfo make_description(QString v) {
      return DetailedFlagsValueInfo(info_type::description, v);
   }
   static DetailedFlagsValueInfo make_friendly_name(QString v) {
      return DetailedFlagsValueInfo(info_type::friendly_name, v);
   }
};
struct DetailedFlagsValue {
   uint32_t    index = 0;
   std::string name;
   DetailedFlagsValueInfo infos[4];
   //
   DetailedFlagsValue(const char* n) : name(n) {}
   DetailedFlagsValue(const char* n, DetailedFlagsValueInfo a) : name(n) {
      this->infos[0] = a;
   }
   DetailedFlagsValue(const char* n, DetailedFlagsValueInfo a, DetailedFlagsValueInfo b) : name(n) {
      this->infos[0] = a;
      this->infos[1] = b;
   }
   DetailedFlagsValue(const char* n, DetailedFlagsValueInfo a, DetailedFlagsValueInfo b, DetailedFlagsValueInfo c) : name(n) {
      this->infos[0] = a;
      this->infos[1] = b;
      this->infos[2] = c;
   }
   DetailedFlagsValue(const char* n, DetailedFlagsValueInfo a, DetailedFlagsValueInfo b, DetailedFlagsValueInfo c, DetailedFlagsValueInfo d) : name(n) {
      this->infos[0] = a;
      this->infos[1] = b;
      this->infos[2] = c;
      this->infos[3] = d;
   }
   //
   QString get_description() const noexcept;
   QString get_friendly_name() const noexcept;
};
struct DetailedFlags {
   std::vector<DetailedFlagsValue> values;
   //
   inline size_t size() const noexcept { return this->values.size(); }
   inline DetailedFlagsValue& operator[](int i) noexcept { return this->values[i]; }
   //
   DetailedFlags(std::initializer_list<DetailedFlagsValue> values) : values(values) {
      size_t s = this->size();
      for (size_t i = 0; i < s; ++i)
         this->values[i].index = i;
   }
   inline int bitcount() const noexcept {
      auto s = this->size();
      if (!s)
         return 0;
      return cobb::bitcount(s);
   }
   //
   const DetailedFlagsValue* item(uint32_t i) const noexcept {
      if (i >= this->size())
         return nullptr;
      return &this->values[i];
   }
};