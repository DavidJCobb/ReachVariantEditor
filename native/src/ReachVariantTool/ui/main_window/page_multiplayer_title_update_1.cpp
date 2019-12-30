#include "page_multiplayer_title_update_1.h"

PageMPTU1Config::PageMPTU1Config(QWidget* parent) : QWidget(parent) {
   ui.setupUi(this);
   //
   auto& editor = ReachEditorState::get();
   QObject::connect(&editor, &ReachEditorState::variantAcquired, this, &PageMPTU1Config::updateFromVariant);
   //
   #include "widget_macros_setup_start.h"
   reach_main_window_setup_flag_checkbox(this->ui.fieldBleedthrough, titleUpdateData.flags, 0x01);
   reach_main_window_setup_flag_checkbox(this->ui.fieldArmorLockCantShed, titleUpdateData.flags, 0x02);
   reach_main_window_setup_flag_checkbox(this->ui.fieldArmorLockCanBeStuck, titleUpdateData.flags, 0x04);
   reach_main_window_setup_flag_checkbox(this->ui.fieldEnableActiveCamoModifiers, titleUpdateData.flags, 0x08);
   reach_main_window_setup_flag_checkbox(this->ui.fieldLimitSwordBlockToSword, titleUpdateData.flags, 0x10);
   reach_main_window_setup_flag_checkbox(this->ui.fieldAutomaticMagnum, titleUpdateData.flags, 0x20);
   reach_main_window_setup_flag_checkbox(this->ui.fieldFlag6, titleUpdateData.flags, 0x40);
   reach_main_window_setup_flag_checkbox(this->ui.fieldFlag7, titleUpdateData.flags, 0x80);
   {  // Precision Bloom
      QDoubleSpinBox* widget = this->ui.fieldPrecisionBloom;
      QObject::connect(widget, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [](int value) {
         auto data = ReachEditorState::get().multiplayerData();
         if (!data)
            return;
         data->titleUpdateData.precisionBloom = value * 5.0F / 100.0F; // normalize from percentage of vanilla to internal format
      });
   }
   reach_main_window_setup_spinbox_dbl(this->ui.fieldArmorLockDamageDrain, titleUpdateData.armorLockDamageDrain);
   reach_main_window_setup_spinbox_dbl(this->ui.fieldArmorLockDamageDrainLimit, titleUpdateData.armorLockDamageDrainLimit);
   reach_main_window_setup_spinbox_dbl(this->ui.fieldActiveCamoEnergyBonus, titleUpdateData.activeCamoEnergyBonus);
   reach_main_window_setup_spinbox_dbl(this->ui.fieldActiveCamoEnergy, titleUpdateData.activeCamoEnergy);
   reach_main_window_setup_spinbox_dbl(this->ui.fieldMagnumDamage, titleUpdateData.magnumDamage);
   reach_main_window_setup_spinbox_dbl(this->ui.fieldMagnumFireDelay, titleUpdateData.magnumFireDelay);
   #include "widget_macros_setup_end.h"
}
void PageMPTU1Config::updateFromVariant(GameVariant* variant) {
   auto mp = variant->get_multiplayer_data();
   if (!mp)
      return;
   #include "widget_macros_update_start.h"
   reach_main_window_update_flag_checkbox(this->ui.fieldBleedthrough, titleUpdateData.flags, 0x01);
   reach_main_window_update_flag_checkbox(this->ui.fieldArmorLockCantShed, titleUpdateData.flags, 0x02);
   reach_main_window_update_flag_checkbox(this->ui.fieldArmorLockCanBeStuck, titleUpdateData.flags, 0x04);
   reach_main_window_update_flag_checkbox(this->ui.fieldEnableActiveCamoModifiers, titleUpdateData.flags, 0x08);
   reach_main_window_update_flag_checkbox(this->ui.fieldLimitSwordBlockToSword, titleUpdateData.flags, 0x10);
   reach_main_window_update_flag_checkbox(this->ui.fieldAutomaticMagnum, titleUpdateData.flags, 0x20);
   reach_main_window_update_flag_checkbox(this->ui.fieldFlag6, titleUpdateData.flags, 0x40);
   reach_main_window_update_flag_checkbox(this->ui.fieldFlag7, titleUpdateData.flags, 0x80);
   {  // Precision Bloom
      auto widget = this->ui.fieldPrecisionBloom;
      const QSignalBlocker blocker(widget);
      widget->setValue(mp->titleUpdateData.precisionBloom * 100.0F / 5.0F); // normalize to percentage of vanilla
   }
   reach_main_window_update_spinbox(this->ui.fieldArmorLockDamageDrain, titleUpdateData.armorLockDamageDrain);
   reach_main_window_update_spinbox(this->ui.fieldArmorLockDamageDrainLimit, titleUpdateData.armorLockDamageDrainLimit);
   reach_main_window_update_spinbox(this->ui.fieldActiveCamoEnergyBonus, titleUpdateData.activeCamoEnergyBonus);
   reach_main_window_update_spinbox(this->ui.fieldActiveCamoEnergy, titleUpdateData.activeCamoEnergy);
   reach_main_window_update_spinbox(this->ui.fieldMagnumDamage, titleUpdateData.magnumDamage);
   reach_main_window_update_spinbox(this->ui.fieldMagnumFireDelay, titleUpdateData.magnumFireDelay);
   #include "widget_macros_update_end.h"
}