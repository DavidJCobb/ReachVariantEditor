#pragma once
#include <string>
#include <QString>
#include <QTextCharFormat>

namespace ReachINI {
   struct syntax_highlight_option {
      bool bold      = false;
      bool italic    = false;
      bool underline = false;
      struct {
         QColor text;
      } colors;

      static syntax_highlight_option fromString(const QString&, bool& error);
      static syntax_highlight_option fromString(const QString&);

      QTextCharFormat toFormat() const noexcept;
   };

   extern QTextCharFormat parse_syntax_highlight_option(const QString&, bool& error);
   extern QTextCharFormat parse_syntax_highlight_option(const std::string&, bool& error);
   extern QString stringify_syntax_highlight_option(const syntax_highlight_option&);
}