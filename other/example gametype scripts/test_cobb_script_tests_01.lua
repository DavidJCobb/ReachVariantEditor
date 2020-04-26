alias last_executed   = object.player[0]
alias has_executed    = object.number[5]
alias spawnee         = object.object[0]
alias original_biped  = player.object[0]
alias round_card_swap = player.timer[0]
alias temp_obj_00     = global.object[0]

declare player.round_card_swap = 10

alias test_widget = script_widget[2]

for each object do -- do thrown grenades count? let's find out!
   global.number[0] = 0
   if current_object.is_of_type(frag_grenade) then
      global.number[0] += 1
   end
   script_widget[3].set_text("Frag Grenades on Map: %i", global.number[0])
   for each player do
      script_widget[3].set_visibility(current_player, true)
   end
end

for each player do
   script_widget[0].set_visibility(current_player, true)
   script_widget[1].set_visibility(current_player, true)
   script_widget[0].set_text("ALPHA")
   script_widget[1].set_text("BRAVO")
   script_widget[0].set_meter_params(number, 6, 10)
   script_widget[1].set_meter_params(number, 3, 10)
   if current_player.is_elite() then 
      current_player.set_loadout_palette(elite_tier_1)
   else
      current_player.set_loadout_palette(spartan_tier_1)
   end
   --
   -- Can we animate the round title card?
   --
   current_player.set_round_card_title("Title 01")
   current_player.set_round_card_text("Text 01")
   current_player.set_round_card_icon(attack)
   --
   current_player.round_card_swap.set_rate(-100%)
   if current_player.round_card_swap.is_zero() and 1 == 0 then -- let's not, actually; animating it is buggy
      current_player.round_card_swap.reset()
      current_player.set_round_card_title("Title 02")
      current_player.set_round_card_text("Text 02")
      current_player.set_round_card_icon(defend)
   end
end
for each object with label "cobb_perms_test" do
   current_object.set_pickup_permissions(no_one) -- only works on weapons
   current_object.set_weapon_pickup_priority(hold_action) -- does this work on grenades?
end
for each object with label "cobb_spawn_test" do
   if current_object.spawnee == no_object then
      current_object.spawnee = current_object.place_at_me(monitor, none, none, 0, 0, 0, none)
      current_object.spawnee.attach_to(current_object, 0, 0, 0, true)
   end
end
for each object with label "cobb_test" do
   current_object.set_shape_visibility(everyone)
   current_object.set_waypoint_visibility(everyone)
   --current_object.set_waypoint_range(0, 16384)
   current_object.set_waypoint_priority(high)
   current_object.set_waypoint_text("%i", current_object.spawn_sequence)
   if current_object.spawn_sequence == 1 then -- Replace biped with male Spartan
      current_object.set_waypoint_text("To Biped")
   end
   if current_object.spawn_sequence == 2 then -- Replace biped; keep reference to old
      current_object.set_waypoint_text("As Biped")
   end
   if current_object.spawn_sequence == 3 then -- Attach biped to biped
      current_object.set_waypoint_text("Attach Bipeds")
   end
   if current_object.spawn_sequence == 4 then -- Assign integer to object
      current_object.set_waypoint_text("Obj = Int")
   end
   if current_object.spawn_sequence == 5 then -- Assign object to integer
      current_object.set_waypoint_text("Int = Obj")
   end
   if current_object.spawn_sequence == 5 then -- Assign player to integer
      current_object.set_waypoint_text("Int = Player")
   end
   if current_object.spawn_sequence == 6 then
      current_object.set_waypoint_text("Become Moderator")
   end
   if current_object.spawn_sequence == 7 then -- Award Wildlife Kill Medal?
      current_object.set_waypoint_text("Medal")
   end
   if current_object.spawn_sequence == 8 then
      current_object.set_waypoint_text("Delete Biped")
   end
   if current_object.spawn_sequence == 9 then
      current_object.set_waypoint_text("Game Over")
   end
   if current_object.spawn_sequence == 10 then
      current_object.set_waypoint_text("Neutral Team")
   end
   if current_object.spawn_sequence == 11 then
      current_object.set_waypoint_text("No Team")
   end
   if current_object.spawn_sequence == 12 then
      current_object.set_waypoint_text("MOTL Tests")
   end
   --
   alias player_count = object.number[0]
   alias volume_count = player.number[0]
   current_object.player_count = 0
   for each player do
      current_player.volume_count = 0
      if current_object.shape_contains(current_player.biped) then
         current_object.player_count += 1
         current_player.volume_count += 1
         --
         if current_object.last_executed != current_player then
            current_object.last_executed = current_player
            --
            -- Run tests.
            --
            if current_object.spawn_sequence == 1 then -- Replace biped with male Spartan
               alias new_biped = temp_obj_00
               --
               new_biped = current_player.biped.place_at_me(spartan, none, none, 0, 0, 0, male)
               current_player.set_biped(new_biped)
               test_widget.set_text("Biped swapped. Old biped not stored in a variable.")
            end
            if current_object.spawn_sequence == 2 then -- Replace biped; keep reference to old
               alias new_biped = temp_obj_00
               --
               current_player.original_biped = current_player.biped
               new_biped = current_player.biped.place_at_me(spartan, none, none, 0, 0, 5, carter)
               current_player.set_biped(new_biped)
               test_widget.set_text("Biped swapped. Old biped stored in a variable.")
            end
            if current_object.spawn_sequence == 3 then -- Attach biped to biped
               alias new_biped = temp_obj_00
               --
               new_biped = current_player.biped.place_at_me(spartan, none, none, 0, 0, 0, male)
               new_biped.attach_to(current_player.biped, 0, 0, 10, true)
               test_widget.set_text("Attempted to spawn a biped and attach it to you.")
            end
            if current_object.spawn_sequence == 4 then -- Assign integer to object
               global.number[0] = 5
               temp_obj_00 = global.number[0]
               test_widget.set_text("Object: %s", temp_obj_00)
               temp_obj_00 = current_player.biped.place_at_me(elite, none, none, 0, 0, 0, ultra)
            end
            if current_object.spawn_sequence == 5 then -- Assign object to integer
               global.number[0] = current_player.biped
               test_widget.set_text("Number: %i", global.number[0])
               temp_obj_00 = current_player.biped.place_at_me(elite, none, none, 0, 0, 0, ultra)
            end
            if current_object.spawn_sequence == 5 then -- Assign player to integer
               global.number[0] = current_player
               test_widget.set_text("Number: %i", global.number[0])
               temp_obj_00 = current_player.biped.place_at_me(elite, none, none, 0, 0, 0, ultra)
            end
            if current_object.spawn_sequence == 6 then
               temp_obj_00 = current_player.biped.place_at_me(monitor, none, none, 0, 0, 5, none)
               current_player.set_biped(temp_obj_00)
               test_widget.set_text("Biped swapped. Old biped not stored in a variable.")
               current_player.biped.add_weapon(gravity_hammer, primary)
            end
            if current_object.spawn_sequence == 7 then -- Medal
               test_widget.set_text("Medal")
               send_incident(headshot_kill, current_player, current_player)
               send_incident(headshot_kill, current_player, current_player)
               send_incident(headshot_kill, current_player, current_player)
               send_incident(headshot_kill, current_player, current_player)
               send_incident(headshot_kill, current_player, current_player)
               send_incident(headshot_kill, current_player, current_player)
               send_incident(headshot_kill, current_player, current_player)
               send_incident(headshot_kill, current_player, current_player)
               send_incident(headshot_kill, current_player, current_player)
               send_incident(headshot_kill, current_player, current_player)
               temp_obj_00 = current_player.biped.place_at_me(elite, none, none, 0, 0, 0, ultra)
            end
            if current_object.spawn_sequence == 8 then
               current_player.biped.delete()
            end
            if current_object.spawn_sequence == 9 then
               send_incident(game_over, all_players, all_players)
            end
            if current_object.spawn_sequence == 10 then
               current_player.team = neutral_team
            end
            if current_object.spawn_sequence == 11 then
               current_player.team = no_team
            end
            if current_object.spawn_sequence == 12 then
               temp_obj_00 = current_object.place_at_me(fx_green, none, none, 0, 0, 0, none)
               temp_obj_00 = current_object.place_at_me(package_cabinet, none, none, 0, 0, 0, none)
               temp_obj_00 = current_object.place_at_me(oni_van, none, none, 0, 0, 0, none)
            end
            temp_obj_00 = no_object
         end
      end
      if current_player.volume_count > 0 then
         test_widget.set_visibility(current_player, true)
      else
         test_widget.set_visibility(current_player, false)
      end
   end
   if current_object.player_count == 0 then
      current_object.last_executed = no_player
   end
end