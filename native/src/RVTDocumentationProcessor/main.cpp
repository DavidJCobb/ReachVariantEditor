#include <QtXml>

#include <QApplication>
#include <QMessageBox>

#include "content/registry.h"
#include "content/category.h"
#include "content/api_namespace.h"
#include "content/api_type.h"

#include "helpers/qt/save_file_to.h"
#include "helpers/qt/xml.h"

#include "util/html.h"
#include "util/link_fixup.h"
#include "util/megalo_syntax_highlight.h"

#include "helpers/qt/xml_html_entity_resolver.h"
#include "helpers/qt/xml_stream_reader_to_dom.h"

#include "ui/MainWindow.h"
#include "ui/FilePicker.h"

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
      .pre_tag_content_tweak      = 
         [](QDomElement pre, QString& out) {
            if (!pre.hasAttribute("lang"))
               out = util::megalo_syntax_highlight(out);
         },
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

void handle_redirect(QDomElement& root, QDir base_save_folder, QString relative_path, QString relative_folder) {
   auto& registry = content::registry::get();
   
   QString destination;
   if (root.hasAttribute("href")) {
      destination = root.attribute("href");
   }
   if (destination.isEmpty()) {
      qDebug() << "Failed to parse a \"redirect\" file.";
      return;
   }
   //
   // BASE HREF doesn't affect META REFRESH addresses, so fixup is different here:
   //
   if (!destination.startsWith("./") && !destination.startsWith("/"))
      destination = destination + "./";
   else if (destination.startsWith("/")) {
      destination = relative_folder + destination;
   }

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
   QString body = QString(
      "<h1>Redirect</h1>\n"
      "If you are not redirected, please <a href=\"%1\">click here</a>."
   ).arg(destination);
   //
   body = registry.page_templates.redirect.create_page({
      .body       = body,
      .relative_folder_path = relative_folder,
      .title      = "Redirect",
      .meta_extra = QString("<meta http-equiv=\"refresh\" content=\"0; url=%1\">").arg(destination)
   });
   cobb::qt::save_file_to(write_to, body);
}

void process_xml_file(content::registry& registry, const QString& path, const QString& xml, const QDir& base_load_folder, const QDir& base_save_folder) {
   QDomDocument doc;
   QString      error;
   {
      //
      // Can't use QDomDocument::setContent; nothing in the QtXml module allows us to 
      // perform parse-time modifications, which are needed in order to get HTML entities 
      // to work (no, including the entity definition from w3.org doesn't seem to help).
      // 
      // If we want to be able to predefine entities, or else support entities without 
      // the XML files having to define them individually, then we gotta do this.
      //
      cobb::qt::xml::XmlStreamReaderToDom  stream;
      cobb::qt::xml::XmlHtmlEntityResolver resolver;
      stream.setEntityResolver(&resolver);
      stream.parse(xml);
      doc   = stream.document;
      error = stream.errorString();
   }
   /*//
   doc.setContent(&file, &error);
   //*/
   if (!error.isEmpty()) {
      qDebug() << error.toLatin1().data();
      return;
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
      //
      // A file whose root element is <reuse src="../relative/path.xml" /> where the path in 
      // question is relative to the "reuse" file, and specifies an XML file whose content 
      // should be copied.
      // 
      // Basically, it means, "Take this other file, and re-render it, but with my path, and 
      // then have that be my output."
      //
      auto src = root.attribute("src");
      if (src.isEmpty())
         return;
      auto target_path = QDir::cleanPath(QDir(base_load_folder.absoluteFilePath(relative_folder)).absoluteFilePath(src));
      auto target_file = QFile(target_path);
      target_file.open(QIODevice::ReadOnly);
      if (!target_file.isOpen()) {
         qDebug() << "Failed to process a \"reuse\" file.\n   Source path: " << path << "\n   Target path: " << target_path << '\n';
         return;
      }
      //
      // Ensure that the target file is an article.
      //
      QXmlStreamReader reader;
      QString          target_xml = target_file.readAll();
      reader.addData(target_xml);
      if (reader.readNextStartElement()) {
         if (reader.name() == "article") {
            qDebug() << "Processing \"reuse\" file...\n   Source path: " << path << "\n   Target path: " << target_path << '\n';
            process_xml_file(registry, path, target_xml, base_load_folder, base_save_folder);
            return;
         }
      }
      qDebug() << "Failed to parse a \"reuse\" file.\n   Source path: " << path << "\n   Target path: " << target_path << '\n';
      return;
   } else if (type == "redirect") {
      handle_redirect(root, base_save_folder, relative_path, relative_folder);
   }
}

int main(int argc, char *argv[]) {
   QApplication a(argc, argv);
   auto* window = new MainWindow;
   window->show();
   QObject::connect(window, &MainWindow::onStartRequested, [](MainWindow* window) {
      window->setEnabled(false);
      QCoreApplication::sendPostedEvents(window); // for some reason, enable state changes aren't instant in this one case specifically?
      //
      QString load_path = window->widgets.input_folder->value();
      QString save_path = window->widgets.output_folder->value();
      if (load_path.isEmpty() || save_path.isEmpty()) {
         QApplication::beep();
         window->setEnabled(true);
         return;
      }
      QDir base_load_folder = load_path;
      QDir base_save_folder = save_path;
      QVector<QString> asset_file_paths;
      bool copy_assets = window->widgets.copy_assets_over->isChecked();
      //
      auto& registry = content::registry::get();
      auto  rdi      = QDirIterator(base_load_folder, QDirIterator::Subdirectories);
      int   count    = 0;
      while (rdi.hasNext()) {
         auto path = rdi.next();
         auto info = rdi.fileInfo();
         if (!info.isFile())
            continue;
         auto ext = info.suffix();
         if (ext.compare("xml", Qt::CaseInsensitive) != 0) {
            if (!copy_assets)
               continue;
            if (info.baseName().isEmpty()) // e.g. ".gitignore"
               continue;
            if (ext.compare("html", Qt::CaseInsensitive) == 0)
               continue;
            if (ext.compare("lnk", Qt::CaseInsensitive) == 0)
               continue;
            asset_file_paths.push_back(path);
            continue;
         }
         auto file = QFile(path);
         file.open(QIODevice::ReadOnly);
         if (file.isOpen()) {
            ++count;
            process_xml_file(registry, path, file.readAll(), base_load_folder, base_save_folder);
         }
      }
      if (count) {
         registry.post_load_mirror_all_relationships();
         for (auto* type : registry.types) {
            type->write(save_path);
         }
         for (auto* ns : registry.namespaces) {
            ns->write(save_path);
         }
         if (copy_assets) {
            for (auto& path : asset_file_paths) {
               QString to = base_save_folder.absoluteFilePath(base_load_folder.relativeFilePath(path));
               {  // Ensure the destination folder exists.
                  QString folder = to;
                  int i = folder.lastIndexOf('/');
                  int j = folder.lastIndexOf('\\');
                  i = std::max(i, j);
                  if (i >= 0)
                     folder.truncate(i);
                  QDir("/").mkpath(folder);
               }
               QFile::copy(path, to);
            }
         }
      }
      qDebug() << L"\nHandled " << count << " files.";
      //
      window->setEnabled(true);
      {
         QString category_list;
         {
            auto cats = registry.categories;
            qSort(cats.begin(), cats.end(), [](content::category* a, content::category* b) {
               return a->id < b->id;
            });
            for (auto* c : cats) {
               if (!category_list.isEmpty())
                  category_list += '\n';
               category_list += c->id;
            }
         }
         if (category_list.isEmpty())
            category_list = "<none>";
         //
         QMessageBox::information(
            window, "Done!",
            QString(copy_assets ? "Processed %1 XML file(s) and attempted to copy %3 asset(s).\n\nCategories seen:\n\n%2" : "Processed %1 files.\n\nCategories seen:\n\n%2")
               .arg(count)
               .arg(category_list)
               .arg(asset_file_paths.size())
         );
      }
      registry.clear();
   });
   //
   return a.exec();
}