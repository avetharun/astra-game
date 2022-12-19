
interact = Audio("interact.wav")

table_interact = function() 
	interact:play()
end

collide_player = function() 
	console.log("pong!")
end

cw.add_interaction("table", table_interact)
cw.add_collision("PLAYER_COLLIDER", "table", collide_player)