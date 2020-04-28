#pragma once
#include <cstdint>
#include <QString>
#include <vector>

struct GameEngineVariantLoadWarningLog {
   std::vector<QString> warnings;

   static GameEngineVariantLoadWarningLog& get() {
      static thread_local GameEngineVariantLoadWarningLog instance;
      return instance;
   }
   inline void clear() noexcept {
      this->warnings.clear();
   }
   inline bool empty() const noexcept {
      return this->warnings.empty();
   }
   inline void push_back(const QString& item) noexcept {
      this->warnings.push_back(item);
   }
   inline size_t size() const noexcept {
      return this->warnings.size();
   }
};