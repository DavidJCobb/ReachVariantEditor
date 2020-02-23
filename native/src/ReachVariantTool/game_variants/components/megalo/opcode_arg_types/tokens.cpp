#include "tokens.h"
#include "../../../types/multiplayer.h"
#include "object.h"
#include "player.h"
#include "scalar.h"
#include "team.h"
#include "timer.h"

namespace Megalo {
   bool OpcodeStringToken::read(cobb::ibitreader& stream) noexcept {
      this->type.read(stream);
      switch (this->type) {
         case OpcodeStringTokenType::none:
            break;
         case OpcodeStringTokenType::player:
            this->value = new OpcodeArgValuePlayer();
            break;
         case OpcodeStringTokenType::team:
            this->value = new OpcodeArgValueTeam();
            break;
         case OpcodeStringTokenType::object:
            this->value = new OpcodeArgValueObject();
            break;
         case OpcodeStringTokenType::number:
         //case OpcodeStringTokenType::number_with_sign:
            this->value = new OpcodeArgValueScalar();
            break;
         case OpcodeStringTokenType::timer:
            this->value = new OpcodeArgValueTimer();
            break;
         default:
            return false;
      }
      if (this->value) {
         this->value->read(stream);
      }
      return true;
   }
   void OpcodeStringToken::write(cobb::bitwriter& stream) const noexcept {
      this->type.write(stream);
      if (this->value)
         this->value->write(stream);
   }
   void OpcodeStringToken::to_string(std::string& out) const noexcept {
      if (this->value)
         this->value->to_string(out);
   }
   //
   //
   //
   OpcodeArgTypeinfo OpcodeArgValueStringTokens2::typeinfo = OpcodeArgTypeinfo(
      OpcodeArgTypeinfo::typeinfo_type::default,
      0,
      &OpcodeArgValueStringTokens2::factory
   );
   //
   bool OpcodeArgValueStringTokens2::read(cobb::ibitreader& stream) noexcept {
      this->stringIndex.read(stream); // string table index pointer; -1 == none
      this->tokenCount.read(stream);
      if (this->tokenCount > max_token_count) {
         printf("Tokens value claimed to have %d tokens; max is %d.\n", this->tokenCount.to_int(), max_token_count);
         return false;
      }
      for (uint8_t i = 0; i < this->tokenCount; i++)
         this->tokens[i].read(stream);
      return true;
   }
   void OpcodeArgValueStringTokens2::postprocess(GameVariantDataMultiplayer* newlyLoaded) noexcept {
      if (this->stringIndex >= 0) {
         auto& list = newlyLoaded->scriptData.strings;
         if (this->stringIndex < list.size())
            this->string = list.get_entry(this->stringIndex);
      }
      for (uint8_t i = 0; i < this->tokenCount; i++)
         this->tokens[i].value->postprocess(newlyLoaded);
   }
   void OpcodeArgValueStringTokens2::write(cobb::bitwriter& stream) const noexcept {
      if (this->string) {
         this->stringIndex = this->string->index;
      }
      this->stringIndex.write(stream);
      this->tokenCount.write(stream);
      for (uint8_t i = 0; i < this->tokenCount; i++)
         this->tokens[i].write(stream);
   }
   void OpcodeArgValueStringTokens2::to_string(std::string& out) const noexcept {
      if (this->tokenCount == 0) {
         if (this->stringIndex == -1) {
            out = "no string";
            return;
         }
         cobb::sprintf(out, "localized string ID %d", this->stringIndex);
         return;
      }
      out.clear();
      for (uint8_t i = 0; i < this->tokenCount; i++) {
         std::string temp;
         //
         auto& token = this->tokens[i];
         token.to_string(temp);
         //
         if (!out.empty())
            out += ", ";
         out += temp;
      }
      cobb::sprintf(out, "format string ID %d and tokens: %s", this->stringIndex, out.c_str());
   }
}