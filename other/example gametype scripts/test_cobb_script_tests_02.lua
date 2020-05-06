
alias temp_int_00 = global.number[0]
alias temp_int_01 = global.number[1]
alias selector    = global.number[2]
alias temp_obj_00 = global.object[0]
alias temp_obj_01 = global.object[1]
alias temp_obj_02 = global.object[2]
alias temp_obj_03 = global.object[3]
alias temp_plr_00 = global.player[0]

alias player_count = object.number[0]
alias is_active    = object.number[1]
alias waypoint_num = global.number[3]
declare waypoint_num with network priority high = 0

alias ui_call_depth_sel = script_widget[0]

for each player do -- set loadout palettes
   if current_player.is_elite() then 
      current_player.set_loadout_palette(elite_tier_1)
   else
      current_player.set_loadout_palette(spartan_tier_1)
   end
end

--
-- Code to test the game's max call depth
--
on pregame: do
   selector = 0
end
for each player do
   ui_call_depth_sel.set_text("Queued Call Depth: %n", selector)
   ui_call_depth_sel.set_visibility(current_player, true)
end
alias depth_test_iterator = global.number[4]
if depth_test_iterator > 0 then
   temp_int_00  = selector
   temp_int_00 -= depth_test_iterator
   for each player do
      game.show_message_to(current_player, none, "Depth test completed only %n iterations of %n, and then halted the script", temp_int_00, selector)
   end
   depth_test_iterator = 0
end
function depth_test()
   depth_test_iterator -= 1
   if depth_test_iterator > 0 then
      depth_test()
   end
end
for each object with label "test_depth_add" do
   waypoint_num = current_object.spawn_sequence
   if waypoint_num <= 0 then
      waypoint_num = 1
   end
   current_object.set_shape_visibility(everyone)
   current_object.set_waypoint_visibility(everyone)
   current_object.set_waypoint_priority(high)
   current_object.set_waypoint_text("Depth +%n", waypoint_num)
   --
   current_object.player_count = 0
   for each player do
      if current_player.is_not_respawning() and current_object.shape_contains(current_player.biped) then
         current_object.player_count += 1
         if current_object.is_active == 0 then
            current_object.is_active = 1
            selector += waypoint_num
         end
      end
   end
   if current_object.player_count == 0 then
      current_object.is_active = 0
   end
end
for each object with label "test_depth_sub" do
   waypoint_num = current_object.spawn_sequence
   if waypoint_num <= 0 then
      waypoint_num = 1
   end
   current_object.set_shape_visibility(everyone)
   current_object.set_waypoint_visibility(everyone)
   current_object.set_waypoint_priority(high)
   current_object.set_waypoint_text("Depth -%n", waypoint_num)
   --
   current_object.player_count = 0
   for each player do
      if current_player.is_not_respawning() and current_object.shape_contains(current_player.biped) then
         current_object.player_count += 1
         if current_object.is_active == 0 then
            current_object.is_active = 1
            selector -= waypoint_num
         end
      end
   end
   if current_object.player_count == 0 then
      current_object.is_active = 0
   end
end
for each object with label "test_depth_exec" do
   current_object.set_shape_visibility(everyone)
   current_object.set_waypoint_visibility(everyone)
   current_object.set_waypoint_priority(high)
   current_object.set_waypoint_text("Test Depth")
   --
   current_object.player_count = 0
   for each player do
      if current_player.is_not_respawning() and current_object.shape_contains(current_player.biped) then
         current_object.player_count += 1
         if current_object.is_active == 0 then
            current_object.is_active = 1
            depth_test_iterator = selector
         end
      end
   end
   if current_object.player_count == 0 then
      current_object.is_active = 0
   end
end
do
   if depth_test_iterator > 0 then
      depth_test()
      if depth_test_iterator == 0 then
         for each player do
            game.show_message_to(current_player, none, "Depth test completed all %n iterations", selector)
         end
      end
      if depth_test_iterator > 0 then
         temp_int_00  = selector
         temp_int_00 -= depth_test_iterator
         for each player do
            game.show_message_to(current_player, none, "Depth test completed only %n iterations of %n, but the script was not halted", temp_int_00, selector)
         end
         depth_test_iterator = 0
      end
   end
end

for each object with label "test_scoreboardpos" do
   alias has = temp_int_00
   has = 0
   for each player do
      if current_object.shape_contains(current_player.biped) then
         has = 1
      end
   end
   if has == 1 then
      temp_plr_00 = no_player
      temp_int_00 = 99
      temp_int_00 = temp_plr_00.get_scoreboard_pos()
      for each player do
         game.show_message_to(current_player, none, "Scoreboard position for NONE: %n", temp_int_00)
      end
   end
end

for each object with label "test_detach_items" do
   for each player do
      if current_object.shape_contains(current_player.biped) then
         temp_obj_00 = current_player.get_armor_ability()
         temp_obj_01 = current_player.get_weapon(primary)
         temp_obj_02 = current_player.get_weapon(secondary)
         --
         temp_obj_00.detach()
         temp_obj_01.detach()
         temp_obj_02.detach()
         --
         -- Make sure the player doesn't just pick them up again, if they are indeed dropped:
         --
         if temp_obj_00 != no_object or temp_obj_01 != no_object or temp_obj_02 != no_object then
            temp_obj_03 = current_player.biped.place_at_me(hill_marker, none, none, 0, 0, 30, none)
            temp_obj_00.attach_to(temp_obj_03, 0, 0, 0, absolute)
            temp_obj_01.attach_to(temp_obj_03, 0, 0, 0, absolute)
            temp_obj_01.attach_to(temp_obj_03, 0, 0, 0, absolute)
            temp_obj_00.detach()
            temp_obj_01.detach()
            temp_obj_02.detach()
            temp_obj_03.delete()
         end
      end
   end
end

