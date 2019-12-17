#pragma once
#include "../limits.h"
#include "../opcode_arg.h"
#include "object.h"
#include "player.h"
#include "scalar.h"
#include "team.h"
#include "timer.h"

namespace Megalo {
   enum class OpcodeStringTokenType : int8_t {
      none = -1,
      //
      player, // player's gamertag
      team,   // "team_none" or team designator string
      object, // "none" or "unknown"
      number, // "%i"
      number_with_sign, // "+%i" if positive; "%i" otherwise according to KSoft.Tool source
      timer,  // "%i:%02i:%02i" or "%i:%02i" or "0:%02i" according to KSoft.Tool source
   };
   //
   class OpcodeStringToken {
      public:
         OpcodeStringTokenType type;
         OpcodeArgValue* value = nullptr;
         //
         ~OpcodeStringToken() {
            if (this->value) {
               delete this->value;
               this->value = nullptr;
            }
         }
         //
         bool read(cobb::bitstream& stream) noexcept {
            this->type = (OpcodeStringTokenType)((int8_t)stream.read_bits(3) - 1);
            switch (this->type) {
               case OpcodeStringTokenType::none:
                  break;
               case OpcodeStringTokenType::player:
                  this->value = new OpcodeArgValuePlayer();
                  break;
               case OpcodeStringTokenType::team:
                  this->value = new OpcodeArgValueTeam(); // MSVC is compiling this as new OpcodeArgValuePlayerSet? What the fuck?
                  break;
               case OpcodeStringTokenType::object:
                  this->value = new OpcodeArgValueObject();
                  break;
               case OpcodeStringTokenType::number:
               case OpcodeStringTokenType::number_with_sign:
                  this->value = new OpcodeArgValueScalar(); // MSVC is compiling this as new OpcodeArgValueObject? What the fuck?
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
         void to_string(std::string& out) const noexcept {
            if (this->value)
               this->value->to_string(out);
         }
   };

   template<int N> class OpcodeArgValueStringTokens : OpcodeArgValue {
      //
      // Format specifiers seen:
      //    %n    Prints a game state value (e.g. Round Limit) as a number.
      //
      public:
         int32_t stringIndex = -1; // format string - index in scriptData::strings
         uint8_t tokenCount  =  0;
         OpcodeStringToken tokens[N];
         //
         virtual bool read(cobb::bitstream& stream) noexcept override {
            this->stringIndex = stream.read_bits<uint32_t>(cobb::bitcount(Limits::max_variant_strings - 1)) - 1; // string table index pointer; -1 == none
            this->tokenCount  = stream.read_bits(cobb::bitcount(N));
            if (this->tokenCount > N) {
               printf("Tokens value claimed to have %d tokens; max is %d.\n", this->tokenCount, N);
               return false;
            }
            for (uint8_t i = 0; i < this->tokenCount; i++)
               this->tokens[i].read(stream);
            return true;
         }
         virtual void to_string(std::string& out) const noexcept override {
            if (this->tokenCount == 0) {
               cobb::sprintf(out, "string ID %d", this->stringIndex);
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
         //
         static OpcodeArgValue* factory(cobb::bitstream&) {
            return new OpcodeArgValueStringTokens;
         }
   };
   using OpcodeArgValueStringTokens1 = OpcodeArgValueStringTokens<1>;
   using OpcodeArgValueStringTokens2 = OpcodeArgValueStringTokens<2>;
   using OpcodeArgValueStringTokens3 = OpcodeArgValueStringTokens<3>;
}