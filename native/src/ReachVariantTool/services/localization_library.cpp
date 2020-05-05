#include "localization_library.h"
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QVector>

namespace {
   struct _lang_code {
      QString         code;
      reach::language value;
      //
      _lang_code(QString c, reach::language l) : code(c), value(l) {}
   };
   _lang_code _languages[] = {
      _lang_code("en", reach::language::english),
      _lang_code("jp", reach::language::japanese),
      _lang_code("de", reach::language::german),
      _lang_code("fr", reach::language::french),
      _lang_code("es", reach::language::spanish),
      _lang_code("mx", reach::language::mexican),
      _lang_code("it", reach::language::italian),
      _lang_code("ko", reach::language::korean),
      _lang_code("zh", reach::language::chinese_traditional),
      _lang_code("pt", reach::language::portugese),
      _lang_code("po", reach::language::polish),
   };
   reach::language _code_to_language(const QString& c) {
      for (int i = 0; i < std::extent<decltype(_languages)>::value; ++i) {
         if (_languages[i].code.compare(c, Qt::CaseInsensitive) == 0)
            return _languages[i].value;
      }
      return reach::language::not_a_language;
   }
   reach::language _code_to_language(const QStringRef& c) {
      for (int i = 0; i < std::extent<decltype(_languages)>::value; ++i) {
         if (_languages[i].code.compare(c, Qt::CaseInsensitive) == 0)
            return _languages[i].value;
      }
      return reach::language::not_a_language;
   }
   //
   ReachStringTable _dummy_string_owner(1000, 99999);
}

LocalizedStringLibrary::Entry::Entry() {
   this->content = new ReachString(_dummy_string_owner);
}
LocalizedStringLibrary::Entry::~Entry() {
   if (this->content) {
      delete this->content;
      this->content = nullptr;
   }
}
LocalizedStringLibrary::Entry::permutation::permutation() {
   this->content = new ReachString(_dummy_string_owner);
}
LocalizedStringLibrary::Entry::permutation::~permutation() {
   if (this->content) {
      delete this->content;
      this->content = nullptr;
   }
}
void LocalizedStringLibrary::Entry::copy(reach::language lang, QString& out) {
   out = QString::fromStdString(this->content->language(lang));
}
void LocalizedStringLibrary::Entry::apply_permutation(reach::language lang, QString& out, int32_t value) {
   uint32_t search = value;
   if (search < 0)
      search = -search;
   //
   for (auto& perm : this->permutations) {
      if (perm.integer == search) {
         out = QString::fromStdString(perm.content->language(lang)).arg(value);
         return;
      }
   }
   out = QString::fromStdString(this->content->language(lang)).arg(value);
}
LocalizedStringLibrary::Entry::permutation* LocalizedStringLibrary::Entry::get_or_create_permutation(uint32_t v) {
   for (auto& perm : this->permutations)
      if (perm.integer == v)
         return &perm;
   auto& p = this->permutations.emplace_back();
   return &p;
}

LocalizedStringLibrary::LocalizedStringLibrary() {
   auto dir = QDir(":/localized_string_library/");
   for (auto entry : dir.entryList()) {
      auto file = QFile(entry);
      if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
         continue;
      //
      auto   stream  = QTextStream(&file);
      Entry* current = nullptr;
      while (!stream.atEnd()) {
         QString line = stream.readLine().trimmed();
         if (line[0] == '#') // comment
            continue;
         if (line[0] == '[') { // header
            if (!line.endsWith(']'))
               continue;
            auto name = line.midRef(1, line.size() - 2);
            if (name.isEmpty())
               continue;
            current = new Entry;
            current->internal_name = name.toString();
            this->entries.push_back(current);
         }
         if (!current) // skip lines until we find a header.
            continue;
         auto index = line.indexOf('=');
         if (index <= 0)
            continue;
         auto key   = line.midRef(0, index).trimmed();
         auto value = line.midRef(index + 1).trimmed();
         if (key.compare(QString("name"), Qt::CaseInsensitive) == 0) {
            current->friendly_name = value.toString();
            continue;
         }
         if (key.compare(QString("cate"), Qt::CaseInsensitive) == 0) {
            current->category = value.toString(); // TODO: enum
            continue;
         }
         if (key.compare(QString("desc"), Qt::CaseInsensitive) == 0) {
            current->description = value.toString();
            continue;
         }
         if (key.compare(QString("tags"), Qt::CaseInsensitive) == 0) {
            auto vec = value.split(',');
            current->tags.reserve(vec.size());
            for (auto& s : vec)
               current->tags.push_back(s.trimmed().toString());
            continue;
         }
         if (key.compare(QString("source"), Qt::CaseInsensitive) == 0) {
            current->source = value.toString();
            continue;
         }
         if (key.compare(QString("sourcetype"), Qt::CaseInsensitive) == 0) {
            current->source_type = value.toString(); // TODO: enum
            continue;
         }
         //
         // SYNTAX:
         // A language code as a key specifies a basic entry. A language code followed by a number specifies 
         // a specific permutation for that number.
         //
         // Example:
         //
         // en =%1 points
         // en0=no points
         // en1=%1 point
         //
         auto code = key.left(2);
         auto lang = _code_to_language(code);
         if (lang != reach::language::not_a_language) {
            if (key.size() > 2) {
               bool ok    = false;
               auto index = key.mid(2).toInt(&ok);
               if (!ok)
                  continue;
               auto perm = current->get_or_create_permutation(index);
               perm->content->language(lang) = value.toString().toStdString();
               continue;
            }
            current->content->language(lang) = value.toString().toStdString();
         }
      }
   }
}
LocalizedStringLibrary::~LocalizedStringLibrary() {
   for (auto entry : this->entries)
      delete entry;
   this->entries.clear();
}