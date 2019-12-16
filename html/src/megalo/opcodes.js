const CONDITION_ID_BITLENGTH    = bitcount(17 - 1);  // condition function ID
const CONDITION_INDEX_BITLENGTH = bitcount(512 - 1); // index in a script's list of conditions
const ACTION_INDEX_BITLENGTH = bitcount(1024 - 1); // index in a script's list of actions

class MegaloScriptOpcodeArg {
   constructor(type) {
      this.type  = type;
      this.value = null;
   }
   parse(stream) {
      switch (this.type) {
         case "bool":
            this.value = stream.readBits(1) != 0;
            break;
            
            
            
      }
   }
}

class MegaloScriptCondition {
   constructor() {
      this.opcodeBase = null;
      this.inverted   = false;
      this.owningTriggerIndex  = -1;
      this.executeBeforeAction = -1;
      //
      this.arguments = []; // Array<MegaloScriptOpcodeArg>
   }
   parse(stream) {
      let type = stream.readBits(CONDITION_ID_BITLENGTH);
      if (type != 0) {
         this.opcodeBase = g_megaloConditions[type];
         this.inverted   = stream.readBits(1) != 0;
         this.owningTriggerIndex  = stream.readBits(CONDITION_INDEX_BITLENGTH);
         this.executeBeforeAction = stream.readBits(ACTION_INDEX_BITLENGTH);
      }
      if (this.opcodeBase) {
         let base     = this.opcodeBase;
         let argCount = base.args.length;
         this.arguments = new Array(argCount);
         for(let i = 0; i < argCount; i++) {
            this.arguments[i] = stream.readUInt32();
         }
      }
   }
}