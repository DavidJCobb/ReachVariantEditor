alias current_haven     = global.object[1]
alias infection_spree   = player.number[5] -- for achievements?
alias is_alpha_zombie   = player.number[3]
alias is_human_in_haven = player.number[2]
alias is_last_human     = player.number[1] -- used to avoid giving Zombie Victory Points to the newly-infected Last Man Standing
alias is_zombie         = player.number[0]
alias haven_exists      = global.number[2] -- used to avoid announcing "Hill Moved" when spawning first Haven
alias survival_timer    = player.timer[1] -- for post-game carnage report stats, which MCC doesn't display

function count_survivors()
   expect global.number[0] == 0
   for each player do
      if current_player.is_zombie == 0 then
         global.number[0] += 1
      end
   end
end

for each player do -- trigger 0
   if current_player.killed_by(kill) then
      global.player[0] = current_player.killer -- implicit opcode: global.player[0] = none; then this. if you don't want the initial assign, use try_get_killer()
      global.number[0] = current_player.kill_damage_modifiers -- implicit opcode
      if global.number[0] == 2 then
         global.object[0] = current_player.try_get_armor_ability() -- implicit opcode
         if global.object[0].has_forge_label(0) and global.object[0].is_in_use then
            send_incident(dlc_achieve_2, global.player[0], global.player[0], 65)
         end
      end
   end
end

for each player do
   if current_player.killed_by(kill) then
      global.number[0] = current_player.kill_damage_modifiers
      if global.number[0] == 4 then
         global.player[0] = current_player.killer
         if global.player[0].killed_by(suicide) then
            send_incident(dlc_achieve_2, current_player, current_player, 68)
         end
      end
   end
end

for each player do
   if current_player.is_zombie != 1 then
      if current_player.species == elite then
         current_player.loadout_palette = 2
      else
         current_player.loadout_palette = 1
      end
   else
      if current_player.species == elite then
         current_player.loadout_palette = 4
      else
         current_player.loadout_palette = 3
      end
   end
end

do -- maintain minimum alpha zombie count
   alias alpha_zombie_count   = global.number[0]
   alias max_possible_zombies = global.number[4] -- player count minus one
   --
   alpha_zombie_count   =  0
   max_possible_zombies = -1
   for each player do
      max_possible_zombies += 1
      if current_player.is_alpha_zombie == 1 then
         alpha_zombie_count += 1
      end
   end
   for each player randomly do
      if  alpha_zombie_count < script_option[0]
      and alpha_zombie_count < max_possible_zombies
      and current_player.number[1] != 1
      and current_player.is_zombie != 1
      then
         current_player.is_zombie = 1
         current_player.timer[2]  = 3
         current_player.is_alpha_zombie = 1
         alpha_zombie_count += 1
      end
   end
   for each player do
      if current_player.is_zombie == 1 and current_player.team != team_3 then
         send_incident(inf_new_infection, current_player, none)
         current_player.team = team_3 -- haven't decided whether I want "team_3" or "team[3]" or "team#3" or "team 3" where "team" is a keyword
         script_traits[0].apply(current_player)
         current_player.biped.kill(true) -- silent
      end
   end
end

for each player do
   hud_widget[0].format(73, global.timer[0]) -- can use a string index or string literal
   hud_widget[0].hide(current_player)
end

for each player do
   current_player.timer[0].rate = -100
   for each player do
      if current_player.team == team_2 then
         current_player.objective_title = ... -- didn't write this down
      end
   end
   for each player do
      if current_player.team == team_3 then
         current_player.objective_title = ... -- didn't write this down
      end
   end
end

for each player do
   if current_player.number[4] == 0 and current_player.timer[0].is_zero then
      send_incident(129, current_player, none)
      current_player.number[4] = 1
   end
end

for each player do -- trigger 18
   current_player.team = team_2 -- human
   if current_player.number[0] == 1 then
      current_player.team = team_3 -- zombie?
      script_traits[0].apply(current_player) -- infected
      if current_player.is_alpha_zombie == 1 and script_option[13] == 1 then
         script_traits[1].apply(current_player) -- alpha zombie
      end
   end
end

for each player do -- trigger 19: handle kills
   if current_player.killed_by(any) then
      alias victim = global.player[0]
      alias killer = global.player[1]
      --
      current_player.number[1] = 0
      victim = current_player
      killer = current_player.killer
      if  victim.killed_by(kill)
      and victim.is_zombie == 1
      and victim.is_zombie != killer.is_zombie
      then
         killer.score += script_option[7]
         send_incident(zombie_kill_kill, killer, victim)
      end
      if  victim.killed_by(kill)
      and script_option[2] == 1 -- safe havens
      and victim.is_zombie == 1
      and victim.is_zombie != killer.is_zombie
      and killer.is_human_in_haven == 1
      then
         killer.score += script_option[6] -- points for killing a zombie from a haven
         send_incident(zombie_kill_kill, killer, victim)
      end
      if  victim.killed_by(kill)
      and killer != none
      and victim.is_zombie == 0 then
         victim.is_zombie = 1
         victim.timer[2]  = 3
         send_incident(inf_new_infection, killer, victim)
         send_incident(infection_kill, killer, victim)
         killer += script_option[10]
         killer.stat[1] += 1
         killer.infection_spree += 1
         if killer.infection_spree > 2 then
            send_incident(dlc_achieve_2, killer, killer, 63) -- Achievement: Emergency Room
         end
      end
      if victim.killed_by(suicide) then
         killer.score += script_option[8]
         if script_option[12] == 1 then -- Suicides Become Zombies
            victim.is_zombie = 1
         end
      end
      if  victim.killed_by(betrayal)
      and killer.timer[2] == 0
      and victim.timer[2] == 0
      and victim.is_zombie == killer.is_zombie
      then
         killer.score += script_option[9] -- Betrayal Points
      end
   end
end

if round_timer.is_zero and game.round_time_limit > 0 then -- trigger 25
   for each player do
      current_player.infection_spree = 0
   end
end

if script_option[2] == 1 and current_haven == none then
   current_haven = unk_96(global.object[2], forge_label[3]
   if haven_exists == 1 then
      send_incident(hill_moved, all_players, all_players)
   end
   haven_exists = 1
end

if script_option[2] == 1 and global.timer[0].is_zero then -- move haven
   global.timer[0].rate = 0
   global.timer[0] = script_option[3]
   current_haven.set_waypoint_visibility(none)
   current_haven.set_shape_visibility(none)
   current_haven.set_display_timer(none)
   current_haven.number[0] = 0
   global.object[2] = current_haven
   current_haven = none
   current_haven = unk_96(global.object[2], forge_label[3])
end

do -- maintain haven waypoint
   current_haven.set_waypoint_visibility(anyone)
   current_haven.set_waypoint_icon(crown)
   current_haven.set_shape_visibility(anyone)
   current_haven.set_waypoint_priority(high)
end

if script_option[2] == 1 then -- trigger 31
   for each player do
      if  current_haven.shape_contains(current_player.biped)
      and current_player.is_zombie == 0
      and current_haven.number[0] == 0
      then
         --
         -- The Haven timer only starts once a human steps inside.
         --
         global.timer[0].rate = -100
         current_haven.number[0] = 1
      end
   end
   if current_haven.number[0] == 1 then
      current_haven.timer[0] = global.timer[0]
      current_haven.set_display_timer(0)
      if current_haven.timer[0] < 6 then
         current_haven.set_waypoint_priority(default)
      end
   end
end

if script_option[1] == 1 then -- trigger 33: handle Last Man Standing status acquired
   alias survivor_count = global.number[0]
   --
   survivor_count = 0
   if global.number[1] == 0 then
      count_survivors()
      if survivor_count == 1 then
         for each player do
            if current_player.is_zombie != 1 then
               script_traits[2].apply(current_player) -- Last Man Standing
               current_player.biped.set_waypoint_icon(skull)
               current_player.biped.set_waypoint_priority(high)
               current_player.is_last_human = 1
               current_player.score += script_option[11]
               send_incident(inf_last_man, current_player, all_players)
               send_incident(dlc_achieve_2, current_player, current_player, 61) -- Achievement: All Alone
            end
         end
         global.number[1] = 1
      end
   end
end

for each player do
   if current_player.is_last_human == 1 then
      script_traits[2].apply(current_player) -- Last Man Standing
   end
end

for each player do -- handle survivors in a haven
   hud_widget[0].hide(current_player)
   current_player.is_human_in_haven = 0
   if  script_option[2] == 1
   and current_haven.shape_contains(current_player)
   and current_player.is_zombie == 0
   then
      current_player.is_human_in_haven = 1
      script_traits[3].apply(current_player) -- Haven
      hud_widget[0].show(current_player)
   end
end

for each player do -- trigger 38: handle zombie victory
   global.timer[1].rate = -100
   if global.timer[1].is_zero then
      alias survivor_count = global.number[0]
      --
      survivor_count = 0
      count_survivors()
      for each player do
         if survivor_count == 1 and current_player.is_zombie == 0 and current_player.killed_by(suicide) then
            --
            -- The Last Man Standing killed themselves.
            --
            survivor_count = 0
         end
      end
      if global.number[0] == 0 then
         send_incident(infection_zombie_win, all_players, all_players)
         for each player do
            if current_player.is_last_human != 1 and current_player.is_zombie == 1 then
               --
               -- Awared Zombie Victory Points to all zombies except the newly-infected 
               -- Last Man Standing.
               --
               current_player.score += script_option[4]
            end
         end
         end_round()
      end
   end
end

if round_timer.is_zero and game.round_time_limit > 0 then -- trigger 42: handle survivor victory
   alias survivor_count = global.number[0]
   --
   survivor_count = 0
   count_survivors()
   if survivor_count != 0 then
      send_incident(infection_survivor_win, all_players, all_players)
      for each player do
         if current_player.is_zombie == 0 then
            current_player.score += script_option[5]
         end
      end
      end_round()
   end
end

for each player do -- update survival time stat
   if current_player.is_zombie == 0 then
      current_player.survival_timer.rate = -100
      if current_player.survival_timer.is_zero then
         current_player.stat[0] += 1
         current_player.survival_timer.reset()
      end
   end
end