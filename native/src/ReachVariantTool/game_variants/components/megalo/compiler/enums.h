#pragma once
#include <vector>
#include <QString>

namespace Megalo {
   namespace Script {
      class Namespace;
      //
      class Enum {
         public:
            struct Value {
               QString name;
               int32_t value = 0;
               //
               Value() {}
               Value(int32_t v, QString n) : name(n), value(v) {}
            };
            //
            QString name;
            std::vector<Value> values;
            //
            Enum() {}
            Enum(QString n, std::initializer_list<Value> v) : name(n), values(v) {}
            Enum(QString n, std::initializer_list<QString> v);
            //
            bool lookup(const QString&, int32_t&) const noexcept;
      };
   }
}