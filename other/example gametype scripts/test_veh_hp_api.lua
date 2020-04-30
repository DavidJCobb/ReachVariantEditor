
alias detach_timer      = player.timer[0]
alias is_active         = object.number[0]
alias test_vehicle      = global.object[0]
alias veh_hp_ui         = player.number[0]
alias cached_max_health = global.number[2]
alias temporary         = global.number[3]

declare player.detach_timer = 10

for each object with label "test_attach_player" do
   alias contains_any = global.number[0]
   --
   current_object.set_shape_visibility(everyone)
   current_object.set_waypoint_visibility(everyone)
   current_object.set_waypoint_priority(high)
   current_object.set_waypoint_text("Attach Player")
   contains_any = 0
   for each player do
      if current_object.shape_contains(current_player.biped) then
         contains_any = 1
         if current_object.is_active == 0 then
            current_object.is_active = 1
            --
            current_player.biped.attach_to(current_object, 0, 0, 0, absolute)
            current_player.detach_timer.reset()
            current_player.detach_timer.set_rate(-100%)
         end
      end
   end
   if contains_any == 0 then
      current_object.is_active = 0
   end
end
for each player do
   if current_player.detach_timer.is_zero() then
      current_player.detach_timer.set_rate(0%)
      current_player.detach_timer.reset()
      current_player.biped.detach()
   end
   if current_player.killer_type_is(guardians | suicide | kill | betrayal | quit) then -- on player death
      current_player.detach_timer.set_rate(0%)
      current_player.detach_timer.reset()
      current_player.biped.detach()
   end
end

for each object with label "test_delete_player" do
   alias contains_any = global.number[0]
   --
   current_object.set_shape_visibility(everyone)
   current_object.set_waypoint_visibility(everyone)
   current_object.set_waypoint_priority(high)
   current_object.set_waypoint_text("Delete Player")
   contains_any = 0
   for each player do
      if current_object.shape_contains(current_player.biped) then
         contains_any = 1
         if current_object.is_active == 0 then
            current_object.is_active = 1
            --
            current_player.biped.delete()
         end
      end
   end
   if contains_any == 0 then
      current_object.is_active = 0
   end
end

for each object with label "test_force_veh" do
   --
   -- Label to be applied to a vehicle with a shape. Can scripts force the player into a 
   -- destroyed or overturned vehicle?
   --
   alias contains_any = global.number[0]
   --
   current_object.set_shape_visibility(everyone)
   current_object.set_waypoint_visibility(everyone)
   current_object.set_waypoint_priority(high)
   current_object.set_waypoint_text("Force Into")
   contains_any = 0
   for each player do
      if current_object.shape_contains(current_player.biped) then
         contains_any = 1
         if current_object.is_active == 0 then
            current_object.is_active = 1
            --
            current_player.force_into_vehicle(current_object)
         end
      end
   end
   if contains_any == 0 then
      current_object.is_active = 0
   end
end

for each object with label "test_veh_hp_spawn" do
   current_object.set_shape_visibility(everyone)
   current_object.set_waypoint_visibility(everyone)
   current_object.set_waypoint_priority(high)
   current_object.set_waypoint_text("Test Vehicle Spawn")
   if test_vehicle == no_object then
      test_vehicle = current_object.place_at_me(warthog, none, none, 0, 0, 0, none)
      cached_max_health = 100
   end
end
for each object with label "test_veh_hp_inc" do
   alias contains_any = global.number[0]
   --
   temporary = current_object.spawn_sequence
   current_object.set_shape_visibility(everyone)
   current_object.set_waypoint_visibility(everyone)
   current_object.set_waypoint_priority(high)
   current_object.set_waypoint_text("HP +%n", temporary)
   contains_any = 0
   for each player do
      if current_object.shape_contains(current_player.biped) then
         contains_any = 1
         if current_object.is_active == 0 then
            current_object.is_active = 1
            --
            test_vehicle.health += current_object.spawn_sequence
         end
      end
   end
   if contains_any == 0 then
      current_object.is_active = 0
   end
end
for each object with label "test_veh_hp_dec" do
   alias contains_any = global.number[0]
   --
   temporary = current_object.spawn_sequence
   current_object.set_shape_visibility(everyone)
   current_object.set_waypoint_visibility(everyone)
   current_object.set_waypoint_priority(high)
   current_object.set_waypoint_text("HP -%n", temporary)
   contains_any = 0
   for each player do
      if current_object.shape_contains(current_player.biped) then
         contains_any = 1
         if current_object.is_active == 0 then
            current_object.is_active = 1
            --
            test_vehicle.health -= current_object.spawn_sequence
         end
      end
   end
   if contains_any == 0 then
      current_object.is_active = 0
   end
end
for each object with label "test_veh_hp_reset" do
   alias contains_any = global.number[0]
   --
   current_object.set_shape_visibility(everyone)
   current_object.set_waypoint_visibility(everyone)
   current_object.set_waypoint_priority(high)
   current_object.set_waypoint_text("Reset")
   contains_any = 0
   for each player do
      if current_object.shape_contains(current_player.biped) then
         contains_any = 1
         if current_object.is_active == 0 then
            current_object.is_active = 1
            --
            test_vehicle.delete()
         end
      end
   end
   if contains_any == 0 then
      current_object.is_active = 0
   end
end
for each object with label "test_veh_hp_max_inc" do
   alias contains_any = global.number[0]
   --
   temporary = current_object.spawn_sequence
   current_object.set_shape_visibility(everyone)
   current_object.set_waypoint_visibility(everyone)
   current_object.set_waypoint_priority(high)
   current_object.set_waypoint_text("Max HP +%n", temporary)
   contains_any = 0
   for each player do
      if current_object.shape_contains(current_player.biped) then
         contains_any = 1
         if current_object.is_active == 0 then
            current_object.is_active = 1
            --
            cached_max_health += current_object.spawn_sequence
            test_vehicle.max_health = cached_max_health
         end
      end
   end
   if contains_any == 0 then
      current_object.is_active = 0
   end
end
for each object with label "test_veh_hp_max_dec" do
   alias contains_any = global.number[0]
   --
   temporary = current_object.spawn_sequence
   current_object.set_shape_visibility(everyone)
   current_object.set_waypoint_visibility(everyone)
   current_object.set_waypoint_priority(high)
   current_object.set_waypoint_text("Max HP -%n", temporary)
   contains_any = 0
   for each player do
      if current_object.shape_contains(current_player.biped) then
         contains_any = 1
         if current_object.is_active == 0 then
            current_object.is_active = 1
            --
            cached_max_health -= current_object.spawn_sequence
            test_vehicle.max_health = cached_max_health
         end
      end
   end
   if contains_any == 0 then
      current_object.is_active = 0
   end
end

for each object with label "test_detach_player" do
   alias contains_any = global.number[0]
   --
   current_object.set_shape_visibility(everyone)
   current_object.set_waypoint_visibility(everyone)
   current_object.set_waypoint_priority(high)
   current_object.set_waypoint_text("Detach Player")
   contains_any = 0
   for each player do
      if current_object.shape_contains(current_player.biped) then
         contains_any = 1
         if current_object.is_active == 0 then
            current_object.is_active = 1
            --
            current_player.biped.detach()
         end
      end
   end
   if contains_any == 0 then
      current_object.is_active = 0
   end
end

for each player do
   current_player.veh_hp_ui = 0
   if test_vehicle != no_object then
      current_player.veh_hp_ui = test_vehicle.health
   end
   script_widget[0].set_visibility(current_player, true)
   script_widget[0].set_text("Test Vehicle HP: %n", hud_player.veh_hp_ui)
end