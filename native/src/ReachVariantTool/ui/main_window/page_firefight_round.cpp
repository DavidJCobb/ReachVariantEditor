#include "page_firefight_round.h"

PageFFRound::PageFFRound(QWidget* parent) : QWidget(parent) {
   ui.setupUi(this);
   //
   auto& editor = ReachEditorState::get();
   QObject::connect(&editor, &ReachEditorState::variantAcquired,  this, &PageFFRound::updateFromVariant);
   QObject::connect(&editor, &ReachEditorState::variantAbandoned, this, [this]() { this->updateFromVariant(nullptr); });
}
void PageFFRound::setIndex(int8_t i) {
   if (i >= std::extent<decltype(GameVariantDataFirefight::rounds)>::value)
      i = -1;
   this->index = i;
   this->updateFromVariant(ReachEditorState::get().variant());
}
void PageFFRound::updateFromVariant(GameVariant* variant) {
   auto ff = variant ? variant->get_firefight_data() : nullptr;
   if (this->index < 0 || !ff) {
      this->ui.tabSkullsContent->clearTarget();
      this->ui.tabWaveInitialContent->clearTarget();
      this->ui.tabWaveMainContent->clearTarget();
      this->ui.tabWaveBossContent->clearTarget();
   }
   this->ui.tabSkullsContent->setTarget(ff->rounds[this->index].skulls);
   this->ui.tabWaveInitialContent->setTarget(ff->rounds[this->index].waveInitial);
   this->ui.tabWaveMainContent->setTarget(ff->rounds[this->index].waveMain);
   this->ui.tabWaveBossContent->setTarget(ff->rounds[this->index].waveBoss);
}