#include "page_multiplayer_settings_map.h"
#include "../../game_variants/base.h"
#include "../../game_variants/components/custom_game_options.h"
#include "../../services/ini.h"

PageMPSettingsMapAndGame::PageMPSettingsMapAndGame(QWidget* parent) : QWidget(parent) {
   ui.setupUi(this);
   //
   auto& editor = ReachEditorState::get();
   QObject::connect(&editor, &ReachEditorState::variantAcquired, this, &PageMPSettingsMapAndGame::updateFromVariant);
   QObject::connect(&ReachINI::getForQt(), &cobb::qt::ini::file::settingChanged, [this](cobb::ini::setting* setting, cobb::ini::setting_value_union oldValue, cobb::ini::setting_value_union newValue) {
      if (setting != &ReachINI::Editing::bHideFirefightNoOps)
         return;
      this->updateEnableStates();
   });
   //
   #include "widget_macros_setup_start.h"
   reach_main_window_setup_flag_checkbox(customGameOptions, this->ui.fieldGrenades,  map.flags, ReachCGMapOptions::flags_t::grenades);
   reach_main_window_setup_flag_checkbox(customGameOptions, this->ui.fieldShortcuts, map.flags, ReachCGMapOptions::flags_t::shortcuts);
   reach_main_window_setup_flag_checkbox(customGameOptions, this->ui.fieldAbilities, map.flags, ReachCGMapOptions::flags_t::abilities);
   reach_main_window_setup_flag_checkbox(customGameOptions, this->ui.fieldPowerups,  map.flags, ReachCGMapOptions::flags_t::powerups);
   reach_main_window_setup_flag_checkbox(customGameOptions, this->ui.fieldTurrets,   map.flags, ReachCGMapOptions::flags_t::turrets);
   reach_main_window_setup_flag_checkbox(customGameOptions, this->ui.fieldIndestructibleVehicles, map.flags, ReachCGMapOptions::flags_t::indestructible_vehicles);
   {  // Weapon Set
      QComboBox* widget = this->ui.fieldWeaponSet;
      QObject::connect(widget, QOverload<int>::of(&QComboBox::currentIndexChanged), [](int index) {
         auto data = ReachEditorState::get().customGameOptions();
         if (!data)
            return;
         reach::weapon_set value;
         switch (index) {
            case 0: // Map Default
               value = reach::weapon_set::map_default; // Map Default
               break;
            case 16:
               value = reach::weapon_set::none; // No Weapons
               break;
            case 17:
               value = reach::weapon_set::random; // Random
               break;
            default:
               value = (reach::weapon_set)(index - 1);
         }
         data->map.weaponSet = value;
      });
   }
   {  // Vehicle Set
      QComboBox* widget = this->ui.fieldVehicleSet;
      QObject::connect(widget, QOverload<int>::of(&QComboBox::currentIndexChanged), [](int index) {
         auto data = ReachEditorState::get().customGameOptions();
         if (!data)
            return;
         reach::vehicle_set value;
         switch (index) {
            case 0: // Map Default
               value = reach::vehicle_set::map_default; // Map Default
               break;
            default:
               value = (reach::vehicle_set)(index - 1);
         }
         data->map.vehicleSet = value;
      });
   }
   reach_main_window_setup_spinbox(customGameOptions, this->ui.fieldPowerupDurationRed,    map.powerups.red.duration);
   reach_main_window_setup_spinbox(customGameOptions, this->ui.fieldPowerupDurationBlue,   map.powerups.blue.duration);
   reach_main_window_setup_spinbox(customGameOptions, this->ui.fieldPowerupDurationYellow, map.powerups.yellow.duration);
   #include "widget_macros_setup_end.h"
}
void PageMPSettingsMapAndGame::updateEnableStates() {
   bool disable = ReachINI::Editing::bHideFirefightNoOps.current.b;
   if (disable) {
      auto variant = ReachEditorState::get().variant();
      if (!variant || variant->get_multiplayer_type() != ReachGameEngine::firefight) {
         disable = false;
      }
   }
   //
   this->ui.fieldGrenades->setDisabled(disable);
   this->ui.fieldShortcuts->setDisabled(disable);
   this->ui.fieldAbilities->setDisabled(disable);
   this->ui.fieldPowerups->setDisabled(disable);
   this->ui.fieldTurrets->setDisabled(disable);
   this->ui.fieldIndestructibleVehicles->setDisabled(disable);
   this->ui.fieldWeaponSet->setDisabled(disable);
   this->ui.fieldVehicleSet->setDisabled(disable);
}
void PageMPSettingsMapAndGame::updateFromVariant(GameVariant* variant) {
   auto data = variant->get_custom_game_options();
   if (!data)
      return;
   this->updateEnableStates();
   #include "widget_macros_update_start.h"
   reach_main_window_update_flag_checkbox(this->ui.fieldGrenades,  map.flags, ReachCGMapOptions::flags_t::grenades);
   reach_main_window_update_flag_checkbox(this->ui.fieldShortcuts, map.flags, ReachCGMapOptions::flags_t::shortcuts);
   reach_main_window_update_flag_checkbox(this->ui.fieldAbilities, map.flags, ReachCGMapOptions::flags_t::abilities);
   reach_main_window_update_flag_checkbox(this->ui.fieldPowerups,  map.flags, ReachCGMapOptions::flags_t::powerups);
   reach_main_window_update_flag_checkbox(this->ui.fieldTurrets,   map.flags, ReachCGMapOptions::flags_t::turrets);
   reach_main_window_update_flag_checkbox(this->ui.fieldIndestructibleVehicles, map.flags, ReachCGMapOptions::flags_t::indestructible_vehicles);
   {  // Weapon Set
      QComboBox* widget = this->ui.fieldWeaponSet;
      const QSignalBlocker blocker(widget);
      auto value = data->map.weaponSet;
      int  index;
      switch (value) {
         case reach::weapon_set::map_default:
            index = 0;
            break;
         case reach::weapon_set::none:
            index = 16;
            break;
         default:
            index = (int)value + 1;
      }
      widget->setCurrentIndex(index);
   }
   {  // Vehicle Set
      QComboBox* widget = this->ui.fieldVehicleSet;
      const QSignalBlocker blocker(widget);
      auto value = data->map.vehicleSet;
      int  index;
      switch (value) {
         case reach::vehicle_set::map_default:
            index = 0;
            break;
         default:
            index = (int)value + 1;
      }
      widget->setCurrentIndex(index);
   }
   reach_main_window_update_spinbox(this->ui.fieldPowerupDurationRed,    map.powerups.red.duration);
   reach_main_window_update_spinbox(this->ui.fieldPowerupDurationBlue,   map.powerups.blue.duration);
   reach_main_window_update_spinbox(this->ui.fieldPowerupDurationYellow, map.powerups.yellow.duration);
   #include "widget_macros_update_end.h"
}