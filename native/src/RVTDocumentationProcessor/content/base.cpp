#include "base.h"
#include "category.h"
#include "registry.h"
#include "../util/html.h"
#include "../util/link_fixup.h"
#include "../util/megalo_syntax_highlight.h"
#include "../helpers/qt/xml.h"

#include "api_namespace.h"
#include "api_type.h"

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
         bool has_id = !note.id.isEmpty();
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
      auto    list = this->get_all_see_also();
      if (list.empty())
         return out;
      out = "<h2>See also</h2>\n<ul>\n";
      for (auto* article : list) {
         assert(article->parent);
         QString text;
         QString path;
         //
         auto* context = article->parent;
         if (context->type != entry_type::ns || context->name != "unnamed")
            text = context->name + '.';
         text += article->name;
         //
         path = QDir::cleanPath(context->relative_subfolder_path() + api_parent_object::sub_sub_folder_name_for(article->type));
         //
         out += QString("   <li><a href=\"%1/%2.html\">%3</a></li>\n")
            .arg(path)
            .arg(article->name)
            .arg(text);
      }
      out += "</ul>\n";
      return out;
   }

   base::relationship* base::get_relationship_entry(base& to) const noexcept {
      bool same = this->type == to.type;
      for (auto* rel : this->related) {
         if (rel->type != to.type) {
            if (!same || rel->type != entry_type::same)
               continue;
         }
         if (rel->name.compare(to.name, Qt::CaseInsensitive) != 0)
            if (rel->name.compare(to.name2, Qt::CaseInsensitive) != 0)
               continue;
         if (rel->context.isEmpty()) {
            if (this->parent != to.parent)
               continue;
         } else {
            if (!to.parent)
               continue;
            if (rel->context.compare(to.parent->name, Qt::CaseInsensitive) != 0)
               if (rel->context.compare(to.parent->name2, Qt::CaseInsensitive) != 0)
                  continue;
         }
         return rel;
      }
      return nullptr;
   }
   QVector<base*> base::get_all_see_also() const noexcept {
      QVector<base*> articles;
      for (auto* cat : this->categories) {
         for (auto* art : cat->articles) {
            if (art == this)
               continue;
            if (articles.indexOf(art) >= 0)
               continue;
            articles.push_back(art);
         }
      }
      {
         auto& registry = registry::get();
         for (auto& rel : this->related) {
            base* target  = rel->target;
            if (!target || !target->parent)
               continue;
            if (articles.indexOf(target) >= 0)
               continue;
            articles.push_back(target);
         }
      }
      //
      qSort(articles.begin(), articles.end(), [](base* a, base* b) {
         if (a->parent && b->parent && a->parent != b->parent) {
            return a->parent->name_is_less_than(*b->parent);
         }
         return a->name_is_less_than(*b);
      });
      return articles;
   }

   void base::_load_blurb(QDomElement& elem, const QString& stem) {
      this->blurb = util::serialize_element(elem, {
         .adapt_indented_pre_tags      = true,
         .include_containing_element   = false,
         .pre_tag_content_tweak        =
            [](QDomElement pre, QString& out) {
               if (!pre.hasAttribute("lang"))
                  out = util::megalo_syntax_highlight(out);
            },
         .url_tweak                    = [stem](QString& out) { util::link_fixup(stem, out);  },
         .wrap_bare_text_in_paragraphs = true,
      });
   }
   void base::_load_description(QDomElement& elem, const QString& stem) {
      this->description = util::serialize_element(elem, {
         .adapt_indented_pre_tags      = true,
         .include_containing_element   = false,
         .pre_tag_content_tweak        =
            [](QDomElement pre, QString& out) {
               if (!pre.hasAttribute("lang"))
                  out = util::megalo_syntax_highlight(out);
            },
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
         .pre_tag_content_tweak      = 
            [](QDomElement pre, QString& out) {
               if (!pre.hasAttribute("lang"))
                  out = util::megalo_syntax_highlight(out);
            },
         .url_tweak                  = [stem](QString& out) { util::link_fixup(stem, out);  },
      });
      elem.setTagName(tag);
   }
   void base::_load_relationship(QDomElement& elem) {
      auto* rel = new relationship;
      this->related.push_back(rel);
      //
      rel->name    = elem.attribute("name");
      rel->context = elem.attribute("context");
      //
      auto type = elem.attribute("type");
      if (type == "action")
         rel->type = entry_type::action;
      else if (type == "condition")
         rel->type = entry_type::condition;
      else if (type == "property")
         rel->type = entry_type::property;
      else if (type == "accessor")
         rel->type = entry_type::accessor;
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
         .pre_tag_content_tweak        =
            [](QDomElement pre, QString& out) {
               if (!pre.hasAttribute("lang"))
                  out = util::megalo_syntax_highlight(out);
            },
         .url_tweak                    = [stem](QString& out) { util::link_fixup(stem, out);  },
         .wrap_bare_text_in_paragraphs = true,
      });
   }

   void base::_load_all_categories_in(QDomElement root) {
      auto& registry = registry::get();
      //
      auto list = cobb::qt::xml::child_elements_of_type(root, "category");
      auto size = list.size();
      for (decltype(size) i = 0; i < size; ++i) {
         auto item = list[i];
         auto id   = item.attribute("id");
         if (id.isEmpty())
            continue;
         auto* cat = registry.get_or_create_category(id);
         if (cat)
            cat->add_article(*this);
      }
   }
}