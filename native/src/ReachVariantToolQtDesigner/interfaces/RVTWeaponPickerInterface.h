#pragma once
#include <QtUiPlugin/QDesignerCustomWidgetInterface>

class RVTWeaponPickerInterface : public QObject, public QDesignerCustomWidgetInterface {
   Q_OBJECT;
   Q_INTERFACES(QDesignerCustomWidgetInterface);
   public:
      RVTWeaponPickerInterface(QObject* parent = nullptr);

      bool isContainer() const { return false; }
      bool isInitialized() const { return this->initialized; }
      QIcon icon() const;
      QString domXml() const;
      QString group() const;
      QString includeFile() const;
      QString name() const;
      QString toolTip() const;
      QString whatsThis() const;
      QWidget* createWidget(QWidget* parent);
      void initialize(QDesignerFormEditorInterface* core);

   private:
      bool initialized = false;
};
