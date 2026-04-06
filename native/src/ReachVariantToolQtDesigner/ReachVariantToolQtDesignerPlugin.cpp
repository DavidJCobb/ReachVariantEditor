#include "ReachVariantToolQtDesignerPlugin.h"
#include <QtCore/QtPlugin>
#include "./interfaces/RVTArmorAbilityPickerInterface.h"
#include "./interfaces/RVTWeaponPickerInterface.h"

ReachVariantToolQtDesignerPlugin::ReachVariantToolQtDesignerPlugin(QObject *parent) : QObject(parent) {
   widgets.append(new RVTArmorAbilityPickerInterface(this));
   widgets.append(new RVTWeaponPickerInterface(this));
}

QList<QDesignerCustomWidgetInterface*> ReachVariantToolQtDesignerPlugin::customWidgets() const {
   return widgets;
}