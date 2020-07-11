#include "page_firefight_lives.h"
#include "../../helpers/bitwise.h"

PageFFLives::PageFFLives(QWidget* parent) : QWidget(parent) {
   ui.setupUi(this);
   //
   auto& editor = ReachEditorState::get();
   QObject::connect(&editor, &ReachEditorState::variantAcquired, this, &PageFFLives::updateFromVariant);
   //
   #include "widget_macros_setup_start.h"
   reach_main_window_setup_spinbox(firefightData, this->ui.fieldStartingLivesSpartan, startingLivesSpartan);
   reach_main_window_setup_spinbox(firefightData, this->ui.fieldStartingLivesElite,   startingLivesElite);
   reach_main_window_setup_spinbox(firefightData, this->ui.fieldEliteKillBonus,       eliteKillBonus);
   reach_main_window_setup_spinbox(firefightData, this->ui.fieldMaxExtraLivesSpartan, maxSpartanExtraLives);
   #include "widget_macros_setup_end.h"
}
void PageFFLives::updateFromVariant(GameVariant* variant) {
   auto data = variant->get_firefight_data();
   if (!data)
      return;
   #include "widget_macros_update_start.h"
   reach_main_window_update_spinbox(this->ui.fieldStartingLivesSpartan, startingLivesSpartan);
   reach_main_window_update_spinbox(this->ui.fieldStartingLivesElite,   startingLivesElite);
   reach_main_window_update_spinbox(this->ui.fieldEliteKillBonus,       eliteKillBonus);
   reach_main_window_update_spinbox(this->ui.fieldMaxExtraLivesSpartan, maxSpartanExtraLives);
   #include "widget_macros_update_end.h"
}