#pragma once
#include <QWidget>
#include "ui_LoadoutForm.h"

class ReachLoadout;

class LoadoutForm : public QWidget {
   Q_OBJECT
   public:
      LoadoutForm(QWidget* parent = nullptr);
      //
      int8_t loadoutIndex() const {
         QVariant value = this->property("loadoutIndex");
         if (!value.isValid())
            return -1;
         return value.toInt();
      }
      void pullFromGameVariant();
      //
   private:
      Ui::LoadoutForm ui;
      //
      ReachLoadout* _getLoadout() const noexcept;
};