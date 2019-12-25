#include "OptionToggleTable.h"
#include "../../editor_state.h"

OptionToggleTreeRowModel* OptionToggleTreeRowModel::get_parent() const noexcept {
   if (this->parent == -1)
      return nullptr;
   return this->owner.rows[this->parent];
}
OptionToggleTreeRowModel* OptionToggleTreeRowModel::get_child(int index) const noexcept {
   uint32_t counter = 0;
   for (auto* row : this->owner.rows) {
      if (row->parent == this->index) {
         if (counter == index)
            return row;
         ++counter;
      }
   }
   return nullptr;
}
int OptionToggleTreeRowModel::get_child_count() const noexcept {
   uint32_t counter = 0;
   for (auto* row : this->owner.rows)
      if (row->parent == this->index)
         ++counter;
   return counter;
}
int OptionToggleTreeRowModel::get_index_in_parent() const noexcept {
   int32_t counter = 0;
   for (auto* row : this->owner.rows) {
      if (row->parent == this->parent) {
         if (row == this)
            return counter;
         ++counter;
      }
   }
   return -1;
}

bool EngineOptionToggleTreeModel::checkDisabledFlag(uint16_t index) const noexcept {
   if (index >= 1272)
      return false;
   auto& editor = ReachEditorState::get();
   auto  variant = editor.currentVariant;
   if (!variant)
      return false;
   return variant->multiplayer.optionToggles.engine.disabled.bits.test(index);
}
bool EngineOptionToggleTreeModel::checkHiddenFlag(uint16_t index) const noexcept {
   if (index >= 1272)
      return false;
   auto& editor = ReachEditorState::get();
   auto  variant = editor.currentVariant;
   if (!variant)
      return false;
   return variant->multiplayer.optionToggles.engine.hidden.bits.test(index);
}
void EngineOptionToggleTreeModel::modifyDisabledFlag(uint16_t index, bool state) noexcept {
   if (index >= 1272)
      return;
   auto& editor = ReachEditorState::get();
   auto  variant = editor.currentVariant;
   if (!variant)
      return;
   variant->multiplayer.optionToggles.engine.disabled.bits.modify(index, state);
}
void EngineOptionToggleTreeModel::modifyHiddenFlag(uint16_t index, bool state) noexcept {
   if (index >= 1272)
      return;
   auto& editor = ReachEditorState::get();
   auto  variant = editor.currentVariant;
   if (!variant)
      return;
   variant->multiplayer.optionToggles.engine.hidden.bits.modify(index, state);
}