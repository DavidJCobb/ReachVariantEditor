#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_ReachVariantTool.h"

class ReachVariantTool : public QMainWindow {
   Q_OBJECT
   //
   public:
      ReachVariantTool(QWidget *parent = Q_NULLPTR);
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
      //
      bool isUpdatingFromVariant = false; // if true, we should ignore all changes to UI controls, since we're the one causing them
      //
   private:
      Ui::ReachVariantToolClass ui;
};
