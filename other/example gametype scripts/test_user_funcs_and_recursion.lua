alias counter = global.number[0]
alias basis   = global.object[0]
alias dummy   = global.number[1]

alias ran_once = global.number[2]

function execute_task()
   counter -= 1
   basis = basis.place_at_me(skull, none, none, 0, 0, 1, none)
   if counter > 0 then
      execute_task()
   end
   dummy += 1
end

for each player do
   if current_player.biped != no_object and ran_once == 0 then
      ran_once = 1
      --
      basis   = current_player.biped
      counter = 5
      execute_task()
   end
end
