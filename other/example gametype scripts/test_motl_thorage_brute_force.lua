for each player do
   if current_player.is_elite() then 
      current_player.set_loadout_palette(elite_tier_1)
   else
      current_player.set_loadout_palette(spartan_tier_1)
   end
end

alias player_count = object.number[0]
alias is_active    = object.number[1]

alias selector = global.number[0]

alias temporary_obj = global.object[0]
alias temporary_num = global.number[1]

declare selector      with network priority high = 0
declare temporary_num with network priority high = 0
declare temporary_obj with network priority low
declare object.player_count with network priority default = 0
declare object.is_active    with network priority default = 0

alias label_for_created = 3

on pregame: do
   selector = 0
end

for each object with label "cobb_motl_reset" do
   current_object.set_shape_visibility(everyone)
   current_object.set_waypoint_visibility(everyone)
   current_object.set_waypoint_priority(high)
   current_object.set_waypoint_text("Reset")
   --
   current_object.player_count = 0
   for each player do
      if current_object.shape_contains(current_player.biped) then
         current_object.player_count += 1
         if current_object.is_active == 0 then
            current_object.is_active = 1
            --
            for each object with label label_for_created do
               current_object.delete()
            end
         end
      end
   end
   if current_object.player_count == 0 then
      current_object.is_active = 0
   end
end
for each object with label "cobb_motl_increment" do
   temporary_num = current_object.spawn_sequence
   if temporary_num <= 0 then
      temporary_num = 1
   end
   current_object.set_shape_visibility(everyone)
   current_object.set_waypoint_visibility(everyone)
   current_object.set_waypoint_priority(high)
   current_object.set_waypoint_text("Sel +%n", temporary_num)
   --
   current_object.player_count = 0
   for each player do
      if current_player.is_not_respawning() and current_object.shape_contains(current_player.biped) then
         current_object.player_count += 1
         if current_object.is_active == 0 then
            current_object.is_active = 1
            selector += temporary_num
            game.play_sound_for(all_players, announce_destination_moved, true)
         end
      end
   end
   if current_object.player_count == 0 then
      current_object.is_active = 0
   end
end
for each object with label "cobb_motl_decrement" do
   temporary_num = current_object.spawn_sequence
   if temporary_num <= 0 then
      temporary_num = 1
   end
   current_object.set_shape_visibility(everyone)
   current_object.set_waypoint_visibility(everyone)
   current_object.set_waypoint_priority(high)
   current_object.set_waypoint_text("Sel -%n", temporary_num)
   --
   current_object.player_count = 0
   for each player do
      if current_player.is_not_respawning() and current_object.shape_contains(current_player.biped) then
         current_object.player_count += 1
         if current_object.is_active == 0 then
            current_object.is_active = 1
            selector -= temporary_num
            game.play_sound_for(all_players, announce_destination_moved, true)
         end
      end
   end
   if current_object.player_count == 0 then
      current_object.is_active = 0
   end
end

for each object with label "cobb_motl_spawn" do
   current_object.set_shape_visibility(everyone)
   current_object.set_waypoint_visibility(everyone)
   current_object.set_waypoint_priority(high)
   current_object.set_waypoint_text("Enter 2 Spawn!")
   --
   current_object.player_count = 0
   for each player do
      if current_player.is_not_respawning() and current_object.shape_contains(current_player.biped) then
         current_object.player_count += 1
         --
         if current_object.is_active == 0 then
            current_object.is_active = 1
            temporary_obj = no_object
            --
            if selector == 1 then
               temporary_obj = current_object.place_at_me(smg, label_for_created, none, 0, 0, 0, none)
            end
            if selector == 2 then
               temporary_obj = current_object.place_at_me(brute_shot, label_for_created, none, 0, 0, 0, none)
            end
            if selector == 3 then
               temporary_obj = current_object.place_at_me(mauler, label_for_created, none, 0, 0, 0, none)
            end
            if selector == 4 then
               temporary_obj = current_object.place_at_me(flamethrower, label_for_created, none, 0, 0, 0, none)
            end
            if selector == 5 then
               temporary_obj = current_object.place_at_me(missile_pod, label_for_created, none, 0, 0, 0, none)
            end
            if selector == 6 then
               temporary_obj = current_object.place_at_me(chopper, label_for_created, none, 0, 0, 0, none)
            end
            if selector == 7 then
               temporary_obj = current_object.place_at_me(prowler, label_for_created, none, 0, 0, 0, none)
            end
            if selector == 8 then
               temporary_obj = current_object.place_at_me(hornet, label_for_created, none, 0, 0, 0, none)
            end
            if selector == 9 then
               temporary_obj = current_object.place_at_me(stingray, label_for_created, none, 0, 0, 0, none)
            end
            if selector == 10 then
               temporary_obj = current_object.place_at_me(heavy_wraith, label_for_created, none, 0, 0, 0, none)
            end
            if selector == 11 then
               temporary_obj = current_object.place_at_me(sabre, label_for_created, none, 0, 0, 0, none)
            end
            if selector == 12 then
               temporary_obj = current_object.place_at_me(pickup_truck, label_for_created, none, 0, 0, 0, none)
            end
            if selector == 13 then
               temporary_obj = current_object.place_at_me(unsc_shield_generator, label_for_created, none, 0, 0, 0, none)
            end
            if selector == 14 then
               temporary_obj = current_object.place_at_me(cov_shield_generator, label_for_created, none, 0, 0, 0, none)
            end
            if selector == 15 then
               temporary_obj = current_object.place_at_me(wall_switch, label_for_created, none, 0, 0, 0, none)
            end
            if selector == 16 then
               temporary_obj = current_object.place_at_me(semi_truck, label_for_created, none, 0, 0, 0, none)
            end
            if selector == 17 then
               temporary_obj = current_object.place_at_me(covenant_crate, label_for_created, none, 0, 0, 0, none)
            end
            if selector == 18 then
               temporary_obj = current_object.place_at_me(flare, label_for_created, none, 0, 0, 0, none)
            end
            if selector == 19 then
               temporary_obj = current_object.place_at_me(glow_stick, label_for_created, none, 0, 0, 0, none)
            end
            if selector == 20 then
               temporary_obj = current_object.place_at_me(danger_zone, label_for_created, none, 0, 0, 0, none)
            end
            if selector == 21 then
               temporary_obj = current_object.place_at_me(data_core_beam, label_for_created, none, 0, 0, 0, none)
            end
            if selector == 22 then
               temporary_obj = current_object.place_at_me(longsword, label_for_created, none, 0, 0, 0, none)
            end
            if selector == 23 then
               temporary_obj = current_object.place_at_me(phantom_scenery, label_for_created, none, 0, 0, 0, none)
            end
            if selector == 24 then
               temporary_obj = current_object.place_at_me(pelican_scenery, label_for_created, none, 0, 0, 0, none)
            end
            if selector == 25 then
               temporary_obj = current_object.place_at_me(phantom, label_for_created, none, 0, 0, 0, none)
            end
            if selector == 26 then
               temporary_obj = current_object.place_at_me(pelican, label_for_created, none, 0, 0, 0, none)
            end
            if selector == 27 then
               temporary_obj = current_object.place_at_me(armory_shelf, label_for_created, none, 0, 0, 0, none)
            end
            if selector == 28 then
               temporary_obj = current_object.place_at_me(covenant_resupply_capsule, label_for_created, none, 0, 0, 0, none)
            end
            if selector == 29 then
               temporary_obj = current_object.place_at_me(covenant_drop_pod, label_for_created, none, 0, 0, 0, none)
            end
            if selector == 30 then
               temporary_obj = current_object.place_at_me(resupply_capsule, label_for_created, none, 0, 0, 0, none)
            end
            if selector == 31 then
               temporary_obj = current_object.place_at_me(resupply_capsule_open, label_for_created, none, 0, 0, 0, none)
            end
            if selector == 32 then
               temporary_obj = current_object.place_at_me(weapon_box, label_for_created, none, 0, 0, 0, none)
            end
            if selector == 33 then
               temporary_obj = current_object.place_at_me(tech_console_stationary, label_for_created, none, 0, 0, 0, none)
            end
            if selector == 34 then
               temporary_obj = current_object.place_at_me(tech_console_wall, label_for_created, none, 0, 0, 0, none)
            end
            if selector == 35 then
               temporary_obj = current_object.place_at_me(invisible_covenant_resupply_capsule, label_for_created, none, 0, 0, 0, none)
            end
            if selector == 36 then
               temporary_obj = current_object.place_at_me(shield_door_small, label_for_created, none, 0, 0, 0, none)
            end
            if selector == 37 then
               temporary_obj = current_object.place_at_me(shield_door_medium, label_for_created, none, 0, 0, 0, none)
            end
            if selector == 38 then
               temporary_obj = current_object.place_at_me(shield_door_large, label_for_created, none, 0, 0, 0, none)
            end
            if selector == 39 then
               temporary_obj = current_object.place_at_me(cargo_truck, label_for_created, none, 0, 0, 0, none)
            end
            if selector == 40 then
               temporary_obj = current_object.place_at_me(cart_electric, label_for_created, none, 0, 0, 0, none)
            end
            if selector == 41 then
               temporary_obj = current_object.place_at_me(forklift, label_for_created, none, 0, 0, 0, none)
            end
            if selector == 42 then
               temporary_obj = current_object.place_at_me(military_truck, label_for_created, none, 0, 0, 0, none)
            end
            if selector == 43 then
               temporary_obj = current_object.place_at_me(oni_van, label_for_created, none, 0, 0, 0, none)
            end
            if selector == 44 then
               temporary_obj = current_object.place_at_me(warthog_turret, label_for_created, none, 0, 0, 0, none)
            end
            if selector == 45 then
               temporary_obj = current_object.place_at_me(falcon_turret_grenade_left, label_for_created, none, 0, 0, 0, none)
            end
            if selector == 46 then
               temporary_obj = current_object.place_at_me(target_locator, label_for_created, none, 0, 0, 0, none)
            end
            if selector == 47 then
               temporary_obj = current_object.place_at_me(shade_turret_anti_air, label_for_created, none, 0, 0, 0, none)
            end
            if selector == 48 then
               temporary_obj = current_object.place_at_me(shade_turret_fuel_rod, label_for_created, none, 0, 0, 0, none)
            end
            if selector == 49 then
               temporary_obj = current_object.place_at_me(shade_turret_plasma, label_for_created, none, 0, 0, 0, none)
            end
            if selector == 50 then
               temporary_obj = current_object.place_at_me(sound_emitter_alarm_1, label_for_created, none, 0, 0, 0, none)
            end
            if selector == 51 then
               temporary_obj = current_object.place_at_me(sound_emitter_alarm_2, label_for_created, none, 0, 0, 0, none)
            end
            if selector == 52 then
               temporary_obj = current_object.place_at_me(package_cabinet, label_for_created, none, 0, 0, 0, none)
            end
            if selector == 53 then
               temporary_obj = current_object.place_at_me(covenant_power_module_stand, label_for_created, none, 0, 0, 0, none)
            end
            if selector == 54 then
               temporary_obj = current_object.place_at_me(covenant_bomb, label_for_created, none, 0, 0, 0, none)
            end
            if selector == 55 then
               temporary_obj = current_object.place_at_me(heavy_barrier, label_for_created, none, 0, 0, 0, none)
            end
            if selector == 56 then
               temporary_obj = current_object.place_at_me(breakpoint_bomb_door, label_for_created, none, 0, 0, 0, none)
            end
            if selector == 57 then
               temporary_obj = current_object.place_at_me(222, label_for_created, none, 0, 0, 0, none)
            end
            if selector == 58 then
               temporary_obj = current_object.place_at_me(223, label_for_created, none, 0, 0, 0, none)
            end
            if selector == 59 then
               temporary_obj = current_object.place_at_me(224, label_for_created, none, 0, 0, 0, none)
            end
            if selector == 60 then
               temporary_obj = current_object.place_at_me(225, label_for_created, none, 0, 0, 0, none)
            end
            if selector == 61 then
               temporary_obj = current_object.place_at_me(226, label_for_created, none, 0, 0, 0, none)
            end
            if selector == 62 then
               temporary_obj = current_object.place_at_me(227, label_for_created, none, 0, 0, 0, none)
            end
            if selector == 63 then
               temporary_obj = current_object.place_at_me(228, label_for_created, none, 0, 0, 0, none)
            end
            if selector == 64 then
               temporary_obj = current_object.place_at_me(229, label_for_created, none, 0, 0, 0, none)
            end
            if selector == 65 then
               temporary_obj = current_object.place_at_me(230, label_for_created, none, 0, 0, 0, none)
            end
            --
            if temporary_obj != no_object then
               game.play_sound_for(all_players, boneyard_generator_power_down, true)
            end
         end
      end
   end
   if current_object.player_count == 0 then
      current_object.is_active = 0
   end
end

for each player do -- HUD widget
   script_widget[0].set_visibility(current_player, true)
   script_widget[0].set_text("Spawn: None (custom list index %n)", selector)
   --
   if selector == 1 then
      script_widget[0].set_text("Spawn: SMG")
   end
   if selector == 2 then
      script_widget[0].set_text("Spawn: Brute Shot")
   end
   if selector == 3 then
      script_widget[0].set_text("Spawn: Mauler")
   end
   if selector == 4 then
      script_widget[0].set_text("Spawn: Flamethrower")
   end
   if selector == 5 then
      script_widget[0].set_text("Spawn: Missile Pod")
   end
   if selector == 6 then
      script_widget[0].set_text("Spawn: Chopper")
   end
   if selector == 7 then
      script_widget[0].set_text("Spawn: Prowler")
   end
   if selector == 8 then
      script_widget[0].set_text("Spawn: Hornet")
   end
   if selector == 9 then
      script_widget[0].set_text("Spawn: Stingray")
   end
   if selector == 10 then
      script_widget[0].set_text("Spawn: Heavy Wraith")
   end
   if selector == 11 then
      script_widget[0].set_text("Spawn: Sabre")
   end
   if selector == 12 then
      script_widget[0].set_text("Spawn: Pickup Truck")
   end
   if selector == 13 then
      script_widget[0].set_text("Spawn: UNSC Shield Generator")
   end
   if selector == 14 then
      script_widget[0].set_text("Spawn: Covenant Shield Generator")
   end
   if selector == 15 then
      script_widget[0].set_text("Spawn: Wall Switch")
   end
   if selector == 16 then
      script_widget[0].set_text("Spawn: Semi Truck")
   end
   if selector == 17 then
      script_widget[0].set_text("Spawn: Covenant Crate")
   end
   if selector == 18 then
      script_widget[0].set_text("Spawn: Flare")
   end
   if selector == 19 then
      script_widget[0].set_text("Spawn: Glow Stick")
   end
   if selector == 20 then
      script_widget[0].set_text("Spawn: Danger Zone")
   end
   if selector == 21 then
      script_widget[0].set_text("Spawn: Data Core Beam")
   end
   if selector == 22 then
      script_widget[0].set_text("Spawn: Longsword")
   end
   if selector == 23 then
      script_widget[0].set_text("Spawn: phantom_scenery")
   end
   if selector == 24 then
      script_widget[0].set_text("Spawn: pelican_scenery")
   end
   if selector == 25 then
      script_widget[0].set_text("Spawn: phantom")
   end
   if selector == 26 then
      script_widget[0].set_text("Spawn: pelican")
   end
   if selector == 27 then
      script_widget[0].set_text("Spawn: armory_shelf")
   end
   if selector == 28 then
      script_widget[0].set_text("Spawn: covenant_resupply_capsule")
   end
   if selector == 29 then
      script_widget[0].set_text("Spawn: covenant_drop_pod")
   end
   if selector == 30 then
      script_widget[0].set_text("Spawn: resupply_capsule")
   end
   if selector == 31 then
      script_widget[0].set_text("Spawn: resupply_capsule_open")
   end
   if selector == 32 then
      script_widget[0].set_text("Spawn: weapon_box")
   end
   if selector == 33 then
      script_widget[0].set_text("Spawn: tech_console_stationary")
   end
   if selector == 34 then
      script_widget[0].set_text("Spawn: tech_console_wall")
   end
   if selector == 35 then
      script_widget[0].set_text("Spawn: invisible_covenant_resupply_capsule")
   end
   if selector == 36 then
      script_widget[0].set_text("Spawn: shield_door_small")
   end
   if selector == 37 then
      script_widget[0].set_text("Spawn: shield_door_medium")
   end
   if selector == 38 then
      script_widget[0].set_text("Spawn: shield_door_large")
   end
   if selector == 39 then
      script_widget[0].set_text("Spawn: cargo_truck")
   end
   if selector == 40 then
      script_widget[0].set_text("Spawn: cart_electric")
   end
   if selector == 41 then
      script_widget[0].set_text("Spawn: forklift")
   end
   if selector == 42 then
      script_widget[0].set_text("Spawn: military_truck")
   end
   if selector == 43 then
      script_widget[0].set_text("Spawn: ONI Van")
   end
   if selector == 44 then
      script_widget[0].set_text("Spawn: warthog_turret")
   end
   if selector == 45 then
      script_widget[0].set_text("Spawn: falcon_turret_grenade_left")
   end
   if selector == 46 then
      script_widget[0].set_text("Spawn: target_locator")
   end
   if selector == 47 then
      script_widget[0].set_text("Spawn: shade_turret_anti_air")
   end
   if selector == 48 then
      script_widget[0].set_text("Spawn: shade_turret_fuel_rod")
   end
   if selector == 49 then
      script_widget[0].set_text("Spawn: shade_turret_plasma")
   end
   if selector == 50 then
      script_widget[0].set_text("Spawn: sound_emitter_alarm_1")
   end
   if selector == 51 then
      script_widget[0].set_text("Spawn: sound_emitter_alarm_2")
   end
   if selector == 52 then
      script_widget[0].set_text("Spawn: package_cabinet")
   end
   if selector == 53 then
      script_widget[0].set_text("Spawn: covenant_power_module_stand")
   end
   if selector == 54 then
      script_widget[0].set_text("Spawn: covenant_bomb")
   end
   if selector == 55 then
      script_widget[0].set_text("Spawn: dlc_invasion_heavy_shield")
   end
   if selector == 56 then
      script_widget[0].set_text("Spawn: dlc_invasion_bomb_door")
   end
   if selector >= 57 and selector <= 65 then
      temporary_num = selector
      temporary_num -= 57
      temporary_num += 222
      script_widget[0].set_text("Spawn: MOTL Index %n", temporary_num)
   end
end