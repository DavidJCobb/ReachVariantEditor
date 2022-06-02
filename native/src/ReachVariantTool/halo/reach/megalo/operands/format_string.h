#pragma once
#include <memory>
#include "halo/reach/bitstreams.fwd.h"
#include "../operand_typeinfo.h"
#include "../operand.h"

#include "../strings.h"
#include "./variables/base.h"

namespace halo::reach::megalo::operands {
   class format_string : public operand {
      public:
         static constexpr operand_typeinfo typeinfo = {
            .internal_name = "format_string",
         };

      public:
         static constexpr size_t max_token_count = 2;
         enum class token_type {
            none = -1,
            //
            player, // player's gamertag
            team,   // "team_none" or team designator string
            object, // "none" or "unknown"
            number, // "%i"
            // halo 4 offers "number_with_sign" here i.e. +5 or -5
            timer,  // "%i:%02i:%02i" or "%i:%02i" or "0:%02i" according to KSoft source
         };
         using token_type_bitnumber = bitnumber<
            3,
            token_type,
            bitnumber_params<token_type>{
               .offset = 1,
            }
         >;

         struct token {
            std::unique_ptr<variables::unknown_type> variable;

            void read(bitreader& stream);
         };

      public:
         string_ref_optional string;
         bitnumber<std::bit_width(max_token_count), uint8_t> token_count;
         std::array<token, max_token_count> tokens;

         virtual void read(bitreader& stream) override;
   };
}
