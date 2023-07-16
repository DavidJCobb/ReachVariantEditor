#pragma once
#include <cstdint>
#include <vector>
#include "./limits_bitnumbers.h"

class GameVariantDataMultiplayer;
namespace Megalo {
   class Action;
   class Condition;
   class Decompiler;
   class Opcode;
   class Trigger;

   //
   // Base class for anything capable of containing code, i.e. normal triggers 
   // and "inline triggers" that exist as part of the `begin` trigger action.
   //
   class CodeBlock {
      public:
         ~CodeBlock();
         CodeBlock() {}
         CodeBlock(const CodeBlock&);
         CodeBlock(CodeBlock&&);

         CodeBlock& operator=(const CodeBlock&);
         CodeBlock& operator=(CodeBlock&&);

         struct {
            //
            // Raw data loaded from a game variant file. Reach uses a struct-of-arrays approach to 
            // serialize trigger data, writing all conditions followed by all actions and then headers 
            // for triggers, with each header identifying the start index and count of each opcode type. 
            // This represents raw struct-of-arrays data; the (opcodes) vector is generated post-load by 
            // the (postprocess_opcodes) member function.
            //
            // The "raw" data is generally only meaningful during the load and save processes, and should 
            // not be checked at any other time. The compiler co-opts the (conditionCount) value here to 
            // handle (Condition::or_group) when compiling new triggers.
            //
            condition_index conditionStart = -1;
            condition_count conditionCount =  0;
            action_index    actionStart    = -1;
            action_count    actionCount    =  0;
            bool            serialized     = false;

            #if _DEBUG
               uint32_t bit_offset = 0;
            #endif
         } raw;

         std::vector<Opcode*> opcodes; // set up by postprocess_opcodes after read; trigger owns the opcodes and deletes them in its destructor
         
         bool read(cobb::ibitreader& stream, GameVariantDataMultiplayer& mp) noexcept;
         void postprocess_opcodes(const std::vector<Condition>& allConditions, const std::vector<Action>& allActions) noexcept;

         void write(cobb::bitwriter& stream) const noexcept;
         //
         void prep_for_flat_opcode_lists(); // call this on ALL instances before calling (generate_flat_opcode_lists) on ANY of them
         void generate_flat_opcode_lists(GameVariantDataMultiplayer& mp, std::vector<Condition*>& allConditions, std::vector<Action*>& allActions);

         void decompile(Decompiler& out, bool is_function = false) noexcept;

         void count_contents(size_t& conditions, size_t& actions) const noexcept;
   };


}