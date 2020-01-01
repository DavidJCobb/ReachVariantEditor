#pragma once
#include <cstdint>
#include <string>

class MPObjectTypeDefinition {
   public:
      std::string name;
      uint32_t    tag_signature = 0;
      std::string tag_path;
      //
      MPObjectTypeDefinition(const char* n, uint32_t s = 0, const char* p = "") : name(n), tag_signature(s), tag_path(p) {};
};

class MPObjectTypeList {
   private:
      MPObjectTypeList();
      //
      MPObjectTypeDefinition* list = nullptr;
      uint32_t count = 0;
   public:
      const MPObjectTypeDefinition& operator[](int i) const; // can throw std::out_of_range
      const uint32_t size() const noexcept { return this->count; }
      //
      static MPObjectTypeList& get() noexcept {
         static MPObjectTypeList instance;
         return instance;
      }
};