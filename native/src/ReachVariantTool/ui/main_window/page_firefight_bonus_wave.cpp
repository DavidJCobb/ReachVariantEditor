#include "page_firefight_bonus_wave.h"
#include "../../editor_state.h"
#include "../../game_variants/types/firefight.h"

PageFFBonusWave::PageFFBonusWave(QWidget* parent) : QWidget(parent) {
   ui.setupUi(this);
   //
   auto& editor = ReachEditorState::get();
   QObject::connect(&editor, &ReachEditorState::variantAcquired, this, &PageFFBonusWave::updateFromVariant);
   //
   #include "widget_macros_setup_start.h"
   reach_main_window_setup_spinbox(firefightData, this->ui.bonusWaveDuration, bonusWaveDuration);
   #include "widget_macros_setup_end.h"
}
void PageFFBonusWave::updateFromVariant(GameVariant* variant) {
   auto data = variant->get_firefight_data();
   if (!data) {
      this->ui.tabSkullsContent->clearTarget();
      this->ui.tabWaveContent->clearTarget();
      return;
   }
   this->ui.tabSkullsContent->setTarget(data->bonusWaveSkulls);
   this->ui.tabWaveContent->setTarget(data->bonusWave);
   #include "widget_macros_update_start.h"
   reach_main_window_update_spinbox(this->ui.bonusWaveDuration, bonusWaveDuration);
   #include "widget_macros_update_end.h"
}