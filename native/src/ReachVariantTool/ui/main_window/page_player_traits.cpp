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
      reach_traits_pane_setup_combobox(this->ui.fieldDamageResist, defense.damageResist);
      reach_traits_pane_setup_combobox(this->ui.fieldHealthMult, defense.healthMult);
      reach_traits_pane_setup_combobox(this->ui.fieldHealthRegenRate, defense.healthRate);
      reach_traits_pane_setup_combobox(this->ui.fieldShieldMult, defense.shieldMult);
      reach_traits_pane_setup_combobox(this->ui.fieldShieldRegenRate, defense.shieldRate);
      reach_traits_pane_setup_combobox(this->ui.fieldShieldRegenDelay, defense.shieldDelay);
      reach_traits_pane_setup_combobox(this->ui.fieldHeadshotImmunity, defense.headshotImmune);
      reach_traits_pane_setup_combobox(this->ui.fieldAssassinationImmunity, defense.assassinImmune);
      reach_traits_pane_setup_combobox(this->ui.fieldShieldVampirism, defense.vampirism);
      reach_traits_pane_setup_combobox(this->ui.fieldCannotDieFromDamage, defense.cannotDieFromDamage);
   }
   {  // Offense
      reach_traits_pane_setup_combobox(this->ui.fieldDamageMult, offense.damageMult);
      reach_traits_pane_setup_combobox(this->ui.fieldMeleeMult, offense.meleeMult);
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
      reach_traits_pane_setup_combobox(this->ui.fieldGrenadeCount, offense.grenadeCount);
      reach_traits_pane_setup_combobox(this->ui.fieldGrenadeRegen, offense.grenadeRegen);
      reach_traits_pane_setup_combobox(this->ui.fieldWeaponPickup, offense.weaponPickup);
      reach_traits_pane_setup_combobox(this->ui.fieldInfiniteAmmo, offense.infiniteAmmo);
      reach_traits_pane_setup_combobox(this->ui.fieldAbilityUsage, offense.abilityUsage);
      reach_traits_pane_setup_combobox(this->ui.fieldAbilitiesDropOnDeath, offense.abilitiesDropOnDeath);
      reach_traits_pane_setup_combobox(this->ui.fieldInfiniteAbility, offense.infiniteAbility);
   }
   {  // Movement
      reach_traits_pane_setup_combobox(this->ui.fieldMovementSpeed, movement.speed);
      reach_traits_pane_setup_spinbox(this->ui.fieldJumpHeight, movement.jumpHeight);
      reach_traits_pane_setup_combobox(this->ui.fieldGravity, movement.gravity);
      reach_traits_pane_setup_combobox(this->ui.fieldMovementUnknown, movement.unknown);
      reach_traits_pane_setup_combobox(this->ui.fieldVehicleUse, movement.vehicleUsage);
   }
   {  // Appearance
      reach_traits_pane_setup_combobox(this->ui.fieldActiveCamo, appearance.activeCamo);
      reach_traits_pane_setup_combobox(this->ui.fieldVisibleWaypoint, appearance.waypoint);
      reach_traits_pane_setup_combobox(this->ui.fieldVisibleName, appearance.visibleName);
      reach_traits_pane_setup_combobox(this->ui.fieldAura, appearance.aura);
      reach_traits_pane_setup_combobox(this->ui.fieldForcedColor, appearance.forcedColor);
   }
   {  // Sensors
      reach_traits_pane_setup_combobox(this->ui.fieldRadarState, sensors.radarState);
      reach_traits_pane_setup_combobox(this->ui.fieldRadarRange, sensors.radarRange);
      reach_traits_pane_setup_combobox(this->ui.fieldDirectionalDamageIndicator, sensors.directionalDamageIndicator);
   }
   #include "widget_macros_setup_end.h"
}
void PagePlayerTraits::updateFromVariant(ReachPlayerTraits* traits) {
   if (!traits)
      return;
   #include "widget_macros_update_start.h"
   {  // Defense
      reach_traits_pane_update_combobox(this->ui.fieldDamageResist,     defense.damageResist);
      reach_traits_pane_update_combobox(this->ui.fieldHealthMult,       defense.healthMult);
      reach_traits_pane_update_combobox(this->ui.fieldHealthRegenRate,  defense.healthRate);
      reach_traits_pane_update_combobox(this->ui.fieldShieldMult,       defense.shieldMult);
      reach_traits_pane_update_combobox(this->ui.fieldShieldRegenRate,  defense.shieldRate);
      reach_traits_pane_update_combobox(this->ui.fieldShieldRegenDelay, defense.shieldDelay);
      reach_traits_pane_update_combobox(this->ui.fieldHeadshotImmunity, defense.headshotImmune);
      reach_traits_pane_update_combobox(this->ui.fieldAssassinationImmunity, defense.assassinImmune);
      reach_traits_pane_update_combobox(this->ui.fieldShieldVampirism,       defense.vampirism);
      reach_traits_pane_update_combobox(this->ui.fieldCannotDieFromDamage,   defense.cannotDieFromDamage);
   }
   {  // Offense
      reach_traits_pane_update_combobox(this->ui.fieldDamageMult,      offense.damageMult);
      reach_traits_pane_update_combobox(this->ui.fieldMeleeMult,       offense.meleeMult);
      {
         auto widget = this->ui.fieldWeaponPrimary;
         const QSignalBlocker blocker(widget);
         widget->setByData((int)traits->offense.weaponPrimary);
      }
      {
         auto widget = this->ui.fieldWeaponSecondary;
         const QSignalBlocker blocker(widget);
         widget->setByData((int)traits->offense.weaponSecondary);
      }
      {
         auto widget = this->ui.fieldArmorAbility;
         const QSignalBlocker blocker(widget);
         widget->setByData((int)traits->offense.ability);
      }
      reach_traits_pane_update_combobox(this->ui.fieldGrenadeCount,    offense.grenadeCount);
      reach_traits_pane_update_combobox(this->ui.fieldGrenadeRegen,    offense.grenadeRegen);
      reach_traits_pane_update_combobox(this->ui.fieldWeaponPickup,    offense.weaponPickup);
      reach_traits_pane_update_combobox(this->ui.fieldInfiniteAmmo,    offense.infiniteAmmo);
      reach_traits_pane_update_combobox(this->ui.fieldAbilityUsage, offense.abilityUsage);
      reach_traits_pane_update_combobox(this->ui.fieldAbilitiesDropOnDeath, offense.abilitiesDropOnDeath);
      reach_traits_pane_update_combobox(this->ui.fieldInfiniteAbility, offense.infiniteAbility);
   }
   {  // Movement
      reach_traits_pane_update_combobox(this->ui.fieldMovementSpeed, movement.speed);
      reach_traits_pane_update_spinbox(this->ui.fieldJumpHeight, movement.jumpHeight);
      reach_traits_pane_update_combobox(this->ui.fieldGravity, movement.gravity);
      reach_traits_pane_update_combobox(this->ui.fieldMovementUnknown, movement.unknown);
      reach_traits_pane_update_combobox(this->ui.fieldVehicleUse, movement.vehicleUsage);
   }
   {  // Appearance
      reach_traits_pane_update_combobox(this->ui.fieldActiveCamo,      appearance.activeCamo);
      reach_traits_pane_update_combobox(this->ui.fieldVisibleWaypoint, appearance.waypoint);
      reach_traits_pane_update_combobox(this->ui.fieldVisibleName,     appearance.visibleName);
      reach_traits_pane_update_combobox(this->ui.fieldAura,            appearance.aura);
      reach_traits_pane_update_combobox(this->ui.fieldForcedColor,     appearance.forcedColor);
   }
   {  // Sensors
      reach_traits_pane_update_combobox(this->ui.fieldRadarState, sensors.radarState);
      reach_traits_pane_update_combobox(this->ui.fieldRadarRange, sensors.radarRange);
      reach_traits_pane_update_combobox(this->ui.fieldDirectionalDamageIndicator, sensors.directionalDamageIndicator);
   }
   #include "widget_macros_update_end.h"
}