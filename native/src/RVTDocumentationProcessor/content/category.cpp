#include "category.h"

namespace content {
   void category::add_article(base& article) {
      if (this->articles.indexOf(&article) >= 0)
         return;
      this->articles.push_back(&article);
      article.categories.push_back(this);
   }
}