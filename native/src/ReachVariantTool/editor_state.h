#pragma once
#include "game_variants/base.h"
#include "game_variants/components/loadouts.h"
#include "game_variants/components/player_traits.h"
#include "game_variants/types/multiplayer.h"

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
      std::wstring       currentFile;
      GameVariant*       currentVariant = nullptr;
      ReachPlayerTraits* currentTraits  = nullptr;
      ReachLoadoutPalette* currentLoadoutPalette = nullptr;
      //
      GameVariantDataMultiplayer* get_multiplayer_data() noexcept {
         auto v = this->currentVariant;
         if (!v)
            return nullptr;
         auto& m = v->multiplayer;
         if (m.data) {
            switch (m.data->get_type()) {
               case ReachGameEngine::multiplayer:
               case ReachGameEngine::forge:
                  return dynamic_cast<GameVariantDataMultiplayer*>(m.data);
            }
         }
         return nullptr;
      }
      //
      void take_game_variant(GameVariant* other, const wchar_t* path) noexcept {
         if (this->currentVariant)
            delete this->currentVariant;
         this->currentVariant = other;
         this->currentTraits  = nullptr;
         this->currentLoadoutPalette = nullptr;
         this->currentFile    = path;
      }
      void start_editing_traits(ReachPlayerTraits* which) {
         this->currentTraits = which;
      }
      void start_editing_loadouts(ReachLoadoutPalette* which) {
         this->currentLoadoutPalette = which;
      }
      void set_variant_file_path(const wchar_t* path) noexcept { // call after Save As
         this->currentFile = path;
      }
      //
      // TODO: Something to consider: if we create a copy of a game variant upon 
      // loading it, then we can use comparisons to check whether things are 
      // actually unchanged (rather than just using a simple bool). Might be slow 
      // though.
      //
};