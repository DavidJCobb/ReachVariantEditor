#include "tokens.h"
#include "../../../types/multiplayer.h"
#include "variables/all_core.h"
#include "../compiler/compiler.h"

namespace Megalo {
   bool OpcodeStringToken::read(cobb::ibitreader& stream, GameVariantDataMultiplayer& mp) noexcept {
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
            this->value = new OpcodeArgValueScalar();
            break;
         case OpcodeStringTokenType::timer:
            this->value = new OpcodeArgValueTimer();
            break;
         default:
            return false;
      }
      if (this->value) {
         this->value->read(stream, mp);
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
   void OpcodeStringToken::decompile(Decompiler& out, Decompiler::flags_t flags) noexcept {
      if (this->value)
         this->value->decompile(out, flags);
   }
   arg_compile_result OpcodeStringToken::compile(Compiler& compiler, Script::VariableReference& arg, uint8_t part) noexcept {
      this->type = OpcodeStringTokenType::none;
      auto type = arg.get_type();
      if (type == &OpcodeArgValuePlayer::typeinfo) {
         this->type  = OpcodeStringTokenType::player;
         this->value = new OpcodeArgValuePlayer;
      } else if (type == &OpcodeArgValueTeam::typeinfo) {
         this->type  = OpcodeStringTokenType::team;
         this->value = new OpcodeArgValueTeam;
      } else if (type == &OpcodeArgValueObject::typeinfo) {
         this->type  = OpcodeStringTokenType::object;
         this->value = new OpcodeArgValueObject;
      } else if (type == &OpcodeArgValueScalar::typeinfo) {
         this->type  = OpcodeStringTokenType::number;
         this->value = new OpcodeArgValueScalar;
      } else if (type == &OpcodeArgValueTimer::typeinfo) {
         this->type  = OpcodeStringTokenType::timer;
         this->value = new OpcodeArgValueTimer;
      } else
         return arg_compile_result::failure();
      return this->value->compile(compiler, arg, 0);
   }
   void OpcodeStringToken::copy(const OpcodeStringToken& other) noexcept {
      this->type = other.type;
      if (this->value)
         delete this->value;
      if (other.value)
         this->value = other.value->clone();
      else
         this->value = nullptr;
   }
   void OpcodeStringToken::clear() {
      delete this->value;
      this->value = nullptr;
      this->type = OpcodeStringTokenType::none;
   }
   //
   //
   //
   OpcodeArgTypeinfo OpcodeArgValueStringTokens2::typeinfo = OpcodeArgTypeinfo(
      "_format_string",
      "Formatted String",
      "A format string and up to two tokens to insert into it.",
      //
      OpcodeArgTypeinfo::flags::none,
      OpcodeArgTypeinfo::default_factory<OpcodeArgValueStringTokens2>
   );
   //
   bool OpcodeArgValueStringTokens2::read(cobb::ibitreader& stream, GameVariantDataMultiplayer& mp) noexcept {
      MegaloStringIndexOptional index;
      index.read(stream);
      this->string = mp.scriptData.strings.get_entry(index);
      //
      this->tokenCount.read(stream);
      if (this->tokenCount > max_token_count) {
         printf("Tokens value claimed to have %d tokens; max is %d.\n", this->tokenCount.to_int(), max_token_count);
         return false;
      }
      for (uint8_t i = 0; i < this->tokenCount; i++)
         this->tokens[i].read(stream, mp);
      return true;
   }
   void OpcodeArgValueStringTokens2::write(cobb::bitwriter& stream) const noexcept {
      MegaloStringIndexOptional index = -1;
      if (this->string)
         index = this->string->index;
      index.write(stream);
      //
      this->tokenCount.write(stream);
      for (uint8_t i = 0; i < this->tokenCount; i++)
         this->tokens[i].write(stream);
   }
   void OpcodeArgValueStringTokens2::to_string(std::string& out) const noexcept {
      if (this->tokenCount == 0) {
         if (!this->string) {
            out = "no string";
            return;
         }
         cobb::sprintf(out, "localized string ID %d", this->string->index);
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
      cobb::sprintf(out, "format string ID %d and tokens: %s", this->string->index, out.c_str());
   }
   void OpcodeArgValueStringTokens2::decompile(Decompiler& out, Decompiler::flags_t flags) noexcept {
      ReachString* format = this->string;
      if (format) {
         std::string english = format->get_content(reach::language::english);
         out.write_string_literal(english);
      } else {
         out.write("none");
      }
      //
      for (uint8_t i = 0; i < this->tokenCount; ++i) {
         out.write(", ");
         this->tokens[i].decompile(out, flags);
      }
   }
   arg_compile_result OpcodeArgValueStringTokens2::compile(Compiler& compiler, Script::string_scanner& arg_text, uint8_t part) noexcept {
      if (part == 0) {
         //
         // Get the format string.
         //
         QString temp;
         if (arg_text.extract_string_literal(temp)) {
            bool multiple = false;
            this->string = compiler.get_variant().scriptData.strings.lookup(temp, multiple);
            if (multiple)
               return arg_compile_result::failure(QString("The format string provided is an exact match to multiple strings in the table. Unable to figure out which one you want."));
            if (!this->string)
               return arg_compile_result::unresolved_string(temp).set_more(arg_compile_result::more_t::optional);
            return arg_compile_result::success().set_more(arg_compile_result::more_t::optional);
         }
         int32_t index;
         if (!arg_text.extract_integer_literal(index)) {
            auto word = arg_text.extract_word();
            if (word.isEmpty())
               return arg_compile_result::failure();
            auto alias = compiler.lookup_absolute_alias(word);
            if (!alias || !alias->is_integer_constant())
               return arg_compile_result::failure();
            index = alias->get_integer_constant();
         }
         this->string = compiler.get_variant().scriptData.strings.get_entry(index);
         if (!this->string)
            return arg_compile_result::failure(QString("String index %1 does not exist.").arg(index));
         return arg_compile_result::success().set_more(arg_compile_result::more_t::optional);
      }
      //
      // All subsequent arguments are format string parameters, which must be variables.
      //
      auto arg = compiler.arg_to_variable(arg_text);
      if (!arg)
         return arg_compile_result::failure("This argument is not a variable.");
      auto result = this->compile(compiler, *arg, part);
      delete arg;
      return result;
   }
   arg_compile_result OpcodeArgValueStringTokens2::compile(Compiler& compiler, Script::VariableReference& arg, uint8_t part) noexcept {
      if (part == 0 || part > max_token_count)
         return arg_compile_result::failure();
      auto& token = this->tokens[part - 1];
      this->tokenCount = part;
      return token.compile(compiler, arg, part).set_more(part < max_token_count ? arg_compile_result::more_t::optional : arg_compile_result::more_t::no);
   }
   void OpcodeArgValueStringTokens2::copy(const OpcodeArgValue* other) noexcept {
      auto cast = dynamic_cast<const OpcodeArgValueStringTokens2*>(other);
      assert(cast);
      this->string = cast->string;
      this->tokenCount = cast->tokenCount;
      //
      size_t i = 0;
      for (; i < this->tokenCount; i++)
         this->tokens[i].copy(cast->tokens[i]);
      for (; i < max_token_count; i++)
         this->tokens[i].clear();
   }
}