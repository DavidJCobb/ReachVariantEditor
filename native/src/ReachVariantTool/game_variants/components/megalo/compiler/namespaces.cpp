#include "namespaces.h"

namespace Megalo {
   namespace Script {
      namespace namespaces {
         Namespace unnamed;
         Namespace global = Namespace("global", true, true);
         Namespace game = Namespace("game", false, false, {

         });
      }
   }
}