#pragma once
#include <QComboBox>
#include "../../game_variants/components/megalo/limits.h"

class MegaloConstTeamIndexCombobox : public QComboBox {
   Q_OBJECT
   public:
      MegaloConstTeamIndexCombobox(QWidget* parent = nullptr) : QComboBox(parent) {
         this->addItem(tr("No Team", "Megalo const team index"), QVariant((int)Megalo::const_team::none));
         this->addItem(tr("Team 1", "Megalo const team index"), QVariant((int)Megalo::const_team::team_1));
         this->addItem(tr("Team 2", "Megalo const team index"), QVariant((int)Megalo::const_team::team_2));
         this->addItem(tr("Team 3", "Megalo const team index"), QVariant((int)Megalo::const_team::team_3));
         this->addItem(tr("Team 4", "Megalo const team index"), QVariant((int)Megalo::const_team::team_4));
         this->addItem(tr("Team 5", "Megalo const team index"), QVariant((int)Megalo::const_team::team_5));
         this->addItem(tr("Team 6", "Megalo const team index"), QVariant((int)Megalo::const_team::team_6));
         this->addItem(tr("Team 7", "Megalo const team index"), QVariant((int)Megalo::const_team::team_7));
         this->addItem(tr("Team 8", "Megalo const team index"), QVariant((int)Megalo::const_team::team_8));
         this->addItem(tr("Neutral", "Megalo const team index"), QVariant((int)Megalo::const_team::neutral));
      }
      void setValue(Megalo::const_team t) {
         auto index = this->findData((int)t);
         if (index >= 0)
            this->setCurrentIndex(index);
      }
};