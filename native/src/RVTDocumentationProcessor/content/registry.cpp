#include "registry.h"
#include "api_accessor.h"
#include "api_namespace.h"
#include "api_method.h"
#include "api_property.h"
#include "api_type.h"

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
   int registry::depth_of(std::filesystem::path path) {
      int depth = 0;
      if (path.has_extension())
         path = path.parent_path();
      for (; !std::filesystem::equivalent(path, this->root_path); ++depth) {
         if (!path.has_parent_path())
            return -1;
         path = path.parent_path();
      }
      return depth;
   }
   QString registry::make_stem(std::filesystem::path path) {
      QString out;
      auto depth = this->depth_of(path);
      if (depth < 0)
         return out;
      for (auto temp = depth; temp; --temp) {
         path = path.parent_path();
         auto fn = path.filename().u8string();
         fn += '/';
         out.insert(0, (const char*)fn.c_str());
      }
      return out;
   }

   void registry::_mirror_relationships_for_member(api_namespace& type, base& member, entry_type member_type) {
      //static_assert(false, "TODO");
   }
   void registry::_mirror_relationships_for_member(api_type& type, base& member, entry_type member_type) {
      /*//
      for (auto& rel : member.related) {
         if (rel.mirrored)
            continue;
         if (!rel.context.isEmpty() && rel.context != type.name) // this is a relationship to something in another type
            continue && static_assert(false, "is this a limitation we want to keep in place? at the VERY least it needs to go behind a constexpr bool");
         base* target = nullptr;
         //
         auto rt = rel.type;
         if (rt == entry_type::same)
            rt = member_type;
         switch (rt) {
            case entry_type::condition:
               target = type.get_condition_by_name(rel.name);
               break;
            case entry_type::action:
               target = type.get_action_by_name(rel.name);
               break;
            case entry_type::property:
               target = type.get_property_by_name(rel.name);
               break;
            case entry_type::accessor:
               target = type.get_accessor_by_name(rel.name);
               break;
            case entry_type::generic:
               //target = type.get_member_by_name(rel.name); // namespaces only
               break;
         }
         if (target == nullptr)
            continue;
         //
         target->related.emplace_back();
         auto& mirrored = target->related.back();
         mirrored.mirrored = true;
         mirrored.name = member.name;
         mirrored.type = member_type;
         //
         for (auto& rel2 : member.related) {
            if (&rel2 == &rel)
               continue;
            target->related.emplace_back();
            auto& mirrored = target->related.back();
            mirrored = rel2;
            mirrored.mirrored = true;
         }
      }
      //*/
   }
   void registry::post_load_mirror_all_relationships() {
      for (auto* type : this->types) {
         for (auto* m : type->accessors)
            this->_mirror_relationships_for_member(*type, *m, entry_type::accessor);
         for (auto* m : type->actions)
            this->_mirror_relationships_for_member(*type, *m, entry_type::action);
         for (auto* m : type->conditions)
            this->_mirror_relationships_for_member(*type, *m, entry_type::condition);
         for (auto* m : type->properties)
            this->_mirror_relationships_for_member(*type, *m, entry_type::property);
      }
      for (auto* ns : this->namespaces) {
         for (auto* m : ns->actions)
            this->_mirror_relationships_for_member(*ns, *m, entry_type::action);
         for (auto* m : ns->conditions)
            this->_mirror_relationships_for_member(*ns, *m, entry_type::condition);
         for (auto* m : ns->members)
            this->_mirror_relationships_for_member(*ns, *m, entry_type::generic);
      }
   }
}