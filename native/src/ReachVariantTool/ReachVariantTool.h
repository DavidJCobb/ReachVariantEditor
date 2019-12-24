#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_ReachVariantTool.h"

class ReachVariantTool : public QMainWindow {
   Q_OBJECT
   //
   public:
      ReachVariantTool(QWidget* parent = Q_NULLPTR); // needs to be public for Qt? but do not call; use the static getter
      //
      static ReachVariantTool& get();
      //
   private slots:
   private:
      void _saveFileImpl(bool saveAs);
      //
      void openFile();
      void saveFile()   { this->_saveFileImpl(false); }
      void saveFileAs() { this->_saveFileImpl(true); }
      void onSelectedPageChanged();
      //
      void refreshWidgetsFromVariant();
      void refreshWindowTitle();
      //
   private:
      Ui::ReachVariantToolClass ui;
};
