#include "localization_library.h"
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QVector>
#include "../game_variants/components/megalo/compiler/string_scanner.h"

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
}

LocalizedStringLibrary::Entry::Entry() {
   this->content = new ReachString;
}
LocalizedStringLibrary::Entry::~Entry() {
   if (this->content) {
      delete this->content;
      this->content = nullptr;
   }
}
LocalizedStringLibrary::Entry::permutation::permutation() {
   this->content = new ReachString;
}
LocalizedStringLibrary::Entry::permutation::~permutation() {
   if (this->content) {
      delete this->content;
      this->content = nullptr;
   }
}
LocalizedStringLibrary::Entry::permutation& LocalizedStringLibrary::Entry::permutation::operator=(permutation&& other) noexcept {
   this->content = other.content;
   other.content = nullptr;
   this->integer = other.integer;
   return *this;
}
LocalizedStringLibrary::Entry::permutation& LocalizedStringLibrary::Entry::permutation::operator=(const permutation& other) noexcept {
   this->integer = other.integer;
   if (!this->content)
      this->content = new ReachString;
   *this->content = *other.content;
   return *this;
}

void LocalizedStringLibrary::Entry::copy(reach::language lang, QString& out) {
   out = QString::fromStdString(this->content->language(lang));
}
void LocalizedStringLibrary::Entry::apply_permutation(reach::language lang, QString& out, int32_t value) {
   uint32_t search = value;
   if (value < 0)
      search = -value;
   //
   auto sub = QString("%1").arg(value);
   for (auto& perm : this->permutations) {
      if (perm.integer == search) {
         out = QString::fromStdString(perm.content->language(lang)).replace("%1", sub); // QString::arg dumps a warning if the string has no %-token
         return;
      }
   }
   out = QString::fromStdString(this->content->language(lang)).replace("%1", sub);
}
bool LocalizedStringLibrary::Entry::matches(const QString& search) const noexcept {
   if (this->friendly_name.contains(search, Qt::CaseInsensitive))
      return true;
   for (auto& tag : this->tags)
      if (tag.contains(search, Qt::CaseInsensitive))
         return true;
   QString temp;
   for (auto& lang : this->content->strings) {
      temp = QString::fromStdString(lang);
      if (temp.contains(search, Qt::CaseInsensitive))
         return true;
   }
   return false;
}
LocalizedStringLibrary::Entry::permutation* LocalizedStringLibrary::Entry::get_or_create_permutation(uint32_t v) {
   for (auto& perm : this->permutations)
      if (perm.integer == v)
         return &perm;
   auto perm = &this->permutations.emplace_back();
   perm->integer = v;
   return perm;
}
LocalizedStringLibrary::Entry& LocalizedStringLibrary::Entry::operator=(Entry&& other) noexcept {
   this->content = other.content;
   other.content = nullptr;
   std::swap(this->permutations, other.permutations);
   std::swap(this->internal_name, other.internal_name);
   std::swap(this->friendly_name, other.friendly_name);
   std::swap(this->description, other.description);
   std::swap(this->category, other.category);
   std::swap(this->tags, other.tags);
   std::swap(this->source, other.source);
   std::swap(this->source_type, other.source_type);
   this->token = other.token;
   return *this;
}
LocalizedStringLibrary::Entry& LocalizedStringLibrary::Entry::operator=(const Entry& other) noexcept {
   if (!this->content)
      this->content = new ReachString;
   *this->content = *other.content;
   //
   this->permutations.reserve(other.permutations.size());
   for (auto& perm : other.permutations) {
      auto& created = this->permutations.emplace_back();
      created = perm;
   }
   //
   this->internal_name = other.internal_name;
   this->friendly_name = other.friendly_name;
   this->description   = other.description;
   this->category = other.category;
   this->source = other.source;
   this->source_type = other.source_type;
   this->tags = other.tags;
   this->token = other.token;
   return *this;
}

LocalizedStringLibrary::LocalizedStringLibrary() {
   auto dir = QDir(":/ScriptEditor/localized_string_library/");
   for (auto entry : dir.entryList()) {
      auto file = QFile(dir.path() + '/' + entry);
      if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
         continue;
      //
      auto   stream  = QTextStream(&file);
      Entry* current = nullptr;
      stream.setCodec("UTF-8");
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
         if (key.compare(QString("token"), Qt::CaseInsensitive) == 0) {
            if (value.compare(QString("integer"), Qt::CaseInsensitive) == 0) {
               current->token = token_type::integer;
            }
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
               perm->content->language(lang) = Megalo::Script::string_scanner::unescape(value.toString()).toStdString();
               continue;
            }
            current->content->language(lang) = Megalo::Script::string_scanner::unescape(value.toString()).toStdString();
         }
      }
   }
}
LocalizedStringLibrary::~LocalizedStringLibrary() {
   for (auto entry : this->entries)
      delete entry;
   this->entries.clear();
}