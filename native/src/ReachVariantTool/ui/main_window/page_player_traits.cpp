#include "page_player_traits.h"

PagePlayerTraits::PagePlayerTraits(QWidget* parent) : QWidget(parent) {
   ui.setupUi(this);
   //
   auto& editor = ReachEditorState::get();
   QObject::connect(&editor, &ReachEditorState::switchedPlayerTraits, this, &PagePlayerTraits::updateFromVariant);
   //
   this->ui.PagePlayerTraitsTabview->setCurrentIndex(0);
   //
   #include "widget_macros_setup_start.h"
   {  // Defense
      reach_state_pane_setup_combobox(playerTraits, this->ui.fieldDamageResist, defense.damageResist);
      reach_state_pane_setup_combobox(playerTraits, this->ui.fieldHealthMult, defense.healthMult);
      reach_state_pane_setup_combobox(playerTraits, this->ui.fieldHealthRegenRate, defense.healthRate);
      reach_state_pane_setup_combobox(playerTraits, this->ui.fieldShieldMult, defense.shieldMult);
      reach_state_pane_setup_combobox(playerTraits, this->ui.fieldShieldRegenRate, defense.shieldRate);
      reach_state_pane_setup_combobox(playerTraits, this->ui.fieldShieldRegenRateOver, defense.overshieldRate);
      reach_state_pane_setup_combobox(playerTraits, this->ui.fieldHeadshotImmunity, defense.headshotImmune);
      reach_state_pane_setup_combobox(playerTraits, this->ui.fieldAssassinationImmunity, defense.assassinImmune);
      reach_state_pane_setup_combobox(playerTraits, this->ui.fieldShieldVampirism, defense.vampirism);
      reach_state_pane_setup_combobox(playerTraits, this->ui.fieldCannotDieFromDamage, defense.cannotDieFromDamage);
   }
   {  // Offense
      reach_state_pane_setup_combobox(playerTraits, this->ui.fieldDamageMult, offense.damageMult);
      reach_state_pane_setup_combobox(playerTraits, this->ui.fieldMeleeMult, offense.meleeMult);
      {
         auto widget = this->ui.fieldWeaponPrimary;
         QObject::connect(widget, QOverload<int>::of(&QComboBox::currentIndexChanged), [widget](int value) {
            auto traits = ReachEditorState::get().playerTraits();
            if (!traits)
               return;
            traits->offense.weaponPrimary = (reach::weapon)widget->currentData().toInt();
         });
      }
      {
         auto widget = this->ui.fieldWeaponSecondary;
         QObject::connect(widget, QOverload<int>::of(&QComboBox::currentIndexChanged), [widget](int value) {
            auto traits = ReachEditorState::get().playerTraits();
            if (!traits)
               return;
            traits->offense.weaponSecondary = (reach::weapon)widget->currentData().toInt();
         });
      }
      {
         auto widget = this->ui.fieldArmorAbility;
         QObject::connect(widget, QOverload<int>::of(&QComboBox::currentIndexChanged), [widget](int value) {
            auto traits = ReachEditorState::get().playerTraits();
            if (!traits)
               return;
            traits->offense.ability = (reach::ability)widget->currentData().toInt();
         });
      }
      reach_state_pane_setup_combobox(playerTraits, this->ui.fieldGrenadeCount, offense.grenadeCount);
      reach_state_pane_setup_combobox(playerTraits, this->ui.fieldGrenadeRegen, offense.grenadeRegen);
      reach_state_pane_setup_combobox(playerTraits, this->ui.fieldWeaponPickup, offense.weaponPickup);
      reach_state_pane_setup_combobox(playerTraits, this->ui.fieldInfiniteAmmo, offense.infiniteAmmo);
      reach_state_pane_setup_combobox(playerTraits, this->ui.fieldAbilityUsage, offense.abilityUsage);
      reach_state_pane_setup_combobox(playerTraits, this->ui.fieldAbilitiesDropOnDeath, offense.abilitiesDropOnDeath);
      reach_state_pane_setup_combobox(playerTraits, this->ui.fieldInfiniteAbility, offense.infiniteAbility);
   }
   {  // Movement
      reach_state_pane_setup_combobox(playerTraits, this->ui.fieldMovementSpeed, movement.speed);
      reach_state_pane_setup_spinbox(playerTraits, this->ui.fieldJumpHeight, movement.jumpHeight);
      reach_state_pane_setup_combobox(playerTraits, this->ui.fieldGravity, movement.gravity);
      reach_state_pane_setup_combobox(playerTraits, this->ui.fieldMovementUnknown, movement.unknown);
      reach_state_pane_setup_combobox(playerTraits, this->ui.fieldVehicleUse, movement.vehicleUsage);
   }
   {  // Appearance
      reach_state_pane_setup_combobox(playerTraits, this->ui.fieldActiveCamo, appearance.activeCamo);
      reach_state_pane_setup_combobox(playerTraits, this->ui.fieldVisibleWaypoint, appearance.waypoint);
      reach_state_pane_setup_combobox(playerTraits, this->ui.fieldVisibleName, appearance.visibleName);
      reach_state_pane_setup_combobox(playerTraits, this->ui.fieldAura, appearance.aura);
      reach_state_pane_setup_combobox(playerTraits, this->ui.fieldForcedColor, appearance.forcedColor);
   }
   {  // Sensors
      reach_state_pane_setup_combobox(playerTraits, this->ui.fieldRadarState, sensors.radarState);
      reach_state_pane_setup_combobox(playerTraits, this->ui.fieldRadarRange, sensors.radarRange);
      reach_state_pane_setup_combobox(playerTraits, this->ui.fieldDirectionalDamageIndicator, sensors.directionalDamageIndicator);
   }
   #include "widget_macros_setup_end.h"
}
void PagePlayerTraits::updateFromVariant(ReachPlayerTraits* mp) {
   if (!mp)
      return;
   #include "widget_macros_update_start.h"
   {  // Defense
      reach_main_window_update_combobox(this->ui.fieldDamageResist,     defense.damageResist);
      reach_main_window_update_combobox(this->ui.fieldHealthMult,       defense.healthMult);
      reach_main_window_update_combobox(this->ui.fieldHealthRegenRate,  defense.healthRate);
      reach_main_window_update_combobox(this->ui.fieldShieldMult,       defense.shieldMult);
      reach_main_window_update_combobox(this->ui.fieldShieldRegenRate,  defense.shieldRate);
      reach_main_window_update_combobox(this->ui.fieldShieldRegenRateOver,   defense.overshieldRate);
      reach_main_window_update_combobox(this->ui.fieldHeadshotImmunity,      defense.headshotImmune);
      reach_main_window_update_combobox(this->ui.fieldAssassinationImmunity, defense.assassinImmune);
      reach_main_window_update_combobox(this->ui.fieldShieldVampirism,       defense.vampirism);
      reach_main_window_update_combobox(this->ui.fieldCannotDieFromDamage,   defense.cannotDieFromDamage);
   }
   {  // Offense
      reach_main_window_update_combobox(this->ui.fieldDamageMult,      offense.damageMult);
      reach_main_window_update_combobox(this->ui.fieldMeleeMult,       offense.meleeMult);
      {
         auto widget = this->ui.fieldWeaponPrimary;
         const QSignalBlocker blocker(widget);
         widget->setByData((int)mp->offense.weaponPrimary);
      }
      {
         auto widget = this->ui.fieldWeaponSecondary;
         const QSignalBlocker blocker(widget);
         widget->setByData((int)mp->offense.weaponSecondary);
      }
      {
         auto widget = this->ui.fieldArmorAbility;
         const QSignalBlocker blocker(widget);
         widget->setByData((int)mp->offense.ability);
      }
      reach_main_window_update_combobox(this->ui.fieldGrenadeCount,    offense.grenadeCount);
      reach_main_window_update_combobox(this->ui.fieldGrenadeRegen,    offense.grenadeRegen);
      reach_main_window_update_combobox(this->ui.fieldWeaponPickup,    offense.weaponPickup);
      reach_main_window_update_combobox(this->ui.fieldInfiniteAmmo,    offense.infiniteAmmo);
      reach_main_window_update_combobox(this->ui.fieldAbilityUsage, offense.abilityUsage);
      reach_main_window_update_combobox(this->ui.fieldAbilitiesDropOnDeath, offense.abilitiesDropOnDeath);
      reach_main_window_update_combobox(this->ui.fieldInfiniteAbility, offense.infiniteAbility);
   }
   {  // Movement
      reach_main_window_update_combobox(this->ui.fieldMovementSpeed, movement.speed);
      reach_main_window_update_spinbox(this->ui.fieldJumpHeight, movement.jumpHeight);
      reach_main_window_update_combobox(this->ui.fieldGravity, movement.gravity);
      reach_main_window_update_combobox(this->ui.fieldMovementUnknown, movement.unknown);
      reach_main_window_update_combobox(this->ui.fieldVehicleUse, movement.vehicleUsage);
   }
   {  // Appearance
      reach_main_window_update_combobox(this->ui.fieldActiveCamo,      appearance.activeCamo);
      reach_main_window_update_combobox(this->ui.fieldVisibleWaypoint, appearance.waypoint);
      reach_main_window_update_combobox(this->ui.fieldVisibleName,     appearance.visibleName);
      reach_main_window_update_combobox(this->ui.fieldAura,            appearance.aura);
      reach_main_window_update_combobox(this->ui.fieldForcedColor,     appearance.forcedColor);
   }
   {  // Sensors
      reach_main_window_update_combobox(this->ui.fieldRadarState, sensors.radarState);
      reach_main_window_update_combobox(this->ui.fieldRadarRange, sensors.radarRange);
      reach_main_window_update_combobox(this->ui.fieldDirectionalDamageIndicator, sensors.directionalDamageIndicator);
   }
   #include "widget_macros_update_end.h"
}