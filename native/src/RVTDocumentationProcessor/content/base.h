#pragma once
#include <QString>
#include <QtXml>

namespace content {
   enum class entry_type {
      generic,
      same, // target is of the same type as subject
      action,
      condition,
      property,
      accessor,
      //
      type,
      ns, // namespace
   };
   
   class api_parent_object;
   class category;

   class base {
      //
      // Common fields and code for most/all of the "things" we can define in XML.
      //
      public:
         const entry_type type = entry_type::generic;
         base() {}
         base(entry_type t) : type(t) {}

         struct note {
            QString title; // optional; generates sub-heading
            QString id;    // if there is a title, then this is the <a name="..."> identifier
            QString content;
         };
         struct relationship { // list items in a "see also" section of the page
            QString    context; // basis.whatever
            QString    name;
            entry_type type = entry_type::same;
            //
            base* target   = nullptr;
            bool  mirrored = false;
         };
         
         api_parent_object* parent = nullptr;
         QVector<category*> categories;
         //
         QString name;
         QString name2;
         QString blurb;
         QString description;
         QString example;
         //
         QVector<note> notes;
         QVector<relationship*> related;

         QString description_to_html() const noexcept;
         QString example_to_html() const noexcept;
         QString notes_to_html() const noexcept;
         QString relationships_to_html(base* member_of, entry_type my_entry_type) const noexcept;

         relationship* get_relationship_entry(base& to) const noexcept;
         QVector<base*> get_all_see_also() const noexcept;
         
      protected:
         void _load_blurb(QDomElement& doc, const QString& stem);
         void _load_description(QDomElement& doc, const QString& stem);
         void _load_example(QDomElement& doc, const QString& stem);
         void _load_relationship(QDomElement& doc);
         void _load_note(QDomElement& doc, QString stem);

         void _load_all_categories_in(QDomElement container);
   };
}
