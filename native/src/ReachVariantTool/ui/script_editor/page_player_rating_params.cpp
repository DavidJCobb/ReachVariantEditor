#include "page_player_rating_params.h"

ScriptEditorPagePlayerRating::ScriptEditorPagePlayerRating(QWidget* parent) : QWidget(parent) {
   ui.setupUi(this);
   //
   auto& editor = ReachEditorState::get();
   //
   QObject::connect(&editor, &ReachEditorState::variantAcquired, this, &ScriptEditorPagePlayerRating::updateFromVariant);
   QObject::connect(&editor, &ReachEditorState::variantAbandoned, [this]() {
      this->updateFromVariant();
   });
   this->floatWidgets.push_back(this->ui.float00);
   this->floatWidgets.push_back(this->ui.float01);
   this->floatWidgets.push_back(this->ui.float02);
   this->floatWidgets.push_back(this->ui.float03);
   this->floatWidgets.push_back(this->ui.float04);
   this->floatWidgets.push_back(this->ui.float05);
   this->floatWidgets.push_back(this->ui.float06);
   this->floatWidgets.push_back(this->ui.float07);
   this->floatWidgets.push_back(this->ui.float08);
   this->floatWidgets.push_back(this->ui.float09);
   this->floatWidgets.push_back(this->ui.float10);
   this->floatWidgets.push_back(this->ui.float11);
   this->floatWidgets.push_back(this->ui.float12);
   this->floatWidgets.push_back(this->ui.float13);
   this->floatWidgets.push_back(this->ui.float14);
   //
   for (size_t i = 0; i < this->floatWidgets.size(); i++) {
      QObject::connect(this->floatWidgets[i], QOverload<double>::of(&QDoubleSpinBox::valueChanged), [i](double v) {
         auto mp = ReachEditorState::get().multiplayerData();
         if (!mp)
            return;
         mp->playerRatingParams.values[i] = v;
      });
   }
   QObject::connect(this->ui.showInScoreboard, &QCheckBox::stateChanged, [](int state) {
      auto mp = ReachEditorState::get().multiplayerData();
      if (!mp)
         return;
      mp->playerRatingParams.showInScoreboard = (state == Qt::Checked);
   });
   //
   this->updateFromVariant(nullptr);
}
void ScriptEditorPagePlayerRating::updateFromVariant(GameVariant* variant) {
   GameVariantDataMultiplayer* mp = nullptr;
   if (variant)
      mp = variant->get_multiplayer_data();
   else
      mp = ReachEditorState::get().multiplayerData();
   if (!mp)
      return;
   //
   const QSignalBlocker blocker(this->ui.showInScoreboard);
   this->ui.showInScoreboard->setChecked(mp->playerRatingParams.showInScoreboard);
   for (size_t i = 0; i < this->floatWidgets.size(); i++) {
      auto widget = this->floatWidgets[i];
      const QSignalBlocker blocker(widget);
      widget->setValue(mp->playerRatingParams.values[i]);
   }
}