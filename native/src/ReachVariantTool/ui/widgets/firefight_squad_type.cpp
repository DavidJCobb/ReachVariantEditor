#include "firefight_squad_type.h"

namespace {
   void _append(QComboBox* widget, reach::firefight_squad type, QString name, QString tooltip) {
      widget->addItem(name, QVariant((int)type));
      widget->setItemData(widget->count() - 1, tooltip, Qt::ItemDataRole::ToolTipRole);
   }
}

FFSquadTypeWidget::FFSquadTypeWidget(QWidget* parent) : QWidget(parent) {
   ui.setupUi(this);
   //
   auto combobox = this->ui.value;
   {
      int  index    = -1;
      auto disambig = "Firefight squad type";
      //
      _append(combobox, reach::firefight_squad::none,
         tr("None", disambig),
         tr("An empty squad.", disambig)
      );
      _append(combobox, reach::firefight_squad::brutes,
         tr("Brutes", disambig),
         tr("Squads of Brutes.", disambig)
      );
      _append(combobox, reach::firefight_squad::brute_kill_team,
         tr("Brute Kill Team", disambig),
         tr("Squads of Brutes led by chieftains.", disambig)
      );
      _append(combobox, reach::firefight_squad::brute_patrol,
         tr("Brute Patrol", disambig),
         tr("Squads of Brutes and Grunts.", disambig)
      );
      _append(combobox, reach::firefight_squad::brute_infantry,
         tr("Brute Infantry", disambig),
         tr("Squads of Brutes and Jackals.", disambig)
      );
      _append(combobox, reach::firefight_squad::brute_tactical,
         tr("Brute Tactical", disambig),
         tr("Squads of Brutes and Skirmishers.", disambig)
      );
      _append(combobox, reach::firefight_squad::brute_chieftains,
         tr("Brute Chieftains", disambig),
         tr("Squads each consisting of a single Brute Chieftain.", disambig)
      );
      _append(combobox, reach::firefight_squad::elites,
         tr("Elites", disambig),
         tr("Squads of Elites.", disambig)
      );
      _append(combobox, reach::firefight_squad::elite_patrol,
         tr("Elite Patrol", disambig),
         tr("Squads of Elites and Grunts.", disambig)
      );
      _append(combobox, reach::firefight_squad::elite_infantry,
         tr("Elite Infantry", disambig),
         tr("Squads of Elites and Jackals.", disambig)
      );
      _append(combobox, reach::firefight_squad::elite_airborne,
         tr("Elite Airborne", disambig),
         tr("Squads of Elite Rangers with jetpacks.", disambig)
      );
      _append(combobox, reach::firefight_squad::elite_tactical,
         tr("Elite Tactical", disambig),
         tr("Squads of Elites and Skirmishers.", disambig)
      );
      _append(combobox, reach::firefight_squad::elite_spec_ops,
         tr("Elite Spec Ops", disambig),
         tr("Squads of Spec Ops Elites.", disambig)
      );
      _append(combobox, reach::firefight_squad::engineers,
         tr("Engineers", disambig),
         tr("Squads each consisting of a single Engineer.", disambig)
      );
      _append(combobox, reach::firefight_squad::elite_generals,
         tr("Elite Generals", disambig),
         tr("Squads each consisting of a single Elite General.", disambig)
      );
      _append(combobox, reach::firefight_squad::grunts,
         tr("Grunts", disambig),
         tr("Squads each consisting of four Grunts.", disambig)
      );
      _append(combobox, reach::firefight_squad::hunter_kill_team,
         tr("Hunter Kill Team", disambig),
         tr("Squads of Hunters, Elites, and Grunts.", disambig)
      );
      _append(combobox, reach::firefight_squad::hunter_patrol,
         tr("Hunter Patrol", disambig),
         tr("Squads of Hunters and Grunts.", disambig)
      );
      _append(combobox, reach::firefight_squad::hunter_strike_team,
         tr("Hunter Strike Team", disambig),
         tr("Squads of Hunters and Elites led by generals.", disambig)
      );
      _append(combobox, reach::firefight_squad::jackal_patrol,
         tr("Jackal Patrol", disambig),
         tr("Squads of Jackals and Grunts.", disambig)
      );
      _append(combobox, reach::firefight_squad::elite_strike_team,
         tr("Elite Strike Team", disambig),
         tr("Squads of Elites led by generals.", disambig)
      );
      _append(combobox, reach::firefight_squad::skirmisher_patrol,
         tr("Skirmisher Patrol", disambig),
         tr("Squads of Skirmishers and Grunts.", disambig)
      );
      _append(combobox, reach::firefight_squad::hunters,
         tr("Hunters", disambig),
         tr("Squads each consisting of a single Hunter.", disambig)
      );
      _append(combobox, reach::firefight_squad::jackal_snipers,
         tr("Jackal Snipers", disambig),
         tr("Squads of Jackals with Needle Rifles and Focus Rifles, but no shields.", disambig)
      );
      _append(combobox, reach::firefight_squad::jackals,
         tr("Jackals", disambig),
         tr("Squads of Jackals armed with shields.", disambig)
      );
      _append(combobox, reach::firefight_squad::hunter_infantry,
         tr("Hunter Infantry", disambig),
         tr("Squads of Hunters and Jackals.", disambig)
      );
      _append(combobox, reach::firefight_squad::guta,
         tr("Guta", disambig),
         tr("Squads each consisting of a single Guta. Note that no maps seem to actually support this option.", disambig)
      );
      _append(combobox, reach::firefight_squad::skirmishers,
         tr("Skirmishers", disambig),
         tr("Squads of Skirmishers.", disambig)
      );
      _append(combobox, reach::firefight_squad::hunter_tactical,
         tr("Hunter Tactical", disambig),
         tr("Squads of Hunters and Skirmishers.", disambig)
      );
      _append(combobox, reach::firefight_squad::skirmisher_infantry,
         tr("Skirmisher Infantry", disambig),
         tr("Squads of Skirmishers and Jackals.", disambig)
      );
      _append(combobox, reach::firefight_squad::heretics,
         tr("Heretics", disambig),
         tr("Squads of Elites wielding human weapons, varying with difficulty (assault rifle, DMR, grenade launcher, magnum, shotgun).", disambig)
      );
      _append(combobox, reach::firefight_squad::heretic_snipers,
         tr("Heretic Snipers", disambig),
         tr("Squads of Elites wielding human sniper rifles.", disambig)
      );
      _append(combobox, reach::firefight_squad::heretic_heavy,
         tr("Heretic Heavy", disambig),
         tr("Squads of Elites wielding human explosive weaponry.", disambig)
      );
   }
   combobox->model()->sort(0); // TODO: can we put "None" at the top of the list?
   combobox->setCurrentIndex(combobox->findData(QVariant((int)reach::firefight_squad::brutes), Qt::ItemDataRole::UserRole));
   //
   QObject::connect(combobox, QOverload<int>::of(&QComboBox::currentIndexChanged), [this](int index) {
      if (!this->target)
         return;
      *this->target = (reach::firefight_squad)(index - 1);
   });
}
void FFSquadTypeWidget::setTarget(squad_type_t& target) {
   this->target = &target;
   this->_updateFromTarget();
}
void FFSquadTypeWidget::clearTarget() {
   this->target = nullptr;
   this->_updateFromTarget();
}
void FFSquadTypeWidget::_updateFromTarget() {
   auto combobox = this->ui.value;
   const auto blocker = QSignalBlocker(combobox);
   combobox->setDisabled(this->target == nullptr);
   //
   auto to_select = reach::firefight_squad::brutes;
   if (this->target)
      to_select = *this->target;
   combobox->setCurrentIndex(combobox->findData(QVariant((int)to_select), Qt::ItemDataRole::UserRole));
}
void FFSquadTypeWidget::setText(const QString& label) {
   this->ui.label->setText(label);
}