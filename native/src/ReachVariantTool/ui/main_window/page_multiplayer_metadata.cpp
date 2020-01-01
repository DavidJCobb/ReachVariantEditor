#include "page_multiplayer_metadata.h"
#include <QMessageBox>
#include "ui/generic/QXBLGamertagValidator.h"

namespace {
   static constexpr int ce_gametypeCategoryIndexActionSack = 16;
}

PageMPMetadata::PageMPMetadata(QWidget* parent) : QWidget(parent) {
   ui.setupUi(this);
   //
   auto& editor = ReachEditorState::get();
   QObject::connect(&editor, &ReachEditorState::variantAcquired, this, &PageMPMetadata::updateFromVariant);
   //
   {  // set up engine categories
      auto widget = this->ui.engineCategory;
      widget->clear();
      widget->addItem(tr("Capture the Flag", "Engine Category"), QVariant(0));
      widget->addItem(tr("Slayer", "Engine Category"), QVariant(1));
      widget->addItem(tr("Oddball", "Engine Category"), QVariant(2));
      widget->addItem(tr("King of the Hill", "Engine Category"), QVariant(3));
      widget->addItem(tr("Juggernaut", "Engine Category"), QVariant(4));
      widget->addItem(tr("Territories", "Engine Category"), QVariant(5));
      widget->addItem(tr("Assault", "Engine Category"), QVariant(6));
      widget->addItem(tr("Infection", "Engine Category"), QVariant(7));
      widget->addItem(tr("VIP", "Engine Category"), QVariant(8));
      widget->addItem(tr("Invasion", "Engine Category"), QVariant(9));
      widget->addItem(tr("Stockpile", "Engine Category"), QVariant(10));
      widget->addItem(tr("Race", "Engine Category"), QVariant(12));
      widget->addItem(tr("Headhunter", "Engine Category"), QVariant(13));
      widget->addItem(tr("Action Sack", "Engine Category"), QVariant(ce_gametypeCategoryIndexActionSack));
   }
   //
   QObject::connect(this->ui.headerName, &QLineEdit::textEdited, [this](const QString& text) {
      auto variant = ReachEditorState::get().variant();
      if (!variant)
         return;
      auto u16s = text.toStdU16String();
      const char16_t* value = u16s.c_str();
      variant->contentHeader.data.set_title(value);
      variant->multiplayer.data->as_multiplayer()->variantHeader.set_title(value);
      //
      emit titleChanged(value);
   });
   QObject::connect(this->ui.headerName, &QLineEdit::textEdited, this, &PageMPMetadata::updateDescriptionCharacterCount);
   //
   auto desc      = this->ui.headerDesc;
   auto descCount = this->ui.headerDescCharacterLimit;
   QObject::connect(this->ui.headerDesc, &QPlainTextEdit::textChanged, [desc, descCount]() {
      auto text    = desc->toPlainText();
      auto variant = ReachEditorState::get().variant();
      if (!variant)
         return;
      auto u16s = text.toStdU16String();
      const char16_t* value = u16s.c_str();
      variant->contentHeader.data.set_description(value);
      variant->multiplayer.data->as_multiplayer()->variantHeader.set_description(value);
   });
   QObject::connect(this->ui.headerDesc, &QPlainTextEdit::textChanged, this, &PageMPMetadata::updateDescriptionCharacterCount);
   //
   QObject::connect(this->ui.engineIcon, QOverload<int>::of(&QComboBox::currentIndexChanged), [](int value) {
      auto variant = ReachEditorState::get().variant();
      if (!variant)
         return;
      auto data = ReachEditorState::get().multiplayerData();
      if (!data)
         return;
      variant->contentHeader.data.engineIcon = value;
      data->engineIcon = value;
      data->variantHeader.engineIcon = value;
   });
   {
      auto widget = this->ui.engineCategory;
      QObject::connect(widget, QOverload<int>::of(&QComboBox::currentIndexChanged), [widget](int value) {
         auto variant = ReachEditorState::get().variant();
         if (!variant)
            return;
         auto data = ReachEditorState::get().multiplayerData();
         if (!data)
            return;
         //
         int cat = widget->currentData().toInt();
         //
         variant->contentHeader.data.engineCategory = cat;
         data->engineCategory = cat;
         data->variantHeader.engineCategory = cat;
      });
   }
   //
   QObject::connect(this->ui.authorGamertag, &QLineEdit::textEdited, [](const QString& text) {
      auto variant = ReachEditorState::get().variant();
      if (!variant)
         return;
      auto latin = text.toLatin1();
      variant->contentHeader.data.createdBy.set_author_name(latin.constData());
      variant->multiplayer.data->as_multiplayer()->variantHeader.createdBy.set_author_name(latin.constData());
   });
   QObject::connect(this->ui.eraseAuthorXUID, &QPushButton::clicked, [this]() {
      auto variant = ReachEditorState::get().variant();
      if (!variant)
         return;
      auto& author_c = variant->contentHeader.data.createdBy;
      auto& author_m = variant->multiplayer.data->as_multiplayer()->variantHeader.createdBy;
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
      uint64_t seconds = time.toSecsSinceEpoch();
      variant->contentHeader.data.createdBy.set_datetime(seconds);
      variant->multiplayer.data->as_multiplayer()->variantHeader.createdBy.set_datetime(seconds);
   });
   QObject::connect(this->ui.editorGamertag, &QLineEdit::textEdited, [](const QString& text) {
      auto variant = ReachEditorState::get().variant();
      if (!variant)
         return;
      auto latin = text.toLatin1();
      variant->contentHeader.data.modifiedBy.set_author_name(latin.constData());
      variant->multiplayer.data->as_multiplayer()->variantHeader.modifiedBy.set_author_name(latin.constData());
   });
   QObject::connect(this->ui.eraseEditorXUID, &QPushButton::clicked, [this]() {
      auto variant = ReachEditorState::get().variant();
      if (!variant)
         return;
      auto& author_c = variant->contentHeader.data.modifiedBy;
      auto& author_m = variant->multiplayer.data->as_multiplayer()->variantHeader.modifiedBy;
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
      uint64_t seconds = time.toSecsSinceEpoch();
      variant->contentHeader.data.modifiedBy.set_datetime(seconds);
      variant->multiplayer.data->as_multiplayer()->variantHeader.modifiedBy.set_datetime(seconds);
   });
   //
   this->ui.authorGamertag->setValidator(QXBLGamertagValidator::getReachInstance());
   this->ui.editorGamertag->setValidator(QXBLGamertagValidator::getReachInstance());
}
void PageMPMetadata::updateDescriptionCharacterCount() {
   auto text = this->ui.headerDesc->toPlainText();
   auto length = text.size();
   auto readout = this->ui.headerDescCharacterLimit;
   readout->setText(tr("%1 / %2").arg(length).arg(127));
   if (length >= 128)
      readout->setStyleSheet("QLabel { color: red; }");
   else
      readout->setStyleSheet("");
}
void PageMPMetadata::updateFromVariant(GameVariant* variant) {
   auto mp = variant->get_multiplayer_data();
   if (!mp)
      return;
   const QSignalBlocker blocker0(this->ui.headerName);
   const QSignalBlocker blocker1(this->ui.headerDesc);
   const QSignalBlocker blocker2(this->ui.authorGamertag);
   const QSignalBlocker blocker3(this->ui.editorGamertag);
   const QSignalBlocker blocker4(this->ui.createdOnDate);
   const QSignalBlocker blocker5(this->ui.editedOnDate);
   this->ui.headerName->setText(QString::fromUtf16(mp->variantHeader.title));
   this->ui.headerDesc->setPlainText(QString::fromUtf16(mp->variantHeader.description));
   this->ui.authorGamertag->setText(QString::fromLatin1(mp->variantHeader.createdBy.author));
   this->ui.editorGamertag->setText(QString::fromLatin1(mp->variantHeader.modifiedBy.author));
   //
   {
      int index;
      {
         const QSignalBlocker blocker0(this->ui.engineIcon);
         const QSignalBlocker blocker1(this->ui.engineCategory);
         this->ui.engineIcon->setCurrentIndex(mp->engineIcon);
         index = this->ui.engineCategory->findData((int)mp->engineCategory);
         if (index != -1)
            this->ui.engineCategory->setCurrentIndex(index);
      }
      if (index == -1) {
         //
         // The game variant uses an extended category; those don't work in MCC. Alert the user and force 
         // it to a good value.
         //
         index = this->ui.engineCategory->findData(ce_gametypeCategoryIndexActionSack);
         if (index != -1) {
            QMessageBox::information(this, tr("Bad gametype category index"), tr("This gametype uses an undefined category number (%1). Although Halo: Reach's Xbox 360 release allowed developers to create custom categories, Halo: The Master Chief Collection only supports the built-in categories as of this writing; game variants in extended categories cannot be located.\n\nThis game variant will be switched to the \"Action Sack\" category.").arg((int)mp->engineCategory));
            this->ui.engineCategory->setCurrentIndex(index);
         }
      }
   }
   //
   QDateTime temp;
   temp.setSecsSinceEpoch(mp->variantHeader.createdBy.timestamp);
   this->ui.createdOnDate->setDateTime(temp);
   temp.setSecsSinceEpoch(mp->variantHeader.modifiedBy.timestamp);
   this->ui.editedOnDate->setDateTime(temp);
   //
   this->updateDescriptionCharacterCount();
}