#pragma once
#include <functional>
#include <stdexcept>
#include <vector>
#include <QString>
#include "../trigger.h"

namespace Megalo {
   class Compiler;
   //
   namespace Script {
      struct ParserPosition {
         int32_t pos  = -1; // position in the script
         int32_t line = -1; // current line number
         int32_t last_newline = -1; // index of last newline in the script
      };
      //
      class VariableReference;
      //
      class ParsedItem {
         public:
            virtual ~ParsedItem() {} // need virtual methods to allow dynamic_cast
            //
            ParsedItem* parent = nullptr;
            ParsedItem* owner  = nullptr; // for conditions
            int32_t line = -1;
            int32_t col  = -1;
            struct {
               int32_t start = -1;
               int32_t end   = -1;
            } range;
            //
            void set_start(ParserPosition&);
            void set_end(ParserPosition&);
      };
      class ParsedOpcode { // used for once we've fully parsed an opcode, so we can retain it in a Block
         public:
            Opcode* data = nullptr;
      };
      class Alias : public ParsedItem {
         public:
            QString name;
            VariableReference* target = nullptr;
            //
            Alias(Compiler&, QString name, QString target);
      };
      class Block : public ParsedItem {
         public:
            enum class Type : uint8_t {
               basic, // do
               root,
               if_block,
               elseif_block,
               else_block,
               for_each_object,
               for_each_object_with_label,
               for_each_player,
               for_each_player_randomly,
               for_each_team,
               function,
            };
            enum class Event : uint8_t {
               none,
               init,
               local_init,
               host_migration,
               double_host_migration,
               object_death,
               local,
               pregame,
            };
         public:
            Trigger* trigger  = nullptr;
            QString  name; // only for functions
            QString  label_name;
            int32_t  label_index = -1;
            Type     type;
            Event    event;
            std::vector<ParsedItem*> items;
            std::vector<ParsedItem*> conditions; // only for if/elseif blocks
            //
            void insert_item(ParsedItem*);
            ParsedItem* item(int32_t); // negative indices wrap around, i.e. -1 is the last element
      };
      class StringLiteral : public ParsedItem {
         public:
            QString value;
      };
      class VariableReference : public ParsedItem {
         public:
            QString content;
            #if !_DEBUG
               static_assert(false, "replace (content) with logic to actuall parse parts");
            #endif
            //
            class Part {};
            //
            std::vector<Part> parts;
            int16_t constant = 0;
            int8_t  type  = -1; // TODO
            int8_t  scope = -1;
            int8_t  which = -1;
            int8_t  index = -1;
            bool    is_read_only = false;
            //
            VariableReference(QString);
            VariableReference(int32_t);
      };
      class FunctionCall : public ParsedItem {
         public:
            VariableReference* context = nullptr;
            QString name;
            std::vector<std::string> args;
      };
      class Assignment : public ParsedItem {
         public:
            VariableReference* target = nullptr; // lhs
            ParsedItem* source = nullptr; // rhs
            QString op;
      };
      class Comparison : public ParsedItem {
         public:
            VariableReference* lhs = nullptr;
            VariableReference* rhs = nullptr;
            QString op;
            bool negated = false;
      };
   }
   //
   class compile_exception : public std::exception {
      private:
         QString reason;
      public:
         compile_exception(const QString& reason) : std::exception(""), reason(reason) {} // TODO: try using a QString instead so we can support Unicode and so our code is cleaner
         //
         [[nodiscard]] virtual char const* what() const {
            return "this is a QString";
         }
         const QChar* why() const noexcept { return reason.constData(); }
   };
   class Compiler {
      public:
         struct Token {
            QString text;
            Script::ParserPosition pos;
            bool ended = false; // whether we hit whitspace, meaning that the current word is "over"
            bool brace = false; // whether we're in square brackets; needed to properly handle constructs like "abc[ d]" or "abc[-1]" at the starts of statements
         };
         using scan_functor_t = std::function<bool(QChar)>;
         //
      public:
         QString text;
         Script::ParserPosition state;
         Script::Block*         root       = nullptr;
         Script::Block*         block      = nullptr; // current block being parsed
         Script::Assignment*    assignment = nullptr; // current assignment being parsed, if any
         Script::Comparison*    comparison = nullptr; // current comparison being parsed, if any
         Script::FunctionCall*  call       = nullptr; // current call being parsed, if any
         Token token;
         Script::Block::Event next_event = Script::Block::Event::none;
         bool negate_next_condition = false;
         //
         void throw_error(const QString& text); // TODO: try using a QString instead so we can support Unicode and so our code is cleaner
         Script::ParserPosition backup_stream_state();
         void restore_stream_state(Script::ParserPosition&);
         void reset_token();
         //
         void scan(scan_functor_t);
         //
         void parse(QString text); // can throw compile_exception
         //
      protected:
         bool is_in_statement() const;
         //
         void _parseActionStart(QChar);
         void _parseAssignment(QChar);
         //
         void _parseBlockConditions();
         bool _parseConditionStart(QChar); // returns "true" at the end of the condition list, i.e. upon reaching the keywrod "then"
         void _parseComparison(QChar);
         //
         void _parseFunctionCallArg();
         void _parseFunctionCall(bool is_condition);
         //
         bool extractIntegerLiteral(int32_t& out);
         bool extractSpecificChar(QChar which); // advances the stream past the char if the char is found. NOTE: cannot be used to search for whitespace
         bool extractStringLiteral(QString& out); // advances the stream past the end-quote if a string literal is found
         QString extractWord(); // advances the stream to the end of the found word, or to the next non-word and non-whitespace character
         bool    extractWord(QString desired); // advances the stream to the end of the desired word only if it is found; no advancement otherwise
         //
         bool _closeCurrentBlock();
         //
         void _handleKeyword_Alias();
         void _handleKeyword_Declare();
         void _handleKeyword_Do();
         void _handleKeyword_Else();
         void _handleKeyword_ElseIf();
         void _handleKeyword_End();
         void _handleKeyword_If();
         void _handleKeyword_For();
         void _handleKeyword_Function();
         void _handleKeyword_On();
   };
}