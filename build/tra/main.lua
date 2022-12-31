cabinet_door_open = Audio("audio/cabinet_door_open.wav")
cabinet_door_open.volume = 15
interact = Audio("audio/interact.wav")
paper_flip = Audio("audio/paper_flip.wav")
paper_flip.volume = 15
lightning = Audio("audio/lightning.wav")
lightning.volume = 10


rei_window_rain_particle = cw.get_particle("rei_bedroom_rain")

has_interacted_table = false
cw.add_interaction("table", function() 
	if (not has_interacted_table) then
		Render.TextBox("table_inspect", "This is my desk.", function()
			Render.TextBox("table_inspect", "It has a note on it.", function()
			paper_flip:play()
				Render.TextBox("table_inspect", "\"Rei, I left some sweets for you on the table. \nFeel free to have some!\n	-Mom\"", function()
					cw.ce("lightning")
					Render.TextBox("table_inspect", "...", function() 
						has_interacted_table = true
					end)
				end)
			end)
		end)
	else
		Render.TextBox("table_inspect", "The note has something red on it..", nil)
	end
end)

can_enter_bedroom_door = true
cw.add_interaction("rei_bedroom_closet", function() 
	m_Rei_ClosetOptions = ABT();
	m_Rei_ClosetOptions:addBool("Sleepwear")
	m_Rei_ClosetOptions:addBool("My dress")
	Render.OptionsBoxAligned("Closet", m_Rei_ClosetOptions, function(options)
		if (options:getBool("Sleepwear")) then
			cw.ce("clothes_pajamas")
		end
		if (options:getBool("My dress")) then
			cw.ce("clothes_default")
		end
	end,
	Alignment.right)

end)
cw.add_interaction("rei_bedroom_door_exit", function()
	if (not has_interacted_table) then
		Render.TextBox("door_exit_denied", "There's a note on my desk. I haven't read it yet", nil)
	end
	if (can_enter_bedroom_door and has_interacted_table) then
		cabinet_door_open:play()
		can_enter_bedroom_door = false
		door_sprite = cw.get_sprite("rei_bedroom_door")
		door_sprite.meta.animate_once = true
		door_sprite.meta.resets = true
		cw.run_async(0.6, function() 
			cw.get_player().transform.position = Vector2(840, 710)
			can_enter_bedroom_door = true
		end)
	end
end)

cw.add_interaction("rei_bedroom_door_enter", function() 
	cabinet_door_open:play()
	cw.get_player().transform.position = Vector2(77, 200)
end)
cw.re("lightning", function() 
	lightning:play()
	cw.get_sprite("rei_bedroom_bloodstain").meta.animate_once = true
	cw.get_sprite("rei_bedroom_bloodstain").meta.resets = false
	cw.run_async(.1, function() 
		cw.set_overlay_enabled(true)
	end)
	cw.run_async(.15, function() 
		cw.set_overlay_enabled(false)
	end)
	cw.run_async(.25, function() 
		cw.set_overlay_enabled(true)
		rei_window_rain_particle.uv = ParticleEffectType.BLOOD.uv
	end)
end)