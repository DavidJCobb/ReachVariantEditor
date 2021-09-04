#pragma once
#include <QDialog>
#include <QObject>
#include <QProgressBar>

class RVTThemeEngine : public QObject {
   Q_OBJECT;
   public:
      RVTThemeEngine(QObject* parent = nullptr);

      inline bool busy() const noexcept { return this->_busy; }

      void changeStyle(const QString& qss);
      void changeStyleFile(const QString& path);

   signals:
      void styleChangeStarted();
      void styleChangeFinished();

   protected:
      bool     _busy  = false;
      QDialog* _modal = nullptr;

      void _createDialog();
};