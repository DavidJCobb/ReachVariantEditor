#pragma once
#include "../../../../formats/localized_string_table.h"
#include "../../../../helpers/bitnumber.h"
#include "../limits.h"
#include "../opcode_arg.h"

namespace Megalo {
   enum class OpcodeStringTokenType : int8_t {
      none = -1,
      //
      player, // player's gamertag
      team,   // "team_none" or team designator string
      object, // "none" or "unknown"
      number, // "%i"
      // number_with_sign, // Halo 4 only // "+%i" if positive; "%i" otherwise according to KSoft.Tool source
      timer,  // "%i:%02i:%02i" or "%i:%02i" or "0:%02i" according to KSoft.Tool source
   };
   //
   class OpcodeStringToken {
      public:
         cobb::bitnumber<3, OpcodeStringTokenType, true> type = OpcodeStringTokenType::none;
         OpcodeArgValue* value = nullptr;
         //
         ~OpcodeStringToken() {
            if (this->value) {
               delete this->value;
               this->value = nullptr;
            }
         }
         //
         bool read(cobb::ibitreader& stream) noexcept;
         void write(cobb::bitwriter& stream) const noexcept;
         void to_string(std::string& out) const noexcept;
   };

   class OpcodeArgValueStringTokens2 : OpcodeArgValue {
      //
      // An opcode argument which consists of a format string and zero or more tokens to 
      // insert into it. The format string is specified as an index into the string 
      // table, and that index can contain printf-style format codes (though I don't 
      // know that the game uses printf directly as opposed to a custom format inspired 
      // by it).
      //
      // Format specifiers seen:
      //    %n    Prints a game state value (e.g. Round Limit) as a number.
      //    %s    Not yet known. "Safe Haven - %s"
      //
      // TODO: Does the game do a raw printf? If so, can we cause crashes using bad 
      //       parameters? If so, any script editor will need to validate parameters.
      //
      public:
         static OpcodeArgTypeinfo typeinfo;
         static OpcodeArgValue* factory(cobb::ibitreader&) {
            return new OpcodeArgValueStringTokens2;
         }
         //
      public:
         static constexpr int max_token_count = 2;
      public:
         MegaloStringIndexOptional stringIndex = -1; // format string - index in scriptData::strings
         MegaloStringRef           string      = MegaloStringRef::make(*this);
         cobb::bitnumber<cobb::bitcount(max_token_count), uint8_t> tokenCount = 0;
         OpcodeStringToken tokens[max_token_count];
         //
         virtual bool read(cobb::ibitreader& stream) noexcept override;
         virtual void postprocess(GameVariantDataMultiplayer* newlyLoaded) noexcept override;
         virtual void write(cobb::bitwriter& stream) const noexcept override;
         virtual void to_string(std::string& out) const noexcept override;
   };
}