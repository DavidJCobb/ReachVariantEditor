#pragma once
#include "game_variants/base.h"

class ReachEditorState {
   public:
      static ReachEditorState& get() {
         static ReachEditorState instance;
         return instance;
      }
      //
      ReachEditorState() {}
      ReachEditorState(const ReachEditorState& other) = delete; // no copy
      //
      std::wstring currentFile;
      GameVariant* currentVariant = nullptr;
      //
      void take_game_variant(GameVariant* other, const wchar_t* path) noexcept {
         if (this->currentVariant)
            delete this->currentVariant;
         this->currentVariant = other;
         this->currentFile    = path;
      }
};