#include "actions.h"
#include "opcode_arg_types/all.h"

namespace Megalo {
   extern std::array<ActionFunction, 99> actionFunctionList = {{
      //
      // The double-curly-braces for this array are NOT a mistake; you should be able to 
      // use single braces but not every compiler handles that correctly.
      //
      ActionFunction( // 0
         "None",
         "This condition does nothing.",
         "None.",
         {}
      ),
      ActionFunction( // 1
         "Modify Score",
         "Modify the score of a player or team.",
         "Modify the score of %1: %2 %3.",
         {
            OpcodeArgBase("target",   OpcodeArgTeamOrPlayerVariableFactory),
            OpcodeArgBase("operator", OpcodeArgValueMathOperatorEnum::factory),
            OpcodeArgBase("operand",  OpcodeArgValueScalar::factory)
         }
      ),
      ActionFunction( // 2
         "Create Object",
         "Create an object.",
         "Create %1 at offset %6 from %3 with Forge label %4, settings (%5), and name %7. Store a reference to it in %2.",
         {
            OpcodeArgBase("type",        OpcodeArgValueMPObjectTypeIndex::factory),
            OpcodeArgBase("result",      OpcodeArgValueObject::factory),
            OpcodeArgBase("spawn point", OpcodeArgValueObject::factory),
            OpcodeArgBase("label",       OpcodeArgValueLabelIndex::factory),
            OpcodeArgBase("flags",       OpcodeArgValueCreateObjectFlags::factory),
            OpcodeArgBase("offset",      OpcodeArgValueVector3::factory),
            OpcodeArgBase("name",        OpcodeArgValueNameIndex::factory),
         }
      ),
      ActionFunction( // 3
         "Delete Object",
         "Delete an object.",
         "Delete %1.",
         {
            OpcodeArgBase("object", OpcodeArgValueObject::factory),
         }
      ),
      ActionFunction( // 4
         "Set Waypoint Visibility",
         "Control the visibility of a waypoint on an object.",
         "Modify waypoint visibility for %1: make visible to %2.",
         {
            OpcodeArgBase("object", OpcodeArgValueObject::factory),
            OpcodeArgBase("who",    OpcodeArgValuePlayerSet::factory),
         }
      ),
      ActionFunction( // 5
         "Set Waypoint Icon",
         "Control the icon of a waypoint on an object.",
         "Set %1's waypoint icon to %2.",
         {
            OpcodeArgBase("object", OpcodeArgValueObject::factory),
            OpcodeArgBase("icon",   OpcodeArgValueWaypointIcon::factory),
         }
      ),
      ActionFunction( // 6
         "Set Waypoint Priority",
         "Control the priority of a waypoint on an object.",
         "Set %1's waypoint priority to %2.",
         {
            OpcodeArgBase("object",   OpcodeArgValueObject::factory),
            OpcodeArgBase("priority", OpcodeArgValueWaypointPriorityEnum::factory),
         }
      ),
      ActionFunction( // 7
         "Set Object Displayed Timer",
         "Set which timer an object displays.",
         "Have %1 display %2.",
         {
            OpcodeArgBase("object", OpcodeArgValueObject::factory),
            OpcodeArgBase("timer",  OpcodeArgValueObjectTimerVariable::factory),
         }
      ),
      ActionFunction( // 8
         "Set Object Distance Range",
         "",
         "Set %1's distance range to minimum %2, maximum %3.",
         {
            OpcodeArgBase("object", OpcodeArgValueObject::factory),
            OpcodeArgBase("min",    OpcodeArgValueScalar::factory), // should be in the range of [-1, 100]
            OpcodeArgBase("max",    OpcodeArgValueScalar::factory), // should be in the range of [-1, 100]
         }
      ),
      ActionFunction( // 9
         "Modify Variable",
         "Modify one variable by another.",
         "Modify variable %1: %3 %2.",
         {
            OpcodeArgBase("a", OpcodeArgAnyVariableFactory, true),
            OpcodeArgBase("b", OpcodeArgAnyVariableFactory),
            OpcodeArgBase("operator", OpcodeArgValueMathOperatorEnum::factory),
         }
      ),
      ActionFunction( // 10
         "Set Object Shape",
         "Add a Shape to an object, or replace its existing Shape.",
         "Set %1's shape to %2.",
         {
            OpcodeArgBase("object", OpcodeArgValueObject::factory),
            OpcodeArgBase("shape",  OpcodeArgValueShape::factory),
         }
      ),
   }};

   bool Action::read(cobb::bitstream& stream) noexcept {
      {
         auto&  list  = actionFunctionList;
         size_t index = stream.read_bits<size_t>(cobb::bitcount(list.size() - 1));
         if (index >= list.size()) {
            printf("Bad action function ID %d.\n", index);
            return false;
         }
         this->function = &list[index];
         if (index == 0) { // The "None" condition loads no further data.
            return true;
         }
      }
      //
      auto&  base     = this->function->arguments;
      size_t argCount = base.size();
      this->arguments.resize(argCount);
      for (size_t i = 0; i < argCount; i++) {
         auto factory = base[i].factory;
         this->arguments[i] = factory(stream);
         if (this->arguments[i]) {
            if (!this->arguments[i]->read(stream)) {
               printf("Failed to load argument %d for action %s.\n", i, this->function->name);
               return false;
            }
         } else {
            printf("Failed to construct argument %d for action %s.\n", i, this->function->name);
            return false;
         }
      }
      #if _DEBUG
         this->to_string(this->debug_str);
      #endif
      return true;
   }
   void Action::to_string(std::string& out) const noexcept {
      if (!this->function) {
         out = "<NO FUNC>";
         return;
      }
      auto f = this->function->format;
      assert(f && "Cannot stringify an action whose function has no format string!");
      out.clear();
      size_t l = strlen(f);
      for (size_t i = 0; i < l; i++) {
         unsigned char c = f[i];
         if (c != '%') {
            out += c;
            continue;
         }
         c = f[++i];
         if (c == '%') {
            out += '%';
            continue;
         }
         if (c >= '1' && c <= '9') {
            c -= '1';
            if (c >= this->arguments.size()) {
               out += '%';
               out += (c + '1');
               continue;
            }
            std::string temporary;
            this->arguments[c]->to_string(temporary);
            out += temporary;
         }
      }
   }
}