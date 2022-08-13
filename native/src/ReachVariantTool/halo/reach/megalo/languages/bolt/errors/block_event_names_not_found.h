#pragma once
#include "./base.h"

namespace halo::reach::megalo::bolt::errors {
   //
   // Occurs with code such as `on : do end`, where the event names between the 
   // "on" keyword and the colon are missing.
   //
   class block_event_names_not_found : public base {
      public:
   };
}