#include "enums.h"
#include "namespaces.h"

namespace Megalo {
   namespace Script {
      Enum::Enum(QString n, std::initializer_list<QString> v) {
         auto size = v.size();
         this->name = n;
         this->values.resize(size);
         //
         size_t i = 0;
         for (auto it = v.begin(); it != v.end(); ++it, ++i) { // really irritating that initializer_list doesn't have operator[]
            this->values[i].name  = *it;
            this->values[i].value = i;
         }
      }
      void Enum::add_value(QString name, int32_t v) noexcept {
         this->values.emplace_back(v, name);
      }
      bool Enum::lookup(const QString& name, int32_t& out) const noexcept {
         for (auto& value : this->values) {
            if (value.name.compare(name, Qt::CaseInsensitive) == 0) {
               out = value.value;
               return true;
            }
         }
         return false;
      }
   }
}