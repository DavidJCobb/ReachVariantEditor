#include "page_multiplayer_title_update_1.h"
#include <QMessageBox>

PageMPTU1Config::PageMPTU1Config(QWidget* parent) : QWidget(parent) {
   ui.setupUi(this);
   //
   auto& editor = ReachEditorState::get();
   QObject::connect(&editor, &ReachEditorState::variantAcquired, this, &PageMPTU1Config::updateFromVariant);
   //
   QObject::connect(this->ui.presetButtonApply, &QPushButton::clicked, [this]() {
      auto variant = ReachEditorState::get().variant();
      if (!variant)
         return;
      auto mp = variant->get_multiplayer_data();
      if (!mp)
         return;
      auto& tu = mp->titleUpdateData;
      //
      auto    which = this->ui.presetList->currentIndex();
      QString prompt;
      switch (which) {
         case 0: // vanilla
            prompt = tr("Reset all Title Update settings to match the vanilla values?");
            break;
         case 1: // TU
            prompt = tr("Reset all Title Update settings to match the Matchmaking TU values?");
            break;
         case 2: // anniversary
            prompt = tr("Reset all Title Update settings to match the Matchmaking Anniversary values?");
            break;
         case 3: // zero bloom
            prompt = tr("Reset all Title Update settings to match the Matchmaking Zero Bloom values?");
            break;
      }
      if (QMessageBox::No == QMessageBox::question(this, tr("Apply Title Update setting preset"), prompt, QMessageBox::Yes | QMessageBox::No, QMessageBox::No)) {
         return;
      }
      switch (which) {
         case 0: tu.make_vanilla(); break;
         case 1: tu.make_patched(); break;
         case 2: tu.make_anniversary(); break;
         case 3: tu.make_zero_bloom(); break;
      }
      this->updateFromVariant(variant);
   });
   //
   #include "widget_macros_setup_start.h"
   reach_main_window_setup_flag_checkbox(this->ui.fieldBleedthrough, titleUpdateData.flags, 0x01);
   reach_main_window_setup_flag_checkbox(this->ui.fieldArmorLockCantShed, titleUpdateData.flags, 0x02);
   reach_main_window_setup_flag_checkbox(this->ui.fieldArmorLockCanBeStuck, titleUpdateData.flags, 0x04);
   reach_main_window_setup_flag_checkbox(this->ui.fieldEnableActiveCamoModifiers, titleUpdateData.flags, 0x08);
   reach_main_window_setup_flag_checkbox(this->ui.fieldLimitSwordBlockToSword, titleUpdateData.flags, 0x10);
   reach_main_window_setup_flag_checkbox(this->ui.fieldAutomaticMagnum, titleUpdateData.flags, 0x20);
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
   reach_main_window_setup_spinbox_dbl(this->ui.fieldActiveCamoEnergyCurveMax, titleUpdateData.activeCamoEnergyCurveMax);
   reach_main_window_setup_spinbox_dbl(this->ui.fieldActiveCamoEnergyCurveMin, titleUpdateData.activeCamoEnergyCurveMin);
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
   {  // Precision Bloom
      auto widget = this->ui.fieldPrecisionBloom;
      const QSignalBlocker blocker(widget);
      widget->setValue(mp->titleUpdateData.precisionBloom * 100.0F / 5.0F); // normalize to percentage of vanilla
   }
   reach_main_window_update_spinbox(this->ui.fieldArmorLockDamageDrain, titleUpdateData.armorLockDamageDrain);
   reach_main_window_update_spinbox(this->ui.fieldArmorLockDamageDrainLimit, titleUpdateData.armorLockDamageDrainLimit);
   reach_main_window_update_spinbox(this->ui.fieldActiveCamoEnergyCurveMax, titleUpdateData.activeCamoEnergyCurveMax);
   reach_main_window_update_spinbox(this->ui.fieldActiveCamoEnergyCurveMin, titleUpdateData.activeCamoEnergyCurveMin);
   reach_main_window_update_spinbox(this->ui.fieldMagnumDamage, titleUpdateData.magnumDamage);
   reach_main_window_update_spinbox(this->ui.fieldMagnumFireDelay, titleUpdateData.magnumFireDelay);
   #include "widget_macros_update_end.h"
}