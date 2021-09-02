#include "syntax_highlight_option_widget.h"

SyntaxHighlightOptionWidget::SyntaxHighlightOptionWidget(QWidget* parent) : QWidget(parent) {
   this->ui.setupUi(this);
   //
   this->ui.color->setHasAlpha(false);
}

QString SyntaxHighlightOptionWidget::name() const noexcept {
   return this->ui.name->text();
}
void SyntaxHighlightOptionWidget::setName(const QString& t) {
   this->ui.name->setText(t);
}

QColor SyntaxHighlightOptionWidget::color() const noexcept {
   return this->ui.color->color();
}
void SyntaxHighlightOptionWidget::setColor(QColor c) {
   return this->ui.color->setColor(c);
}

bool SyntaxHighlightOptionWidget::bold() const noexcept {
   return this->ui.bold->isChecked();
}
void SyntaxHighlightOptionWidget::setBold(bool c) {
   this->ui.bold->setChecked(c);
}

bool SyntaxHighlightOptionWidget::italic() const noexcept {
   return this->ui.italic->isChecked();
}
void SyntaxHighlightOptionWidget::setItalic(bool c) {
   this->ui.italic->setChecked(c);
}

bool SyntaxHighlightOptionWidget::underline() const noexcept {
   return this->ui.underline->isChecked();
}
void SyntaxHighlightOptionWidget::setUnderline(bool c) {
   this->ui.underline->setChecked(c);
}

ReachINI::syntax_highlight_option SyntaxHighlightOptionWidget::toOption() const noexcept {
   return {
      .bold      = this->ui.bold->isChecked(),
      .italic    = this->ui.italic->isChecked(),
      .underline = this->ui.underline->isChecked(),
      .colors = {
         .text = this->color(),
      },
   };
}
QTextCharFormat SyntaxHighlightOptionWidget::toFormat() const noexcept {
   return this->toOption().toFormat();
}

void SyntaxHighlightOptionWidget::setFromString(const QString& text) {
   bool error;
   auto data = ReachINI::syntax_highlight_option::fromString(text, error);
   if (!error) {
      this->ui.bold->setChecked(data.bold);
      this->ui.italic->setChecked(data.italic);
      this->ui.underline->setChecked(data.underline);
      this->setColor(data.colors.text);
   }
}
QString SyntaxHighlightOptionWidget::toString() const noexcept {
   return ReachINI::stringify_syntax_highlight_option(this->toOption());
}