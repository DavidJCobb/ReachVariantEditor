#include "io_process.h"

GameVariantSaveProcess::~GameVariantSaveProcess() {
   for (auto* subrecord : this->xrvt_subrecords)
      if (subrecord)
         delete subrecord;
   this->xrvt_subrecords.clear();
}
RVTEditorBlock::subrecord* GameVariantSaveProcess::create_subrecord(uint32_t signature) noexcept {
   auto subrecord = new RVTEditorBlock::subrecord;
   subrecord->signature = signature;
   this->xrvt_subrecords.push_back(subrecord);
   return subrecord;
}