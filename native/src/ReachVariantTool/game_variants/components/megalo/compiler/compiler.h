#pragma once
#include <functional>
#include <optional>
#include <vector>
#include <QMultiMap>
#include <QString>
#include "../../../../helpers/string_scanner.h"
#include "../opcode_arg_type_registry.h"
#include "../trigger.h"
#include "../variable_declarations.h"
#include "parsing/base.h"
#include "parsing/alias.h"
#include "parsing/variable_reference.h"
#include "../../../types/multiplayer.h"
#include "variable_usage_set.h"

namespace Megalo {
   class OpcodeArgValueMegaloScope;
   class Compiler;
   //
   namespace Script {
      class Enum;
      class Namespace;
      //
      class Comparison;
      class Block : public ParsedItem {
         public:
            enum class Type : uint8_t {
               basic, // do
               root,
               if_block,
               altif_block, // like elseif, but not mutually exclusive with its previous-sibling branch
               alt_block,   // like else,   but not mutually exclusive with its previous-sibling branch
               for_each_object,
               for_each_object_with_label,
               for_each_player,
               for_each_player_randomly,
               for_each_team,
               function
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
               __error, // used when an event name is already taken, so we can catch when a duplicate "on" declaration hits EOF without a block after it. e.g. "on foo: do end   on foo:"
            };
            //
         public:
            OpcodeArgValueMegaloScope* inlined_trigger = nullptr;
            Trigger* trigger = nullptr;
            Trigger* tr_wrap = nullptr; // wrapper trigger; use to fix (on event: for ... do end) so that it compiles properly (since the game doesn't allow event types on loop triggers)
            QString  name; // only for functions
            bool     has_inline_specifier = false;
            QString  label_name;
            int32_t  label_index  = -1;
            size_t   caller_count = 0; // for functions
            Type     type  = Type::basic;
            Event    event = Event::none;
            std::vector<ParsedItem*> items; // contents are owned by this Block and deleted in the destructor
            std::vector<Comparison*> conditions_alt; // only for alt(if) blocks // contents are owned by this Block and deleted in the destructor
            std::vector<Comparison*> conditions; // only for if blocks // contents are owned by this Block and deleted in the destructor

            Block* get_nearest_function() {
               if (this->type == Type::function)
                  return this;
               auto* bp = dynamic_cast<Block*>(this->parent);
               if (!bp)
                  return nullptr;
               return bp->get_nearest_function();
            }
            
            ~Block();
            void insert_condition(Comparison*);
            void insert_item(ParsedItem*);
            ParsedItem* item(int32_t); // negative indices wrap around, i.e. -1 is the last element
            void remove_item(ParsedItem*);
            int32_t index_of_item(const ParsedItem*) const noexcept;
            //
            void get_ifs_for_alt(std::vector<Block*>& out) const;
            void make_alt_of(const Block& other);
            //
            void clear(bool deleting = false);
            void compile(Compiler&);
            //
            void for_each_function(std::function<void(Block*)>);
            //
            constexpr bool is_event_trigger() const noexcept { return this->event != Event::none; }
            constexpr bool is_inline_trigger() const noexcept {
               if (!this->has_inline_specifier)
                  return false;
               if (this->event != Event::none)
                  return false;
               if (this->parent == nullptr) {
                  //
                  // Top-level triggers cannot be inlined. An inlined trigger is an action; 
                  // actions must exist somewhere inside of a non-inlined trigger (including 
                  // inside of an inlined descendant trigger).
                  //
                  return false;
               }
               switch (this->type) {
                  case Type::basic:
                  case Type::if_block:
                     return true;
                  case Type::altif_block:
                  case Type::alt_block:
                     /*//
                     {
                        std::vector<Block*> ifs;
                        this->get_ifs_for_alt(ifs);
                        if (ifs.empty())
                           break;
                        auto* base = ifs.back();
                        assert(base != nullptr);
                        assert(base->type == Type::if_block);
                        if (base->is_inline_trigger())
                           return true;
                     }
                     return false;
                     //*/
                     return true;
               }
               return false;
            }
            
         protected:
            void _get_effective_items(std::vector<ParsedItem*>& out, bool include_functions = true); // returns the list of items with only Statements and Blocks, i.e. only things that generate compiled output
            bool _is_if_block() const noexcept;
            void _make_trigger(Compiler&);
            void _setup_trigger_forge_label(Compiler&);
      };
      class Statement : public ParsedItem {
         public:
            Opcode* opcode = nullptr; // a fully-compiled opcode; can be any condition or action, including "run nested trigger" for calls to user-defined functions // owned by this Statement and deleted in the destructor
            VariableReference* lhs = nullptr; // owned by this Statement and deleted in the destructor
            VariableReference* rhs = nullptr; // owned by this Statement and deleted in the destructor
            //
            ~Statement();
      };
      class Comparison : public Statement {
         public:
            bool next_is_or = false;
            //
            Comparison* clone() const noexcept; // doesn't clone (lhs) or (rhs)
            void negate() noexcept;
      };
      class UserDefinedEnum {
         public:
            Enum*  definition = nullptr; // owned by this object
            Block* parent     = nullptr; // used so we can tell when the enum goes out of scope
            //
            UserDefinedEnum(Enum* d, Block* p) : definition(d), parent(p) {}
            ~UserDefinedEnum();
            //
            UserDefinedEnum(UserDefinedEnum&&);
            UserDefinedEnum& operator=(const UserDefinedEnum&) noexcept;
            UserDefinedEnum& operator=(UserDefinedEnum&&) noexcept;
      };
      class UserDefinedFunction {
         //
         // This struct maps the name of a user-defined function to its index in the trigger list, and to its block. 
         // It is used to keep track of which user-defined functions are currently in-scope; the trigger index is 
         // used to compile calls to these functions, while the Block pointer is used so that we can detect when 
         // the function's containing block (i.e. this->block->parent) is closed (such that we can know when the 
         // function goes out of scope).
         //
         public:
            QString name;
            int32_t trigger_index = -1;
            Block*  content = nullptr;
            Block*  parent  = nullptr; // used so we can tell when the function goes out of scope

            std::vector<const Alias*> descendant_aliases; // used for allocation
            
            UserDefinedFunction() {}
            UserDefinedFunction(const QString& n, int32_t ti, Block& b) : name(n), trigger_index(ti), content(&b), parent(dynamic_cast<Block*>(b.parent)) {}
      };
   }
   //
   class Compiler : public cobb::string_scanner {
      public:
         struct LogEntry {
            QString text;
            pos     pos;
            //
            LogEntry() {}
            LogEntry(const QString& t, cobb::string_scanner::pos p) : text(t), pos(p) {}
         };
         using log_t = std::vector<LogEntry>;
         struct log_checkpoint {
            size_t notices      = 0;
            size_t warnings     = 0;
            size_t errors       = 0;
            size_t fatal_errors = 0;
         };
         
         enum class unresolved_string_pending_action {
            create,
            use_existing,
         };
         class unresolved_str {
            friend Compiler;
            //
            // Some function arguments take a string. We want script authors to be able to specify strings as 
            // string literals, as integer literals (referring to indices in the string table), or as aliases 
            // of integer literals. However, what do we do if the script author specifies a non-existent 
            // string? If they used a bad index, then we should just fail compiling with an error. However, 
            // if they used a string literal that doesn't match any English-language strings in the variant, 
            // or if they used a string literal that matches multiple identical English-language strings in 
            // the variant, then we should treat it as an "unresolved string reference."
            //
            // Unresolved string references will be remembered throughout the compile process. When the script 
            // is fully compiled, the UI will allow the script author to choose how to handle these -- whether 
            // we should create the new string in the string table, or use an existing string. (The author can 
            // also just abort compiling.) Once the user has chosen an action, we can properly compile the 
            // affected OpcodeArgValue by stringifying the index of the chosen string, and repeating the 
            // original OpcodeArgValue::compile call with that string as the "argument text."
            //
            // Crucially, (unresolved_str) does not own its pointer to the OpcodeArgValue. This means that we 
            // can only add unresolved string references to the compiler's list when we're sure that their 
            // containing Opcodes will be retained (i.e. no fatal errors or anything else that would lead to 
            // the opcode being discarded). In practice, string values only make sense as function call args, 
            // so we only need to handle this there: we create a temporary (unresolved_str_list) to hold any 
            // unresolved string references encountered when parsing a function call's arguments, and when 
            // we're sure that we're going to retain the Opcode for that function call, we commit this temp-
            // orary list to the compiler's main list. (Conveniently, this also allows us to make this system 
            // work with the way we resolve function overloads.)
            //
            protected:
               OpcodeArgValue* value = nullptr;
               uint8_t part = 0;
               bool    handled = false;
               //
            public:
               using action = unresolved_string_pending_action;
               //
               struct {
                  unresolved_string_pending_action action = unresolved_string_pending_action::create;
                  uint8_t index = 0;
               } pending;
               //
               unresolved_str(OpcodeArgValue& v, uint8_t p) : value(&v), part(p) {}
         };
         using unresolved_str_list = QMultiMap<QString, unresolved_str>;
         
      protected:
         using keyword_handler_t = void (Compiler::*)(const pos);
         
      protected:
         enum class c_joiner {
            none,
            c_and,
            c_or,
         };

         struct UnresolvedTriggerForgeLabel {
            Trigger* trigger = nullptr;
            QString  label_name;
         };

         struct UnresolvedOpcodeForgeLabel {
            struct argument {
               size_t  argument_index;
               QString label_name;
            };

            Opcode* opcode = nullptr;
            std::vector<argument> labels;
         };
         
         Script::Block* root  = nullptr; // Compiler has ownership of all Blocks, Statements, etc., and will delete them when it is destroyed.
         Script::Block* block = nullptr; // current block being parsed
         Script::Block::Event next_event = Script::Block::Event::none;
         bool     inline_next_block     = false;
         bool     negate_next_condition = false;
         c_joiner next_condition_joiner = c_joiner::none;
         std::vector<Script::Alias*> aliases_in_scope; // unowned pointers; the aliases are owned by their containing Blocks
         std::vector<Script::UserDefinedEnum> enums_in_scope;
         std::vector<Script::UserDefinedFunction> functions_in_scope;
         std::vector<Script::Block*> already_compiled_blocks;
         GameVariantDataMultiplayer& variant;
         //
         log_t notices;
         log_t warnings;
         log_t errors;
         log_t fatal_errors;
         //
         std::vector<UnresolvedTriggerForgeLabel> triggers_pending_forge_labels;
         std::vector<UnresolvedOpcodeForgeLabel>  opcodes_pending_forge_labels;
         
         void _commit_unresolved_strings(unresolved_str_list&);
         
      public:
         struct {
            bool success = false;
            std::vector<Trigger*> triggers; // owned by the Compiler UNLESS (results.success) is true
            TriggerEntryPoints    events;
            unresolved_str_list   unresolved_strings;
            struct {
               VariableDeclarationSet global = VariableDeclarationSet(Megalo::variable_scope::global);
               VariableDeclarationSet player = VariableDeclarationSet(Megalo::variable_scope::player);
               VariableDeclarationSet object = VariableDeclarationSet(Megalo::variable_scope::object);
               VariableDeclarationSet team   = VariableDeclarationSet(Megalo::variable_scope::team);
            } variables;
         } results;
         //
         Compiler(GameVariantDataMultiplayer& mp) : string_scanner(""), variant(mp) {}
         ~Compiler();
         //
         inline const GameVariantDataMultiplayer& get_variant() const noexcept { return this->variant; }
         //
         void raise_error(const QString& text);
         void raise_error(const pos& pos, const QString& text);
         void raise_fatal(const QString& text);
         void raise_fatal(const pos& pos, const QString& text);
         void raise_warning(const QString& text);
         void raise_warning(const pos& pos, const QString& text);
         void raise_notice(const QString& text);
         void raise_notice(const pos& pos, const QString& text);
         void validate_format_string_tokens(const QString&);
         //
         void parse(QString text); // parse and compile the text
         void apply(); // applies compiled content to the game variant, and relinquishes ownership of it
         bool handle_unresolved_string_references(); // handles any strings with an action set; returns success bool (failure if any unresolved remain)
         inline unresolved_str_list& get_unresolved_string_references() noexcept { return this->results.unresolved_strings; }
         //
         QVector<QString> get_new_forge_label_names() const;
         bool new_forge_label_not_yet_tracked(const QString& name) const;
         
         inline bool has_errors() const noexcept { return !this->errors.empty() || !this->fatal_errors.empty(); }
         inline bool has_fatal() const noexcept { return !this->fatal_errors.empty(); }
         inline const log_t& get_notices() const noexcept { return this->notices; }
         inline const log_t& get_warnings() const noexcept { return this->warnings; }
         inline const log_t& get_non_fatal_errors() const noexcept { return this->errors; }
         inline const log_t& get_fatal_errors() const noexcept { return this->fatal_errors; }
         
         [[nodiscard]] static bool is_keyword(QString word);
         [[nodiscard]] bool try_decode_enum_reference(QString word, int32_t& out) const;
         [[nodiscard]] bool try_get_integer(string_scanner&, int32_t& out) const;
         [[nodiscard]] arg_compile_result try_get_integer_or_word(
            string_scanner&,
            int32_t& out_int,
            QString& out_name,
            QString thing_getting,
            OpcodeArgTypeinfo* word_must_be_imported_from = nullptr,
            int32_t limit_int = -1,
            OpcodeArgTypeinfo* prefer_imported_names_when_leading_integers = nullptr // HACK for incident names that begin with numbers
         ) const;
         //
         [[nodiscard]] Script::Alias* lookup_relative_alias(QString name, const OpcodeArgTypeinfo* relative_to) const;
         [[nodiscard]] Script::Alias* lookup_absolute_alias(QString name) const;
         [[nodiscard]] Script::UserDefinedEnum* lookup_user_defined_enum(QString name) const;
         [[nodiscard]] Script::UserDefinedFunction* lookup_user_defined_function(QString name) const;
         
         log_checkpoint create_log_checkpoint();
         void revert_to_log_checkpoint(log_checkpoint);
         bool checkpoint_has_errors(log_checkpoint) const noexcept;
         
         Script::VariableReference* arg_to_variable(QString) noexcept; // caller is responsible for freeing the returned variable
         Script::VariableReference* arg_to_variable(string_scanner& arg) noexcept; // caller is responsible for freeing the returned variable // runs (arg.extract_word)
         void imply_variable(variable_scope, variable_type, uint8_t index) noexcept; // for OpcodeArgValueObjectPlayerVariable
         
         enum class name_source {
            none,
            action,
            condition,
            imported_name,
            namespace_name,
            namespace_member,
            static_typename,
            variable_typename,
         };
         [[nodiscard]] static name_source check_name_is_taken(const QString& name, OpcodeArgTypeRegistry::type_list_t& name_is_imported_from);
         
         int32_t _index_of_trigger(const Trigger&) const noexcept; // is public for Block
         void _trigger_needs_forge_label(Trigger&, QString name); // is public for Block
         
      protected:
         VariableDeclarationSet* _get_variable_declaration_set(variable_scope) noexcept;
         
         struct statement_side {
            statement_side() = delete;
            enum type : int {
               none,
               integer,
               string,
               word,
            };
         };
         using statement_side_t = statement_side::type;
         statement_side_t _extract_statement_side(QString& out_str, int32_t& out_int);
         //
         static keyword_handler_t __get_handler_for_keyword(QString) noexcept;

         Action* __compile_assignment_intrinsic(Script::VariableReference& lhs, Script::VariableReference& rhs, const QString& op, bool checked = true);
         
         Script::VariableReference* __parseActionRHS(QString& op, const pos& prior, Script::VariableReference* lhs, bool allow_abs_hack);
         void _parseAction();
         bool _parseCondition(); // return value = stop looking for more conditions
         bool __parseConditionEnding();
         //
         void _parseBlockConditions();
         //
         void _applyConditionModifiers(Script::Comparison*); // applies "not", "and", "or", and then resets the relevant state on the Compiler
         Script::VariableReference* __parseVariable(QString, bool is_alias_definition = false, bool is_write_access = false); // adds the variable to the appropriate VariableDeclarationSet as appropriate
         //
         void __parseFunctionArgs(const OpcodeBase&, Opcode&, unresolved_str_list&, UnresolvedOpcodeForgeLabel&);
         Script::Statement* _parseFunctionCall(const pos& pos, QString stem, bool is_condition, Script::VariableReference* assign_to = nullptr);
         
         void _openBlock(Script::Block*);
         bool _closeCurrentBlock();
         //
         struct extract_int_result {
            extract_int_result() = delete;
            enum type : int {
               failure,
               success,
               floating_point,  // extracted an integer literal, but it ended with a decimal point
            };
         };
         extract_int_result::type extract_integer_literal_detailed(int32_t& out);
         bool extract_integer_literal(int32_t& out);
         //
         QString extract_operator();
         //
         #pragma region Alias declaration helpers
            struct _allocation_request_type {
               const Script::Namespace* context_namespace = nullptr;
               const OpcodeArgTypeinfo* context_type_info = nullptr;
               const OpcodeArgTypeinfo* member_type_info  = nullptr;

               bool    parse_or_fail(Compiler& compiler, const QString& member_type);
               size_t  max_slots_available() const;
               bool    existing_alias_matches(const Script::Alias& existing) const;
               QString to_alias_target(size_t index) const;
            };
            Script::Alias* _allocate_alias(QString name, const _allocation_request_type&);
            const Script::Alias* _alias_is_explicit_reference_to_allocated(const Script::Alias& new_alias);
            void _store_new_alias(Script::Alias&);
         #pragma endregion
         #pragma region Alt(if) helpers
            void _set_up_branch_conditions_for_alt(Script::Block& alt, bool is_top_level);
         #pragma endregion
         #pragma region Variable declaration helpers
            void _declare_variable(Script::VariableReference& variable, Script::VariableReference* initial, VariableDeclaration::network_enum networking, bool network_specified);
         #pragma endregion
         //
         void _handleKeyword_Alias(const pos start);
         void _handleKeyword_Alt(const pos start);
         void _handleKeyword_AltIf(const pos start);
         void _handleKeyword_Declare(const pos start); // INCOMPLETE
         void _handleKeyword_Do(const pos start);
         void _handleKeyword_End(const pos start);
         void _handleKeyword_Enum(const pos start);
         void _handleKeyword_If(const pos start);
         void _handleKeyword_Inline(const pos start);
         void _handleKeyword_For(const pos start);
         void _handleKeyword_Function(const pos start);
         void _handleKeyword_On(const pos start);
   };
}