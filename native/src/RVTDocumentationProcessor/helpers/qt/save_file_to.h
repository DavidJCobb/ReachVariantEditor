#pragma once
#include <QDir>
#include <QSaveFile>
#include <QString>
#include <QTextStream>

namespace cobb::qt {
   extern inline void save_file_to(const QString& file_path, const QString& content) {
      QString folder = file_path;
      {
         int i = folder.lastIndexOf('/');
         int j = folder.lastIndexOf('\\');
         i = std::max(i, j);
         if (i >= 0)
            folder.truncate(i);
      }
      QDir("/").mkpath(folder);
      //
      auto save = QSaveFile(file_path);
      if (save.open(QIODevice::WriteOnly)) {
         QTextStream stream(&save);
         stream.setCodec("UTF-8");
         stream << content;
         save.commit();
      }
   }
}
