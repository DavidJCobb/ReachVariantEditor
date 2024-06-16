#pragma once
#include "../ini.h"
#include <QObject>

namespace cobb {
   namespace qt {
      namespace ini {
         using setting             = cobb::ini::setting;
         using setting_value_union = cobb::ini::setting_value_union;
         class file : public QObject, public ::cobb::ini::file {
            Q_OBJECT
            private:
               using _superclass = ::cobb::ini::file;
            signals:
               void settingChanged(setting*, setting_value_union oldValue, setting_value_union newValue);
            protected:
               virtual void _send_change_event(setting*, setting_value_union oldValue, setting_value_union newValue) override;
            public:
               file() : QObject(nullptr), _superclass() {}
               file(std::filesystem::path filepath) : QObject(nullptr), _superclass(filepath) {}
               file(std::filesystem::path filepath, std::filesystem::path backup, std::filesystem::path working) : QObject(nullptr), _superclass(filepath, backup, working) {}
         };
      }
   }
}