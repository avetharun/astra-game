
interact = Audio("interact.wav")


cw.add_interaction("table", function() 
	interact:play()
	Render.TextBox("table_inspect", "This is a table.", nil) 
	end
)

cw.add_interaction("rei_bedroom_door_exit", function() 
	interact:play()
	Render.TextBox("bedroom_exit", "This is a door.", nil) 
	end
)
cw.add_interaction("rei_bedroom_bed", function() 
	interact:play()
	Render.TextBox("bedroom_bed", "This is a bed.", function() 
		Render.TextBox("bedroom_bed", "Sleep in it?", nil)
	end
	) 
	end
)