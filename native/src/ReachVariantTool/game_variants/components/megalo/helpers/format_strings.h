#pragma once
#include <QString>
#include <QVector>

namespace Megalo {
   struct format_string_issue {
      size_t  index = 0;
      QString code;
      bool crash      = false;
      bool error      = false;
      bool always_one = false;
      //
      bool is_floating_point = false;
      bool no_params_allowed = false;
      bool unusual_and_buggy = false;
   };

   extern QVector<format_string_issue> check_format_string(const QString&);
}