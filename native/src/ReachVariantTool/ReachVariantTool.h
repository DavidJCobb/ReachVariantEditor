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
      void openFile();
      void saveFile();
      //
   private:
      Ui::ReachVariantToolClass ui;
};
