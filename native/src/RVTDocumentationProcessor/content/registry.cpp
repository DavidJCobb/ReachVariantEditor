#include "registry.h"
#include "api_accessor.h"
#include "api_namespace.h"
#include "api_method.h"
#include "api_property.h"
#include "api_type.h"
#include "category.h"

namespace content {
   registry::registry() {
      {
         auto file = QFile("_article.html");
         file.open(QIODevice::ReadOnly);
         if (file.isOpen()) {
            this->page_templates.article.setContent(QString::fromUtf8(file.readAll()));
         }
      }
      {
         auto file = QFile("_ns_member.html");
         file.open(QIODevice::ReadOnly);
         if (file.isOpen()) {
            this->page_templates.ns_member.setContent(QString::fromUtf8(file.readAll()));
         }
      }
      {
         auto file = QFile("_redirect.html");
         file.open(QIODevice::ReadOnly);
         if (file.isOpen()) {
            this->page_templates.redirect.setContent(QString::fromUtf8(file.readAll()));
         }
      }
      {
         auto file = QFile("_type.html");
         file.open(QIODevice::ReadOnly);
         if (file.isOpen()) {
            this->page_templates.type.setContent(QString::fromUtf8(file.readAll()));
         }
      }
   }
   registry::~registry() {
      this->clear();
   }
   void registry::clear() {
      for (auto* t : this->types)
         if (t)
            delete t;
      this->types.clear();
      //
      for (auto* t : this->namespaces)
         if (t)
            delete t;
      this->namespaces.clear();
      //
      for (auto* c : this->categories)
         if (c)
            delete c;
      this->categories.clear();
   }
   api_type* registry::get_type(const QString& name) {
      for (auto* t : this->types)
         if (t->name == name || t->name2 == name)
            return t;
      return nullptr;
   }
   void registry::load_type(const QString& relative_folder_path, QDomDocument& doc) {
      auto* type = new api_type;
      this->types.push_back(type);
      type->relative_folder_path = QDir::cleanPath(relative_folder_path);
      type->load(doc);
   }
   void registry::load_namespace(const QString& relative_folder_path, QDomDocument& doc) {
      auto* item = new api_namespace;
      this->namespaces.push_back(item);
      item->relative_folder_path = QDir::cleanPath(relative_folder_path);
      item->load(doc);
   }

   api_parent_object* registry::get_namespace_or_type(const QString& name) const noexcept {
      for (auto* t : this->types)
         if (t->name.compare(name, Qt::CaseInsensitive) == 0 || t->name2.compare(name, Qt::CaseInsensitive) == 0)
            return t;
      for (auto* n : this->namespaces)
         if (n->name.compare(name, Qt::CaseInsensitive) == 0 || n->name2.compare(name, Qt::CaseInsensitive) == 0)
            return n;
      return nullptr;
   }
   category* registry::get_or_create_category(const QString& id) {
      for (auto* c : this->categories)
         if (id.compare(c->id, Qt::CaseInsensitive) == 0)
            return c;
      auto* c = new category;
      c->id = id;
      this->categories.push_back(c);
      return c;
   }

   void registry::_mirror_relationships_for_member(base& member) {
      for (auto* rel : member.related) {
         if (rel->mirrored || rel->target)
            continue;
         auto* context = member.parent;
         if (!rel->context.isEmpty())
            context = this->get_namespace_or_type(rel->context);
         if (!context) {
            continue; // TODO: Warn
         }
         //
         auto rt = rel->type;
         if (rt == entry_type::same)
            rt = member.type;
         //
         rel->target = context->lookup_nested_article(rt, rel->name);
         if (rel->target && rel->target != &member) {
            auto& other = *rel->target;
            //
            auto* existing = other.get_relationship_entry(member);
            if (existing) {
               existing->target = &member;
            } else {
               auto* mirrored = new base::relationship;
               mirrored->mirrored = true;
               mirrored->target   = &member;
               other.related.push_back(mirrored);
            }
            rel->mirrored = true;
         }
      }
   }
   void registry::post_load_mirror_all_relationships() {
      for (auto* type : this->types) {
         for(auto* m : type->children)
            this->_mirror_relationships_for_member(*m);
      }
      for (auto* ns : this->namespaces) {
         for (auto* m : ns->children)
            this->_mirror_relationships_for_member(*m);
      }
   }
}