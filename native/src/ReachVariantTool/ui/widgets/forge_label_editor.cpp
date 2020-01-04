#include "forge_label_editor.h"

ForgeLabelEditorWidget::ForgeLabelEditorWidget(uint8_t labelIndex, QWidget* parent) : QWidget(parent), labelIndex(labelIndex) {
   ui.setupUi(this);
   //
   auto& editor = ReachEditorState::get();
   QObject::connect(&editor, &ReachEditorState::variantAcquired, this, &ForgeLabelEditorWidget::updateFromVariant);

   // TODO

   this->updateFromVariant(editor.variant());
}
void ForgeLabelEditorWidget::updateFromVariant(GameVariant* variant) {
   if (!variant)
      return;
   auto mp = variant->get_multiplayer_data();
   if (!mp)
      return;
   auto& labels = mp->scriptContent.forgeLabels;
   if (this->labelIndex >= labels.size())
      return;
   auto& label = labels[this->labelIndex];
   //
   const QSignalBlocker blocker0(this->ui.fieldName);
   const QSignalBlocker blocker1(this->ui.fieldFlagRequireNumber);
   const QSignalBlocker blocker2(this->ui.fieldFlagRequireObjectType);
   const QSignalBlocker blocker3(this->ui.fieldFlagRequireTeam);
   const QSignalBlocker blocker4(this->ui.fieldRequireNumber);
   const QSignalBlocker blocker5(this->ui.fieldRequireObjectType);
   const QSignalBlocker blocker6(this->ui.fieldRequireTeam);
   const QSignalBlocker blocker7(this->ui.fieldMapMustHaveAtLeast);
   if (label.name) {
      this->ui.fieldName->setText(QString::fromUtf8(label.name->english().c_str()));
   } else
      this->ui.fieldName->setText("");
   this->ui.fieldFlagRequireNumber->setChecked(label.requirements & Megalo::ReachForgeLabel::requirement_flags::number);
   this->ui.fieldFlagRequireObjectType->setChecked(label.requirements & Megalo::ReachForgeLabel::requirement_flags::objects_of_type);
   this->ui.fieldFlagRequireTeam->setChecked(label.requirements & Megalo::ReachForgeLabel::requirement_flags::assigned_team);
   this->ui.fieldRequireNumber->setValue(label.requiredNumber);
   this->ui.fieldRequireObjectType->setValue(label.requiredObjectType);
   this->ui.fieldRequireTeam->setValue(label.requiredTeam);
   this->ui.fieldMapMustHaveAtLeast->setValue(label.mapMustHaveAtLeast);
}