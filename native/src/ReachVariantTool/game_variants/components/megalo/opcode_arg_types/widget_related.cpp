#include "widget_related.h"
#include "../../../types/multiplayer.h"

namespace Megalo {
   bool OpcodeArgValueWidget::read(cobb::ibitreader& stream) noexcept {
      if (stream.read_bits(1) != 0) { // absence bit
         this->index = index_of_none;
         return true;
      }
      this->index = stream.read_bits(cobb::bitcount(max_index - 1));
      return true;
   }
   void OpcodeArgValueWidget::write(cobb::bitwriter& stream) const noexcept {
      if (!this->value) {
         stream.write(1, 1); // absence bit
         return;
      }
      stream.write(0, 1); // absence bit
      stream.write(this->value->index, cobb::bitcount(max_index - 1));
   }
   void OpcodeArgValueWidget::postprocess(GameVariantDataMultiplayer* newlyLoaded) noexcept {
      this->postprocessed = true;
      if (this->index == index_of_none)
         return;
      auto& list = newlyLoaded->scriptContent.widgets;
      if (this->index >= list.size())
         return;
      this->value = list[this->index];
   }
   //
   void OpcodeArgValueWidget::to_string(std::string& out) const noexcept {
      if (!this->postprocessed) {
         if (this->index == index_of_none) {
            out = "no HUD widget";
            return;
         }
         cobb::sprintf(out, "HUD widget %u", this->index);
         return;
      }
      if (!this->value) {
         if (this->index == index_of_none) {
            out = "no HUD widget";
            return;
         }
         cobb::sprintf(out, "invalid HUD widget %u", this->index);
         return;
      }
      HUDWidgetDeclaration* f = this->value;
      cobb::sprintf(out, "HUD widget %u", f->index);
      return;
   }
   //
   //
   //
   bool OpcodeArgValueMeterParameters::read(cobb::ibitreader& stream) noexcept {
      this->type.read(stream);
      switch (this->type) {
         case MeterType::none:
            break;
         case MeterType::timer:
            this->timer.read(stream);
            break;
         case MeterType::number:
            this->numerator.read(stream);
            this->denominator.read(stream);
            break;
         default:
            printf("Widget Meter Parameters had bad type %u.\n", (int)this->type);
            return false;
      }
      return true;
   }
   void OpcodeArgValueMeterParameters::write(cobb::bitwriter& stream) const noexcept {
      this->type.write(stream);
      switch (this->type) {
         case MeterType::none:
            break;
         case MeterType::timer:
            this->timer.write(stream);
            break;
         case MeterType::number:
            this->numerator.write(stream);
            this->denominator.write(stream);
            break;
         default:
            assert(false && "Widget Meter Parameters had a bad type.");
      }
   }
   void OpcodeArgValueMeterParameters::to_string(std::string& out) const noexcept {
      std::string temp;
      switch (this->type) {
         case MeterType::none:
            out = "no meter";
            return;
         case MeterType::timer:
            this->timer.to_string(out);
            out = "timer " + out;
            return;
         case MeterType::number:
            this->numerator.to_string(out);
            this->denominator.to_string(temp);
            cobb::sprintf(out, "%s / %s", out.c_str(), temp.c_str());
            return;
      }
   }
}