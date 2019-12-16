class MegaloConditionFunction {
   constructor(name, desc, format, args, details) {
      this.name    = name;
      this.desc    = desc;
      this.format  = format;
      this.args    = args || [];
      details = details || {};
      this.verb_normal = details.verb_normal || "is";
      this.verb_invert = details.verb_invert || "is not";
   }
}

let g_conditionList = [
   new MegaloConditionFunction( // 0
      "None",
      "This condition does nothing.",
      "None."
   ),
   new MegaloConditionFunction( // 1
      "Compare",
      "Compares the values of two variables.",
      "%1 %v %3 %2.",
      [
         MegaloValueAnyFactory,
         MegaloValueAnyFactory,
         MegaloValueCompareOperatorEnum.factory,
      ]
   ),
   new MegaloConditionFunction( // 2
      "In Boundary",
      "Checks whether one object is inside of another object's Shape.",
      "%1 %v inside of %2's shape.",
      [
         MegaloValueObject.factory,
         MegaloValueObject.factory,
      ]
   ),
   new MegaloConditionFunction( // 3
      "Killer Type",
      "Checks what killed a player.",
      "%1 %v killed by any of: %2.",
      [
         MegaloValuePlayer.factory, // victim
         MegaloValueKillerTypeFlags.factory,
      ],
      { verb_normal: "was", verb_invert: "was not" }
   ),
   new MegaloConditionFunction( // 4
      "Team Disposition", // Alliance Status?
      "Unknown.",
      "Team %1 %v disposition %3 with team %2.",
      [
         MegaloValueTeam.factory,
         MegaloValueTeam.factory,
         MegaloValueTeamDispositionEnum.factory,
      ],
      { verb_normal: "has", verb_invert: "does not have" }
   ),
   new MegaloConditionFunction( // 5
      "Timer Is Zero",
      "Checks whether a timer is zero.",
      "Timer %1 %v at zero.",
      [ MegaloValueTimer.factory ]
   ),
   new MegaloConditionFunction( // 6
      "Objects Are Same Type",
      "Checks whether two objects are of the same type.",
      "%1 %v of the same type as %2.",
      [
         MegaloValueObject.factory,
         MegaloValueMPObjectTypeIndex.factory, // TODO: increment by 1 after reading
      ]
   ),
   new MegaloConditionFunction( // 7
      "Team Has Players",
      "Checks whether a team has one or more players on it.",
      "%1 %v one or more players on it.",
      [ MegaloValueTeam.factory ],
      { verb_normal: "has", verb_invert: "does not have" }
   ),
   new MegaloConditionFunction( // 8
      "Object Out Of Bounds",
      "Checks whether an object has fallen out of bounds, e.g. into a Soft Kill Zone.",
      "%1 %v out of bounds.",
      [ MegaloValueObject.factory ]
   ),
   new MegaloConditionFunction( // 9
      "Deprecated-09",
      "This condition always returns false.", // TODO: does it return (true) if inverted?
      "Never. (Unused argument: %1)",
      [ MegaloValuePlayer.factory ]
   ),
   new MegaloConditionFunction( // 10
      "Player Assisted Kill Of",
      "Checks whether one player assisted in the slaying of another player. Note that you don't count as \"assisting\" your own kills.",
      "%1 %v in the killing of %2.",
      [
         MegaloValuePlayer.factory, // attacker
         MegaloValuePlayer.factory, // victim
      ],
      { verb_normal: "assisted", verb_invert: "did not assist" }
   ),
   new MegaloConditionFunction( // 11
      "Object Has Label",
      "Checks whether an object has a given Forge label.",
      "%1 %v label %2.",
      [
         MegaloValueObject.factory,
         MegaloValueLabelIndex.factory,
      ],
      { verb_normal: "has", verb_invert: "does not have" }
   ),
   new MegaloConditionFunction( // 12
      "Player Is Not Respawning",
      "Checks whether a player isn't on the respawn screen.",
      "%1 %v respawning.",
      [ MegaloValuePlayer.factory ]
   ),
   new MegaloConditionFunction( // 13
      "Equipment In Use",
      "Unknown.",
      "%1 %v in use.",
      [ MegaloValueObject.factory ]
   ),
   new MegaloConditionFunction( // 14
      "Species Is Spartan",
      "Checks whether a player is a Spartan.",
      "%1 %v a Spartan.",
      [ MegaloValuePlayer.factory ]
   ),
   new MegaloConditionFunction( // 15
      "Species Is Elite",
      "Checks whether a player is an Elite.",
      "%1 %v an Elite.",
      [ MegaloValuePlayer.factory ]
   ),
   new MegaloConditionFunction( // 16
      "Species Is Monitor",
      "Checks whether a player is a Monitor.",
      "%1 %v a Monitor.",
      [ MegaloValuePlayer.factory ]
   ),
   new MegaloConditionFunction( // 17
      "Is Matchmaking",
      "This opcode's function is unverified, but it's believed to test whether the current match is in Matchmaking.",
      "This match %v taking place in Matchmaking."
   ),
];


class MegaloCondition {
   constructor() {
      this.func     = null;
      this.inverted = false;
      this.or_group = 0;
      this.action   = 0;
      this.args     = [];
      this.string   = "";
   }
   read(stream) {
      let fi = stream.readBits(_bitcount(g_conditionList.length - 1));
      if (fi == 0) // special-case: read no data for None.
         return true;
      this.func = g_conditionList[fi];
      if (!this.func) {
         console.warn("Condition Function #" + fi + " does not exist.");
         return false;
      }
      this.inverted = stream.readBits(1) != 0;
      this.or_group = stream.readBits(_bitcount(512 - 1));
      this.action   = stream.readBits(_bitcount(1024 - 1));
      //
      for(let i = 0; i < this.func.args.length; i++) {
         let factory = this.func.args[i];
         if (!factory || typeof factory != "function") {
            console.warn("This is not a MegaloValue factory: ", factory);
            throw new TypeError("Bad MegaloValue factory specified in a condition function's arguments.");
         }
         this.args[i] = factory(stream);
         if (!this.args[i]) { // bad any-type-value, typically
            console.warn("Failed to extract argument " + i + " in this condition.");
            return false;
         }
         this.args[i].read(stream);
      }
      this.string = this.toString();
      return true;
   }
   toString() {
      if (!this.func)
         return "<NO FUNC>";
      let s = "";
      let f = this.func.format;
      for(let i = 0; i < f.length; i++) {
         let c = f[i];
         if (c != "%") {
            s += c;
            continue;
         }
         c = f[++i];
         if (c == "%") {
            s += "%";
            continue;
         }
         if (c == "v") {
            s += this.inverted ? this.func.verb_invert : this.func.verb_normal;
            continue;
         }
         if (!isNaN(+c)) {
            let a = this.args[(+c) - 1];
            if (a)
               s += a.toString();
            else
               s += "%" + c;
         }
      }
      return s;
   }
}