#include "debug_helper_functions.h"
#include "../../editor_state.h"
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextCodec>
#include <QTextStream>



namespace DebugHelperFunctions {
   void break_on_variant() {
      auto variant = ReachEditorState::get().variant();
      __debugbreak();
   }
   void export_variant_triggers_english(QWidget* parentWindow) {
      auto variant = ReachEditorState::get().variant();
      if (!variant)
         return;
      auto mp = variant->get_multiplayer_data();
      if (!mp)
         return;
      QString fileName = QFileDialog::getSaveFileName(
         parentWindow,
         QObject::tr("Save Triggers as Text"), // window title
         "",
         QObject::tr("Text File (*.txt);;All Files (*)") // filetype filters
      );
      if (fileName.isEmpty())
         return;
      QFile file(fileName);
      if (!file.open(QIODevice::WriteOnly)) {
         QMessageBox::information(parentWindow, QObject::tr("Unable to open file for writing"), file.errorString());
         return;
      }
      QTextStream out(&file);
      out << QString::fromUtf16(mp->variantHeader.title);
      out << "\r\n\r\n";
      //
      {  // Scripted traits
         out << "SCRIPTED TRAITS:\r\n";
         auto& list = mp->scriptData.traits;
         auto  size = list.size();
         for (size_t i = 0; i < size; i++) {
            auto& item      = list[i];
            auto  formatted = QString("%1: %2\r\n").arg(i);
            if (item.name) {
               formatted = formatted.arg(QString::fromUtf8(item.name->get_content(reach::language::english).c_str()));
            } else {
               formatted = formatted.arg("");
            }
            out << formatted;
         }
         out << "\r\n";
      }
      {  // Forge labels
         out << "FORGE LABELS:\r\n";
         auto& list = mp->scriptContent.forgeLabels;
         auto  size = list.size();
         for (size_t i = 0; i < size; i++) {
            auto& label     = list[i];
            auto  formatted = QString("%1: %2\r\n").arg(i);
            if (label.name) {
               formatted = formatted.arg(QString::fromUtf8(label.name->get_content(reach::language::english).c_str()));
            } else {
               formatted = formatted.arg("");
            }
            out << formatted;
         }
         out << "\r\n";
      }
      auto& triggers = mp->scriptContent.triggers;
      for (size_t i = 0; i < triggers.size(); ++i) {
         auto& trigger = triggers[i];
         if (trigger.entryType == Megalo::entry_type::subroutine)
            continue;
         std::string formatted;
         cobb::sprintf(formatted, "TRIGGER #%d:\r\n", i);
         out << formatted.c_str();
         formatted.clear();
         trigger.to_string(triggers, formatted);
         out << formatted.c_str();
         out << "\r\n";
      }
   }
   void export_variant_strings(QWidget* parentWindow) {
      auto variant = ReachEditorState::get().variant();
      if (!variant)
         return;
      auto mp = variant->get_multiplayer_data();
      if (!mp)
         return;
      QString fileName = QFileDialog::getSaveFileName(
         parentWindow,
         QObject::tr("Save Strings as Text"), // window title
         "",
         QObject::tr("Text File (*.txt);;All Files (*)") // filetype filters
      );
      if (fileName.isEmpty())
         return;
      QFile file(fileName);
      if (!file.open(QIODevice::WriteOnly)) {
         QMessageBox::information(parentWindow, QObject::tr("Unable to open file for writing"), file.errorString());
         return;
      }
      QTextStream out(&file);
      out.setCodec(QTextCodec::codecForName("UTF-8"));
      out << QString::fromUtf16(mp->variantHeader.title);
      out << "\r\n\r\n";
      auto& table = mp->scriptData.strings;
      for (size_t i = 0; i < table.strings.size(); i++) {
         auto& string = table.strings[i];
         out << "STRING #" << i << ":\r\n";
         for (auto& str : string.languages()) {
            out << "   ";
            out << QString::fromUtf8(str.c_str()) << "\r\n";
         }
         out << "\r\n";
      }
   }
   //
   void export_documentation(QWidget* parentWindow) {

      //
      // TODO
      //

   }
}