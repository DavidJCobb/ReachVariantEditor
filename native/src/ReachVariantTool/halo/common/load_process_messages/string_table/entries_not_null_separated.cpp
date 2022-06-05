#include "entries_not_null_separated.h"

namespace halo::common::load_process_messages {
   QString string_table_entries_not_null_separated::to_string() const {
      QString lang;
      switch (this->info.language) {
         using enum halo::localization_language;
         case english:
            lang = QString("English");
            break;
         case japanese:
            lang = QString("Japanese");
            break;
         case german:
            lang = QString("German");
            break;
         case french:
            lang = QString("French");
            break;
         case spanish:
            lang = QString("Spanish");
            break;
         case mexican:
            lang = QString("Spanish (Latin American)");
            break;
         case italian:
            lang = QString("Italian");
            break;
         case korean:
            lang = QString("Korean");
            break;
         case chinese_simplified:
            lang = QString("Chinese (Simplified)");
            break;
         case chinese_traditional:
            lang = QString("Chinese (Traditional)");
            break;
         case portugese:
            lang = QString("Portugese");
            break;
         case polish:
            lang = QString("Polish");
            break;
         default:
            lang = QString("<unknown language>");
            break;
      }
      return QString("The game requires that string table entries and their translations be null-separated; overlap is allowed only for identical translations. String #%1 specified offset %3 for its %2 translation. This offset does not come after a null terminator.")
         .arg(this->info.string_index)
         .arg(lang)
         .arg(this->info.string_offset);
   }
}