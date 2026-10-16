#pragma once
#include <QWidget>

namespace DebugHelperFunctions {
   void break_on_variant();
   bool crash();
   void export_variant_triggers_english(QWidget* parentWindow);
   void export_variant_strings(QWidget* parentWindow);
   void suspend_other_threads();
   void test_loading_hrek_fonts(QWidget* parentWindow);
}