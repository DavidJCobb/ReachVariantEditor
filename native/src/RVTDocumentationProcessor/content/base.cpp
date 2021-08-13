#include "base.h"
#include "../util/html.h"
#include "../util/link_fixup.h"
#include "../util/megalo_syntax_highlight.h"
#include "../helpers/qt/xml.h"

namespace content {
   QString base::description_to_html() const noexcept {
      if (this->description.isEmpty()) {
         if (this->blurb.isEmpty())
            return "<p>No description available.</p>";
         return this->blurb;
      }
      return this->description;
   }
   QString base::example_to_html() const noexcept {
      QString out;
      if (this->example.isEmpty())
         return out;
      return QString("<h2>Example</h2>\n<pre>%1</pre>\n").arg(this->example);
   }
   QString base::notes_to_html() const noexcept {
      QString out;
      if (this->notes.empty())
         return out;
      bool has_any_named = false;
      bool has_any_bare  = false;
      //
      out += "<h2>Notes</h2>\n";
      for (auto& note : this->notes) {
         if (note.title.isEmpty()) {
            has_any_bare = true;
            continue;
         }
         has_any_named = true;
         //
         bool has_id = note.id.isEmpty();
         out += QString("<h3>%1%2%3</h3>\n")
            .arg(has_id ? QString("<a name=\"%1\">").arg(note.id) : "")
            .arg(note.title)
            .arg(has_id ? "</a>" : "");
         out += note.content;
      }
      if (has_any_bare) {
         if (has_any_named)
            out += "<h3>Other notes</h3>\n";
         out += "<ul>\n";
         for (auto& note : this->notes) {
            if (!note.title.isEmpty())
               continue;
            out += QString("   <li>%1</li>\n").arg(note.content);
         }
         out += "</ul>";
      }
      return out;
   }
   QString base::relationships_to_html(base* member_of, entry_type my_entry_type) const noexcept {
      QString out;
      if (this->related.empty())
         return out;
      out = "<h2>See also</h2>\n<ul>\n";
      for (auto& rel : this->related) {
         QString context = rel.context;
         if (context.isEmpty() && member_of)
            context = member_of->name;
         //
         QString type_folder;
         auto rt = rel.type;
         if (rt == entry_type::same)
            rt = my_entry_type;
         switch (rt) {
            case entry_type::action:    type_folder  = "actions/";    break;
            case entry_type::condition: type_folder = "conditions/"; break;
            case entry_type::property:  type_folder = "properties/"; break;
            case entry_type::accessor:  type_folder = "accessors/";  break;
         }
         //
         QString context_text = context;
         if (context_text == "ns_unnamed")
            context_text.clear();
         else if (context_text.startsWith("ns_"))
            context_text.remove(0, 3);
         if (!context_text.isEmpty())
            context_text += '.';
         //
         out += QString("   <li><a href=\"script/api/%1/%2%3.html\">%4%3</a></li>\n")
            .arg(context)
            .arg(type_folder)
            .arg(rel.name)
            .arg(context_text);
      }
      out += "</ul>\n";
      return out;
   }

   void base::_load_blurb(QDomElement& elem, const QString& stem) {
      this->blurb = util::serialize_element(elem, {
         .adapt_indented_pre_tags      = true,
         .include_containing_element   = false,
         .pre_tag_content_tweak        = [](QString& out) { out = util::megalo_syntax_highlight(out); },
         .url_tweak                    = [stem](QString& out) { util::link_fixup(stem, out);  },
         .wrap_bare_text_in_paragraphs = true,
      });
   }
   void base::_load_description(QDomElement& elem, const QString& stem) {
      this->description = util::serialize_element(elem, {
         .adapt_indented_pre_tags      = true,
         .include_containing_element   = false,
         .pre_tag_content_tweak        = [](QString& out) { out = util::megalo_syntax_highlight(out); },
         .url_tweak                    = [stem](QString& out) { util::link_fixup(stem, out);  },
         .wrap_bare_text_in_paragraphs = true,
      });
   }
   void base::_load_example(QDomElement& elem, const QString& stem) {
      auto tag = elem.tagName();
      elem.setTagName("pre");
      this->example = util::serialize_element(elem, {
         .adapt_indented_pre_tags    = true,
         .include_containing_element = false,
         .pre_tag_content_tweak      = [](QString& out) { out = util::megalo_syntax_highlight(out); },
         .url_tweak                  = [stem](QString& out) { util::link_fixup(stem, out);  },
      });
      elem.setTagName(tag);
   }
   void base::_load_relationship(QDomElement& elem) {
      this->related.push_back(relationship());
      auto& rel = this->related.back();
      //
      rel.name    = elem.attribute("name");
      rel.context = elem.attribute("context");
      //
      auto type = elem.attribute("type");
      if (type == "action")
         rel.type = entry_type::action;
      else if (type == "condition")
         rel.type = entry_type::condition;
      else if (type == "property")
         rel.type = entry_type::property;
      else if (type == "accessor")
         rel.type = entry_type::accessor;
   }
   void base::_load_note(QDomElement& elem, QString stem) {
      this->notes.push_back(note());
      auto& note = this->notes.back();
      //
      note.title = elem.attribute("title");
      note.id    = elem.attribute("id");
      note.content = util::serialize_element(elem, {
         .adapt_indented_pre_tags      = true,
         .include_containing_element   = false,
         .pre_tag_content_tweak        = [](QString& out) { out = util::megalo_syntax_highlight(out); },
         .url_tweak                    = [stem](QString& out) { util::link_fixup(stem, out);  },
         .wrap_bare_text_in_paragraphs = true,
      });
   }
}