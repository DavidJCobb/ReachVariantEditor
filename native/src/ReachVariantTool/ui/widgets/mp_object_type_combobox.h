#pragma once
#include <QComboBox>
#include "../../game_variants/data/mp_object_types.h"

class MPObjectTypeCombobox : public QComboBox {
   Q_OBJECT
   public:
      MPObjectTypeCombobox(QWidget* parent = nullptr) : QComboBox(parent) {
         auto& list = MPObjectTypeList::get();
         for (uint32_t i = 0; i < list.size(); i++) {
            auto& definition = list[i];
            this->addItem(QString::fromLatin1(definition.name.c_str()), i);
         }
         auto m = this->model();
         if (m)
            m->sort(0);
      }
      void setValue(uint32_t t) {
         auto index = this->findData((int)t);
         if (index >= 0)
            this->setCurrentIndex(index);
      }
};