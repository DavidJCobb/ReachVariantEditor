#pragma once
#include <QString>

namespace util {
   extern uint get_minimum_indent(const QString&, uint* last_non_space = nullptr);
   extern QString minimize_indent(const QString&);
}