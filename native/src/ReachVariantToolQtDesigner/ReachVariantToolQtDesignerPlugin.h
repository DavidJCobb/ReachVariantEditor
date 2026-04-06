#pragma once
#include <QtUiPlugin/QDesignerCustomWidgetCollectionInterface>

class ReachVariantToolQtDesignerPlugin : public QObject, public QDesignerCustomWidgetCollectionInterface {
   Q_OBJECT;
   Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QDesignerCustomWidgetInterface" FILE "reachvarianttoolqtdesignerplugin.json");
   Q_INTERFACES(QDesignerCustomWidgetCollectionInterface);
   public:
       ReachVariantToolQtDesignerPlugin(QObject* parent = nullptr);

       QList<QDesignerCustomWidgetInterface*> customWidgets() const override;

   private:
      QList<QDesignerCustomWidgetInterface*> widgets;
};
