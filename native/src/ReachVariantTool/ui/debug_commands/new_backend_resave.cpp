#include "new_backend_resave.h"
#include <cstdint>
#include <QByteArray>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QWidget>
#include "halo/reach/game_variant.h"

namespace {
   bool _point_of_divergence(const QByteArray& src, const QByteArray& dst, size_t& bytes, uint8_t& bits, const int start = 0) {
      int length = std::min(src.size(), dst.size());
      for (int i = start; i < length; ++i) {
         uint8_t a = src.at(i);
         uint8_t b = dst.at(i);
         if (a == b)
            continue;
         //
         bytes = i;
         bits  = 0;
         for (int j = 7; j >= 0; --j) {
            uint8_t x = a & (1 << j);
            uint8_t y = b & (1 << j);
            if (x != y) {
               bits = 7 - j;
               break;
            }
         }
         return true;
      }
      return false;
   }
}

namespace rvt::debug_commands {
   extern void new_backend_resave(QWidget* parent) {
      auto paths = QFileDialog::getOpenFileNames(parent, "Select game variant files", "", "Game variants (*.bin)");
      if (paths.isEmpty())
         return;
      auto destination = QFileDialog::getExistingDirectory(parent, "Save copies to", "");
      if (destination.isEmpty())
         return;
      //
      QString results;
      for (const auto& path : paths) {
         QFile file(path);
         if (!file.open(QIODevice::ReadOnly)) {
            results += QString("%1\n - Failed to open. Windows error text: %2\n\n")
               .arg(path)
               .arg(file.errorString());
            continue;
         }
         auto buffer = file.readAll();
         //
         halo::reach::game_variant variant;
         auto load_process = variant.read(buffer);
         if (load_process.has_fatal()) {
            results += QString("%1\n - Failed to load. Load process fatal error.\n - %2\n\n")
               .arg(path)
               .arg(load_process.get_fatal().data->to_string());
            continue;
         }
         //
         //auto this_dest = destination + '/' + file.fileName();
         auto this_dest = destination + '/' + QFileInfo(file).fileName(); // god damn it, Qt...
         QFile out_file(this_dest);
         if (!out_file.open(QIODevice::WriteOnly)) {
            results += QString("%1\n - Failed to resave. Windows error text: %2\n - Destination: %3\n\n")
               .arg(path)
               .arg(file.errorString())
               .arg(this_dest);
            continue;
         }
         //
         auto output = variant.write();
         out_file.write(output.constData(), output.size());
         //
         size_t  pod_bytes;
         uint8_t pod_bits;
         bool    failed = _point_of_divergence(buffer, output, pod_bytes, pod_bits);
         bool    initial_failed = failed;
         //
         if (failed) {
            results += QString("%1\n - File content diverged at 0x%2+%3b.\n")
               .arg(path)
               .arg(pod_bytes, 0, 16)
               .arg(pod_bits);
            //
            bool retry = true;
            while (failed && retry) {
               retry = false;
               if (pod_bytes < 0x2F0) {
                  results += QString(" - Possibly CHDR. Retrying...\n");
                  //
                  failed = _point_of_divergence(buffer, output, pod_bytes, pod_bits, 0x2F0);
                  retry  = true;
                  continue;
               }
               //
               if (pod_bytes >= 0x2F0 && pod_bytes <= 0x5310) { // MPVR
                  if (pod_bytes >= 0x318) {
                     results += QString(" - MPVR bitstream; offset 0x%2+%3b.\n")
                        .arg(pod_bytes - 0x318, 0, 16)
                        .arg(pod_bits);
                  } else {
                     results += QString(" - MPVR header or hash; offset 0x%2+%3b from block signature start. Retrying...\n")
                        .arg(pod_bytes - 0x2F0, 0, 16)
                        .arg(pod_bits);
                     //
                     failed = _point_of_divergence(buffer, output, pod_bytes, pod_bits, 0x2F0 + 0x28);
                     retry  = true;
                     continue;
                  }
               }
            }
         }
         if (failed) {
            results += "\n";
            continue;
         }
         if (!initial_failed) {
            results += QString("%1\n").arg(path);
         }
         if (buffer.size() > output.size()) {
            constexpr auto _signature_fsm = []() {
               halo::util::four_cc out;
               out.bytes[0] = '_';
               out.bytes[1] = 'f';
               out.bytes[2] = 's';
               out.bytes[3] = 'm';
               return out;
            }();

            halo::util::four_cc signature;
            for (int j = 0; j < 4; ++j) {
               signature.bytes[j] = buffer.at(output.size() + j);
            }
            if (signature == _signature_fsm) {
               results += QString(" - FSM chunk missing from output (can happen by design), but no further errors up to this point.\n");
            } else {
               results += QString(" - File content diverged at 0x%2+%3b.\n")
                  .arg(path)
                  .arg(output.size(), 0, 16)
                  .arg(0);
            }
            results += "\n";
            continue;
         }
         results += QString(" - File identical!\n");
         results += "\n";
      }
      //
      QMessageBox::information(parent, "Results", results);
   }
}