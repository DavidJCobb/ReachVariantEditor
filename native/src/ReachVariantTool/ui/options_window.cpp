#include "options_window.h"
#include "../helpers/ini.h"
#include "../helpers/qt/color.h"
#include "../services/ini.h"
#include "../services/syntax_highlight_option.h"
#include <QFileDialog>

ProgramOptionsDialog::ProgramOptionsDialog(QWidget* parent) : QDialog(parent) {
   ui.setupUi(this);
   //
   this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint); // no need for What's This yet
   this->ui.tabWidget->setCurrentIndex(0);
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
         ReachINI::DefaultLoadPath::sCustomPath.pendingStr = (const char*)text.toUtf8();
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
         ReachINI::DefaultSavePath::sCustomPath.pendingStr = (const char*)text.toUtf8();
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
       ReachINI::UIWindowTitle::sTheme.pendingStr = (const char*)text.toUtf8();
    });
   QObject::connect(this->ui.themeFileDialog, &QPushButton::pressed, this, &ProgramOptionsDialog::openFile);
   //
   #pragma region Script code editor
      #pragma region General
      {
         auto* fontpicker = this->ui.codeEditFont;
         {
            auto& setting = ReachINI::CodeEditor::sFontFamily;
            auto  family  = QString::fromUtf8(setting.currentStr.c_str());
            QFont font    = QFont(family);
            if (!font.exactMatch()) {
               // This typeface isn't available
               family = QString::fromUtf8(setting.initialStr.c_str());
               font   = QFont(family);
            }
            fontpicker->setCurrentFont(font);
         }
         QObject::connect(fontpicker, &QFontComboBox::currentFontChanged, this, [](const QFont& font) {
            ReachINI::CodeEditor::sFontFamily.pendingStr = (const char*)font.family().toUtf8();
         });
      }
      {
         auto& enable = ReachINI::CodeEditor::bOverrideBackColor;
         auto& color  = ReachINI::CodeEditor::sBackColor;
         //
         auto* e_widget = this->ui.codeEditEnableBackColor;
         auto* c_widget = this->ui.codeEditBackColor;
         e_widget->setChecked(enable.current.b);
         {
            cobb::qt::css_color_parse_error error;
            QColor c = cobb::qt::parse_css_color(QString::fromUtf8(color.currentStr.c_str()), error);
            if (error != cobb::qt::css_color_parse_error::none) {
               c = cobb::qt::parse_css_color(QString::fromUtf8(color.initialStr.c_str()), error);
            }
            c_widget->setColor(c);
         }
         QObject::connect(e_widget, &QCheckBox::toggled, this, [&enable, &color](bool checked) {
            enable.pending.b = checked;
         });
         QObject::connect(c_widget, &ColorPickerButton::colorChanged, this, [&enable, &color](const QColor& c) {
            color.pendingStr = (const char*)cobb::qt::stringify_css_color(c, cobb::qt::css_color_format::rgb).toUtf8();
         });
      }
      {
         auto& enable = ReachINI::CodeEditor::bOverrideTextColor;
         auto& color  = ReachINI::CodeEditor::sTextColor;
         //
         auto* e_widget = this->ui.codeEditEnableTextColor;
         auto* c_widget = this->ui.codeEditTextColor;
         e_widget->setChecked(enable.current.b);
         {
            cobb::qt::css_color_parse_error error;
            QColor c = cobb::qt::parse_css_color(QString::fromUtf8(color.currentStr.c_str()), error);
            if (error != cobb::qt::css_color_parse_error::none) {
               c = cobb::qt::parse_css_color(QString::fromUtf8(color.initialStr.c_str()), error);
            }
            c_widget->setColor(c);
         }
         QObject::connect(e_widget, &QCheckBox::toggled, this, [&enable, &color](bool checked) {
            enable.pending.b = checked;
         });
         QObject::connect(c_widget, &ColorPickerButton::colorChanged, this, [&enable, &color](const QColor& c) {
            color.pendingStr = (const char*)cobb::qt::stringify_css_color(c, cobb::qt::css_color_format::rgb).toUtf8();
         });
      }
      #pragma endregion
      #pragma region Syntax highlighting
      {  // syn
         QComboBox* widget = this->ui.synHighType;
         widget->clear();
         QObject::connect(widget, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this]() {
            const auto blocker0 = QSignalBlocker(this->ui.synHighBold);
            const auto blocker1 = QSignalBlocker(this->ui.synHighItalic);
            const auto blocker2 = QSignalBlocker(this->ui.synHighUnderline);
            const auto blocker3 = QSignalBlocker(this->ui.synHighColor);
            //
            cobb::ini::setting* setting = nullptr;
            {
               auto list = this->currentSyntaxHighlightOptions();
               if (!list.isEmpty())
                  setting = list[0];
            }
            if (!setting) { // shouldn't happen
               this->ui.synHighBold->setChecked(false);
               this->ui.synHighBold->setEnabled(false);
               this->ui.synHighItalic->setChecked(false);
               this->ui.synHighItalic->setEnabled(false);
               this->ui.synHighUnderline->setChecked(false);
               this->ui.synHighUnderline->setEnabled(false);
               this->ui.synHighColor->setEnabled(false);
            } else {
               auto value = QString::fromUtf8(setting->pendingStr.c_str());
               if (value.isEmpty())
                  value = QString::fromUtf8(setting->currentStr.c_str());
               auto option = ReachINI::syntax_highlight_option::fromString(value);
               //
               this->ui.synHighBold->setChecked(option.bold);
               this->ui.synHighBold->setEnabled(true);
               this->ui.synHighItalic->setChecked(option.italic);
               this->ui.synHighItalic->setEnabled(true);
               this->ui.synHighUnderline->setChecked(option.underline);
               this->ui.synHighUnderline->setEnabled(true);
               this->ui.synHighColor->setEnabled(true);
               this->ui.synHighColor->setColor(option.colors.text);
            }
         });
         widget->addItem("Comment",    QStringList({"sFormatCommentBlock", "sFormatCommentLine"}));
         widget->addItem("Keyword",    "sFormatKeyword");
         widget->addItem("Subkeyword", "sFormatSubkeyword");
         widget->addItem("Number",     "sFormatNumber");
         widget->addItem("Operator",   "sFormatOperator");
         widget->addItem("String",     "sFormatStringSimple");
         //
         QObject::connect(this->ui.synHighBold, &QAbstractButton::toggled, this, [this](bool checked) {
            auto c = this->syntaxHighlightOptionFromUI();
            c.bold = checked;
            this->setCurrentSyntaxHighlightOptions(c);
         });
         QObject::connect(this->ui.synHighItalic, &QAbstractButton::toggled, this, [this](bool checked) {
            auto c = this->syntaxHighlightOptionFromUI();
            c.italic = checked;
            this->setCurrentSyntaxHighlightOptions(c);
         });
         QObject::connect(this->ui.synHighUnderline, &QAbstractButton::toggled, this, [this](bool checked) {
            auto c = this->syntaxHighlightOptionFromUI();
            c.underline = checked;
            this->setCurrentSyntaxHighlightOptions(c);
         });
         QObject::connect(this->ui.synHighColor, &ColorPickerButton::colorChanged, this, [this](QColor color) {
            auto c = this->syntaxHighlightOptionFromUI();
            c.colors.text = color;
            this->setCurrentSyntaxHighlightOptions(c);
         });
      }
      #pragma endregion
      #pragma region Compiler
      QObject::connect(this->ui.compilerEnableInlineIfs, &QCheckBox::stateChanged, [](int state) {
         ReachINI::Compiler::bInlineIfs.pending.b = state == Qt::CheckState::Checked;
      });
      #pragma endregion
   #pragma endregion
}

QVector<cobb::ini::setting*> ProgramOptionsDialog::currentSyntaxHighlightOptions() const {
   QVector<cobb::ini::setting*> out;
   //
   auto& ini  = ReachINI::get();
   auto  data = this->ui.synHighType->currentData();
   if (data.type() == QMetaType::QStringList) {
      auto list = data.value<QStringList>();
      for (auto& name : list) {
         auto* s = ini.get_setting("CodeEditor", name.toStdString().c_str());
         if (s)
            out.push_back(s);
      }
   } else if (data.type() == QMetaType::QString) {
      auto* s = ini.get_setting("CodeEditor", data.value<QString>().toStdString().c_str());
      if (s)
         out.push_back(s);
   }
   return out;
}
ReachINI::syntax_highlight_option ProgramOptionsDialog::syntaxHighlightOptionFromUI() const {
   ReachINI::syntax_highlight_option out;
   out.bold        = this->ui.synHighBold->isChecked();
   out.italic      = this->ui.synHighItalic->isChecked();
   out.underline   = this->ui.synHighUnderline->isChecked();
   out.colors.text = this->ui.synHighColor->color();
   return out;
}
void ProgramOptionsDialog::setCurrentSyntaxHighlightOptions(const ReachINI::syntax_highlight_option& c) const {
   auto data     = ReachINI::stringify_syntax_highlight_option(c);
   auto settings = this->currentSyntaxHighlightOptions();
   for (auto* s : settings)
      s->pendingStr = data.toStdString();
}

void ProgramOptionsDialog::close() {
   ReachINI::get().abandon_pending_changes();
   this->done(0);
}
void ProgramOptionsDialog::refreshWidgetsFromINI() {
   using default_dir_type = ReachINI::DefaultPathType;
   {  // Compiler
      const QSignalBlocker blocker0(this->ui.compilerEnableInlineIfs);
      this->ui.compilerEnableInlineIfs->setChecked(ReachINI::Compiler::bInlineIfs.current.b);
   }
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
      uint32_t      defaultEnum   = ReachINI::DefaultLoadPath::uPathType.initial.u;
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
      uint32_t      defaultEnum   = ReachINI::DefaultSavePath::uPathType.initial.u;
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
        ReachINI::UIWindowTitle::sTheme.pendingStr = (const char*)fileName.toUtf8();
    }
}
