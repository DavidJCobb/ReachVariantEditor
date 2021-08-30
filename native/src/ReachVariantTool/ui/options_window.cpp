#include "options_window.h"
#include "../helpers/ini.h"
#include "../services/ini.h"
#include <qdir.h>
#include <qstringlist.h>
#include <QFileDialog>
#include <filesystem>

ProgramOptionsDialog::ProgramOptionsDialog(QWidget* parent) : QDialog(parent) {
   ui.setupUi(this);
   //
   this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint); // no need for What's This yet
   //
   QObject::connect(this->ui.buttonCancel, &QPushButton::clicked, this, &ProgramOptionsDialog::close);
   QObject::connect(this->ui.buttonSave,   &QPushButton::clicked, this, &ProgramOptionsDialog::saveAndClose);
   //
   {  // Editing
      QObject::connect(this->ui.hideFirefightNoOps, &QCheckBox::stateChanged, [](int state) {
         ReachINI::Editing::bHideFirefightNoOps.pending.b = state == Qt::CheckState::Checked;
      });
   }
   //
   using default_dir_type = ReachINI::DefaultPathType;
   {  // Default load directory
      this->ui.defaultOpenPathAutoMCCSaved->setProperty("enum",    (uint32_t)default_dir_type::mcc_saved_content);
      this->ui.defaultOpenPathAutoMCCMM->setProperty("enum",       (uint32_t)default_dir_type::mcc_matchmaking_content);
      this->ui.defaultOpenPathAutoMCCBuiltIn->setProperty("enum",  (uint32_t)default_dir_type::mcc_built_in_content);
      this->ui.defaultOpenPathCWD->setProperty("enum", (uint32_t)default_dir_type::current_working_directory);
      this->ui.defaultOpenPathUseCustom->setProperty("enum",       (uint32_t)default_dir_type::custom);
      QObject::connect(this->ui.defaultOpenPathAutoMCCSaved,    &QRadioButton::toggled, this, &ProgramOptionsDialog::defaultLoadTypeChanged);
      QObject::connect(this->ui.defaultOpenPathAutoMCCMM,       &QRadioButton::toggled, this, &ProgramOptionsDialog::defaultLoadTypeChanged);
      QObject::connect(this->ui.defaultOpenPathAutoMCCBuiltIn,  &QRadioButton::toggled, this, &ProgramOptionsDialog::defaultLoadTypeChanged);
      QObject::connect(this->ui.defaultOpenPathCWD, &QRadioButton::toggled, this, &ProgramOptionsDialog::defaultLoadTypeChanged);
      QObject::connect(this->ui.defaultOpenPathUseCustom,       &QRadioButton::toggled, this, &ProgramOptionsDialog::defaultLoadTypeChanged);
      QObject::connect(this->ui.defaultOpenPathCustom, &QLineEdit::textEdited, this, [](const QString& text) {
         ReachINI::DefaultLoadPath::sCustomPath.pendingStr = text.toUtf8();
      });
   }
   {  // Default save directory
      this->ui.defaultSavePathCurrentFile->setProperty("enum",     (uint32_t)default_dir_type::path_of_open_file);
      this->ui.defaultSavePathAutoMCCSaved->setProperty("enum",    (uint32_t)default_dir_type::mcc_saved_content);
      this->ui.defaultSavePathCWD->setProperty("enum", (uint32_t)default_dir_type::current_working_directory);
      this->ui.defaultSavePathUseCustom->setProperty("enum",       (uint32_t)default_dir_type::custom);
      QObject::connect(this->ui.defaultSavePathCurrentFile,     &QRadioButton::toggled, this, &ProgramOptionsDialog::defaultSaveTypeChanged);
      QObject::connect(this->ui.defaultSavePathAutoMCCSaved,    &QRadioButton::toggled, this, &ProgramOptionsDialog::defaultSaveTypeChanged);
      QObject::connect(this->ui.defaultSavePathCWD, &QRadioButton::toggled, this, &ProgramOptionsDialog::defaultSaveTypeChanged);
      QObject::connect(this->ui.defaultSavePathUseCustom,       &QRadioButton::toggled, this, &ProgramOptionsDialog::defaultSaveTypeChanged);
      QObject::connect(this->ui.defaultSavePathCustom, &QLineEdit::textEdited, this, [](const QString& text) {
         ReachINI::DefaultSavePath::sCustomPath.pendingStr = text.toUtf8();
      });
      QObject::connect(this->ui.defaultSavePathExcludeMCCNative, &QCheckBox::stateChanged, [](int state) {
         ReachINI::DefaultSavePath::bExcludeMCCBuiltInFolders.pending.b = state == Qt::CheckState::Checked;
      });
   }
   QObject::connect(this->ui.optionFullFilePathsInWindowTitle, &QCheckBox::stateChanged, [](int state) {
      ReachINI::UIWindowTitle::bShowFullPath.pending.b = state == Qt::CheckState::Checked;
   });
   QObject::connect(this->ui.optionVariantNameInWindowTitle, &QCheckBox::stateChanged, [](int state) {
      ReachINI::UIWindowTitle::bShowVariantTitle.pending.b = state == Qt::CheckState::Checked;
   });
   QObject::connect(this->ui.optionTheme, &QLineEdit::textEdited, this, [](const QString& text) {
       ReachINI::UIWindowTitle::sTheme.pendingStr = text.toUtf8();
    });
   QObject::connect(this->ui.themeFileDialog, &QPushButton::pressed, this, &ProgramOptionsDialog::openFile);
}
void ProgramOptionsDialog::close() {
   ReachINI::get().abandon_pending_changes();
   this->done(0);
}
void ProgramOptionsDialog::refreshWidgetsFromINI() {
   using default_dir_type = ReachINI::DefaultPathType;
   {  // Editing
      const QSignalBlocker blocker0(this->ui.hideFirefightNoOps);
      this->ui.hideFirefightNoOps->setChecked(ReachINI::Editing::bHideFirefightNoOps.current.b);
   }
   {  // Default load directory
      std::vector<QRadioButton*> buttons = {
         this->ui.defaultOpenPathAutoMCCSaved,
         this->ui.defaultOpenPathAutoMCCMM,
         this->ui.defaultOpenPathAutoMCCBuiltIn,
         this->ui.defaultOpenPathCWD,
         this->ui.defaultOpenPathUseCustom,
      };
      QRadioButton* defaultWidget = nullptr;
      uint32_t      defaultEnum   = ReachINI::DefaultLoadPath::uPathType.default.u;
      bool found = false;
      auto type  = ReachINI::DefaultLoadPath::uPathType.current.u;
      for (auto widget : buttons) {
         const QSignalBlocker blocker(widget);
         bool is = type == widget->property("enum");
         if (is)
            found = true;
         widget->setChecked(is);
         if (!found) {
            if (widget->property("enum") == defaultEnum)
               defaultWidget = widget;
         }
      }
      if (!found && defaultWidget) {
         const QSignalBlocker blocker(defaultWidget);
         defaultWidget->setChecked(true);
      }
      //
      this->ui.defaultOpenPathCustom->setText(QString::fromUtf8(ReachINI::DefaultLoadPath::sCustomPath.currentStr.c_str()));
   }
   {  // Default save directory
      std::vector<QRadioButton*> buttons = {
         this->ui.defaultSavePathCurrentFile,
         this->ui.defaultSavePathAutoMCCSaved,
         this->ui.defaultSavePathCWD,
         this->ui.defaultSavePathUseCustom,
      };
      QRadioButton* defaultWidget = nullptr;
      uint32_t      defaultEnum   = ReachINI::DefaultSavePath::uPathType.default.u;
      bool found = false; // if the setting's current value doesn't match any widget, use the default
      auto type  = ReachINI::DefaultSavePath::uPathType.current.u;
      for (auto widget : buttons) {
         const QSignalBlocker blocker(widget);
         bool is = type == widget->property("enum");
         if (is)
            found = true;
         widget->setChecked(is);
         if (!found) {
            if (widget->property("enum") == defaultEnum)
               defaultWidget = widget;
         }
      }
      if (!found && defaultWidget) {
         const QSignalBlocker blocker(defaultWidget);
         defaultWidget->setChecked(true);
      }
      //
      this->ui.defaultSavePathCustom->setText(QString::fromUtf8(ReachINI::DefaultSavePath::sCustomPath.currentStr.c_str()));
      this->ui.defaultSavePathExcludeMCCNative->setChecked(ReachINI::DefaultSavePath::bExcludeMCCBuiltInFolders.current.b);
   }
   {  // Window path
      const QSignalBlocker blocker0(this->ui.optionFullFilePathsInWindowTitle);
      const QSignalBlocker blocker1(this->ui.optionVariantNameInWindowTitle);
      this->ui.optionFullFilePathsInWindowTitle->setChecked(ReachINI::UIWindowTitle::bShowFullPath.current.b);
      this->ui.optionVariantNameInWindowTitle->setChecked(ReachINI::UIWindowTitle::bShowVariantTitle.current.b);
      this->ui.optionTheme->setText(ReachINI::UIWindowTitle::sTheme.currentStr.c_str());
   }
}
void ProgramOptionsDialog::saveAndClose() {
   ReachINI::get().save();
   this->accept();
}
void ProgramOptionsDialog::defaultLoadTypeChanged() {
   using type = ReachINI::DefaultPathType;
   type which;
   if (this->ui.defaultOpenPathAutoMCCSaved->isChecked())
      which = type::mcc_saved_content;
   else if (this->ui.defaultOpenPathAutoMCCBuiltIn->isChecked())
      which = type::mcc_built_in_content;
   else if (this->ui.defaultOpenPathAutoMCCMM->isChecked())
      which = type::mcc_matchmaking_content;
   else if (this->ui.defaultOpenPathCWD->isChecked())
      which = type::current_working_directory;
   else if (this->ui.defaultOpenPathUseCustom->isChecked())
      which = type::custom;
   ReachINI::DefaultLoadPath::uPathType.pending.u = (uint32_t)which;
}
void ProgramOptionsDialog::defaultSaveTypeChanged() {
   using type = ReachINI::DefaultPathType;
   type which;
   if (this->ui.defaultSavePathCurrentFile->isChecked())
      which = type::path_of_open_file;
   else  if (this->ui.defaultSavePathAutoMCCSaved->isChecked())
      which = type::mcc_saved_content;
   else if (this->ui.defaultSavePathCWD->isChecked())
      which = type::current_working_directory;
   else if (this->ui.defaultSavePathUseCustom->isChecked())
      which = type::custom;
   ReachINI::DefaultSavePath::uPathType.pending.u = (uint32_t)which;
}

void ProgramOptionsDialog::openFile() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select theme"), "~/", tr("QT Stylesheets (*.qss)"));
    if (!fileName.isNull()) {
        this->ui.optionTheme->setText(fileName.toUtf8());
        ReachINI::UIWindowTitle::sTheme.pendingStr = fileName.toUtf8();
    }
}