#include "compiler.h"
#include "enums.h"
#include "namespaces.h"
#include "../../../helpers/qt/string.h"
#include "../opcode_arg_types/all_indices.h" // OpcodeArgValueTrigger
#include "../opcode_arg_types/variables/all_core.h"
#include "../opcode_arg_types/variables/any_variable.h"
#include "../opcode_arg_types/variables/base.h"

//
// Attempt to emulate quirks in Bungie's compiler:
//
//  - Else(if) blocks don't merge into the parent trigger even if they are the last 
//    block in their containing block, even though they can, and even though if-blocks 
//    do (confirmed)
//
//  - Comparisons that are reproduced and negated in else(if) blocks are negated by 
//    changing the operator rather than using the "negated" flag (speculated)
//
#define MEGALO_COMPILE_MIMIC_BUNGIE_ARTIFACTS 1

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
         //">>=",
         //"<<=",
         //">>>=",
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
         case Block::Type::altif_block:
         case Block::Type::alt_block:
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
         this->clear(true);
      }
      void Block::insert_condition(Comparison* item) {
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
      int32_t Block::index_of_item(const ParsedItem* target) const noexcept {
         auto size = this->items.size();
         for (size_t i = 0; i < size; ++i)
            if (this->items[i] == target)
               return i;
         return -1;
      }
      void Block::get_ifs_for_alt(std::vector<Block*>& out) {
         out.clear();
         auto parent = dynamic_cast<Block*>(this->parent);
         if (!parent)
            return;
         auto i = parent->index_of_item(this);
         if (--i < 0)
            return;
         std::vector<Block*> temp;
         for (; i >= 0; i--) {
            auto sibling = parent->items[i];
            auto block   = dynamic_cast<Block*>(sibling);
            if (!block)
               break;
            switch (block->type) {
               case Type::if_block:
               case Type::altif_block:
               case Type::alt_block:
                  temp.push_back(block);
            }
            if (block->type != Type::altif_block)
               break;
         }
         out.reserve(temp.size());
         for (auto it = temp.rbegin(); it != temp.rend(); ++it)
            out.push_back(*it);
      }
      void Block::make_alt_of(const Block& other) {
         size_t size_e = other.conditions_alt.size();
         size_t size_c = other.conditions.size();
         this->conditions_alt.reserve(this->conditions_alt.size() + size_e + size_c);
         for (size_t i = 0; i < size_e; ++i) {
            auto cnd   = other.conditions_alt[i];
            auto clone = cnd->clone();
            this->conditions_alt.push_back(clone);
            if (i == size_e - 1)
               clone->next_is_or = false;
         }
         for (size_t i = 0; i < size_c; ++i) {
            auto cnd   = other.conditions[i];
            auto clone = cnd->clone();
            clone->negate();
            this->conditions_alt.push_back(clone);
            if (i == size_c - 1)
               clone->next_is_or = false;
         }
      }
      //
      void Block::clear(bool deleting) {
         //
         // We don't want to delete aliases unless we're deleting the entire Block, or we'll get 
         // memory access errors.
         //
         if (deleting) {
            for (auto item : this->items)
               delete item;
            this->items.clear();
         } else {
            std::vector<ParsedItem*> keep;
            //
            for (auto item : this->items) {
               if (auto alias = dynamic_cast<Alias*>(item)) {
                  keep.push_back(alias);
                  continue;
               }
               delete item;
            }
            this->items.clear();
            std::swap(this->items, keep);
         }
         //
         for (auto condition : this->conditions_alt)
            delete condition;
         this->conditions_alt.clear();
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
            #if MEGALO_COMPILE_MIMIC_BUNGIE_ARTIFACTS != 1
            case Type::altif_block: // Bungie/343i don't collapse terminating alt(if) blocks into their parent trigger?
            case Type::alt_block:
            #endif
               return true;
         }
         return false;
      }
      void Block::_setup_trigger_forge_label(Compiler& compiler) {
         //
         // TODO: MAKE THIS AND OpcodeArgValueForgeLabel::compile SHARE SOME HELPER FUNCTION, IDEALLY 
         // ON Compiler.
         //
         auto t = this->trigger;
         //
         if (this->label_name.isEmpty()) {
            auto  index = this->label_index;
            auto& list  = compiler.get_variant().scriptContent.forgeLabels;
            if (index < 0 || index >= list.size()) {
               compiler.raise_error(QString("Label index %1 does not exist.").arg(index));
            } else {
               t->forgeLabel = &list[index];
            }
         } else {
            int32_t index = -1;
            auto&   list  = compiler.get_variant().scriptContent.forgeLabels;
            for (size_t i = 0; i < list.size(); ++i) {
               auto& label = list[i];
               ReachString* name = label.name;
               if (!name)
                  continue;
               QString english = QString::fromUtf8(name->get_content(reach::language::english).c_str());
               if (english == this->label_name) {
                  if (index != -1) {
                     QString lit = cobb::string_scanner::escape(this->label_name, '"');
                     compiler.raise_error(QString("The specified string literal (\"%1\") matches multiple defined Forge labels. Use an index instead.").arg(lit));
                  }
                  index = i;
               }
            }
            if (index == -1) {
               QString lit = cobb::string_scanner::escape(this->label_name, '"');
               compiler.raise_error(QString("The specified string literal (\"%1\") does not match any defined Forge label.").arg(lit));
            } else
               t->forgeLabel = &list[index];
         }
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
                  if (item->type == Block::Type::for_each_object_with_label) {
                     this->label_name  = item->label_name;
                     this->label_index = item->label_index;
                     this->_setup_trigger_forge_label(compiler);
                  }
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
         auto   t = this->trigger = new Trigger;
         size_t ti = compiler.results.triggers.size(); // index of this trigger in the full trigger list
         compiler.results.triggers.push_back(t);
         if (this->parent) {
            t->entryType = entry_type::normal;
            //
            auto block = dynamic_cast<const Block*>(this->parent);
            if (block && block->type != Type::root)
               t->entryType = entry_type::subroutine;
         }
         //
         if (this->event != Event::none) {
            t->entryType = _block_event_to_trigger_entry(this->event);
            //
            if (this->event == Event::double_host_migration) // double host migration uses the same entry_type as host migration, so we need to special-case it
               compiler.results.events.indices.doubleHostMigrate = ti;
            else
               compiler.results.events.set_index_of_event(t->entryType, ti);
         }
         switch (this->type) {
            case Type::basic:
            case Type::if_block:
            case Type::altif_block:
            case Type::alt_block:
               break;
            case Type::for_each_object:
               t->blockType = block_type::for_each_object;
               break;
            case Type::for_each_object_with_label:
               t->blockType = block_type::for_each_object_with_label;
               this->_setup_trigger_forge_label(compiler);
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
            for (auto item : this->conditions_alt) {
               auto cnd = dynamic_cast<Condition*>(item->opcode);
               if (cnd) {
                  this->trigger->opcodes.push_back(cnd);
                  cnd->or_group = count;
                  item->opcode = nullptr;
               }
               if (!item->next_is_or)
                  count += 1;
            }
            for (auto item : this->conditions) {
               auto cnd = dynamic_cast<Condition*>(item->opcode);
               if (cnd) {
                  this->trigger->opcodes.push_back(cnd);
                  cnd->or_group = count;
                  item->opcode = nullptr;
               }
               if (!item->next_is_or)
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
               if (block->type == Block::Type::function) // don't mis-compile function definitions as function calls
                  continue;
               if (block->trigger != this->trigger) {
                  //
                  // Create a "call nested trigger" opcode.
                  //
                  auto call = new Action;
                  this->trigger->opcodes.push_back(call);
                  call->function = &actionFunction_runNestedTrigger;
                  auto arg = (call->function->arguments[0].typeinfo.factory)();
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
      //
      Comparison* Comparison::clone() const noexcept {
         auto out = new Comparison;
         out->next_is_or = this->next_is_or;
         if (this->opcode)
            out->opcode = this->opcode->clone();
         return out;
      }
      void Comparison::negate() noexcept {
         this->next_is_or = !this->next_is_or;
         if (auto opcode = this->opcode) {
            auto cnd = dynamic_cast<Condition*>(opcode);
            if (cnd) {
               #if MEGALO_COMPILE_MIMIC_BUNGIE_ARTIFACTS
               if (cnd->function == &_get_comparison_opcode()) {
                  //
                  // TODO: I think for comparisons, Bungie flips the operator rather'n using the (inverted) flag.
                  //
                  auto arg = dynamic_cast<OpcodeArgValueCompareOperatorEnum*>(cnd->arguments[2]);
                  if (arg) {
                     arg->invert();
                     return;
                  }
               }
               #endif
               cnd->inverted = !cnd->inverted;
            }
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
      this->aliases_in_scope.clear(); // don't free contents; every Alias should have been inside of a Block and freed by that Block
      this->functions_in_scope.clear();
   }
   //
   bool Compiler::try_decode_enum_reference(QString word, int32_t& out) const {
      auto i = word.indexOf('.');
      if (i <= 0)
         return false;
      auto j = word.indexOf('.', i + 1);
      if (j <= 0)
         j = word.size();
      QString base = word.left(i);
      QString prop = word.mid(i + 1, j - i - 1);
      //
      const Script::Enum* match = nullptr;
      //
      auto ns = Script::namespaces::get_by_name(base);
      if (ns) {
         auto member = ns->get_member(prop);
         if (!member)
            return false;
         if (!member->is_enum_member())
            return false;
         match = member->enumeration;
         //
         // Move onto next part:
         //
         base = prop;
         i = j;
         j = word.indexOf('.', i + 1);
         if (j >= 0)
            return false; // "namespace_name.enum_name.value.something_else"
         prop = word.right(word.size() - i - 1);
      } else if (auto alias = this->lookup_absolute_alias(base)) {
         if (alias->is_enumeration())
            match = alias->get_enumeration();
         else
            return false;
      } else {
         //
         // TODO: check for user-defined enums, once we implement those
         //
         return false;
      }
      #if _DEBUG
         assert(match);
      #endif
      return match->lookup(prop, out);
   }
   bool Compiler::try_get_integer(string_scanner& str, int32_t& out) const {
      if (str.extract_integer_literal(out))
         return true;
      auto word  = str.extract_word();
      if (word.isEmpty())
         return false;
      auto alias = this->lookup_absolute_alias(word);
      if (alias && alias->is_integer_constant()) {
         out = alias->get_integer_constant();
         return true;
      }
      if (!alias) {
         //
         // Okay. Maybe it's a reference to an enum value.
         //
         if (this->try_decode_enum_reference(word, out))
            return true;
      }
      return false;
   }
   arg_compile_result Compiler::try_get_integer_or_word(string_scanner& str, int32_t& out_int, QString& out_name, QString thing_getting, OpcodeArgTypeinfo* word_must_be_imported_from, int32_t limit_int) const {
      //
      // This function is provided as a helper for OpcodeArgValue::compile overloads, and attempts to do the 
      // following tasks:
      //
      //  - If (str) is any value that represents an integer, then extract that integer. Validate it if the 
      //    caller wants us to; if it's valid, then send it to the caller and exit.
      //
      //     - Values that represent integers include: integer literals; aliases of integer constants; enum 
      //       values; and aliases of enum values.
      //
      //  - If (str) is not a value that represents an integer, then provide the caller with a word...
      //
      //     - If the word is the name of an alias, then provide the caller with the integer constant or 
      //       imported name that the alias refers to; if the alias refers to something else, then fail with 
      //       an error.
      //
      //     - If the word is not an alias, then check if the caller wants us to constrain it to names that 
      //       a specific type imports; if so, then apply those constraints.
      //
      //     - Provide the word back to the caller.
      //
      //  - If (str) has no word, then fail with an error.
      //
      // One practical example of this function's usage is the "incident" type, which accepts an incident 
      // name, the word "none", or the integer index of an incident.
      //
      out_int = 0;
      out_name.clear();
      if (!str.extract_integer_literal(out_int)) {
         auto word  = str.extract_word();
         if (word.isEmpty())
            return arg_compile_result::failure();
         auto alias = this->lookup_absolute_alias(word);
         if (alias) {
            if (alias->is_integer_constant()) {
               out_int = alias->get_integer_constant();
            } else if (alias->is_imported_name()) {
               out_name = alias->target_imported_name;
            } else {
               return arg_compile_result::failure(QString("Alias \"%1\" cannot be used here. Only integer literals, %2, and aliases of either may appear here.").arg(alias->name).arg(thing_getting));
            }
         } else {
            if (this->try_decode_enum_reference(word, out_int))
               return arg_compile_result::success();
            if (word_must_be_imported_from) {
               if (word_must_be_imported_from->has_imported_name(word)) {
                  out_name = word;
                  return arg_compile_result::success();
               }
               return arg_compile_result::failure();
            }
            out_name = word;
            return arg_compile_result::success();
         }
      }
      if (limit_int >= 0) {
         if (out_int < 0 || out_int > limit_int)
            return arg_compile_result::failure(QString("Integer literal %1 is out of bounds; valid integers range from 0 to %2.").arg(out_int).arg(limit_int));
      }
      return arg_compile_result::success();
   }
   /*static*/ bool Compiler::is_keyword(QString s) {
      s = s.toLower();
      if (s == "alias") // declare an alias
         return true;
      if (s == "alt") // close an if- or altif-block and open a new block
         return true;
      if (s == "altif") // close an if- or altif-block and open a new block with conditions
         return true;
      if (s == "and") // bridge conditions
         return true;
      if (s == "declare") // declare a variable
         return true;
      if (s == "do") // open a generic block
         return true;
      if (s == "else") // reserved
         return true;
      if (s == "elseif") // reserved
         return true;
      if (s == "end") // close a block
         return true;
      if (s == "enum") // user-defined enums
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
      if (s == "then") // close an if- or altif-statement's conditions
         return true;
      return false;
   }
   //
   void Compiler::_commit_unresolved_strings(Compiler::unresolved_str_list& add) {
      this->results.unresolved_strings += add;
      add.clear();
   }
   bool Compiler::handle_unresolved_string_references() {
      auto& string_table = this->variant.scriptData.strings;
      auto& list         = this->results.unresolved_strings;
      //
      std::vector<QString> to_remove;
      bool  any_unresolved = false;
      for (auto& key : list.uniqueKeys()) {
         bool    string_resolved = true;
         int32_t new_index       = -1;
         for (auto& ref : list.values(key)) {
            int32_t index = ref.pending.index;
            if (ref.pending.action == unresolved_string_pending_action::create) {
               if (new_index < 0) { // string not yet created?
                  auto str = string_table.add_new();
                  if (!str) {
                     string_resolved = false;
                     continue;
                  }
                  for (int i = 0; i < reach::language_count; ++i)
                     str->get_write_access((reach::language)i) = key.toStdString();
                  new_index = str->index;
                  assert(new_index >= 0 && "Something went wrong. A newly-created ReachString did not have its index member set by the containing string table.");
               }
               index = new_index;
            } else {
               assert(index >= 0 && "For unresolved_string_pending_action::use_existing, you must specify a valid string index to use.");
            }
            auto arg    = string_scanner(QString("%1").arg(index));
            auto result = ref.value->compile(*this, arg, ref.part);
            if (result.is_unresolved_string() || result.is_failure()) {
               string_resolved = false;
               continue;
            }
            ref.handled = true;
         }
         if (string_resolved)
            to_remove.push_back(key);
         else
            any_unresolved = true;
      }
      for (auto& key : to_remove)
         list.remove(key);
      return !any_unresolved;
   }
   //
   Script::Alias* Compiler::lookup_relative_alias(QString name, const OpcodeArgTypeinfo* relative_to) const {
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
   Script::Alias* Compiler::lookup_absolute_alias(QString name) const {
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
         if (alias->is_relative_alias())
            continue;
         if (name.compare(alias->name, Qt::CaseInsensitive) == 0)
            return alias;
      }
      return nullptr;
   }
   Script::UserDefinedFunction* Compiler::lookup_user_defined_function(QString name) {
      auto& list = this->functions_in_scope;
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
      int32_t index;
      if (arg.extract_integer_literal(index))
         return new Script::VariableReference(index);
      //
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
      while (!this->is_at_effective_end()) {
         this->_parseAction();
         if (this->has_fatal())
            break;
      };
      //
      if (!this->has_fatal()) {
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
            Opcode* incomplete    = nullptr;
            size_t  incomplete_ai = 0;
            bool    for_missing_label = false;
            for (auto trigger : this->results.triggers) {
               trigger->count_contents(cc, ac);
               if (!for_missing_label) {
                  if (trigger->blockType == block_type::for_each_object_with_label && !trigger->forgeLabel)
                     for_missing_label = true;
               }
               if (!incomplete) {
                  for (auto opcode : trigger->opcodes) {
                     if (!opcode->function) {
                        incomplete = opcode;
                        break;
                     }
                     auto&  list = opcode->arguments;
                     size_t size = list.size();
                     for (size_t i = 0; i < size; ++i) {
                        if (!list[i]) {
                           incomplete    = opcode;
                           incomplete_ai = i;
                           break;
                        }
                     }
                     if (incomplete)
                        break;
                  }
               }
            }
            if (incomplete) {
               QString error = "One or more of the instructions in your script failed to compile fully. If no other errors were logged, then this may be the result of a bug in the compiler itself; consider reporting this issue and sending your script to this program's developer to test with. ";
               if (auto func = incomplete->function) {
                  auto& base = func->arguments[incomplete_ai];
                  QString detail = QString("The first such opcode in question was \"%1\". Argument index %2 (type %3) is the first missing argument found.")
                     .arg(func->name)
                     .arg(incomplete_ai)
                     .arg(base.typeinfo.friendly_name);
                  error += detail;
               }
               this->raise_error(error);
            }
            if (for_missing_label)
               this->raise_error("At least one for-each-object-with-label loop failed to compile because its Forge label was unspecified. If no other errors relating to Forge labels were logged, then this may be the result of a bug in the compiler itself; consider reporting this issue and sending your script to this program's developer to test with.");
            if (tc > Limits::max_triggers)
               this->raise_error(QString("The compiled script contains %1 triggers, but only a maximum of %2 are allowed.").arg(tc).arg(Limits::max_triggers));
            if (cc > Limits::max_conditions)
               this->raise_error(QString("The compiled script contains %1 conditions, but only a maximum of %2 are allowed.").arg(cc).arg(Limits::max_conditions));
            if (ac > Limits::max_actions)
               this->raise_error(QString("The compiled script contains %1 actions, but only a maximum of %2 are allowed.").arg(ac).arg(Limits::max_actions));
         }
         //
         if (!this->has_errors())
            this->results.success = true;
      }
      return;
   }
   void Compiler::apply() {
      if (!this->results.success)
         return;
      assert(!this->has_errors() && !this->has_fatal() && "Do not attempt to apply compiled content when there were compiler errors. Do something with the logged errors!");
      assert(!this->get_unresolved_string_references().size() && "Do not attempt to apply compiled content when unresolved string references exist.");
      //
      auto& mp       = this->variant;
      //
      auto& triggers = mp.scriptContent.triggers;
      triggers.clear(); // this isn't a vector; the list type owns its contents
      for (auto* trigger : this->results.triggers)
         triggers.push_back(trigger);
      this->results.triggers.clear();
      //
      mp.scriptContent.entryPoints = this->results.events;
      //
      auto& mp_vars  = mp.scriptContent.variables;
      auto& new_vars = this->results.variables;
      mp_vars.global.adopt(new_vars.global);
      mp_vars.object.adopt(new_vars.object);
      mp_vars.player.adopt(new_vars.player);
      mp_vars.team.adopt(new_vars.team);
   }
   
   Compiler::extract_int_result::type Compiler::extract_integer_literal_detailed(int32_t& out) {
      auto result = string_scanner::extract_integer_literal(out);
      if (result) {
         if (this->extract_specific_char('.', true)) {
            this->raise_error("Unexpected decimal point. Floating-point numbers are not supported.");
            //
            // Extract the decimal content.
            //
            int32_t discard;
            string_scanner::extract_integer_literal(discard);
            //
            return extract_int_result::floating_point;
         }
      }
      return result ? extract_int_result::success : extract_int_result::failure;
   }
   bool Compiler::extract_integer_literal(int32_t& out) {
      return this->extract_integer_literal_detailed(out) == extract_int_result::success;
   }
   QString Compiler::extract_operator() {
      auto prior = this->backup_stream_state();
      QString word;
      this->scan([this, &word](QChar c) {
         if (string_scanner::is_whitespace_char(c))
            return word.size() > 0;
         if (!string_scanner::is_operator_char(c))
            return true;
         word += c;
         return false;
      });
      if (word.isEmpty())
         this->restore_stream_state(prior);
      return word;
   }

   Compiler::statement_side_t Compiler::_extract_statement_side(QString& out_str, int32_t& out_int) {
      out_str.clear();
      if (this->extract_integer_literal(out_int))
         return statement_side::integer;
      QString temp;
      if (this->extract_string_literal(temp))
         return statement_side::string;
      out_str = this->extract_word();
      if (!out_str.isEmpty())
         return statement_side::word;
      return statement_side::none;
   }
   /*static*/ Compiler::keyword_handler_t Compiler::__get_handler_for_keyword(QString word) noexcept {
      if (word == "alias")
         return &Compiler::_handleKeyword_Alias;
      else if (word == "alt")
         return &Compiler::_handleKeyword_Alt;
      else if (word == "altif")
         return &Compiler::_handleKeyword_AltIf;
      else if (word == "declare")
         return &Compiler::_handleKeyword_Declare;
      else if (word == "do")
         return &Compiler::_handleKeyword_Do;
      else if (word == "end")
         return &Compiler::_handleKeyword_End;
      else if (word == "for")
         return &Compiler::_handleKeyword_For;
      else if (word == "function")
         return &Compiler::_handleKeyword_Function;
      else if (word == "if")
         return &Compiler::_handleKeyword_If;
      else if (word == "on")
         return &Compiler::_handleKeyword_On;
      return nullptr;
   }
   void Compiler::_parseAction() {
      Script::VariableReference* lhs = nullptr;
      Script::VariableReference* rhs = nullptr;
      //
      auto prior = this->backup_stream_state();
      //
      #pragma region Parsing
         QString word;
         int32_t integer;
         #pragma region Lefthand side
            statement_side_t lhs_type = this->_extract_statement_side(word, integer);
            if (lhs_type == statement_side::string) {
               this->raise_error("You cannot assign to a string literal.");
            } else if (lhs_type == statement_side::integer) {
               lhs = new Script::VariableReference(integer);
            } else if (lhs_type == statement_side::word) {
               if (auto handler = Compiler::__get_handler_for_keyword(word)) {
                  ((*this).*(handler))();
                  return;
               }
               if (word == "and" || word == "or" || word == "not" || word == "then") {
                  this->raise_fatal(QString("The \"%1\" keyword cannot appear here.").arg(word));
                  return;
               }
               if (word == "else" || word == "elseif") {
                  this->raise_fatal(QString("Word \"%1\" is reserved for potential future use as a keyword. It cannot appear here.").arg(word));
                  return;
               }
               if (this->extract_specific_char('(')) {
                  this->_parseFunctionCall(prior, word, false);
                  //
                  // Assigning something to the result of a function call is an error. Let's check for that 
                  // so we can give an intuitive error message.
                  //
                  auto    prior = this->backup_stream_state();
                  QString op    = this->extract_operator();
                  if (!op.isEmpty()) {
                     this->restore_stream_state(prior);
                     this->raise_fatal(QString("An operator such as %1 cannot appear after a function call; you cannot assign to or compare the return value of a function.").arg(op));
                  }
                  return;
               }
               //
               // If we got here, then the parsed word must be the lefthand side of an assignment statement.
               //
               lhs = this->__parseVariable(word, false, true);
            } else {
               auto op = this->extract_operator();
               if (!op.isEmpty())
                  this->raise_fatal(QString("Statements cannot begin with an operator such as %1.").arg(op));
               else
                  this->raise_fatal("Expected the start of a new statement.");
               return;
            }
         #pragma endregion
         #pragma region Operator
            auto op = this->extract_operator();
            if (op.isEmpty()) {
               this->raise_fatal("Expected an operator.");
               return;
            }
            if (!::_is_assignment_operator(op))
               this->raise_error(QString("Operator %1 is not an assignment operator.").arg(op));
            //
         #pragma endregion
         #pragma region Righthand side
            statement_side_t rhs_type = this->_extract_statement_side(word, integer);
            if (rhs_type == statement_side::string) {
               this->raise_error("You cannot assign a string literal to a variable.");
            } else if (rhs_type == statement_side::integer) {
               rhs = new Script::VariableReference(integer);
            } else if (rhs_type == statement_side::word) {
               if (Compiler::is_keyword(word)) {
                  this->raise_fatal(QString("Keyword \"%1\" cannot appear here.").arg(word));
                  return;
               }
               if (this->extract_specific_char('(')) {
                  if (op != ce_assignment_operator) {
                     this->raise_error(QString("Operator %1 cannot be used to assign the result of a function call to a variable. Use operator %2.").arg(op).arg(ce_assignment_operator));
                  }
                  this->_parseFunctionCall(prior, word, false, lhs);
                  return;
               }
               rhs = this->__parseVariable(word);
            } else {
               this->raise_fatal("Expected the righthand side of an assignment statement.");
               return;
            }
         #pragma endregion
      #pragma endregion
      //
      auto statement = new Script::Statement;
      statement->set_start(prior);
      statement->set_end(this->state);
      this->block->insert_item(statement);
      //
      statement->lhs = lhs;
      statement->rhs = rhs;
      lhs->owner = statement;
      rhs->owner = statement;
      //
      if (lhs && !lhs->is_invalid) {
         bool bad = true;
         //
         if (lhs->is_constant_integer())
            this->raise_error("Cannot assign to a constant integer.");
         else if (lhs->is_read_only())
            this->raise_error(QString("Cannot assign to \"%1\". The referenced value is read-only.").arg(lhs->to_string()));
         else
            bad = false;
         //
         if (bad)
            return;
      }
      //
      // Compile the assignment opcode:
      //
      auto opcode = std::make_unique<Action>();
      {
         auto l_accessor = !lhs || lhs->is_invalid ? nullptr : lhs->get_accessor_definition();
         auto r_accessor = !rhs || rhs->is_invalid ? nullptr : rhs->get_accessor_definition();
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
               opcode->arguments.resize(accessor->arguments.size());
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
               if (!rhs->is_invalid) {
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
               }
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
               opcode->arguments.resize(accessor->arguments.size());
               //
               if (!lhs->is_invalid) {
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
               }
               //
               // Compile the right-hand side (the value to assign):
               //
               int  ai  = mapping->arg_context;
               auto arg = (accessor->arguments[ai].typeinfo.factory)();
               opcode->arguments[ai] = arg;
               auto result = arg->compile(*this, *rhs, 0);
               if (!result.is_success()) {
                  QString error = "The righthand side of this assignment failed to compile. ";
                  if (!result.error.isEmpty())
                     error += result.error;
                  this->raise_error(error);
               } else
                  assert(!result.is_unresolved_string() && "The righthand side of an assignment statement thinks it's an unresolved string reference.");
            }
            //
            opcode->function = accessor;
            //
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
               if (op != ce_assignment_operator)
                  this->raise_error("This accessor can only be invoked using the = operator.");
            } else {
               //
               // Compile the assignment operator.
               //
               auto op_string = string_scanner(op);
               auto op_arg = (accessor->arguments[mapping->arg_operator].typeinfo.factory)();
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
            if (!lhs->is_invalid) {
               auto result = opcode->arguments[0]->compile(*this, *lhs, 0);
               if (!result.is_success()) {
                  QString error = "The lefthand side of this assignment failed to compile. ";
                  if (!result.error.isEmpty())
                     error += result.error;
                  this->raise_error(error);
               } else
                  assert(!result.is_unresolved_string() && "The lefthand side of an assignment statement thinks it's an unresolved string reference.");
            }
            if (!rhs->is_invalid) {
               auto result = opcode->arguments[1]->compile(*this, *rhs, 0);
               if (!result.is_success()) {
                  QString error = "The righthand side of this assignment failed to compile. ";
                  if (!result.error.isEmpty())
                     error += result.error;
                  this->raise_error(error);
               } else
                  assert(!result.is_unresolved_string() && "The righthand side of an assignment statement thinks it's an unresolved string reference.");
            }
            //
            auto op_string = string_scanner(op);
            auto result = opcode->arguments[2]->compile(*this, op_string, 0);
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
   }
   bool Compiler::_parseCondition() {
      Script::VariableReference* lhs = nullptr;
      Script::VariableReference* rhs = nullptr;
      //
      auto prior = this->backup_stream_state();
      //
      #pragma region Parsing
         QString word;
         int32_t integer;
         #pragma region Lefthand side
            if (this->extract_word("not")) {
               if (this->negate_next_condition) {
                  this->raise_fatal("Constructions of the form \"not not\" are not valid.");
                  return true;
               }
               this->negate_next_condition = true;
            }
            statement_side_t lhs_type = this->_extract_statement_side(word, integer);
            if (lhs_type == statement_side::string) {
               this->raise_error("You cannot compare a string literal.");
            } else if (lhs_type == statement_side::integer) {
               lhs = new Script::VariableReference(integer);
            } else if (lhs_type == statement_side::word) {
               if (word.compare("not", Qt::CaseInsensitive) == 0) {
                  this->raise_fatal("Constructions of the form \"not not\" are not valid.");
                  return true;
               }
               if (Compiler::is_keyword(word)) {
                  this->raise_fatal(QString("The \"%1\" keyword cannot appear here.").arg(word));
                  return true;
               }
               if (this->extract_specific_char('(')) {
                  auto statement = this->_parseFunctionCall(prior, word, true);
                  if (this->has_fatal())
                     return true;
                  //
                  // Comparing something to the result of a function call is an error. Let's check for that 
                  // so we can give an intuitive error message.
                  //
                  auto    prior = this->backup_stream_state();
                  QString op    = this->extract_operator();
                  if (!op.isEmpty()) {
                     this->restore_stream_state(prior);
                     this->raise_fatal("You cannot compare the result of a function call.");
                     return true;
                  }
                  auto result = this->__parseConditionEnding();
                  if (statement) {
                     auto comparison = dynamic_cast<Script::Comparison*>(statement);
                     assert(comparison);
                     this->_applyConditionModifiers(comparison);
                  }
                  return result;
               }
               //
               // If we got here, then the parsed word must be the lefthand side of a comparison statement.
               //
               lhs = this->__parseVariable(word, false, false);
            } else {
               this->raise_fatal("Expected the start of a new condition.");
               return true;
            }
         #pragma endregion
         #pragma region Operator
            auto op = this->extract_operator();
            if (op.isEmpty()) {
               this->raise_fatal("Expected an operator.");
               return true;
            }
            if (!::_is_comparison_operator(op))
               this->raise_error(QString("Operator %1 is not a comparison operator.").arg(op));
            //
         #pragma endregion
         #pragma region Righthand side
            statement_side_t rhs_type = this->_extract_statement_side(word, integer);
            if (rhs_type == statement_side::string) {
               this->raise_error("You cannot assign a string literal to a variable.");
            } else if (rhs_type == statement_side::integer) {
               rhs = new Script::VariableReference(integer);
            } else if (rhs_type == statement_side::word) {
               if (Compiler::is_keyword(word)) {
                  this->raise_fatal(QString("Keyword \"%1\" cannot appear here.").arg(word));
                  return true;
               }
               if (this->extract_specific_char('(')) {
                  this->raise_fatal("You cannot compare the result of a function call.");
                  return true;
               }
               rhs = this->__parseVariable(word);
            } else {
               this->raise_fatal("Expected the righthand side of a comparison statement.");
               return true;
            }
         #pragma endregion
      #pragma endregion
      //
      auto statement = new Script::Comparison;
      statement->set_start(prior);
      statement->set_end(this->state);
      this->block->insert_condition(statement);
      //
      statement->lhs = lhs;
      statement->rhs = rhs;
      lhs->owner = statement;
      rhs->owner = statement;
      //
      {
         bool fail = !(lhs && rhs);
         if (lhs && !lhs->is_invalid && lhs->get_accessor_definition()) {
            this->raise_error(QString("Cannot use accessors such as \"%1\" in comparisons.").arg(lhs->to_string()));
            fail = true;
         }
         if (rhs && !rhs->is_invalid && rhs->get_accessor_definition()) {
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
            if (!lhs->is_invalid) {
               auto result = opcode->arguments[0]->compile(*this, *lhs, 0);
               if (!result.is_success()) {
                  QString error = "The lefthand side of this comparison failed to compile. ";
                  if (!result.error.isEmpty())
                     error += result.error;
                  this->raise_error(error);
               } else
                  assert(!result.is_unresolved_string() && "The lefthand side of a comparison statement thinks it's an unresolved string reference.");
            }
            if (!rhs->is_invalid) {
               auto result = opcode->arguments[1]->compile(*this, *rhs, 0);
               if (!result.is_success()) {
                  QString error = "The righthand side of this comparison failed to compile. ";
                  if (!result.error.isEmpty())
                     error += result.error;
                  this->raise_error(error);
               } else
                  assert(!result.is_unresolved_string() && "The righthand side of a comparison statement thinks it's an unresolved string reference.");
            }
            auto op_string = string_scanner(op);
            auto result    = opcode->arguments[2]->compile(*this, op_string, 0);
            if (!result.is_success()) {
               QString error = "The operator in this comparison failed to compile. ";
               if (!result.error.isEmpty())
                  error += result.error;
               this->raise_error(error);
            } else
               assert(!result.is_unresolved_string() && "The operator in a comparison statement thinks it's an unresolved string reference.");
            //
            statement->opcode = opcode.release();
         }
      }
      auto result = this->__parseConditionEnding();
      this->_applyConditionModifiers(statement);
      return result;
   }
   bool Compiler::__parseConditionEnding() {
      auto word = this->extract_word();
      if (word.compare("and", Qt::CaseInsensitive) == 0) {
         this->next_condition_joiner = c_joiner::and;
      } else if (word.compare("or", Qt::CaseInsensitive) == 0) {
         this->next_condition_joiner = c_joiner::or;
      } else if (word.compare("then", Qt::CaseInsensitive) == 0) {
         return true;
      } else {
         if (Compiler::is_keyword(word))
            this->raise_fatal(QString("Expected one of the following keywords: and; or; then. Saw keyword \"%1\" instead; that keyword cannot appear here.").arg(word));
         else
            this->raise_fatal(QString("Expected one of the following keywords: and; or; then. Saw \"%1\" instead.").arg(word));
         return true;
      }
      return false;
   }

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
         if (type->is_variable()) { // VariableReference can also refer to things like script widgets
            if (!basis && var->resolved.nested.type)
               basis = var->resolved.top_level.type;
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
      }
      return var;
   }
   //
   void Compiler::_parseBlockConditions() {
      bool stop = false;
      do {
         stop = this->_parseCondition();
         if (stop)
            return;
         if (this->has_fatal())
            return;
      } while (!this->is_at_effective_end());
      if (!stop)
         this->raise_fatal("Unexpected end-of-file while parsing a block's conditions.");
   }
   //
   void Compiler::__parseFunctionArgs(const OpcodeBase& function, Opcode& opcode, Compiler::unresolved_str_list& unresolved_strings) {
      auto& mapping = function.mapping;
      opcode.function = &function;
      opcode.arguments.resize(function.arguments.size());
      //
      int8_t mapped_arg_count = mapping.mapped_arg_count();
      //
      if (mapped_arg_count == 0) {
         QChar   terminator = '\0';
         QString all_args   = this->extract_up_to_any_of(")", terminator).trimmed();
         if (terminator == '\0') {
            this->raise_fatal("Expected a , or a ).");
            return;
         }
         if (!all_args.isEmpty())
            this->raise_error("Too many arguments passed to the function.");
         return;
      }
      std::vector<QString> raw_args;
      {
         QChar terminator = '\0';
         do {
            auto arg = this->extract_up_to_any_of(",)", terminator).trimmed();
            if (terminator == '\0') {
               this->raise_fatal("Expected a , or a ).");
               return;
            }
            raw_args.push_back(arg);
         } while (terminator == ',');
      }
      //
      int8_t opcode_arg_index = 0;
      int8_t opcode_arg_part  = 0;
      int8_t script_arg_index = 0;
      for (; script_arg_index < raw_args.size(); ++script_arg_index) {
         if (opcode_arg_index >= mapped_arg_count) {
            this->raise_error("Too many arguments passed to the function.");
            return;
         }
         //
         auto  mapped_index     = mapping.arg_index_mappings[opcode_arg_index];
         auto& current_argument = opcode.arguments[mapped_index];
         if (!current_argument) { // if we're handling multiple (opcode_arg_part)s on the same opcode, then this will already exist
            current_argument = (function.arguments[mapped_index].typeinfo.factory)();
            if (!current_argument) {
               this->raise_error("Unknown error: failed to instantiate an OpcodeArgValue while parsing arguments to the function call.");
               return;
            }
         }
         //
         string_scanner argument(raw_args[script_arg_index]);
         //
         // It would probably make sense, at this point, to check whether the argument is empty and issue 
         // a compile error from here if so, right? The problem is that we can't know whether that would 
         // constitute an irresolvable failure (impossible to parse further arguments) or a resolvable 
         // failure (can attempt to error-check further arguments). We have to try to compile the argument 
         // text, even if it's literally empty, in order to discover that information.
         //
         arg_compile_result result = current_argument->compile(*this, argument, opcode_arg_part);
         bool failure = result.is_failure();
         bool success = result.is_success();
         if (failure) {
            bool irresolvable = result.is_irresolvable_failure();
            //
            QString error = QString("Failed to parse script argument %1 (type %2).").arg(script_arg_index + 1).arg(function.arguments[mapped_index].typeinfo.friendly_name);
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
         bool has_more   = script_arg_index + 1 < raw_args.size();
         bool another    = needs_more || result.can_take_another();
         if (needs_more && !has_more) {
            this->raise_error("Not enough arguments passed to the function.");
            return;
         }
         if (success) {
            if (!argument.is_at_effective_end()) {
               this->raise_error(QString("Failed to parse script argument %1 (type %2). There was unexpected content at the end of the argument.").arg(script_arg_index + 1).arg(function.arguments[mapped_index].typeinfo.friendly_name));
            } else {
               if (result.is_unresolved_string())
                  unresolved_strings.insert(
                     result.get_unresolved_string(), // key
                     unresolved_str(*current_argument, opcode_arg_part) // value
                  );
            }
         }
         if (another && has_more) {
            ++opcode_arg_part;
            continue;
            //
            // Only skip the below if there actually are more arguments to consume; otherwise, the last-minute check to 
            // ensure that all arguments were parsed will fail when it shouldn't.
            //
         }
         ++opcode_arg_index;
         opcode_arg_part = 0;
      }
      if (opcode_arg_index < mapped_arg_count)
         this->raise_error("Not enough arguments passed to the function.");
   }
   namespace {
      template<typename T, int I> void _find_opcode_bases(const std::array<T, I>& list, std::vector<const OpcodeBase*>& results, QString function_name, Script::VariableReference* context) {
         for (auto& action : list) {
            auto& mapping = action.mapping;
            if (context) {
               if (!action.context_is(*context->get_type()))
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
   Script::Statement* Compiler::_parseFunctionCall(const pos& call_start, QString stem, bool is_condition, Script::VariableReference* assign_to) {
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
      QString function_name;
      std::unique_ptr<Script::VariableReference> context = nullptr;
      bool context_is_game = false;
      {  // Identify the context and the function name, i.e. context.function_name(arg, arg, arg)
         int size = stem.size();
         int i    = size - 1;
         for (; i >= 0; --i) {
            auto c = stem[i];
            if (QString("[]").indexOf(c) >= 0) {
               this->raise_fatal(call_start, "Function names cannot contain square brackets.");
               return nullptr;
            }
            if (c == '.') {
               function_name = stem.right(size - 1 - i);
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
               return nullptr;
            }
            function_name = stem;
         } else {
            //
            // We're a member function.
            //
            stem = stem.left(i);
            if (stem.compare("game", Qt::CaseInsensitive) == 0) {
               //
               // The "game" namespace is allowed to contain member functions. VariableReferences 
               // cannot resolve to namespaces, so we have to handle this here in the compiler.
               //
               context_is_game = true;
            } else {
               context.reset(this->__parseVariable(stem));
               if (this->has_fatal()) // the VariableReference may contain a syntax error
                  return nullptr;
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
                  return nullptr;
               }
            }
         }
      }
      //
      if (!context && !context_is_game) {  // Handle user-defined function calls
         auto func = this->lookup_user_defined_function(function_name);
         if (func) {
            if (is_condition) {
               this->raise_error(call_start, QString("User-defined functions such as \"%1\" cannot be called from inside of a condition.").arg(function_name));
               if (!this->skip_to(')'))
                  this->raise_fatal("Unable to locate the nearest ')' glyph; possible unterminated function call. Parsing cannot continue.");
               return nullptr;
            }
            if (assign_to) {
               this->raise_error(call_start, QString("User-defined functions such as \"%1\" cannot return values.").arg(function_name));
               if (!this->skip_to(')'))
                  this->raise_fatal("Unable to locate the nearest ')' glyph; possible unterminated function call. Parsing cannot continue.");
               return nullptr;
            }
            if (!this->extract_specific_char(')')) {
               this->raise_error(call_start, QString("Expected ')'. User-defined functions such as \"%1\" cannot have arguments passed to them.").arg(function_name));
               if (!this->skip_to(')'))
                  this->raise_fatal("Unable to locate the nearest ')' glyph; possible unterminated function call. Parsing cannot continue.");
               return nullptr;
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
            return nullptr;
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
         return nullptr;
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
         this->__parseFunctionArgs(*function, *opcode.get(), unresolved_strings); // advances us past the closing ")" EVEN IF IT FAILS FOR ANY REASON
         if (!this->checkpoint_has_errors(check)) {
            match = function;
            break;
         }
      }
      if (!match) {
         if (opcode_bases.size() > 1) {
            this->revert_to_log_checkpoint(check);
            if (context)
               this->raise_error(call_start, QString("The arguments you passed to %1.%2 did not match any of its function signatures.").arg(context->get_type()->internal_name.c_str()).arg(function_name));
            else if (context_is_game)
               this->raise_error(call_start, QString("The arguments you passed to %1.%2 did not match any of its function signatures.").arg("game").arg(function_name));
            else
               this->raise_error(call_start, QString("The arguments you passed to %1 did not match any of its function signatures.").arg(function_name));
         }
         return nullptr;
      }
      //
      // If we've reached this point without any errors, then we should be just after the terminating ')' for the function call, 
      // and the Opcode should have had its (arguments) array sized appropriately.
      //
      if (context) {
         auto index = match->mapping.arg_context;
         const OpcodeArgBase& base = match->arguments[index];
         opcode->arguments[index] = (base.typeinfo.factory)();
         auto result = opcode->arguments[index]->compile(*this, *context, 0);
         if (result.is_failure()) {
            QString error = "Failed to compile the context for this function call. ";
            if (!result.error.isEmpty())
               error += result.error;
            this->raise_error(call_start, error);
         }
      }
      if (assign_to) {
         //
         // We're assigning the return value of this function call to something, so let's first make 
         // sure that the function actually returns a value.
         //
         auto index = match->index_of_out_argument();
         bool fail  = assign_to->is_invalid;
         if (!fail) {
            if (assign_to->is_read_only()) {
               this->raise_error("You cannot assign to this value.");
               fail = true;
            } else if (assign_to->is_accessor()) {
               this->raise_error("You cannot assign the return value of a function to an accessor.");
               fail = true;
            }
         }
         if (index < 0) {
            if (context)
               this->raise_error(call_start, QString("Function %1.%2 does not return a value.").arg(context->get_type()->internal_name.c_str()).arg(function_name));
            else if (context_is_game)
               this->raise_error(call_start, QString("Function %1.%2 does not return a value.").arg("game").arg(function_name));
            else
               this->raise_error(call_start, QString("Function %1 does not return a value.").arg(function_name));
            fail = true;
         }
         //
         const OpcodeArgBase& base = match->arguments[index];
         //
         // Verify that the variable we're assigning our return value to is of the right type:
         //
         auto target_type = assign_to->get_type();
         if (&base.typeinfo != target_type) {
            QString context_name = "";
            if (context)
               context_name = QString("%1.").arg(context->get_type()->internal_name.c_str());
            //
            if (target_type) {
               this->raise_error(call_start, QString("Function %1%2 returns a %3, not a %4.")
                  .arg(context_name)
                  .arg(function_name)
                  .arg(base.typeinfo.internal_name.c_str())
                  .arg(target_type->internal_name.c_str())
               );
            } else {
               this->raise_error(call_start, QString("Function %1%2 returns a %3. Could not verify whether you are assigning it to a variable of the correct type.")
                  .arg(context_name)
                  .arg(function_name)
                  .arg(base.typeinfo.internal_name.c_str())
               );
            }
            fail = true;
         }
         //
         if (!fail) {
            //
            // The type is correct, so set the out-argument.
            //
            opcode->arguments[index] = (base.typeinfo.factory)();
            opcode->arguments[index]->compile(*this, *assign_to, 0);
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
                  auto result = blank->arguments[0]->compile(*this, *assign_to, 0);
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
                  statement->set_start(this->state);
                  statement->set_end(this->state);
                  statement->opcode = blank;
                  this->block->insert_item(statement);
               }
            }
         }
      } else {
         auto index = match->index_of_out_argument();
         if (index >= 0) {
            //
            // This function returns a value, but we are not calling it in an assign statement (i.e. we are discarding its 
            // return value). We need to check whether that's allowed.
            //
            bool error = true;
            if (match->mapping.flags & OpcodeFuncToScriptMapping::flags::return_value_can_be_discarded) {
               //
               // If the return value is allowed to be discarded, then handle that scenario by compiling a "none" value 
               // as the out-argument.
               //
               auto& base = match->arguments[index];
               auto  arg  = opcode->arguments[index] = (base.typeinfo.factory)();
               auto* var  = dynamic_cast<Variable*>(arg);
               assert(arg && "Failed to handle OpcodeFuncToScriptMapping::flags::return_value_can_be_discarded: failed to create the argument.");
               assert(var && "Failed to handle OpcodeFuncToScriptMapping::flags::return_value_can_be_discarded: created argument is not a variable.");
               if (var->set_to_zero_or_none()) {
                  error = false;
               }
            }
            //
            if (error) {
               if (context)
                  this->raise_error(call_start, QString("Function %1.%2 returns a value, and that value must be assigned to a variable.").arg(context->get_type()->internal_name.c_str()).arg(function_name));
               else if (context_is_game)
                  this->raise_error(call_start, QString("Function %1.%2 returns a value, and that value must be assigned to a variable.").arg("game").arg(function_name));
               else
                  this->raise_error(call_start, QString("Function %1 returns a value, and that value must be assigned to a variable.").arg(function_name));
            }
         }
      }
      Script::Statement* statement;
      if (is_condition)
         statement = new Script::Comparison;
      else
         statement = new Script::Statement;
      statement->opcode = opcode.release();
      statement->set_end(this->state);
      if (is_condition) {
         auto cnd = dynamic_cast<Script::Comparison*>(statement);
         this->block->insert_condition(cnd);
      } else {
         this->block->insert_item(statement);
      }
      this->_commit_unresolved_strings(unresolved_strings);
      return statement;
   }
   //
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
   void Compiler::imply_variable(variable_scope vs, variable_type vt, uint8_t index) noexcept {
      auto set = this->_get_variable_declaration_set(vs);
      if (!set)
         return;
      set->imply(vt, index);
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
      auto start = this->backup_stream_state();
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
      Script::Alias* item = nullptr;
      //
      int32_t value;
      if (this->extract_integer_literal(value)) { // need to handle this separately from word parsing so that negative numbers are interpreted properly
         item = new Script::Alias(*this, name, value);
      } else {
         auto target = this->extract_word();
         if (target.isEmpty()) {
            this->raise_fatal("An alias declaration must supply a target.");
            return;
         }
         item = new Script::Alias(*this, name, target);
      }
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
   void Compiler::_handleKeyword_Alt() {
      if (this->block->type != Script::Block::Type::if_block && this->block->type != Script::Block::Type::altif_block) {
         auto prev = this->block->item(-1);
         auto p_bl = dynamic_cast<Script::Block*>(prev);
         if (p_bl) {
            if (p_bl->type == Script::Block::Type::if_block || p_bl->type == Script::Block::Type::altif_block)
               this->raise_fatal("Unexpected \"alt\". This keyword should not be preceded by the \"end\" keyword.");
         }
         this->raise_fatal("Unexpected \"alt\".");
         return;
      }
      if (!this->_closeCurrentBlock()) {
         this->raise_fatal("Unexpected \"alt\".");
         return;
      }
      auto item = new Script::Block;
      item->type = Script::Block::Type::alt_block;
      item->set_start(this->state);
      this->block->insert_item(item);
      this->_openBlock(item);
      {
         std::vector<Script::Block*> blocks;
         item->get_ifs_for_alt(blocks);
         for (auto block : blocks)
            item->make_alt_of(*block);
      }
   }
   void Compiler::_handleKeyword_AltIf() {
      if (this->block->type != Script::Block::Type::if_block && this->block->type != Script::Block::Type::altif_block) {
         auto prev = this->block->item(-1);
         auto p_bl = dynamic_cast<Script::Block*>(prev);
         if (p_bl) {
            if (p_bl->type == Script::Block::Type::if_block || p_bl->type == Script::Block::Type::altif_block)
               this->raise_fatal("Unexpected \"altif\". This keyword should not be preceded by the \"end\" keyword.");
         }
         this->raise_fatal("Unexpected \"altif\".");
         return;
      }
      if (!this->_closeCurrentBlock()) {
         this->raise_fatal("Unexpected \"altif\".");
         return;
      }
      auto item = new Script::Block;
      item->type = Script::Block::Type::altif_block;
      item->set_start(this->state);
      this->block->insert_item(item);
      this->_openBlock(item);
      {
         std::vector<Script::Block*> blocks;
         item->get_ifs_for_alt(blocks);
         for (auto block : blocks)
            item->make_alt_of(*block);
      }
      this->_parseBlockConditions();
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
            this->raise_error(QString("This is a redeclaration of variable %1. The redeclaration specifies an initial value even though a previous declaration already provided one.").arg(str));
         }
         if (network_specified && !decl->networking_is_implicit()) {
            bad = true;
            this->raise_error(QString("This is a redeclaration of variable %1. The redeclaration specifies a network type even though a previous declaration already provided one.").arg(str));
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
      auto  start = this->backup_stream_state();
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
               auto    result = this->extract_integer_literal_detailed(int_initial);
               if (result == extract_int_result::floating_point) {
                  this->raise_error("You cannot initialize a variable to a floating-point value.");
               } else if (result == extract_int_result::success) {
                  str_initial = QString("%1").arg(int_initial);
               } else if (result == extract_int_result::failure) {
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
                  auto    result = this->extract_integer_literal_detailed(int_initial);
                  if (result == extract_int_result::floating_point) {
                     this->raise_error("You cannot initialize a variable to a floating-point value.");
                  } else if (result == extract_int_result::success) {
                     str_initial = QString("%1").arg(int_initial);
                  } else if (result == extract_int_result::failure) {
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
               auto scope = top.scope;
               if (!scope || scope->is_variable_scope()) {
                  this->raise_error("When declaring one variable, you cannot use another variable as the initial value.");
                  return;
               }
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
      item->set_start(this->state);
      item->event = this->next_event;
      this->next_event = Script::Block::Event::none;
      this->block->insert_item(item);
      this->_openBlock(item);
   }
   void Compiler::_handleKeyword_End() {
      if (!this->_closeCurrentBlock())
         this->raise_fatal("Unexpected \"end\".");
   }
   void Compiler::_handleKeyword_If() {
      auto item = new Script::Block;
      item->type = Script::Block::Type::if_block;
      item->set_start(this->state);
      item->event = this->next_event;
      this->next_event = Script::Block::Event::none;
      this->block->insert_item(item);
      this->_openBlock(item);
      this->_parseBlockConditions();
   }
   void Compiler::_handleKeyword_For() {
      auto start = this->backup_stream_state();
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
            if (!this->extract_word("label")) {
               this->raise_fatal("Invalid for-each-object-with-label loop: expected the word \"label\".");
               return;
            }
            if (!this->extract_string_literal(label)) {
               if (!this->try_get_integer(*this, label_index)) {
                  this->raise_fatal("Invalid for-each-object-with-label loop: the label must be specified as a string literal or as a numeric label index.");
                  return;
               }
               label_is_index = true;
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
      auto start = this->backup_stream_state();
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
      this->next_event = Script::Block::Event::none;
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
         //
         // Host migrations and double host migrations use the same trigger entry type, so we need to 
         // handle double host migrations as a special-case here.
         //
         auto    et    = _block_event_to_trigger_entry(this->next_event);
         int32_t index = this->results.events.get_index_of_event(et);
         if (this->next_event == Script::Block::Event::double_host_migration)
            index = this->results.events.indices.doubleHostMigrate;
         //
         if (index != TriggerEntryPoints::none) {
            this->raise_error(QString("Only one trigger can be assigned to handle each event type. Event type \"%1\" is already in use.").arg(words));
            this->next_event = Script::Block::Event::__error;
         } else {
            if (this->next_event == Script::Block::Event::double_host_migration)
               this->results.events.indices.doubleHostMigrate = TriggerEntryPoints::reserved;
            else
               this->results.events.set_index_of_event(et, TriggerEntryPoints::reserved);
         }
      } else {
         this->raise_error(prior, QString("Invalid event name: \"%s\".").arg(words));
      }
   }
}