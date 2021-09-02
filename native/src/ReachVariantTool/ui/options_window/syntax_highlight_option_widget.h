#pragma once
#include <QTextFormat>
#include <QWidget>
#include "ui_syntax_highlight_option_widget.h"
#include "../../services/syntax_highlight_option.h"

class SyntaxHighlightOptionWidget : public QWidget {
   Q_OBJECT;
   public:
      SyntaxHighlightOptionWidget(QWidget* parent = nullptr);

      QString name() const noexcept;
      void setName(const QString&);

      QColor color() const noexcept;
      void setColor(QColor);

      bool bold() const noexcept;
      void setBold(bool);

      bool italic() const noexcept;
      void setItalic(bool);

      bool underline() const noexcept;
      void setUnderline(bool);

      ReachINI::syntax_highlight_option toOption() const noexcept;
      QTextCharFormat toFormat() const noexcept;

      void setFromString(const QString&);
      QString toString() const noexcept;

   private:
      Ui::SyntaxHighlightOptionWidget ui;
};