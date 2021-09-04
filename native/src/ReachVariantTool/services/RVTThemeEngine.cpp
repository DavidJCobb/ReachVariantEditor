#include "RVTThemeEngine.h"
#include <QApplication>
#include <QBoxLayout>
#include <QFile>
#include <QLabel>
#include <QStyle>

RVTThemeEngine::RVTThemeEngine(QObject* parent) : QObject(parent) {
   this->_createDialog();
}

void RVTThemeEngine::changeStyle(const QString& qss) {
   if (this->busy())
      return;
   this->_busy = true;
   emit styleChangeStarted();
   //
   qDebug("QSS change starting...");
   if (auto* app = qobject_cast<QApplication*>(QApplication::instance())) {
      bool startup = app->topLevelWindows().isEmpty();
      if (!startup) {
         this->_modal->show();
         QCoreApplication::processEvents(); // ensure the modal gets to render initially
      }
      //
      if (auto* style = app->style()) {
         //
         // Fastest way to do a style switch is to manually unpolish. We also need 
         // to clear the stylesheet data in order to ensure a clean reset. (Simply 
         // switching from one stylesheet to another can cause Qt to fail to clear 
         // some properties, like padding on QTreeView.)
         // 
         // Wiping the stylesheet entirely has some benefits: we don't need to 
         // manually repolish and repaint every widget, as we would if we were 
         // both unpolishing and doing a direct stylesheet-to-stylesheet swap.
         //
         style->unpolish(app);
         app->setStyleSheet("");
      }
      app->setStyleSheet(qss);
      //
      this->_modal->hide();
   }
   qDebug("QSS change done.");
   //
   this->_busy = false;
   emit styleChangeFinished();
}
void RVTThemeEngine::changeStyleFile(const QString& path) {
   QString qss;
   {
      QFile file(path);
      if (!file.open(QFile::ReadOnly))
         return;
      qss = QString::fromUtf8(file.readAll());
   }
   this->changeStyle(qss);
}

void RVTThemeEngine::_createDialog() {
   //
   // The window we create here should be designed around the fact that the application hangs 
   // while stylesheets are being changed (blame Qt; there's no way to do the whole process 
   // asynchronously). That means we can't even visually update the window, e.g. with an 
   // indeterminate/looping progress bar.
   //
   if (this->_modal)
      return;
   auto* dialog = this->_modal = new QDialog();
   dialog->setWindowModality(Qt::WindowModality::ApplicationModal);
   dialog->setWindowTitle(tr("Progress"));
   //
   auto* layout = new QBoxLayout(QBoxLayout::Direction::Down);
   dialog->setLayout(layout);
   layout->addWidget(new QLabel("Changing styles... Please wait...", dialog));
}