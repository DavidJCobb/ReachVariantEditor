#pragma once
#include "base.h"
#include <QVector>

namespace content {
   class category {
      public:
         QString id;
         QVector<base*> articles;

         void add_article(base&);
   };
}