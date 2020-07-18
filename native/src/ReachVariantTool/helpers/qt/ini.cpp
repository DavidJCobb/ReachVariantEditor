#include "ini.h"

namespace cobb {
   namespace qt {
      namespace ini {
         void file::_send_change_event(setting* s, setting_value_union oldValue, setting_value_union newValue) {
            auto copy = this->changeCallbacks; // in case a callback unregisters itself
            for (auto cb : copy)
               cb(s, oldValue, newValue);
            emit settingChanged(s, oldValue, newValue);
         }
      }
   }
}