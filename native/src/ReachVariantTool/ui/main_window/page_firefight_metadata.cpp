#include "page_firefight_metadata.h"
#include "../../game_variants/types/firefight.h"
#include <QMessageBox>
#include "../generic/QXBLGamertagValidator.h"

PageFFMetadata::PageFFMetadata(QWidget* parent) : QWidget(parent) {
   ui.setupUi(this);
   //
   auto& editor = ReachEditorState::get();
   QObject::connect(&editor, &ReachEditorState::variantAcquired, this, &PageFFMetadata::updateFromVariant);
   //
   QObject::connect(this->ui.headerName, &QLineEdit::textEdited, [this](const QString& text) {
      auto variant = ReachEditorState::get().variant();
      if (!variant)
         return;
      auto ff = variant->get_firefight_data();
      if (!ff)
         return;
      auto u16s = text.toStdU16String();
      const char16_t* value = u16s.c_str();
      variant->contentHeader.data.set_title(value);
      ff->variantHeader.set_title(value);
      //
      emit titleChanged(value);
   });
   QObject::connect(this->ui.headerName, &QLineEdit::textEdited, this, &PageFFMetadata::updateDescriptionCharacterCount);
   //
   auto desc      = this->ui.headerDesc;
   auto descCount = this->ui.headerDescCharacterLimit;
   QObject::connect(this->ui.headerDesc, &QPlainTextEdit::textChanged, [desc, descCount]() {
      auto text    = desc->toPlainText();
      auto variant = ReachEditorState::get().variant();
      if (!variant)
         return;
      auto ff = variant->get_firefight_data();
      if (!ff)
         return;
      auto u16s = text.toStdU16String();
      const char16_t* value = u16s.c_str();
      variant->contentHeader.data.set_description(value);
      ff->variantHeader.set_description(value);
   });
   QObject::connect(this->ui.headerDesc, &QPlainTextEdit::textChanged, this, &PageFFMetadata::updateDescriptionCharacterCount);
   //
   QObject::connect(this->ui.authorGamertag, &QLineEdit::textEdited, [](const QString& text) {
      auto variant = ReachEditorState::get().variant();
      if (!variant)
         return;
      auto ff = variant->get_firefight_data();
      if (!ff)
         return;
      auto latin = text.toLatin1();
      variant->contentHeader.data.createdBy.set_author_name(latin.constData());
      ff->variantHeader.createdBy.set_author_name(latin.constData());
   });
   QObject::connect(this->ui.eraseAuthorXUID, &QPushButton::clicked, [this]() {
      auto variant = ReachEditorState::get().variant();
      if (!variant)
         return;
      auto ff = variant->get_firefight_data();
      if (!ff)
         return;
      auto& author_c = variant->contentHeader.data.createdBy;
      auto& author_m = ff->variantHeader.createdBy;
      if (author_c.has_xuid() || author_m.has_xuid()) {
         author_c.erase_xuid();
         author_m.erase_xuid();
         QMessageBox::information(this, tr("Operation complete"), tr("The XUID and \"is online ID\" flag for this file's author have been wiped."));
         return;
      } else {
         author_c.erase_xuid();
         author_m.erase_xuid();
         QMessageBox::information(this, tr("Operation complete"), tr("The XUID and \"is online ID\" flag for this file's author were already blank."));
         return;
      }
   });
   QObject::connect(this->ui.createdOnDate, &QDateTimeEdit::dateTimeChanged, [](const QDateTime& time) {
      auto variant = ReachEditorState::get().variant();
      if (!variant)
         return;
      auto ff = variant->get_firefight_data();
      if (!ff)
         return;
      uint64_t seconds = time.toSecsSinceEpoch();
      variant->contentHeader.data.createdBy.set_datetime(seconds);
      ff->variantHeader.createdBy.set_datetime(seconds);
   });
   QObject::connect(this->ui.editorGamertag, &QLineEdit::textEdited, [](const QString& text) {
      auto variant = ReachEditorState::get().variant();
      if (!variant)
         return;
      auto ff = variant->get_firefight_data();
      if (!ff)
         return;
      auto latin = text.toLatin1();
      variant->contentHeader.data.modifiedBy.set_author_name(latin.constData());
      ff->variantHeader.modifiedBy.set_author_name(latin.constData());
   });
   QObject::connect(this->ui.eraseEditorXUID, &QPushButton::clicked, [this]() {
      auto variant = ReachEditorState::get().variant();
      if (!variant)
         return;
      auto ff = variant->get_firefight_data();
      if (!ff)
         return;
      auto& author_c = variant->contentHeader.data.modifiedBy;
      auto& author_m = ff->variantHeader.modifiedBy;
      if (author_c.has_xuid() || author_m.has_xuid()) {
         author_c.erase_xuid();
         author_m.erase_xuid();
         QMessageBox::information(this, tr("Operation complete"), tr("The XUID and \"is online ID\" flag for this file's editor have been wiped."));
         return;
      } else {
         author_c.erase_xuid();
         author_m.erase_xuid();
         QMessageBox::information(this, tr("Operation complete"), tr("The XUID and \"is online ID\" flag for this file's editor were already blank."));
         return;
      }
   });
   QObject::connect(this->ui.editedOnDate, &QDateTimeEdit::dateTimeChanged, [](const QDateTime& time) {
      auto variant = ReachEditorState::get().variant();
      if (!variant)
         return;
      auto ff = variant->get_firefight_data();
      if (!ff)
         return;
      uint64_t seconds = time.toSecsSinceEpoch();
      variant->contentHeader.data.modifiedBy.set_datetime(seconds);
      ff->variantHeader.modifiedBy.set_datetime(seconds);
   });
   //
   this->ui.authorGamertag->setValidator(QXBLGamertagValidator::getReachInstance());
   this->ui.editorGamertag->setValidator(QXBLGamertagValidator::getReachInstance());
}
void PageFFMetadata::updateDescriptionCharacterCount() {
   auto text = this->ui.headerDesc->toPlainText();
   auto length = text.size();
   auto readout = this->ui.headerDescCharacterLimit;
   readout->setText(tr("%1 / %2").arg(length).arg(127));
   if (length >= 128)
      readout->setStyleSheet("QLabel { color: red; }");
   else
      readout->setStyleSheet("");
}
void PageFFMetadata::updateFromVariant(GameVariant* variant) {
   auto ff = variant->get_firefight_data();
   if (!ff)
      return;
   const QSignalBlocker blocker0(this->ui.headerName);
   const QSignalBlocker blocker1(this->ui.headerDesc);
   const QSignalBlocker blocker2(this->ui.authorGamertag);
   const QSignalBlocker blocker3(this->ui.editorGamertag);
   const QSignalBlocker blocker4(this->ui.createdOnDate);
   const QSignalBlocker blocker5(this->ui.editedOnDate);
   this->ui.headerName->setText(QString::fromUtf16(ff->variantHeader.title));
   this->ui.headerDesc->setPlainText(QString::fromUtf16(ff->variantHeader.description));
   this->ui.authorGamertag->setText(QString::fromLatin1(ff->variantHeader.createdBy.author));
   this->ui.editorGamertag->setText(QString::fromLatin1(ff->variantHeader.modifiedBy.author));
   //
   QDateTime temp;
   temp.setSecsSinceEpoch(ff->variantHeader.createdBy.timestamp);
   this->ui.createdOnDate->setDateTime(temp);
   temp.setSecsSinceEpoch(ff->variantHeader.modifiedBy.timestamp);
   this->ui.editedOnDate->setDateTime(temp);
   //
   this->updateDescriptionCharacterCount();
}