#include "./headless.h"
#include <cassert>
#include <print>
#include <QDataStream>
#include <QFile>
#include <QSaveFile>
#include "./cli.h"
#include "./editor_state.h"
#include "./game_variants/base.h"
#include "./game_variants/io_process.h"
#include "./game_variants/warnings.h"
#include "./game_variants/components/megalo/compiler/compiler.h"
#include "./game_variants/components/megalo/decompiler/decompiler.h"

namespace rvt::headless {
   extern bool load_game_variant(const std::filesystem::path& path) {
      auto& editor  = ReachEditorState::get();
      auto* variant = new GameVariant;
      {
         auto    w_path = path.wstring();
         QString q_path = QString::fromStdWString(w_path);
         QFile   file(q_path);
         if (!file.open(QIODevice::ReadOnly)) {
            std::println(stderr, "Failed to open the game variant: %1", file.errorString().toStdString());
            return false;
         }
         auto buffer = file.readAll();
         bool success;
         if (q_path.endsWith(".mglo", Qt::CaseInsensitive)) {
            success = variant->read_mglo(buffer.data(), buffer.size());
         } else {
            success = variant->read(buffer.data(), buffer.size());
         }
         if (!success) {
            std::println(stderr, "Failed to open the game variant: %1", file.errorString().toStdString());
            return false;
         }
         {
            auto& log = GameEngineVariantLoadWarningLog::get();
            if (!log.warnings.empty()) {
               std::println("The following warnings were encountered:\n");
               for (auto& warning : log.warnings) {
                  std::println(" - {}", warning.toStdString());
               }
            }
         }

         editor.takeVariant(variant, w_path.c_str());
         return true;
      }
   }

   extern bool recompile(const command_line_params& params) {
      QString code;
      if (params.megalo_source.empty()) {
         std::println(stderr, "No Megalo source file specified.");
         return false;
      }
      {
         QString q_path = QString::fromStdWString(params.megalo_source);
         QFile   file(q_path);
         if (!file.open(QIODevice::ReadOnly)) {
            std::println(stderr, "Failed to open the Megalo source file: {}", file.errorString().toStdString());
            return false;
         }
         code = QString(file.readAll());
      }

      auto& editor  = ReachEditorState::get();
      auto* variant = editor.variant();
      assert(variant != nullptr);
      auto* mp = variant->get_multiplayer_data();
      if (!mp) {
         std::println(stderr, "This game variant is not a multiplayer variant, and so cannot have Megalo code.");
         return false;
      }

      Megalo::Compiler compiler(*mp);
      compiler.parse(code);

      for (auto& item : compiler.get_fatal_errors()) {
         std::println("[FATAL] Line {} Col {}: {}", item.pos.line, item.pos.col(), item.text.toStdString());
      }
      for (auto& item : compiler.get_non_fatal_errors()) {
         std::println("[ERROR] Line {} Col {}: {}", item.pos.line, item.pos.col(), item.text.toStdString());
      }
      for (auto& item : compiler.get_warnings()) {
         std::println("[WARN:] Line {} Col {}: {}", item.pos.line, item.pos.col(), item.text.toStdString());
      }
      for (auto& item : compiler.get_notices()) {
         std::println("[NOTE:] Line {} Col {}: {}", item.pos.line, item.pos.col(), item.text.toStdString());
      }

      if (compiler.has_errors()) {
         std::println("Compilation failed.");
         return false;
      }

      {
         auto& list = compiler.get_unresolved_string_references();
         if (!list.empty()) {
            std::println("Creating {} new strings.", list.size());
            for (auto& item : list) {
               item.pending.action = Megalo::Compiler::unresolved_string_pending_action::create;
            }
            compiler.handle_unresolved_string_references();
         }
      }
      compiler.apply();

      return true;
   }

   extern bool save_game_variant(const command_line_params& params) {
      auto path = params.game_variant.output;
      if (path.empty()) {
         path = params.game_variant.input;
         assert(!path.empty());
      }
      QString q_path = QString::fromStdWString(path.wstring());

      QSaveFile file(q_path);
      if (!file.open(QIODevice::WriteOnly)) {
         std::println(stderr, "Unable to open destination file for writing: {}", file.errorString().toStdString());
         return false;
      }

      GameVariantSaveProcess save_process;
      if (q_path.endsWith(".mglo", Qt::CaseInsensitive)) {
         save_process.set_flag(GameVariantSaveProcess::flag::save_bare_mglo);
      }

      ReachEditorState::get().variant()->write(save_process);
      if (save_process.variant_is_editor_only()) {
         std::println(stderr, "The updated game variant exceeds Reach's file format limits. Refusing to save changes.");
         file.cancelWriting();
         return false;
      }

      QDataStream out(&file);
      out.setVersion(QDataStream::Qt_4_5);
      out.writeRawData((const char*)save_process.writer.bytes.data(), save_process.writer.bytes.get_bytespan());
      file.commit();

      std::println(stderr, "Updated game variant has been saved.");
      return true;
   }
}