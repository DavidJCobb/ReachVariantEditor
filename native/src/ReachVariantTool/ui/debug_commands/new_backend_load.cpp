#include "new_backend_load.h"
#include <QByteArray>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QWidget>
#include "halo/reach/game_variant.h"

namespace rvt::debug_commands {
   extern void new_backend_load(QWidget* parent) {
      auto path = QFileDialog::getOpenFileName(parent, "Select game variant file", "", "Game variants (*.bin)");
      if (path.isEmpty())
         return;
      QFile file(path);
      if (!file.open(QIODevice::ReadOnly)) {
         QMessageBox::information(parent, QObject::tr("Unable to open file"), QObject::tr("An error occurred while trying to open this file.\n\nWindows error text:\n%1").arg(file.errorString()));
         return;
      }
      auto buffer = file.readAll();
      //
      halo::reach::game_variant variant;
      auto load_process = variant.read(buffer);
      __debugbreak();
   }
}