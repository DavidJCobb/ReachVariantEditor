#include <QtXml>

#include <QApplication>
#include <QFileDialog>

#include "content/registry.h"
#include "content/api_namespace.h"
#include "content/api_type.h"

#include "helpers/qt/save_file_to.h"
#include "helpers/qt/xml.h"

#include "util/html.h"
#include "util/link_fixup.h"
#include "util/megalo_syntax_highlight.h"

void handle_article(QDomElement& root, QDir base_save_folder, QString relative_path, QString relative_folder) {
   auto& registry = content::registry::get();
   //
   QDomElement node;
   {
      auto list = root.elementsByTagName("body");
      if (list.size())
         node = list.item(0).toElement();
   }
   if (node.isNull())
      return;
   //
   QString write_to = QDir::cleanPath(base_save_folder.absoluteFilePath(relative_path));
   QString write_folder;
   {
      int i = write_to.lastIndexOf('.');
      if (i > 0) {
         write_to.truncate(i + 1);
         write_to += "html";
      }
      //
      write_folder = write_to;
      i = write_folder.lastIndexOf('/');
      int j = write_folder.lastIndexOf('\\');
      i = std::max(i, j);
      if (i >= 0)
         write_folder.truncate(i);
      else
         write_folder.clear();
   }
   //
   const auto& tmp = registry.page_templates.article;
   QString title;
   QString body;
   {
      auto node = cobb::qt::xml::first_child_element_of_type(root, "title", Qt::CaseInsensitive);
      if (!node.isNull())
         title = node.text();
   }
   body = QString("<h1>%1</h1>").arg(title) + util::serialize_element(node, {
      .adapt_indented_pre_tags    = true,
      .include_containing_element = false,
      .pre_tag_content_tweak      = [](QString& out) { out = util::megalo_syntax_highlight(out); },
      .url_tweak                  = [relative_folder](QString& out) { util::link_fixup(relative_folder, out);  },
   });
   //
   body = registry.page_templates.article.create_page({
      .body  = body,
      .relative_folder_path = relative_folder,
      .title = title,
   });
   cobb::qt::save_file_to(write_to, body);
}

int main(int argc, char *argv[]) {
   QApplication a(argc, argv);
   //
   //ReachVariantTool w;
   //w.show();
   //return a.exec();

   auto& registry = content::registry::get();

   QString dir = QFileDialog::getExistingDirectory(nullptr, "Select folder");
   if (dir.isEmpty())
      return 0;
   QString output_dir = QFileDialog::getExistingDirectory(nullptr, "Select folder", dir);
   if (output_dir.isEmpty())
      return 0;
   //
   QDir base_load_folder = QDir(dir);
   QDir base_save_folder = QDir(output_dir);
   auto rdi   = QDirIterator(base_load_folder, QDirIterator::Subdirectories);
   int  count = 0;
   while (rdi.hasNext()) {
      auto path = rdi.next();
      auto info = rdi.fileInfo();
      if (!info.isFile() || info.suffix().compare("xml", Qt::CaseInsensitive) != 0)
         continue;
      auto file = QFile(path);
      file.open(QIODevice::ReadOnly);
      if (file.isOpen()) {
         ++count;
         //
         QDomDocument doc;
         QString      error;
         doc.setContent(&file, &error);
         if (!error.isEmpty()) {
            qDebug() << error.toLatin1().data();
            continue;
         }
         //
         QString relative_path   = base_load_folder.relativeFilePath(path);
         QString relative_folder;
         {
            relative_folder = relative_path;
            int i = relative_folder.lastIndexOf('/');
            int j = relative_folder.lastIndexOf('\\');
            i = std::max(i, j);
            if (i >= 0)
               relative_folder.truncate(i + 1);
            else
               relative_folder.clear();
         }
         //
         auto root = doc.documentElement();
         auto type = root.nodeName();
         if (type == "article") {
            handle_article(root, base_save_folder, relative_path, relative_folder);
         } else if (type == "script-namespace") {
            registry.load_namespace(relative_folder, doc);
         } else if (type == "script-type") {
            registry.load_type(relative_folder, doc);
         } else if (type == "reuse") {
            auto path = root.attribute("src");
            qDebug() << "Found a \"reuse\" file; unsure what to do with it: " << path << '\n';
         }
      }
   }
   if (count) {
      for (auto* type : registry.types) {
         type->write(output_dir);
      }
      for (auto* ns : registry.namespaces) {
         ns->write(output_dir);
      }
   }
   qDebug() << L"\nHandled " << count << " files.";
   return 0;
}