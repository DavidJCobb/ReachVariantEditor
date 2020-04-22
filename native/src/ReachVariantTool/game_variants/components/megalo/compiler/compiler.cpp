#include "compiler.h"
#include "namespaces.h"
#include "../../../helpers/qt/string.h"
#include "../opcode_arg_types/all_indices.h" // OpcodeArgValueTrigger
#include "../opcode_arg_types/variables/all_core.h"
#include "../opcode_arg_types/variables/any_variable.h"
#include "../opcode_arg_types/variables/base.h"

namespace {
   constexpr char* ce_assignment_operator = "=";
   bool _is_assignment_operator(QString s) {
      constexpr char* operators[] = {
         "=",
         "+=",
         "-=",
         "*=",
         "/=",
         "%=",
         ">>=",
         "<<=",
         ">>>=",
         "~=",
         "^=",
         "&=",
         "|="
      };
      for (size_t i = 0; i < std::extent<decltype(operators)>::value; i++)
         if (s == operators[i])
            return true;
      return false;
   }
   bool _is_comparison_operator(QString s) {
      constexpr char* operators[] = {
         "==",
         "!=",
         ">=",
         "<=",
         ">",
         "<"
      };
      for (size_t i = 0; i < std::extent<decltype(operators)>::value; i++)
         if (s == operators[i])
            return true;
      return false;
   }
   const Megalo::ActionFunction& _get_assignment_opcode() {
      using namespace Megalo;
      static const ActionFunction* result = nullptr;
      if (result)
         return *result;
      //
      for (auto& opcode : actionFunctionList) {
         auto& mapping = opcode.mapping;
         if (mapping.type == OpcodeFuncToScriptMapping::mapping_type::assign) {
            result = &opcode;
            return *result;
         }
      }
      assert(false && "Where is the assignment opcode?");
      __assume(0); // tell MSVC this is unreachable
   }
   const Megalo::ConditionFunction& _get_comparison_opcode() {
      using namespace Megalo;
      static const ConditionFunction* result = nullptr;
      if (result)
         return *result;
      //
      for (auto& opcode : conditionFunctionList) {
         auto& mapping = opcode.mapping;
         if (mapping.type == OpcodeFuncToScriptMapping::mapping_type::compare) {
            result = &opcode;
            return *result;
         }
      }
      assert(false && "Where is the comparison opcode?");
      __assume(0); // tell MSVC this is unreachable
   }
   //
   Megalo::block_type _block_type_to_trigger_type(Megalo::Script::Block::Type type) {
      using namespace Megalo;
      using namespace Megalo::Script;
      switch (type) {
         case Block::Type::basic:
         case Block::Type::if_block:
         case Block::Type::elseif_block:
         case Block::Type::else_block:
            break;
         case Block::Type::for_each_object:
            return block_type::for_each_object;
            break;
         case Block::Type::for_each_object_with_label:
            return block_type::for_each_object_with_label;
            break;
         case Block::Type::for_each_player:
            return block_type::for_each_player;
            break;
         case Block::Type::for_each_player_randomly:
            return block_type::for_each_player_randomly;
            break;
         case Block::Type::for_each_team:
            return block_type::for_each_team;
            break;
      }
      return block_type::normal;
   }
   Megalo::entry_type _block_event_to_trigger_entry(Megalo::Script::Block::Event type) {
      using namespace Megalo;
      using namespace Megalo::Script;
      switch (type) {
         case Block::Event::double_host_migration:
         case Block::Event::host_migration:
            return Megalo::entry_type::on_host_migration;
         case Block::Event::init:
            return Megalo::entry_type::on_init;
         case Block::Event::local:
            return Megalo::entry_type::local;
         case Block::Event::local_init:
            return Megalo::entry_type::on_local_init;
         case Block::Event::object_death:
            return Megalo::entry_type::on_object_death;
         case Block::Event::pregame:
            return Megalo::entry_type::pregame;
      }
      return entry_type::normal;
   }
}
namespace Megalo {
   namespace Script {
      Block::~Block() {
         this->clear();
      }
      void Block::insert_condition(ParsedItem* item) {
         this->conditions.push_back(item);
         item->owner = this;
      }
      void Block::insert_item(ParsedItem* item) {
         this->items.push_back(item);
         item->parent = this;
      }
      ParsedItem* Block::item(int32_t i) {
         if (i < 0) {
            i = this->items.size() + i;
            if (i < 0)
               return nullptr;
         } else if (i >= this->items.size())
            return nullptr;
         return this->items[i];
      }
      void Block::remove_item(ParsedItem* item) {
         int i;
         int size = this->items.size();
         for (i = size - 1; i >= 0; --i) {
            if (this->items[i] == item) {
               this->items.erase(this->items.begin() + i);
               item->parent = nullptr;
               return;
            }
         }
      }
      //
      void Block::clear() {
         for (auto item : this->items)
            delete item;
         this->items.clear();
         for (auto condition : this->conditions)
            delete condition;
         this->conditions.clear();
      }
      //
      void Block::_get_effective_items(std::vector<ParsedItem*>& out, bool include_functions) {
         out.clear();
         for (auto item : this->items) {
            if (auto block = dynamic_cast<Block*>(item)) {
               if (!include_functions) {
                  if (block->type == Type::function)
                     continue;
               }
               out.push_back(item);
               continue;
            }
            if (dynamic_cast<Statement*>(item)) {
               out.push_back(item);
               continue;
            }
         }
      }
      bool Block::_is_if_block() const noexcept {
         switch (this->type) {
            case Type::if_block:
            case Type::elseif_block:
            case Type::else_block:
               return true;
         }
         return false;
      }
      void Block::_make_trigger(Compiler& compiler) {
         if (this->type == Type::function) {
            assert(this->trigger && "The Compiler should've given this user-defined-function Block a trigger when it was first opened.");
            this->trigger->blockType = block_type::normal;
            this->trigger->entryType = entry_type::subroutine;
            if (this->event != Event::none)
               this->trigger->entryType = _block_event_to_trigger_entry(this->event);
            //
            std::vector<ParsedItem*> items;
            this->_get_effective_items(items, false);
            if (items.size() == 1) {
               //
               // If the function only contains a single block, then use that block's type. For example, the following 
               // function should compile as a single for-loop trigger:
               //
               //    function example()    -- this Block owns   the Trigger
               //       for each player do -- this Block shares the Trigger
               //          action
               //       end
               //    end
               //
               auto item = dynamic_cast<Block*>(this->items[0]);
               if (item) {
                  //
                  // TODO: Currently, we do not allow nested blocks to be event handlers. However, I want 
                  // to be future-compatible and make sure that most parts of our code handle that sensibly 
                  // anyway, in part because the file format would allow for it and I'm not 100% sure that 
                  // the runtime wouldn't. Accordingly, we should make the following changes: we should 
                  // bail out of this branch without making the nested block share a trigger with the 
                  // containing function block, IF both blocks have an event type and these types differ 
                  // from each other.
                  //
                  item->trigger = this->trigger; // get the inner Block to write into the function's trigger
                  this->trigger->blockType = _block_type_to_trigger_type(item->type);
               }
            }
            return;
         }
         if (this->trigger)
            //
            // If we already have a trigger, then we're meant to compile our content directly into that 
            // trigger. This will generally be the case for things like the last if-block inside of a 
            // trigger:
            //
            //    for each player do
            //       action
            //       if condition then -- not the last thing in the block, so needs its own Trigger
            //          action
            //       end
            //       if condition then -- the last thing in the block, so writes into parent block's Trigger
            //          action
            //       end
            //    end
            //
            return;
         //
         auto   t  = this->trigger = new Trigger;
         size_t ti = compiler.results.triggers.size(); // index of this trigger in the full trigger list
         compiler.results.triggers.push_back(t);
         if (this->parent)
            t->entryType = entry_type::subroutine;
         //
         if (this->event != Event::none) {
            t->entryType = _block_event_to_trigger_entry(this->event);
            compiler.results.events.set_index_of_event(t->entryType, ti);
         }
         switch (this->type) {
            case Type::basic:
            case Type::if_block:
            case Type::elseif_block:
            case Type::else_block:
               break;
            case Type::for_each_object:
               t->blockType = block_type::for_each_object;
               break;
            case Type::for_each_object_with_label:
               t->blockType = block_type::for_each_object_with_label;
               break;
            case Type::for_each_player:
               t->blockType = block_type::for_each_player;
               break;
            case Type::for_each_player_randomly:
               t->blockType = block_type::for_each_player_randomly;
               break;
            case Type::for_each_team:
               t->blockType = block_type::for_each_team;
               break;
         }
      }
      void Block::compile(Compiler& compiler) {
         std::vector<ParsedItem*> items;
         this->_get_effective_items(items);
         size_t size = items.size();
         if (!size) {
            //
            // Don't compile the contents of an empty block. (This can happen both when the script author 
            // writes an empty block, and when we run this function on the root to compile any loose 
            // statements just before the start of a top-level block.)
            //
            return;
         }
         //
         this->_make_trigger(compiler);
         {
            auto& count = this->trigger->raw.conditionCount; // multiple Blocks can share one Trigger, so let's co-opt this field to keep track of the or-group. it'll be reset when we save the variant anyway
            //
            // TODO: If the current block is an else or elseif block, then we need to copy and negate the 
            // conditions of the previous-sibling if or elseif block.
            //
            for (auto item : this->conditions) {
               #if _DEBUG
                  assert(this->type != Type::root && "The root block shouldn't contain any conditions!");
               #endif
               auto cmp = dynamic_cast<Comparison*>(item);
               assert(cmp);
               auto cnd = dynamic_cast<Condition*>(cmp->opcode);
               if (cnd) {
                  this->trigger->opcodes.push_back(cnd);
                  cnd->or_group = count;
                  cmp->opcode = nullptr;
               }
               //
               if (!cmp->next_is_or)
                  count += 1;
            }
         }
         for (size_t i = 0; i < size; i++) {
            bool is_last = (i == size - 1);
            auto item    = items[i];
            auto block   = dynamic_cast<Block*>(item);
            if (block) {
               if (is_last && !block->is_event_trigger() && block->_is_if_block())
                  block->trigger = this->trigger;
               block->compile(compiler);
               if (!block->trigger) // empty blocks get skipped
                  continue;
               if (block->trigger != this->trigger) {
                  //
                  // Create a "call nested trigger" opcode.
                  //
                  auto call  = new Action;
                  this->trigger->opcodes.push_back(call);
                  call->function = &actionFunction_runNestedTrigger;
                  auto arg   = (call->function->arguments[0].typeinfo.factory)();
                  call->arguments.push_back(arg);
                  auto arg_c = dynamic_cast<OpcodeArgValueTrigger*>(arg);
                  assert(arg_c && "The argument to the ''run nested trigger'' opcode isn't OpcodeArgValueTrigger anymore? Did someone change the opcode-base?");
                  arg_c->value = compiler._index_of_trigger(block->trigger);
                  assert(arg_c->value >= 0 && "Nested block trigger isn't in the Compiler's trigger list?!");
               }
               continue;
            }
            auto statement = dynamic_cast<Statement*>(item);
            if (statement) {
               if (auto opcode = statement->opcode) {
                  this->trigger->opcodes.push_back(opcode);
                  statement->opcode = nullptr;
               }
               continue;
            }
         }
      }
      //
      Statement::~Statement() {
         if (this->lhs) {
            delete this->lhs;
            this->lhs = nullptr;
         }
         if (this->rhs) {
            delete this->rhs;
            this->rhs = nullptr;
         }
         if (this->opcode) {
            delete this->opcode;
            this->opcode = nullptr;
         }
      }
   }
   //
   Compiler::~Compiler() {
      for (auto trigger : this->results.triggers)
         delete trigger;
      this->results.triggers.clear();
      //
      if (this->block) {
         if (this->block != this->root && !this->block->parent) // don't double-free the root; if the block has a parent, its parent is responsible for freeing it
            delete this->block;
         this->block = nullptr;
      }
      if (this->root) {
         delete this->root;
         this->root = nullptr;
      }
      if (auto statement = this->assignment) {
         if (!statement->owner && !statement->parent) // only free the memory if it wasn't appended to a Block; otherwise, the memory should already be freed
            delete statement;
         this->assignment = nullptr;
      }
      if (auto statement = this->comparison) {
         if (!statement->owner && !statement->parent) // only free the memory if it wasn't appended to a Block; otherwise, the memory should already be freed
            delete statement;
         this->comparison = nullptr;
      }
      this->aliases_in_scope.clear(); // don't free contents; every Alias should have been inside of a Block and freed by that Block
      this->functions_in_scope.clear();
   }
   //
   /*static*/ bool Compiler::is_keyword(QString s) {
      s = s.toLower();
      if (s == "alias") // declare an alias
         return true;
      if (s == "and") // bridge conditions
         return true;
      if (s == "declare") // declare a variable
         return true;
      if (s == "do") // open a generic block
         return true;
      if (s == "else") // close an if- or elseif-block and open a new block
         return true;
      if (s == "elseif") // close an if- or elseif-block and open a new block with conditions
         return true;
      if (s == "end") // close a block
         return true;
      if (s == "for") // open a for loop block
         return true;
      if (s == "function") // open a function block
         return true;
      if (s == "if") // open a new block with conditions
         return true;
      if (s == "not") // indicate that the next condition should be negated
         return true;
      if (s == "on") // designate the event handler type of the next top-level block
         return true;
      if (s == "or") // bridge conditions
         return true;
      if (s == "then") // close an if- or elseif-statement's conditions
         return true;
      return false;
   }
   //
   void Compiler::_commit_unresolved_strings(Compiler::unresolved_str_list& add) {
      auto& list = this->results.unresolved_strings;
      list.reserve(list.size() + add.size());
      for (auto& e : add)
         list.push_back(e);
      add.clear();
   }
   //
   Script::Alias* Compiler::lookup_relative_alias(QString name, const OpcodeArgTypeinfo* relative_to) {
      auto& list = this->aliases_in_scope;
      size_t size = list.size();
      if (!size)
         return nullptr;
      //
      // Search the list in reverse order, so that newer aliases shadow older ones, and aliases in an 
      // inner scope shadow aliases in an outer scope.
      //
      for (signed int i = size - 1; i >= 0; --i) {
         auto alias = list[i];
         if (!alias->is_relative_alias())
            continue;
         if (alias->get_basis_type() != relative_to)
            continue;
         if (name.compare(alias->name, Qt::CaseInsensitive) == 0)
            return alias;
      }
      return nullptr;
   }
   Script::Alias* Compiler::lookup_absolute_alias(QString name) {
      auto&  list = this->aliases_in_scope;
      size_t size = list.size();
      if (!size)
         return nullptr;
      //
      // Search the list in reverse order, so that newer aliases shadow older ones, and aliases in an 
      // inner scope shadow aliases in an outer scope.
      //
      for (signed int i = size - 1; i >= 0; --i) {
         auto alias = list[i];
         if (alias->is_relative_alias())
            continue;
         if (name.compare(alias->name, Qt::CaseInsensitive) == 0)
            return alias;
      }
      return nullptr;
   }
   Script::UserDefinedFunction* Compiler::lookup_user_defined_function(QString name) {
      auto&  list = this->functions_in_scope;
      size_t size = list.size();
      if (!size)
         return nullptr;
      for (size_t i = 0; i < size; i++) {
         auto& func = list[i];
         if (func.name.isEmpty()) // used for invalid function names, when the bad names are non-fatal rather than fatal errors
            continue;
         if (func.name.compare(name, Qt::CaseInsensitive) == 0)
            return &func;
      }
      return nullptr;
   }
   //
   /*static*/ Compiler::name_source Compiler::check_name_is_taken(const QString& name, OpcodeArgTypeRegistry::type_list_t& name_is_imported_from) {
      name_is_imported_from.clear();
      auto& type_registry = OpcodeArgTypeRegistry::get();
      //
      if (auto type = type_registry.get_variable_type(name))
         return name_source::variable_typename;
      if (auto type = type_registry.get_static_indexable_type(name))
         return name_source::static_typename;
      for (auto& member : Script::namespaces::unnamed.members)
         if (cobb::qt::stricmp(name, member.name) == 0)
            return name_source::namespace_member;
      //
      // Search the opcode lists to see if the name is in use by a non-member function:
      //
      for (auto& opcode : actionFunctionList) {
         auto& mapping = opcode.mapping;
         if (mapping.arg_context != OpcodeFuncToScriptMapping::no_context)
            //
            // We don't care about member functions.
            //
            continue;
         if (cobb::qt::stricmp(name, mapping.primary_name) == 0 || cobb::qt::stricmp(name, mapping.secondary_name) == 0) {
            switch (mapping.type) {
               case OpcodeFuncToScriptMapping::mapping_type::property_get:
               case OpcodeFuncToScriptMapping::mapping_type::property_set:
                  //
                  // Accessors are members; we don't care about them.
                  //
                  break;
               default:
                  return name_source::action;
            }
         }
      }
      for (auto& opcode : conditionFunctionList) {
         auto& mapping = opcode.mapping;
         if (mapping.arg_context != OpcodeFuncToScriptMapping::no_context)
            continue;
         if (cobb::qt::stricmp(name, mapping.primary_name) == 0 || cobb::qt::stricmp(name, mapping.secondary_name) == 0)
            return name_source::condition;
      }
      //
      // Do not allow the shadowing of imported names:
      //
      type_registry.lookup_imported_name(name, name_is_imported_from);
      if (name_is_imported_from.size())
         return name_source::imported_name;
      //
      return name_source::none;
   }
   //
   Compiler::log_checkpoint Compiler::create_log_checkpoint() {
      log_checkpoint point;
      point.warnings = this->warnings.size();
      point.errors = this->errors.size();
      point.fatal_errors = this->fatal_errors.size();
      return point;
   }
   void Compiler::revert_to_log_checkpoint(Compiler::log_checkpoint point) {
      this->warnings.resize(point.warnings);
      this->errors.resize(point.errors);
      this->fatal_errors.resize(point.fatal_errors);
   }
   bool Compiler::checkpoint_has_errors(Compiler::log_checkpoint point) const noexcept {
      if (this->warnings.size() > point.warnings)
         return true;
      if (this->errors.size() > point.errors)
         return true;
      if (this->fatal_errors.size() > point.fatal_errors)
         return true;
      return false;
   }
   //
   Script::VariableReference* Compiler::arg_to_variable(QString arg) noexcept {
      return this->__parseVariable(arg);
   }
   Script::VariableReference* Compiler::arg_to_variable(string_scanner& arg) noexcept {
      auto text = arg.extract_word();
      if (text.isEmpty())
         return nullptr;
      return this->__parseVariable(text);
   }
   //
   void Compiler::raise_error(const QString& text) {
      this->errors.emplace_back(text, this->state);
   }
   void Compiler::raise_error(const Compiler::pos& pos, const QString& text) {
      this->errors.emplace_back(text, pos);
   }
   void Compiler::raise_fatal(const QString& text) {
      this->fatal_errors.emplace_back(text, this->state);
   }
   void Compiler::raise_fatal(const Compiler::pos& pos, const QString& text) {
      this->fatal_errors.emplace_back(text, pos);
   }
   void Compiler::raise_warning(const QString& text) {
      this->warnings.emplace_back(text, this->state);
   }
   void Compiler::reset_token() {
      this->token = Token();
   }
   //
   void Compiler::parse(QString text) {
      this->text = text;
      if (!this->root) {
         this->root = new Script::Block;
         this->root->type = Script::Block::Type::root;
         this->root->set_start(this->backup_stream_state());
      }
      this->block = this->root;
      //
      this->assignment = nullptr;
      this->comparison = nullptr;
      this->reset_token();
      this->scan([this](QChar c) {
         if (!this->is_in_statement())
            //
            // If we're not in a statement, then the next token must be a word. If that word is a 
            // keyword, then we handle it accordingly. If it is not a keyword, then it must be 
            // followed either by an operator (in which case we're in an assign statement) or by 
            // an opening parentheses (in which case the statement is a function call).
            //
            this->_parseActionStart(c);
         else if (this->assignment)
            this->_parseAssignment(c);
         //
         if (this->has_fatal())
            return true;
         return false;
      });
      if (!this->has_fatal()) {
         if (this->assignment)
            this->raise_fatal("An assignment statement is missing its righthand side.");
         if (this->comparison)
            this->raise_fatal("The file ended before a statement could be fully processed.");
         if (this->block != this->root)
            this->raise_fatal("Unclosed block.");
         if (this->next_event != Script::Block::Event::none)
            this->raise_fatal("The file ended with an \"on\" keyword but no following block.");
         this->root->set_end(this->state);
         //
         {  // Ensure that we're under the count limits for triggers, conditions, and actions.
            size_t tc = this->results.triggers.size();
            size_t cc = 0;
            size_t ac = 0;
            for (auto trigger : this->results.triggers)
               trigger->count_contents(cc, ac);
            if (tc > Limits::max_triggers)
               this->raise_error(QString("The compiled script contains %1 triggers, but only a maximum of %2 are allowed.").arg(tc).arg(Limits::max_triggers));
            if (cc > Limits::max_conditions)
               this->raise_error(QString("The compiled script contains %1 conditions, but only a maximum of %2 are allowed.").arg(tc).arg(Limits::max_conditions));
            if (ac > Limits::max_actions)
               this->raise_error(QString("The compiled script contains %1 actions, but only a maximum of %2 are allowed.").arg(tc).arg(Limits::max_actions));
         }
         //
         if (!this->has_errors())
            this->results.success = true;
      }
      return;
   }

   void Compiler::_parseActionStart(QChar c) {
      if (this->token.text.isEmpty()) {
         if (c != '-' && string_scanner::is_operator_char(c)) { // minus-as-numeric-sign must be special-cased
            this->raise_fatal(QString("Unexpected %1. Statements cannot begin with an operator.").arg(c));
            return;
         }
         if (string_scanner::is_syntax_char(c)) {
            this->raise_fatal(QString("Unexpected %1.").arg(c));
            return;
         }
         if (string_scanner::is_quote_char(c)) {
            this->raise_fatal(QString("Unexpected %1. Statements cannot begin with a string literal.").arg(c));
            return;
         }
         if (c == '(' || c == ')') {
            this->raise_fatal(QString("Unexpected %1. Parentheses are only allowed as delimiters for function arguments.").arg(c));
            return;
         }
         if (string_scanner::is_whitespace_char(c))
            return;
         this->token.text += c;
         this->token.pos = this->backup_stream_state();
         return;
      }
      if (c == '[') {
         this->token.brace = true;
         this->token.text += c;
         return;
      }
      if (this->token.brace) {
         if (c == ']')
            this->token.brace = false;
         this->token.text += c;
         return;
      }
      if (string_scanner::is_whitespace_char(c)) {
         this->token.ended = true;
         //
         // Handle keywords here, if appropriate.
         //
         using _handler_t = decltype(&Compiler::_handleKeyword_Alias);
         _handler_t handler = nullptr;
         //
         auto& word = this->token.text;
         if (word == "and" || word == "or" || word == "not" || word == "then") {
            this->raise_fatal(QString("The \"%1\" keyword cannot appear here.").arg(word));
            return;
         }
         if (word == "alias")
            handler = &Compiler::_handleKeyword_Alias;
         else if (word == "declare")
            handler = &Compiler::_handleKeyword_Declare;
         else if (word == "do")
            handler = &Compiler::_handleKeyword_Do;
         else if (word == "else")
            handler = &Compiler::_handleKeyword_Else;
         else if (word == "elseif")
            handler = &Compiler::_handleKeyword_ElseIf;
         else if (word == "end")
            handler = &Compiler::_handleKeyword_End;
         else if (word == "for")
            handler = &Compiler::_handleKeyword_For;
         else if (word == "function")
            handler = &Compiler::_handleKeyword_Function;
         else if (word == "if")
            handler = &Compiler::_handleKeyword_If;
         else if (word == "on")
            handler = &Compiler::_handleKeyword_On;
         //
         if (handler) {
            auto prior = this->state;
            ((*this).*(handler))();
            this->reset_token();
            if (prior.offset < this->state.offset) {
               //
               // The handler code advanced the position to the end of the keyword's relevant 
               // content (e.g. the end of a block declaration). However, our containing loop 
               // (or rather, Compiler::scan call) will increment the position one more time, 
               // so we need to rewind by one.
               //
               --this->state.offset;
            }
         }
         //
         // If (handler) is null, then the word wasn't a keyword. Move to the next iteration 
         // of the parsing loop; we'll eventually feed the word to a new statement.
         //
         return;
      }
      if (string_scanner::is_quote_char(c)) {
         this->raise_fatal(QString("Unexpected %1. Statements of the form {word \"string\"} are not valid.").arg(c));
         return;
      }
      if (c == '(') {
         this->_parseFunctionCall(false);
         --this->state.offset; // _parseFunctionCall moved us to the end of the call, but we're being run from inside of a scan-functor -- effectively a loop -- so we're going to advance one more
         return;
      }
      if (c == ')' || c == ',') {
         this->raise_fatal(QString("Unexpected %1.").arg(c));
         return;
      }
      if (string_scanner::is_operator_char(c)) {
         this->assignment = new Script::Statement;
         this->assignment->set_start(this->token.pos);
         {
            this->assignment->lhs = this->__parseVariable(this->token.text);
            auto ref = this->assignment->lhs;
            ref->owner = this->assignment;
            if (!ref->is_invalid) {
               if (ref->is_constant_integer())
                  this->raise_error("Cannot assign to a constant integer.");
               else if (ref->is_read_only())
                  this->raise_error(QString("Cannot assign to \"%1\". The referenced value is read-only.").arg(ref->to_string()));
            }
         }
         this->reset_token();
         this->token.text = c;
         this->token.pos  = this->backup_stream_state();
         return;
      }
      if (this->token.ended) {
         this->raise_fatal("Statements of the form {word word} are not valid.");
         return;
      }
      this->token.text += c;
      if (this->token.text[0] == '-' && !c.isNumber()) {
         //
         // We allowed the word to start with "-" in case it was a number, but it 
         // has turned out not to be a number. That means that the "-" was an 
         // operator, not a numeric sign. Wait, that's illegal.
         //
         this->raise_fatal("Unexpected -. Statements cannot begin with an operator.");
         return;
      }
   }
   void Compiler::_parseAssignment(QChar c) {
      assert(this->assignment && "This should not have been called!");
      #pragma region Code to finish parsing an assignment (i.e. the operator and righthand side)
         if (this->assignment->op.isEmpty()) {
            //
            // If the statement doesn't have an operator stored, then the operator is currently 
            // being parsed and exists in (token).
            //
            if (string_scanner::is_operator_char(c)) {
               this->token.text += c;
               return;
            }
            auto a = this->assignment;
            a->op = this->token.text;
            if (!_is_assignment_operator(this->token.text))
               this->raise_error(QString("Operator %1 is not a valid assignment operator.").arg(this->token.text));
            this->reset_token();
            //
            // Fall through to righthand-side handling so we don't miss the first character 
            // after the operator in cases like {a=b} where there's no whitespace.
            //
         }
         if ((!this->token.text.isEmpty() || c != '-') && string_scanner::is_operator_char(c)) {
            this->raise_fatal(QString("Unexpected %1 on the righthand side of an assignment statement.").arg(c));
            return;
         }
         if (string_scanner::is_quote_char(c)) {
            this->raise_error(QString("Unexpected %1. You cannot assign strings to variables.").arg(c));
            //
            // TODO: Do we need to advance past the current character with ++this->state.pos first?
            //
            if (!this->skip_to(c))
               this->raise_fatal("Unable to find the closing quote for the string literal. Parsing cannot continue.");
            else {
               this->reset_token();
               delete this->assignment;
               this->assignment = nullptr;
            }
            return;
         }
         if (this->token.text.isEmpty()) {
            if (string_scanner::is_whitespace_char(c))
               return;
            if (c == '(') {
               this->raise_fatal("Unexpected (. Parentheses are only allowed as delimiters for function arguments.");
               return;
            }
         } else {
            if (c == '(') {
               if (this->assignment->op != ce_assignment_operator) {
                  this->raise_error(QString("Operator %1 cannot be used to assign the result of a function call to a variable. Use operator =.").arg(this->assignment->op));
               }
               this->_parseFunctionCall(false);
               --this->state.offset; // _parseFunctionCall moved us to the end of the call, but we're being run from inside of a scan-functor -- effectively a loop -- so we're going to advance one more
               return;
               //
               // From here on out, the code for parsing function calls will handle what 
               // remains. The end of the function call is also the end of this statement, 
               // so the code for parsing function calls will "close" this statement just 
               // fine.
               //
            }
         }
         if (c == ')' || c == ',') {
            this->raise_fatal(QString("Unexpected %1.").arg(c));
            return;
         }
         if (!string_scanner::is_whitespace_char(c)) {
            if (this->token.text.isEmpty())
               this->token.pos = this->backup_stream_state();
            this->token.text += c;
            return;
         }
      #pragma endregion
      //
      // If we get here, then we've encountered the end of the statement's righthand side.
      //
      #pragma region Code to compile an assignment
         auto statement = this->assignment;
         statement->set_end(this->state);
         this->block->insert_item(statement);
         this->assignment = nullptr;
         //
         statement->rhs = this->__parseVariable(this->token.text);
         statement->rhs->owner = statement;
         this->reset_token();
         //
         // TODO: Consider writing code for type checking non-accessor assignments in advance... but 
         // maybe leave it turned off: I want to check the results of type-mismatched assignments in-
         // game. I'm pretty sure that numbers and timers can be assigned to each other, but I want to 
         // know if other types do anything sensible, do nothing at all, clear the target variable, or 
         // crash, so that I can program in a compiler warning or compiler error as appropriate.
         //

         //
         // Compile the assignment opcode:
         //
         auto opcode = std::make_unique<Action>();
         {
            auto lhs = statement->lhs;
            auto rhs = statement->rhs;
            auto l_accessor = lhs->get_accessor_definition();
            auto r_accessor = rhs->get_accessor_definition();
            if (l_accessor || r_accessor) {
               //
               // This is an accessor assignment, not a standard assignment.
               //
               if (l_accessor && r_accessor) {
                  this->raise_error("Cannot assign one accessor to another accessor.");
                  return;
               }
               const OpcodeFuncToScriptMapping* mapping = nullptr;
               const OpcodeBase* accessor = nullptr;
               QString acc_name;
               if (l_accessor) {
                  auto setter = l_accessor->setter;
                  if (!setter) {
                     this->raise_error("This accessor cannot be assigned to.");
                     return;
                  }
                  accessor = setter;
                  acc_name = lhs->resolved.accessor_name;
                  mapping  = &setter->mapping;
                  lhs->strip_accessor();
                  //
                  // Compile the left-hand side (the context-argument):
                  //
                  auto ai  = mapping->arg_context;
                  auto arg = (accessor->arguments[ai].typeinfo.factory)();
                  opcode->arguments[ai] = arg;
                  auto result = arg->compile(*this, *lhs, 0);
                  if (!result.is_success()) {
                     QString error = "The lefthand side of this assignment failed to compile. ";
                     if (!result.error.isEmpty())
                        error += result.error;
                     this->raise_error(error);
                  } else
                     assert(!result.is_unresolved_string() && "The lefthand side of an assignment statement thinks it's an unresolved string reference.");
                  //
                  // Compile the right-hand side (the value to assign):
                  //
                  ai  = accessor->index_of_operand_argument();
                  arg = (accessor->arguments[ai].typeinfo.factory)();
                  opcode->arguments[ai] = arg;
                  result = arg->compile(*this, *rhs, 0);
                  if (!result.is_success()) {
                     QString error = "The righthand side of this assignment failed to compile. ";
                     if (!result.error.isEmpty())
                        error += result.error;
                     this->raise_error(error);
                  } else
                     assert(!result.is_unresolved_string() && "The righthand side of an assignment statement thinks it's an unresolved string reference.");
               } else {
                  auto getter = r_accessor->getter;
                  if (!getter) {
                     this->raise_error("This accessor cannot be read.");
                     return;
                  }
                  accessor = getter;
                  acc_name = rhs->resolved.accessor_name;
                  mapping  = &getter->mapping;
                  rhs->strip_accessor();
                  //
                  // Compile the left-hand side (the out-argument):
                  //
                  int  ai  = accessor->index_of_out_argument();
                  auto arg = (accessor->arguments[ai].typeinfo.factory)();
                  opcode->arguments[ai] = arg;
                  auto result = arg->compile(*this, *lhs, 0);
                  if (!result.is_success()) {
                     QString error = "The lefthand side of this assignment failed to compile. ";
                     if (!result.error.isEmpty())
                        error += result.error;
                     this->raise_error(error);
                  } else
                     assert(!result.is_unresolved_string() && "The lefthand side of an assignment statement thinks it's an unresolved string reference.");
                  //
                  // Compile the right-hand side (the value to assign):
                  //
                  ai  = mapping->arg_context;
                  arg = (accessor->arguments[ai].typeinfo.factory)();
                  opcode->arguments[ai] = arg;
                  result = arg->compile(*this, *rhs, 0);
                  if (!result.is_success()) {
                     QString error = "The righthand side of this assignment failed to compile. ";
                     if (!result.error.isEmpty())
                        error += result.error;
                     this->raise_error(error);
                  } else
                     assert(!result.is_unresolved_string() && "The righthand side of an assignment statement thinks it's an unresolved string reference.");
               }
               assert(mapping);
               if (accessor->get_name_type()) {
                  //
                  // The accessor is variably named. We need to compile the name.
                  //
                  auto  op_string = string_scanner(acc_name);
                  auto  ai   = mapping->arg_name;
                  auto& base = accessor->arguments[ai];
                  opcode->arguments[ai] = (base.typeinfo.factory)();
                  auto result = opcode->arguments[ai]->compile(*this, op_string, 0);
                  if (!result.is_success()) {
                     QString error = "The accessor name in this assignment failed to compile. ";
                     if (!result.error.isEmpty())
                        error += result.error;
                     this->raise_error(error);
                  } else
                     assert(!result.is_unresolved_string() && "The accessor name in an assignment statement thinks it's an unresolved string reference.");
               }
               if (mapping->arg_operator == OpcodeFuncToScriptMapping::no_argument) {
                  //
                  // This accessor doesn't have an "operator" argument, so throw an error if we're using the 
                  // wrong argument. (We should only be using accessors in the first place if there *is* an 
                  // opcode argument, but it's possible that only one of the getter and setter may have it.)
                  //
                  if (statement->op != "=")
                     this->raise_error("This accessor can only be invoked using the = operator.");
               } else {
                  //
                  // Compile the assignment operator.
                  //
                  auto op_string = string_scanner(statement->op);
                  auto op_arg    = (accessor->arguments[mapping->arg_operator].typeinfo.factory)();
                  opcode->arguments[mapping->arg_operator] = op_arg;
                  auto result = op_arg->compile(*this, op_string, 0);
                  if (!result.is_success()) {
                     QString error = "The operator in this assignment failed to compile. ";
                     if (!result.error.isEmpty())
                        error += result.error;
                     this->raise_error(error);
                  } else
                     assert(!result.is_unresolved_string() && "The operator in an assignment statement thinks it's an unresolved string reference.");
               }
            } else {
               auto base = &_get_assignment_opcode();
               opcode->function = base;
               opcode->arguments.resize(3);
               opcode->arguments[0] = (base->arguments[0].typeinfo.factory)();
               opcode->arguments[1] = (base->arguments[1].typeinfo.factory)();
               opcode->arguments[2] = (base->arguments[2].typeinfo.factory)();
               //
               auto result = opcode->arguments[0]->compile(*this, *lhs, 0);
               if (!result.is_success()) {
                  QString error = "The lefthand side of this assignment failed to compile. ";
                  if (!result.error.isEmpty())
                     error += result.error;
                  this->raise_error(error);
               } else
                  assert(!result.is_unresolved_string() && "The lefthand side of an assignment statement thinks it's an unresolved string reference.");
               //
               result = opcode->arguments[1]->compile(*this, *rhs, 0);
               if (!result.is_success()) {
                  QString error = "The righthand side of this assignment failed to compile. ";
                  if (!result.error.isEmpty())
                     error += result.error;
                  this->raise_error(error);
               } else
                  assert(!result.is_unresolved_string() && "The righthand side of an assignment statement thinks it's an unresolved string reference.");
               //
               auto op_string = string_scanner(statement->op);
               result = opcode->arguments[2]->compile(*this, op_string, 0);
               if (!result.is_success()) {
                  QString error = "The operator in this assignment failed to compile. ";
                  if (!result.error.isEmpty())
                     error += result.error;
                  this->raise_error(error);
               } else
                  assert(!result.is_unresolved_string() && "The operator in an assignment statement thinks it's an unresolved string reference.");
            }
         }
         statement->opcode = opcode.release();
      #pragma endregion
   }
   //
   void Compiler::_applyConditionModifiers(Script::Comparison* condition) {
      if (!condition)
         return;
      auto opcode = dynamic_cast<Condition*>(condition->opcode);
      if (opcode) // it could have failed to compile
         opcode->inverted = this->negate_next_condition;
      if (this->next_condition_joiner == c_joiner::or)
         condition->next_is_or = true;
      this->next_condition_joiner = c_joiner::none;
      this->negate_next_condition = false;
   }
   Script::VariableReference* Compiler::__parseVariable(QString text, bool is_alias_definition, bool is_write_access) {
      //
      // A quick note: Alias::Alias doesn't need to use this. That just declares an alias to a 
      // variable. If the alias is actually used, it'll be resolved through VariableReference 
      // and we'll catch it here (or in the code for user-written variable declarations -- the 
      // only part of the compiler that shouldn't create VariableReferences through this helper 
      // function).
      //
      auto var = new Script::VariableReference(*this, text);
      if (this->has_fatal()) // fatal error occurred while parsing the text
         return var;
      var->resolve(*this, is_alias_definition, is_write_access);
      if (!var->is_invalid) {
         //
         // Implicitly declare the variable:
         //
         auto type  = var->get_type();
         auto basis = var->get_alias_basis_type();
         //
         variable_scope scope_v = getVariableScopeForTypeinfo(basis);
         variable_type  type_v  = getVariableTypeForTypeinfo(type);
         if (scope_v == variable_scope::not_a_scope || type_v == variable_type::not_a_variable) {
            this->raise_error(QString("Unable to generate an implicit variable declaration for \"%1\".").arg(var->to_string()));
            return var;
         }
         //
         int32_t index = 0;
         if (scope_v == variable_scope::global) {
            index = var->resolved.top_level.index;
         } else {
            index = var->resolved.nested.index;
         }
         auto set  = this->_get_variable_declaration_set(scope_v);
         auto decl = set->get_or_create_declaration(type_v, index);
      }
      return var;
   }
   //
   void Compiler::_parseBlockConditions() {
      this->comparison = nullptr;
      this->reset_token();
      this->scan([this](QChar c) {
         if (!this->comparison) {
            if (this->_parseConditionStart(c))
               return true; // stop the loop; we found the "then" keyword
            if (this->has_fatal())
               return true;
            return false;
         }
         //
         // If, on the other hand, we're in a statement, then we need to finish that up.
         //
         this->_parseComparison(c);
         if (this->has_fatal())
            return true;
         return false;
      });
   }
   bool Compiler::_parseConditionStart(QChar c) {
      if (this->token.text.isEmpty()) {
         if (c != '-' && string_scanner::is_operator_char(c)) { // minus-as-numeric-sign must be special-cased
            this->raise_fatal(QString("Unexpected %1. Conditions cannot begin with an operator.").arg(c));
            return false;
         }
         if (string_scanner::is_syntax_char(c)) {
            this->raise_fatal(QString("Unexpected %1.").arg(c));
            return false;
         }
         if (string_scanner::is_quote_char(c)) {
            this->raise_fatal(QString("Unexpected %1. Conditions cannot begin with a string literal.").arg(c));
            return false;
         }
         if (c == '(' || c == ')') {
            this->raise_fatal(QString("Unexpected %1. Parentheses are only allowed as delimiters for function arguments.").arg(c));
            return false;
         }
         if (string_scanner::is_whitespace_char(c))
            return false;
         this->token.text += c;
         this->token.pos = this->backup_stream_state();
         return false;
      }
      if (c == '[') {
         this->token.brace = true;
         this->token.text += c;
         return false;
      }
      if (this->token.brace) {
         if (c == ']')
            this->token.brace = false;
         this->token.text += c;
         return false;
      }
      if (string_scanner::is_whitespace_char(c)) {
         this->token.ended = true;
         //
         // Handle keywords here, if appropriate.
         //
         auto& word = this->token.text;
         if (word == "then") {
            if (this->negate_next_condition)
               this->raise_fatal("Expected a condition after \"not\".");
            switch (this->next_condition_joiner) {
               case c_joiner::and:
                  this->raise_fatal("Expected a condition after \"and\".");
                  break;
               case c_joiner::or:
                  this->raise_fatal("Expected a condition after \"or\".");
                  break;
            }
            return true;
         }
         if (word == "alias")
            this->raise_fatal(QString("You cannot place %1 declarations inside of conditions.").arg(word));
         else if (word == "do")
            this->raise_fatal("You cannot open or close blocks inside of conditions. (If the \"do\" was meant to mark the end of conditions, use \"then\" instead.)");
         else if (word == "else" || word == "elseif")
            this->raise_fatal(QString("Unexpected \"%1\". A list of conditions must end with \"then\".").arg(word));
         else if (word == "for" || word == "function" || word == "if")
            this->raise_fatal("You cannot open or close blocks inside of conditions. End the list of conditions using the \"then\" keyword.");
         else if (word == "on")
            this->raise_fatal("You cannot mark event handlers inside of conditions.");
         else if (word == "and") {
            if (this->negate_next_condition) // this check only works because we do not allow (not not condition)
               this->raise_fatal("Constructions of the form {not and} and {not or} are not valid.");
            else if (this->next_condition_joiner != c_joiner::none)
               this->raise_fatal("Constructions of the form {or and} and {and and} are not valid.");
            else
               this->next_condition_joiner = c_joiner::and;
            //
            this->reset_token();
         } else if (word == "or") {
            if (this->negate_next_condition) // this check only works because we do not allow (not not condition)
               this->raise_fatal("Constructions of the form {not and} and {not or} are not valid.");
            else if (this->next_condition_joiner != c_joiner::none)
               this->raise_fatal("Constructions of the form {and or} and {or or} are not valid.");
            else
               this->next_condition_joiner = c_joiner::or;
            //
            this->reset_token();
         } else if (word == "not") {
            if (this->negate_next_condition) {
               //
               // NOTE: If we decide to allow (not not condition) and just have each "not" toggle the negate-next-condition flag, 
               // then we also need to modify the code that checks for (not and condition) and (not or condition) in order to make 
               // sure that (not not and condition) and (not not or condition) are still considered invalid. That code is in this 
               // same function.
               //
               this->raise_fatal("Constructions of the form {not not condition} are not valid. Use a single \"not\" or no \"not\" at all.");
               return false;
            }
            this->negate_next_condition = true;
            this->reset_token();
         }
         return false;
      }
      if (string_scanner::is_quote_char(c)) {
         this->raise_fatal(QString("Unexpected %1. Statements of the form {word \"string\"} are not valid.").arg(c));
         return false;
      }
      if (c == '(') {
         this->_parseFunctionCall(true);
         --this->state.offset; // _parseFunctionCall moved us to the end of the call, but we're being run from inside of a scan-functor -- effectively a loop -- so we're going to advance one more
         return false;
      }
      if (c == ')' || c == ',') {
         this->raise_fatal(QString("Unexpected %1.").arg(c));
         return false;
      }
      if (string_scanner::is_operator_char(c)) {
         this->comparison = new Script::Comparison;
         this->comparison->set_start(this->token.pos);
         this->comparison->lhs = this->__parseVariable(this->token.text);
         this->comparison->lhs->owner = this->assignment;
         this->reset_token();
         this->token.text = c;
         this->token.pos = this->backup_stream_state();
         return false;
      }
      if (this->token.ended) {
         this->raise_fatal("Statements of the form {word word} are not valid.");
         return false;
      }
      this->token.text += c;
      if (this->token.text[0] == '-' && !c.isNumber()) {
         //
         // We allowed the word to start with "-" in case it was a number, but it 
         // has turned out not to be a number. That means that the "-" was an 
         // operator, not a numeric sign. Wait, that's illegal.
         //
         this->raise_fatal("Unexpected -. Statements cannot begin with an operator.");
         return false;
      }
      return false;
   }
   void Compiler::_parseComparison(QChar c) {
      assert(this->comparison && "This should not have been called!");
      #pragma region Code to finish parsing a comparison (i.e. the operator and righthand side)
         if (this->comparison->op.isEmpty()) {
            //
            // If the statement doesn't have an operator stored, then the operator is currently 
            // being parsed and exists in (token).
            //
            if (string_scanner::is_operator_char(c)) {
               this->token.text += c;
               return;
            }
            auto c = this->comparison;
            c->op = this->token.text;
            if (!_is_comparison_operator(this->token.text))
               this->raise_error(QString("Operator %1 is not a valid comparison operator.").arg(this->token.text));
            this->reset_token();
            //
            // Fall through to righthand-side handling so we don't miss the first character 
            // after the operator in cases like {a==b} where there's no whitespace.
            //
         }
         //
         // Handle the righthand side.
         //
         if ((!this->token.text.isEmpty() || c != '-') && string_scanner::is_operator_char(c)) {
            this->raise_fatal(QString("Unexpected %1 on the righthand side of a comparison statement.").arg(c));
            return;
         }
         if (string_scanner::is_quote_char(c)) {
            this->raise_error(QString("Unexpected %1. You cannot compare variables to strings.").arg(c));
            //
            // TODO: Do we need to advance past the current character with ++this->state.pos first?
            //
            if (!this->skip_to(c))
               this->raise_fatal("Unable to find the closing quote for the string literal. Parsing cannot continue.");
            else {
               this->reset_token();
               delete this->comparison;
               this->comparison = nullptr;
            }
            return;
         }
         if (this->token.text.isEmpty() && string_scanner::is_whitespace_char(c))
            return;
         if (c == "(") {
            if (!this->token.text.isEmpty())
               this->raise_fatal(QString("Unexpected %1. You cannot compare variables to the result of a function call.").arg(c));
            this->raise_fatal(QString("Unexpected %1. Parentheses are only allowed as delimiters for function arguments.").arg(c));
            return;
         }
         if (c == ')' || c == ',') {
            this->raise_fatal(QString("Unexpected %1.").arg(c));
            return;
         }
         if (!string_scanner::is_whitespace_char(c)) {
            if (this->token.text.isEmpty())
               this->token.pos = this->backup_stream_state();
            this->token.text += c;
            return;
         }
      #pragma endregion
      //
      // If we get here, then we've encountered the end of the statement's righthand side.
      //
      #pragma region Code to compile a comparison
         this->comparison->set_end(this->state);
         this->comparison->rhs = this->__parseVariable(this->token.text);
         this->comparison->rhs->owner = this->comparison;
         //
         {
            auto lhs  = this->assignment->lhs;
            auto rhs  = this->assignment->rhs;
            bool fail = false;
            if (lhs->get_accessor_definition()) {
               this->raise_error(QString("Cannot use accessors such as \"%1\" in comparisons.").arg(lhs->to_string()));
               fail = true;
            }
            if (rhs->get_accessor_definition()) {
               this->raise_error(QString("Cannot use accessors such as \"%1\" in comparisons.").arg(rhs->to_string()));
               fail = true;
            }
            if (!fail) {
               auto opcode = std::make_unique<Condition>();
               //
               auto base = &_get_comparison_opcode();
               opcode->function = base;
               opcode->arguments.resize(3);
               opcode->arguments[0] = (base->arguments[0].typeinfo.factory)();
               opcode->arguments[1] = (base->arguments[1].typeinfo.factory)();
               opcode->arguments[2] = (base->arguments[2].typeinfo.factory)();
               //
               auto result = opcode->arguments[0]->compile(*this, *lhs, 0);
               if (!result.is_success()) {
                  QString error = "The lefthand side of this comparison failed to compile. ";
                  if (!result.error.isEmpty())
                     error += result.error;
                  this->raise_error(error);
               } else
                  assert(!result.is_unresolved_string() && "The lefthand side of a comparison statement thinks it's an unresolved string reference.");
               //
               result = opcode->arguments[1]->compile(*this, *rhs, 0);
               if (!result.is_success()) {
                  QString error = "The righthand side of this comparison failed to compile. ";
                  if (!result.error.isEmpty())
                     error += result.error;
                  this->raise_error(error);
               } else
                  assert(!result.is_unresolved_string() && "The righthand side of a comparison statement thinks it's an unresolved string reference.");
               //
               auto op_string = string_scanner(this->assignment->op);
               result = opcode->arguments[2]->compile(*this, op_string, 0);
               if (!result.is_success()) {
                  QString error = "The operator in this comparison failed to compile. ";
                  if (!result.error.isEmpty())
                     error += result.error;
                  this->raise_error(error);
               } else
                  assert(!result.is_unresolved_string() && "The operator in a comparison statement thinks it's an unresolved string reference.");
               //
               this->comparison->opcode = opcode.release();
            }
         }
         this->_applyConditionModifiers(this->comparison);
         //
         this->block->insert_condition(this->comparison);
         this->comparison = nullptr;
         this->reset_token();
      #pragma endregion
   }
   //
   void Compiler::__parseFunctionArgs(const OpcodeBase& function, Opcode& opcode, Compiler::unresolved_str_list& unresolved_strings) {
      auto& mapping = function.mapping;
      opcode.arguments.resize(function.arguments.size());
      //
      int8_t opcode_arg_index = 0;
      int8_t opcode_arg_part  = 0;
      int8_t script_arg_index = 0;
      std::unique_ptr<OpcodeArgValue> current_argument = nullptr;
      bool comma = false;
      do {
         if (opcode_arg_index >= mapping.mapped_arg_count()) {
            this->raise_error("Too many arguments passed to the function.");
            return;
         }
         QString raw_argument;
         {
            comma = false;
            //
            QChar delim = '\0';
            this->scan([this, &comma, &delim, &raw_argument](QChar c) {
               if (delim == '\0') { // can't use a constexpr for the "none" value because lambdas don't like that, and can't use !delim because a null QChar doesn't test as false, UGH
                  if (c == ',' || c == ')') {
                     comma = (c == ',');
                     return true; // stop
                  }
                  if (c == '[')
                     delim = ']';
                  else if (string_scanner::is_quote_char(c))
                     delim = c;
               } else {
                  if (c == delim)
                     delim = '\0';
               }
               raw_argument += c;
               return false;
            });
         }
         //
         auto& base = function.arguments[mapping.arg_index_mappings[script_arg_index]];
         if (!current_argument) {
            current_argument.reset((base.typeinfo.factory)());
            if (!current_argument) {
               this->raise_error("Unknown error: failed to instantiate an OpcodeArgValue while parsing arguments to the function call.");
               return;
            }
         }
         //
         string_scanner argument(raw_argument);
         arg_compile_result result = current_argument->compile(*this, argument, opcode_arg_part);
         if (!argument.is_at_effective_end()) {
            this->raise_error(QString("Failed to parse script argument %1. There was unexpected content at the end of the argument.").arg(script_arg_index));
            return;
         }
         bool failure = result.is_failure();
         bool success = result.is_success();
         if (failure) {
            bool irresolvable = result.is_irresolvable_failure();
            //
            QString error = QString("Failed to parse script argument %1.").arg(script_arg_index);
            if (!result.error.isEmpty()) {
               error.reserve(error.size() + 1 + result.error.size());
               error += ' ';
               error += result.error;
            }
            if (irresolvable) {
               error += ' ';
               error += "This error has made it impossible to attempt to parse this function call's remaining arguments. They will be skipped.";
            }
            this->raise_error(error);
            if (irresolvable)
               return;
         }
         bool needs_more = result.needs_another();
         bool another    = needs_more || result.can_take_another();
         if (needs_more && !comma) {
            this->raise_error("Not enough arguments passed to the function.");
            return;
         }
         if (success) {
            if (result.is_unresolved_string())
               unresolved_strings.emplace_back(*current_argument.get(), result.get_unresolved_string(), opcode_arg_part);
            opcode.arguments[opcode_arg_index] = current_argument.release();
         }
         script_arg_index++;
         if (another)
            ++opcode_arg_part;
         else {
            ++opcode_arg_index;
            opcode_arg_part = 0;
         }
      } while (comma);
      if (opcode_arg_index < mapping.mapped_arg_count())
         this->raise_error("Not enough arguments passed to the function.");
   }
   namespace {
      template<typename T, int I> void _find_opcode_bases(const std::array<T, I>& list, std::vector<const OpcodeBase*>& results, QString function_name, Script::VariableReference* context) {
         for (auto& action : list) {
            auto& mapping = action.mapping;
            if (context) {
               if (mapping.arg_context == OpcodeFuncToScriptMapping::no_context || mapping.arg_context == OpcodeFuncToScriptMapping::game_namespace)
                  continue;
               auto& base = action.arguments[mapping.arg_context];
               if (&base.typeinfo != context->get_type())
                  continue;
            } else {
               if (mapping.arg_context != OpcodeFuncToScriptMapping::no_context)
                  continue;
            }
            if (cobb::qt::stricmp(function_name, mapping.primary_name) == 0 || cobb::qt::stricmp(function_name, mapping.secondary_name) == 0)
               results.push_back(&action);
         }
      }
      template<typename T, int I> void _find_game_ns_opcode_bases(const std::array<T, I>& list, std::vector<const OpcodeBase*>& results, QString function_name) {
         for (auto& action : list) {
            auto& mapping = action.mapping;
            if (mapping.arg_context != OpcodeFuncToScriptMapping::game_namespace)
               continue;
            if (cobb::qt::stricmp(function_name, mapping.primary_name) == 0 || cobb::qt::stricmp(function_name, mapping.secondary_name) == 0)
               results.push_back(&action);
            if (cobb::qt::stricmp(function_name, mapping.primary_name) == 0 || cobb::qt::stricmp(function_name, mapping.secondary_name) == 0)
               results.push_back(&action);
         }
      }
   }
   void Compiler::_parseFunctionCall(bool is_condition) {
      //
      // When this function is called, the stream position should be just after the 
      // opening parentheses for the call arguments. Assuming no syntax errors are 
      // encountered, this function advances the stream position to just after the 
      // ")" glyph that marks the end of the call arguments; if you are calling 
      // this function from inside a functor being run by MSimpleParser.scan, then 
      // you will need to rewind the stream position by one to avoid skipping the 
      // glyph after the ")", because the functor is being run in a loop and the 
      // loop will advance the stream by one more character.
      //
      // At the time this function is called, (this->token) should refer to the 
      // combined context (if any) and function name.
      //
      // Called from _parseActionStart, _parseConditionStart, and _parseAssignment.
      //
      auto call_start = this->token.pos;
      QString function_name;
      std::unique_ptr<Script::VariableReference> context = nullptr;
      bool context_is_game = false;
      {  // Identify the context and the function name, i.e. context.function_name(arg, arg, arg)
         auto& text = this->token.text;
         //
         int size = text.size();
         int i    = size - 1;
         for (; i >= 0; --i) {
            auto c = text[i];
            if (QString("[]").indexOf(c) >= 0) {
               this->raise_fatal(call_start, "Function names cannot contain square brackets.");
               return;
            }
            if (c == '.') {
               function_name = text.chopped(i + 1); // oh, but be sure not to confuse this with (QString::chop), which does the literal exact opposite!
               break;
            }
         }
         if (function_name.isEmpty()) { // there was no '.', or it was at the end
            //
            // There was no ".", or it was at the end of the string. The latter case is a syntax 
            // error; the former case means this is a non-member functon and the entirety of (text) 
            // is the function name.
            //
            if (i == size - 1) { // "name.()"
               this->raise_fatal(call_start, "Constructions of the form {name.()} are syntax errors. A function name is required.");
               return;
            }
            function_name = text;
         } else {
            //
            // We're a member function.
            //
            text = text.mid(0, i);
            if (text.compare("game", Qt::CaseInsensitive) == 0) {
               //
               // The "game" namespace is allowed to contain member functions. VariableReferences 
               // cannot resolve to namespaces, so we have to handle this here in the compiler.
               //
               context_is_game = true;
            } else {
               context.reset(this->__parseVariable(text));
               if (this->has_fatal()) // the VariableReference may contain a syntax error
                  return;
               //
               // Handle errors that may have occurred when resolving the variable.
               //
               if (context->is_invalid) {
                  this->raise_error(call_start, QString("Unable to identify the context of the function call; cannot determine what type function \"%1\" is a member of.").arg(function_name));
                  //
                  // Try to skip to the end of the function call so that parsing can continue.
                  //
                  if (!this->skip_to(')'))
                     this->raise_fatal("Unable to locate the nearest ')' glyph; possible unterminated function call. Parsing cannot continue.");
                  return;
               }
            }
         }
      }
      this->reset_token();
      ++this->state.offset; // advance past the open-paren
      //
      if (!context && !context_is_game) {  // Handle user-defined function calls
         auto func = this->lookup_user_defined_function(function_name);
         if (func) {
            if (is_condition) {
               this->raise_error(call_start, QString("User-defined functions such as \"%1\" cannot be called from inside of a condition.").arg(function_name));
               if (!this->skip_to(')'))
                  this->raise_fatal("Unable to locate the nearest ')' glyph; possible unterminated function call. Parsing cannot continue.");
               return;
            }
            if (this->assignment) {
               this->raise_error(call_start, QString("User-defined functions such as \"%1\" cannot return values.").arg(function_name));
               if (!this->skip_to(')'))
                  this->raise_fatal("Unable to locate the nearest ')' glyph; possible unterminated function call. Parsing cannot continue.");
               return;
            }
            if (!this->extract_specific_char(')')) {
               this->raise_error(call_start, QString("Expected ')'. User-defined functions such as \"%1\" cannot have arguments passed to them.").arg(function_name));
               if (!this->skip_to(')'))
                  this->raise_fatal("Unable to locate the nearest ')' glyph; possible unterminated function call. Parsing cannot continue.");
               return;
            }
            //
            // Now, we need to compile a call to the user-defined function.
            //
            // Normally, we only create a Block's Trigger when we're compiling the block in full. 
            // However, in order to compile calls to a user-defined function as we find them, we 
            // need to be able to know the index of that function's final trigger... which means 
            // that the function needs to *have* a trigger. So, we create the trigger at the 
            // time that we open the function.
            //
            assert(func->trigger_index >= 0 && "A user-defined function, tracked by the compiler, has not had its trigger index stored properly.");
            //
            auto  opcode = new Action;
            auto& base   = actionFunction_runNestedTrigger;
            auto  arg    = (base.arguments[0].typeinfo.factory)();
            opcode->function = &base;
            opcode->arguments.push_back(arg);
            auto arg_c   = dynamic_cast<OpcodeArgValueTrigger*>(arg);
            assert(arg_c && "The argument to the ''run nested trigger'' opcode isn't OpcodeArgValueTrigger anymore? Did someone change the opcode-base?");
            arg_c->value = func->trigger_index;
            //
            auto statement = new Script::Statement;
            statement->opcode = opcode;
            statement->set_start(call_start);
            statement->set_end(this->state);
            this->block->insert_item(statement);
            return;
         }
         //
         // If we get here, then the non-member function was not a user-defined function. Fall 
         // through to looking for built-in non-member functions.
         //
      }
      //
      std::vector<const OpcodeBase*> opcode_bases;
      if (context_is_game) {
         if (is_condition)
            _find_game_ns_opcode_bases(conditionFunctionList, opcode_bases, function_name);
         else
            _find_game_ns_opcode_bases(actionFunctionList, opcode_bases, function_name);
      } else {
         if (is_condition)
            _find_opcode_bases(conditionFunctionList, opcode_bases, function_name, context.get());
         else
            _find_opcode_bases(actionFunctionList, opcode_bases, function_name, context.get());
      }
      if (!opcode_bases.size()) {
         if (context)
            this->raise_error(call_start, QString("Type %1 does not have a member function named \"%2\".").arg(context->get_type()->internal_name.c_str()).arg(function_name));
         else if (context_is_game)
            this->raise_error(call_start, QString("The game namespace does not have a member function named \"%1\".").arg(function_name));
         else
            this->raise_error(call_start, QString("There is no non-member function named \"%1\".").arg(function_name));
         //
         // Try to skip to the end of the function call so that parsing can continue.
         //
         if (!this->skip_to(')'))
            this->raise_fatal("Unable to locate the nearest ')' glyph; possible unterminated function call. Parsing cannot continue.");
         return;
      }
      //
      const OpcodeBase* match = nullptr;
      std::unique_ptr<Opcode> opcode;
      if (is_condition) {
         opcode.reset(new Condition);
      } else {
         opcode.reset(new Action);
      }
      auto start = this->backup_stream_state();
      auto check = this->create_log_checkpoint();
      unresolved_str_list unresolved_strings;
      for (auto* function : opcode_bases) {
         //
         // If two opcodes have the same name and context (or lack thereof), then they are overloads of 
         // each other with different arguments. Use trial-and-error to determine which one the script 
         // author is invoking.
         //
         opcode->reset();
         unresolved_strings.clear();
         this->revert_to_log_checkpoint(check);
         this->restore_stream_state(start);
         //
         this->__parseFunctionArgs(*function, *opcode.get(), unresolved_strings);
         if (!this->checkpoint_has_errors(check)) {
            match = function;
            break;
         }
      }
      if (!match) {
         if (opcode_bases.size() > 1) {
            this->revert_to_log_checkpoint(check);
            this->raise_error(call_start, QString("The arguments you passed to %1.%2 did not match any of its function signatures.").arg(context->get_type()->internal_name.c_str()).arg(function_name));
         }
         if (!this->skip_to(')'))
            this->raise_fatal("Unable to locate the nearest ')' glyph; possible unterminated function call. Parsing cannot continue.");
         return;
      }
      //
      // If we've reached this point without any errors, then we should be just before the terminating ')' for the function call.
      //
      if (!this->extract_specific_char(')')) {
         this->raise_fatal("Expected ')'.");
         return;
      }
      if (context) {
         auto index = match->mapping.arg_context;
         const OpcodeArgBase& base = match->arguments[index];
         opcode->arguments[index] = (base.typeinfo.factory)();
         opcode->arguments[index]->compile(*this, *context, 0);
      }
      if (this->assignment) {
         //
         // We're assigning the return value of this function call to something, so let's first make 
         // sure that the function actually returns a value.
         //
         auto index = match->index_of_out_argument();
         bool fail  = false;
         if (index < 0) {
            this->raise_error(call_start, QString("Function %1.%2 does not return a value.").arg(context->get_type()->internal_name.c_str()).arg(function_name));
            fail = true;
         }
         if (this->assignment->lhs->is_read_only()) {
            this->raise_error("You cannot assign to this value.");
            fail = true;
         } else if (this->assignment->lhs->is_accessor()) {
            this->raise_error("You cannot assign the return value of a function to an accessor.");
            fail = true;
         }
         //
         const OpcodeArgBase& base = match->arguments[index];
         //
         // Verify that the variable we're assigning our return value to is of the right type:
         //
         auto target_type = this->assignment->lhs->get_type();
         if (&base.typeinfo != target_type) {
            this->raise_error(call_start, QString("Function %1.%2 returns a %3, not a %4.")
               .arg(context->get_type()->internal_name.c_str())
               .arg(function_name)
               .arg(base.typeinfo.internal_name.c_str())
               .arg(target_type->internal_name.c_str())
            );
            fail = true;
         }
         //
         if (!fail) {
            //
            // The type is correct, so set the out-argument.
            //
            opcode->arguments[index] = (base.typeinfo.factory)();
            opcode->arguments[index]->compile(*this, *this->assignment->lhs, 0);
            //
            if (match->mapping.flags & OpcodeFuncToScriptMapping::flags::secondary_property_zeroes_result) {
               //
               // There are several opcodes that will return a result only if there is a result 
               // to return. The function to get a player's Armor Ability, for example, will only 
               // write to the specified object variable if the player has an Armor Ability; if 
               // the player does not, then the variable is not modified (as opposed to clearing 
               // it). The OpcodeFuncToScriptMapping class allows opcodes to have two names, and 
               // offers a flag which indicates alternate behavior for the second name. This 
               // allows us to do this:
               //
               //    some_object = current_player.get_armor_ability()
               //
               // as a shorthand for this:
               //
               //    some_object = no_object
               //    some_object = current_player.try_get_armor_ability()
               //
               // We just compile an assignment to none/zero.
               //
               if (cobb::qt::stricmp(function_name, match->mapping.secondary_name) == 0) {
                  auto base  = &_get_assignment_opcode();
                  auto blank = new Action;
                  blank->function = base;
                  blank->arguments.resize(3);
                  blank->arguments[0] = (base->arguments[0].typeinfo.factory)(); // lhs
                  auto result = blank->arguments[0]->compile(*this, *this->assignment->lhs, 0);
                  if (result.is_failure())
                     this->raise_error("Failed to compile the lefthand side of an implicit assignment (before a function call).");
                  //
                  auto lhs = dynamic_cast<OpcodeArgValueAnyVariable*>(blank->arguments[0]);
                  assert(lhs && "Each side of the assignment opcode should be an OpcodeArgValueAnyVariable. If for any reason this has changed, update the compiler code.");
                  auto rhs = blank->arguments[1] = lhs->create_zero_or_none(); // rhs
                  if (!rhs)
                     this->raise_error("Failed to compile the righthand side of an implicit assignment (before a function call).");
                  //
                  auto op_string = string_scanner("=");
                  blank->arguments[2] = (base->arguments[2].typeinfo.factory)(); // operator
                  result = blank->arguments[2]->compile(*this, op_string, 0);
                  if (result.is_failure())
                     this->raise_error("Failed to compile the operator in an implicit assignment (before a function call).");
                  //
                  auto statement = new Script::Statement;
                  statement->opcode = blank;
                  this->block->insert_item(statement);
               }
            }
         }
      }
      this->reset_token();
      Script::Statement* statement = this->assignment;
      if (!statement) {
         if (is_condition)
            statement = new Script::Comparison;
         else
            statement = new Script::Statement;
      }
      statement->opcode = opcode.release();
      statement->set_end(this->state);
      if (is_condition) {
         this->_applyConditionModifiers(dynamic_cast<Script::Comparison*>(statement));
         this->block->insert_condition(statement);
      } else {
         this->block->insert_item(statement);
      }
      this->_commit_unresolved_strings(unresolved_strings);
      this->assignment = nullptr;
   }
   //
   bool Compiler::is_in_statement() const {
      return this->assignment || this->comparison;
   }
   int32_t Compiler::_index_of_trigger(Trigger* t) const noexcept {
      auto&  list = this->results.triggers;
      size_t size = list.size();
      for (size_t i = 0; i < size; ++i)
         if (list[i] == t)
            return i;
      return -1;
   }
   VariableDeclarationSet* Compiler::_get_variable_declaration_set(variable_scope vs) noexcept {
      auto& sets = this->results.variables;
      switch (vs) {
         case variable_scope::global:
            return &sets.global;
         case variable_scope::object:
            return &sets.object;
         case variable_scope::player:
            return &sets.player;
         case variable_scope::team:
            return &sets.team;
      }
      return nullptr;
   }
   //
   void Compiler::_openBlock(Script::Block* block) { // (block) should already have been appended to its parent
      this->block = block;
      auto root = this->root;
      if (block->parent == root) {
         //
         // We're opening a top-level Block. Before we do that, let's see if the root block contains any 
         // statements; if so, let's put those in their own trigger.
         //
         // Because (block) was already added to the root, we will need to remove it in order to avoid 
         // accidentally compiling it early or (in Block::clear) deleting it early.
         //
         root->remove_item(block);
         //
         assert(root->trigger == nullptr);
         root->compile(*this);
         root->trigger = nullptr;
         root->clear();
         //
         root->insert_item(block);
      }
   }
   bool Compiler::_closeCurrentBlock() {
      if (this->block == this->root)
         return false;
      this->block->set_end(this->state);
      {  // The block's aliases are going out of scope.
         size_t size = this->aliases_in_scope.size();
         if (size > 0) {
            signed int i = size - 1;
            for (; i >= 0; --i) {
               auto alias = this->aliases_in_scope[i];
               if (alias->parent != this->block)
                  break;
            }
            ++i; // convert from "index of last alias to keep" to "number of aliases to keep"
            if (i < size)
               this->aliases_in_scope.resize(i);
         }
      }
      {  // The block's contained functions are going out of scope.
         auto& list = this->functions_in_scope;
         list.erase(std::remove_if(list.begin(), list.end(),
            [this](Script::UserDefinedFunction& entry) {
               auto func_parent = entry.parent;
               if (!func_parent || func_parent == this->block)
                  return true;
               return false;
            }),
            list.end()
         );
      }
      auto parent = dynamic_cast<Script::Block*>(this->block->parent);
      if (!parent)
         return false;
      if (parent == this->root) {
         //
         // Only compile this block and its descendants if it was a top-level block.
         //
         // It's tempting to compile every block, including nested ones, as it closes, but that 
         // would result in a trigger order inconsistent with Bungie and 343i's gametypes. Inner 
         // blocks would close before their containing outer blocks, so their triggers would end 
         // up being numbered before the outer blocks' triggers, whereas in official gametypes 
         // the triggers are numbered from the outside in and from the top down.
         //
         this->block->compile(*this);
         //
         // And now that the block is compiled, discard it.
         //
         parent->remove_item(this->block);
         delete this->block;
      }
      this->block = parent;
      return true;
   }
   //
   void Compiler::_handleKeyword_Alias() {
      auto start = this->token.pos;
      //
      auto name = this->extract_word();
      if (name.isEmpty()) {
         this->raise_fatal("An alias declaration must supply a name.");
         return;
      }
      if (!this->extract_specific_char('=')) {
         this->raise_fatal("Expected \"=\".");
         return;
      }
      auto target = this->extract_word();
      if (target.isEmpty()) {
         this->raise_fatal("An alias declaration must supply a target.");
         return;
      }
      //
      auto item = new Script::Alias(*this, name, target);
      if (this->has_fatal()) { // the alias name had a fatal error e.g. using a keyword as a name
         delete item;
         return;
      }
      item->set_start(start);
      item->set_end(this->state);
      this->block->insert_item(item);
      if (!item->invalid) // aliases can also run into non-fatal errors
         this->aliases_in_scope.push_back(item);
   }
   //
   void Compiler::_declare_variable(Script::VariableReference& variable, Script::VariableReference* initial, VariableDeclaration::network_enum networking, bool network_specified) {
      auto type  = variable.get_type();
      auto basis = variable.get_alias_basis_type();
      //
      variable_scope scope_v = getVariableScopeForTypeinfo(basis);
      variable_type  type_v  = getVariableTypeForTypeinfo(type);
      if (scope_v == variable_scope::not_a_scope) {
         this->raise_error("Encountered a problem when trying to interpret this variable declaration: bad scope.");
         return;
      }
      if (type_v == variable_type::not_a_variable) {
         this->raise_error("Encountered a problem when trying to interpret this variable declaration: bad variable type.");
         return;
      }
      //
      int32_t index = 0;
      if (scope_v == variable_scope::global) {
         index = variable.resolved.top_level.index;
      } else {
         index = variable.resolved.nested.index;
      }
      //
      auto set  = this->_get_variable_declaration_set(scope_v);
      auto decl = set->get_or_create_declaration(type_v, index);
      assert(decl && "Failed to get-or-create variable declaration.");
      if (decl->is_implicit()) {
         decl->make_explicit();
      } else {
         //
         // This is a redeclaration of a previously declared variable. Raise an error if the redeclaration is 
         // inconsistent with any prior declaration, or a warning otherwise.
         //
         auto str = variable.to_string();
         bool bad = false;
         if (initial && !decl->initial_value_is_implicit()) {
            bad = true;
            this->raise_error(QString("This is a redeclaration of variable %1. The redeclaration species an initial value even though a previous declaration already provided one.").arg(str));
         }
         if (network_specified && !decl->networking_is_implicit()) {
            bad = true;
            this->raise_error(QString("This is a redeclaration of variable %1. The redeclaration species a network type even though a previous declaration already provided one.").arg(str));
         }
         if (bad) {
            return;
         } else {
            this->raise_warning(QString("This is a redeclaration of variable %1.").arg(str));
         }
      }
      if (network_specified) {
         decl->make_networking_explicit();
         //
         if (decl->has_network_type()) {
            decl->networking = networking;
         } else {
            this->raise_error("Variables of this type cannot have a networking type set.");
         }
      }
      if (initial) {
         decl->make_initial_value_explicit();
         if (decl->has_initial_value()) {
            if (decl->get_type() == variable_type::team) {
               bool success;
               auto team = initial->to_const_team(&success);
               if (!success) {
                  this->raise_error("Variables of this type can only use constant teams as their initial values (i.e. team[0], neutral_team, no_team, etc.).");
                  return;
               }
               decl->initial.team = team;
            } else {
               if (initial->get_type() != &OpcodeArgValueScalar::typeinfo) {
                  this->raise_error("Variables of this type can only use numeric values as their initial values.");
                  return;
               }
               auto result = decl->initial.number->compile(*this, *initial, 0);
               if (result.is_failure())
                  this->raise_error("Failed to compile this variable declaration's initial value.");
            }
         } else {
            this->raise_error("Variables of this type cannot have initial values provided.");
         }
      }
   }
   void Compiler::_handleKeyword_Declare() {
      auto  start = this->token.pos;
      using net_t = Megalo::variable_network_priority;
      //
      // declare [word]
      // declare [word] = [value]
      // declare [word] with network priority [word]
      // declare [word] with network priority [word] = [value]
      //
      QString str_variable = this->extract_word();
      QString str_initial;
      if (str_variable.isEmpty()) {
         this->raise_fatal("Expected a variable name.");
         return;
      }
      if (Compiler::is_keyword(str_variable)) {
         this->raise_fatal(QString("A keyword such as \"%1\" cannot be used as the initial value of a variable being declared.").arg(str_variable));
         return;
      }
      //
      bool  has_network = false;
      net_t network     = net_t::default;
      #pragma region Parsing and extracting all relevant tokens
         auto  after_name = this->backup_stream_state();
         auto  word       = this->extract_word();
         if (word.isEmpty()) {
            //
            // This is either (declare name) or (declare name = ...).
            //
            if (this->extract_specific_char('=')) {
               int32_t int_initial;
               auto    result = this->extract_integer_literal(int_initial);
               if (result == extract_result::floating_point) {
                  this->raise_error("You cannot initialize a variable to a floating-point value.");
               } else if (result == extract_result::success) {
                  str_initial = QString("%1").arg(int_initial);
               } else if (result == extract_result::failure) {
                  str_initial = this->extract_word();
                  if (Compiler::is_keyword(str_initial)) {
                     this->raise_fatal(QString("A keyword such as \"%1\" cannot be used as the initial value of a variable being declared.").arg(str_initial));
                     return;
                  }
               }
            }
         } else {
            if (word.compare("with", Qt::CaseInsensitive) == 0) {
               if (!this->extract_word("network")) {
                  this->raise_fatal("Expected the word \"network\".");
                  return;
               }
               if (!this->extract_word("priority")) {
                  this->raise_fatal("Expected the word \"network\".");
                  return;
               }
               //
               has_network = true;
               //
               word = this->extract_word();
               if (word.isEmpty()) {
                  this->raise_fatal("Expected a network priority (\"default\", \"low\", \"high\", or \"local\").");
                  return;
               }
               if (word.compare("default", Qt::CaseInsensitive) == 0) {
                  network = net_t::default;
               } else if (word.compare("low", Qt::CaseInsensitive) == 0) {
                  network = net_t::low;
               } else if (word.compare("high", Qt::CaseInsensitive) == 0) {
                  network = net_t::high;
               } else if (word.compare("local", Qt::CaseInsensitive) == 0) {
                  network = net_t::none;
               } else {
                  this->raise_fatal(QString("Word \"%1\" is not a network priority.").arg(word));
                  return;
               }
               //
               // Next, grab an initial value if there is one:
               //
               if (this->extract_specific_char('=')) {
                  int32_t int_initial;
                  auto    result = this->extract_integer_literal(int_initial);
                  if (result == extract_result::floating_point) {
                     this->raise_error("You cannot initialize a variable to a floating-point value.");
                  } else if (result == extract_result::success) {
                     str_initial = QString("%1").arg(int_initial);
                  } else if (result == extract_result::failure) {
                     str_initial = this->extract_word();
                     if (Compiler::is_keyword(str_initial)) {
                        this->raise_fatal(QString("A keyword such as \"%1\" cannot be used as the initial value of a variable being declared.").arg(str_initial));
                        return;
                     }
                  }
               }
            } else {
               //
               // The word was not what we expected. That means that this must be a (declare name) 
               // statement with no initial value.
               //
               this->restore_stream_state(after_name);
            }
         }
      #pragma endregion
      //
      Script::VariableReference* variable = nullptr;
      Script::VariableReference* initial  = nullptr;
      //
      variable = new Script::VariableReference(*this, str_variable);
      variable->resolve(*this, true);
      if (variable->is_invalid) {
         delete variable;
         if (!str_initial.isEmpty())
            this->raise_error("Could not identify the variable being declared; this means that the initial value cannot be checked for errors.");
         return;
      }
      {  // Validate (variable).
         bool bad = true;
         if (variable->is_property() || variable->is_accessor()) {
            this->raise_error(start, QString("Invalid variable declaration. You cannot declare a property or accessor such as \"%1\".").arg(variable->to_string()));
         } else if (variable->is_statically_indexable_value()) {
            this->raise_error(start, QString("Invalid variable declaration. Value \"%1\" is a built-in value, always exists, and therefore cannot be declared.").arg(variable->to_string()));
         } else if (!variable->get_type()->is_variable()) {
            this->raise_error(start, QString("Invalid variable declaration. Value \"%1\" does not refer to a variable.").arg(variable->to_string()));
         } else {
            bad = false;
         }
         if (bad) {
            delete variable;
            return;
         }
         auto basis = variable->get_alias_basis_type();
         if (!basis) {
            auto top  = variable->resolved.top_level.type;
            auto nest = variable->resolved.nested.type;
            if (top && nest) {
               this->raise_error(start, QString("Invalid variable declaration. You cannot declare a specific nested variable e.g. global.player[0].number[1]; you can only declare nested variables within a scope e.g. player.number[1]."));
            }
         }
      }
      if (!str_initial.isEmpty()) {
         initial = new Script::VariableReference(*this, str_initial);
         initial->resolve(*this);
         if (initial->is_invalid) {
            delete variable;
            delete initial;
            return;
         }
         if (initial->is_property() || initial->is_accessor()) {
            this->raise_error(QString("Invalid variable declaration. Properties and accessors cannot be used as the initial values of variables."));
            delete variable;
            delete initial;
            return;
         }
         if (initial->get_type() == &OpcodeArgValueScalar::typeinfo) {
            auto& res = initial->resolved;
            auto& top = res.top_level;
            if (top.type && top.type->is_variable() && !top.is_static && !top.is_constant) {
               this->raise_error("When declaring one variable, you cannot use another variable as the initial value.");
               return;
            }
            if (res.nested.type) {
               this->raise_error("When declaring one variable, you cannot use another variable as the initial value.");
               return;
            }
         }
      }
      this->_declare_variable(*variable, initial, network, has_network);
      //
      delete variable;
      if (initial)
         delete initial;
   }

   void Compiler::_handleKeyword_Do() {
      auto item = new Script::Block;
      item->type = Script::Block::Type::basic;
      item->set_start(this->token.pos);
      item->event = this->next_event;
      this->next_event = Script::Block::Event::none;
      this->block->insert_item(item);
      this->_openBlock(item);
   }
   void Compiler::_handleKeyword_Else() {
      if (this->block->type != Script::Block::Type::if_block && this->block->type != Script::Block::Type::elseif_block) {
         auto prev = this->block->item(-1);
         auto p_bl = dynamic_cast<Script::Block*>(prev);
         if (p_bl) {
            if (p_bl->type == Script::Block::Type::if_block || p_bl->type == Script::Block::Type::elseif_block)
               this->raise_fatal("Unexpected \"else\". This keyword should not be preceded by the \"end\" keyword.");
         }
         this->raise_fatal("Unexpected \"else\".");
         return;
      }
      if (!this->_closeCurrentBlock()) {
         this->raise_fatal("Unexpected \"else\".");
         return;
      }
      auto item = new Script::Block;
      item->type = Script::Block::Type::else_block;
      item->set_start(this->token.pos);
      this->block->insert_item(item);
      this->_openBlock(item);
   }
   void Compiler::_handleKeyword_ElseIf() {
      if (this->block->type != Script::Block::Type::if_block && this->block->type != Script::Block::Type::elseif_block) {
         auto prev = this->block->item(-1);
         auto p_bl = dynamic_cast<Script::Block*>(prev);
         if (p_bl) {
            if (p_bl->type == Script::Block::Type::if_block || p_bl->type == Script::Block::Type::elseif_block)
               this->raise_fatal("Unexpected \"elseif\". This keyword should not be preceded by the \"end\" keyword.");
         }
         this->raise_fatal("Unexpected \"elseif\".");
         return;
      }
      if (!this->_closeCurrentBlock()) {
         this->raise_fatal("Unexpected \"elseif\".");
         return;
      }
      auto item = new Script::Block;
      item->type = Script::Block::Type::elseif_block;
      item->set_start(this->token.pos);
      this->block->insert_item(item);
      this->_openBlock(item);
      this->_parseBlockConditions();
      this->reset_token();
   }
   void Compiler::_handleKeyword_End() {
      if (!this->_closeCurrentBlock())
         this->raise_fatal("Unexpected \"end\".");
   }
   void Compiler::_handleKeyword_If() {
      auto item = new Script::Block;
      item->type = Script::Block::Type::if_block;
      item->set_start(this->token.pos);
      item->event = this->next_event;
      this->next_event = Script::Block::Event::none;
      this->block->insert_item(item);
      this->_openBlock(item);
      this->_parseBlockConditions();
      this->reset_token();
   }
   void Compiler::_handleKeyword_For() {
      auto start = this->token.pos;
      //
      if (!this->extract_word("each")) {
         this->raise_fatal("The \"for\" keyword must be followed by \"each\".");
         return;
      }
      auto word = this->extract_word();
      if (word.isEmpty()) {
         this->raise_fatal("Invalid for-loop.");
         return;
      }
      auto    type = Script::Block::Type::basic;
      QString label;
      int32_t label_index = -1;
      bool    label_is_index = false;
      if (word == "team") {
         type = Script::Block::Type::for_each_team;
         if (!this->extract_word("do")) {
            this->raise_fatal("Invalid for-each-team loop: expected the word \"do\".");
            return;
         }
      } else if (word == "player") {
         type = Script::Block::Type::for_each_player;
         word = this->extract_word();
         if (word == "randomly") {
            type = Script::Block::Type::for_each_player_randomly;
            word = this->extract_word();
         }
         if (word != "do") {
            if (type == Script::Block::Type::for_each_player_randomly)
               this->raise_fatal("Invalid for-each-player-randomly loop: expected the word \"do\".");
            this->raise_fatal("Invalid for-each-player loop: expected the word \"randomly\" or the word \"do\".");
            return;
         }
      } else if (word == "object") {
         type = Script::Block::Type::for_each_object;
         word = this->extract_word();
         if (word != "do") {
            if (word != "with") {
               this->raise_fatal("Invalid for-each-object loop: expected the word \"with\" or the word \"do\".");
               return;
            }
            type = Script::Block::Type::for_each_object_with_label;
            word = this->extract_word();
            if (word == "no") {
               if (!this->extract_word("label")) {
                  this->raise_fatal("Invalid for-each-object-with-label loop: must use the phrase \"no label\" or specify a label.");
                  return;
               }
            } else {
               if (word != "label") {
                  this->raise_fatal("Invalid for-each-object-with-label loop: expected the word \"label\".");
                  return;
               }
               if (!this->extract_string_literal(label)) {
                  if (!this->extract_integer_literal(label_index)) {
                     this->raise_fatal("Invalid for-each-object-with-label loop: the label must be specified as a string literal or as a numeric label index.");
                     return;
                  }
                  label_is_index = true;
               }
            }
            if (!this->extract_word("do")) {
               this->raise_fatal("Invalid for-each-object-with-label loop: expected the word \"do\".");
               return;
            }
         }
      } else {
         this->raise_fatal("Invalid for-loop.");
         return;
      }
      //
      auto item = new Script::Block;
      item->type        = type;
      item->label_name  = label;
      item->label_index = label_index;
      item->set_start(start);
      item->event       = this->next_event;
      this->next_event  = Script::Block::Event::none;
      this->block->insert_item(item);
      this->_openBlock(item);
   }
   void Compiler::_handleKeyword_Function() {
      auto start = this->token.pos;
      //
      auto name = this->extract_word();
      if (name.isEmpty()) {
         this->raise_fatal("A function must have a name.");
         return;
      }
      if (name[0].isNumber()) {
         //
         // Do not allow a function's name to start with a number. We want this to be consistent with 
         // alias names, which disallow numbers at their start so that it's easier for opcode argument 
         // compile functions to check for both integer literals and integer alias names.
         //
         this->raise_error("A function's name cannot begin with a number.");
         //
         // This error shouldn't halt parsing. Set the function's name to empty to signal that the 
         // Block represents a function with an invalid name.
         //
         name = "";
      } else {
         for (QChar c : name) {
            if (QString("[].").contains(c)) {
               this->raise_fatal(QString("Unexpected %1 inside of a function name.").arg(c));
               return;
            }
         }
         if (Compiler::is_keyword(name)) {
            this->raise_fatal(QString("Keyword \"%1\" cannot be used as the name of a function.").arg(name));
            return;
         }
      }
      if (!this->extract_specific_char('(')) {
         this->raise_fatal("Expected \"(\".");
         return;
      }
      if (!this->extract_specific_char(')')) {
         this->raise_fatal("Expected \")\". User-defined functions cannot have arguments.");
         return;
      }
      if (!name.isEmpty()) { // Run additional checks on the function name.
         if (this->lookup_user_defined_function(name)) {
            this->raise_fatal(QString("A user-defined function named \"%1\" is already in scope. Functions cannot shadow each other.").arg(name));
            name = "";
         } else {
            //
            // Do not allow user-defined functions to shadow built-ins:
            //
            OpcodeArgTypeRegistry::type_list_t sources;
            auto built_in_type = Compiler::check_name_is_taken(name, sources);
            //
            bool fail = true;
            switch (built_in_type) {
               case name_source::action:
               case name_source::condition:
                  this->raise_error(QString("User-defined functions cannot shadow built-in functions such as %1.").arg(name));
                  break;
               case name_source::static_typename:
               case name_source::variable_typename:
                  this->raise_error(QString("User-defined functions cannot shadow built-in type names such as %1.").arg(name));
                  break;
               case name_source::namespace_member:
               case name_source::imported_name:
                  this->raise_error(QString("User-defined functions cannot shadow built-in values such as %1.").arg(name));
                  break;
               case name_source::none:
               default:
                  fail = false;
                  break;
            }
            if (fail)
               name = "";
         }
      }
      //
      auto item = new Script::Block;
      item->type = Script::Block::Type::function;
      item->name = name;
      item->set_start(start);
      item->event = this->next_event;
      this->next_event = Script::Block::Event::none;
      this->block->insert_item(item);
      this->_openBlock(item);
      //
      // Normally, we only create a Block's Trigger when we're compiling the block in full. However, 
      // in order to compile calls to this user-defined function as we find them, we need to know 
      // the index of the function's trigger... which means that the function needs to *have* a 
      // trigger. So, we'll create it early.
      //
      item->trigger = new Trigger;
      this->functions_in_scope.push_back(Script::UserDefinedFunction(name, this->results.triggers.size(), item)); // remember this function and its index
      this->results.triggers.push_back(item->trigger);
   }
   void Compiler::_handleKeyword_On() {
      if (this->block != this->root) {
         this->raise_error("Only top-level (non-nested) blocks can be event handlers.");
         if (!this->skip_to(':'))
            this->raise_fatal("Unable to locate the nearest ':' glyph. Parsing cannot continue.");
         return;
      }
      if (this->next_event != Script::Block::Event::none)
         this->raise_error("A single trigger cannot be the handler for multiple events.");
      QString words;
      auto    prior = this->backup_stream_state();
      while (!this->extract_specific_char(':')) {
         auto w = this->extract_word();
         if (w.isEmpty()) {
            this->raise_error(prior, "No valid event name specified.");
            if (!this->skip_to(':'))
               this->raise_fatal("Unable to locate the nearest ':' glyph. Parsing cannot continue.");
            return;
         }
         if (!words.isEmpty())
            words += ' ';
         words += w;
      }
      if (words.isEmpty()) {
         this->raise_error(prior, "No valid event name specified.");
         if (!this->skip_to(':'))
            this->raise_fatal("Unable to locate the nearest ':' glyph. Parsing cannot continue.");
         return;
      }
      auto event = Script::Block::Event::none;
      if (words.compare("init", Qt::CaseInsensitive) == 0) {
         this->next_event = Script::Block::Event::init;
      } else if (words.compare("local init", Qt::CaseInsensitive) == 0) {
         this->next_event = Script::Block::Event::local_init;
      } else if (words.compare("host migration", Qt::CaseInsensitive) == 0) {
         this->next_event = Script::Block::Event::host_migration;
      } else if (words.compare("double host migration", Qt::CaseInsensitive) == 0) {
         this->next_event = Script::Block::Event::double_host_migration;
      } else if (words.compare("object death", Qt::CaseInsensitive) == 0) {
         this->next_event = Script::Block::Event::object_death;
      } else if (words.compare("local", Qt::CaseInsensitive) == 0) {
         this->next_event = Script::Block::Event::local;
      } else if (words.compare("pregame", Qt::CaseInsensitive) == 0) {
         this->next_event = Script::Block::Event::pregame;
      }
      //
      if (this->next_event != Script::Block::Event::none) {
         auto et    = _block_event_to_trigger_entry(this->next_event);
         auto index = this->results.events.get_index_of_event(et);
         if (index != TriggerEntryPoints::none) {
            this->raise_error(QString("Only one trigger can be assigned to handle each event type. Event type \"%1\" is already in use.").arg(words));
            this->next_event = Script::Block::Event::__error;
         } else {
            this->results.events.set_index_of_event(et, TriggerEntryPoints::reserved);
         }
      } else {
         this->raise_error(prior, QString("Invalid event name: \"%s\".").arg(words));
      }
   }
}