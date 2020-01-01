#pragma once
#include <cstdint>
#include <string>

class MPObjectNameDefinition {
   public:
      std::string name;
      std::string internal;
      //
      MPObjectNameDefinition(const char* n, const char* i) : name(n), internal(i) {};
};

class MPObjectNameList {
   private:
      MPObjectNameList();
      //
      MPObjectNameDefinition* list = nullptr;
      uint32_t count = 0;
   public:
      const MPObjectNameDefinition& operator[](int i) const; // can throw std::out_of_range
      const uint32_t size() const noexcept { return this->count; }
      //
      static MPObjectNameList& get() noexcept {
         static MPObjectNameList instance;
         return instance;
      }
};