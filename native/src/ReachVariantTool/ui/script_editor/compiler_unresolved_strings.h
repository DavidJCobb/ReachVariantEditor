#pragma once
#include <QtWidgets/QDialog>
#include "ui_compiler_unresolved_strings.h"
#include "../../game_variants/components/megalo/compiler/compiler.h"

class CompilerUnresolvedStringsDialog : public QDialog {
   Q_OBJECT
   //
   public:
      using unresolved_str = Megalo::Compiler::unresolved_str;
      //
      CompilerUnresolvedStringsDialog(QWidget* parent = Q_NULLPTR);
      //
      static bool handleCompiler(QWidget* parent, Megalo::Compiler&);
      //
   private slots:
   private:
      void commit();
      void takeCompiler(Megalo::Compiler&);
      void selectReference(int32_t str, int32_t ref);
      void updateStatus();
      //
      QList<unresolved_str*> getRefsByKey(const QString &);
      //
      unresolved_str* getUnresolvedStr();
      //
   private:
      Ui::CompilerUnresolvedStringsDialog ui;
      //
      Megalo::Compiler* _compiler = nullptr;
      int32_t _currentStr = -1;
      int32_t _currentRef = -1;
      MegaloStringRef _dummyStringRef; // used to interface with the ReachStringPicker widget
      //
      QList<QString> _keys;
};
