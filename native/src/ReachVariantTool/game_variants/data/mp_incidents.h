#pragma once
#include <cstdint>
#include <string>

class MPIncidentDefinition {
   public:
      std::string name;
      std::string internal;
      std::string description;
      //
      MPIncidentDefinition(const char* n, const char* i, const char* d = "") : name(n), internal(i), description(d) {};
};

class MPIncidentList {
   private:
      MPIncidentList();
      //
      MPIncidentDefinition* list = nullptr;
      uint32_t count = 0;
   public:
      const MPIncidentDefinition& operator[](int i) const; // can throw std::out_of_range
      const uint32_t size() const noexcept { return this->count; }
      //
      static MPIncidentList& get() noexcept {
         static MPIncidentList instance;
         return instance;
      }
};