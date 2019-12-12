const PLAYER_TRAITS_SPAWN_ITEM_UNCHANGED = -3; // weapon or ability

let BooleanTrait = new TypedEnum({
   name: "BooleanTrait",
}, {
   unchanged: 0,
   disabled:  1,
   enabled:   2,
});
let ArmorAbility = new TypedEnum({
   name: "ArmorAbility",
}, {
   // -4: random?
   unchanged:  -3,
   // -2: map default?
   none:       -1,
   sprint:      0,
   jetpack:     1,
   armor_lock:  2,
   // 3
   active_camo: 4,
   // 5
   // 6
   hologram:    7,
   evade:       8,
   drop_shield: 9,
});
let Weapon = new TypedEnum({
   name: "Weapon",
}, {
   random:           -4,
   unchanged:        -3, // default
   map_default:      -2,
   none:             -1, // UI doesn't allow this for primary weapon
   dmr:               0,
   assault_rifle:     1,
   plasma_pistol:     2,
   spiker:            3,
   energy_sword:      4,
   magnum:            5,
   needler:           6,
   plasma_rifle:      7,
   rocket_launcher:   8,
   shotgun:           9,
   sniper_rifle:     10,
   spartan_laser:    11,
   gravity_hammer:   12,
   plasma_repeater:  13,
   needle_rifle:     14,
   focus_rifle:      15,
   plasma_launcher:  16,
   concussion_rifle: 17,
   grenade_launcher: 18,
   golf_club:        19,
   fuel_rod_gun:     20,
});

class GameVariantPlayerTraits {
   constructor(bitstream) {
      this.health = {
         damageResist:   new Bitnumber(4, 0),
         healthMult:     new Bitnumber(3, 0),
         healthRate:     new Bitnumber(4, 0),
         shieldMult:     new Bitnumber(3, 0),
         shieldRate:     new Bitnumber(4, 0),
         shieldDelay:    new Bitnumber(4, 0),
         headshotImmune: new Bitnumber(2, 0, { enum: BooleanTrait }),
         vampirism :     new Bitnumber(3, 0),
         assassinImmune: new Bitnumber(2, 0, { enum: BooleanTrait }),
         unk09:          new Bitnumber(2, 0),
      };
      this.weapons = {
         damageMult:      new Bitnumber(4, 0),
         meleeMult:       new Bitnumber(4, 0),
         primaryWeapon:   new Bitnumber(8, -3, { signed: true, enum: Weapon }),
         secondaryWeapon: new Bitnumber(8, -3, { signed: true, enum: Weapon }),
         grenadeCount:    new Bitnumber(4, 0),
         infiniteAmmo:    new Bitnumber(2, 0),
         grenadeRegen:    new Bitnumber(2, 0),
         weaponPickup:    new Bitnumber(2, 0),
         abilityDrop:     new Bitnumber(2, 0), // this and the next two may actually be one value
         infiniteAbility: new Bitnumber(2, 0), // 
         abilityUsage:    new Bitnumber(2, 0), // 
         ability:         new Bitnumber(8, -3, { signed: true, enum: ArmorAbility }),
      };
      this.movement = {
         speed:        new Bitnumber(5, 0),
         gravity:      new Bitnumber(4, 0),
         vehicleUsage: new Bitnumber(4, 0),
         unknown:      new Bitnumber(2, 0),
         jumpHeight:   new Bitnumber(9, -1, { presence_bit: true, if_absent: -1 }),
      };
      this.appearance = {
         activeCamo:  new Bitnumber(3, 0),
         waypoint:    new Bitnumber(2, 0),
         visibleName: new Bitnumber(2, 0),
         aura:        new Bitnumber(3, 0),
         forcedColor: new Bitnumber(4, 0),
      };
      this.sensors = {
         radarState: new Bitnumber(3, 0),
         radarRange: new Bitnumber(3, 0),
         directionalDamageIndicator: new Bitnumber(2, 0),
      };
      //
      if (bitstream)
         this.parse(bitstream);
   }
   parse(stream) {
      //
      // We can just blindly loop over the values. JavaScript keeps 
      // fields in the order they were defined, though debuggers may 
      // sort them alphabetically when displaying them.
      //
      for(let k in this.health)
         this.health[k].read(stream);
      for(let k in this.weapons)
         this.weapons[k].read(stream);
      for(let k in this.movement)
         this.movement[k].read(stream);
      for(let k in this.appearance)
         this.appearance[k].read(stream);
      for(let k in this.sensors)
         this.sensors[k].read(stream);
   }
}

class GameVariantMegaloPlayerTraits extends GameVariantPlayerTraits {
   constructor(stream) {
      let nameStringIndex = stream.readMegaloVariantStringIndex();
      let descStringIndex = stream.readMegaloVariantStringIndex();
      super(stream);
      this.nameStringIndex = nameStringIndex;
      this.descStringIndex = descStringIndex;
   }
}