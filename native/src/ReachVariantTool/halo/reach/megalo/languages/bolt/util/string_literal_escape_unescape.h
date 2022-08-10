#pragma once
#include <QString>

namespace halo::reach::megalo::bolt::util {
   extern QString string_literal_escape(QStringView, QChar delim);
   extern QString string_literal_unescape(QStringView);
}