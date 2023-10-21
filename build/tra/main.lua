cabinet_door_open = Audio("audio/cabinet_door_open.wav")
cabinet_door_open.volume = 15
interact = Audio("audio/interact.wav")
paper_flip = Audio("audio/paper_flip.wav")
paper_flip.volume = 15
lightning = Audio("audio/lightning.wav")
lightning.volume = 10

cw.add_start("player_wakeup_start", function()
	cw.get_player().transform.position = Vector2(455,230)
end)


rei_window_rain_particle = cw.get_particle("rei_bedroom_rain")
has_interacted_table = false
cw.add_interaction("table", function()
	if (not has_interacted_table and not CWInventory.getinventory("player_inventory"):containsID("m_rei_bedroom_letter")) then
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
	end
end)
has_picked_up_bippo = false
can_enter_bedroom_door = true
m_candleItem = cw.get_item("rei_bedroom_candle_item")
m_bippoItem = cw.get_item("rei_bedroom_bippo_item")
m_letterItem = cw.get_item("m_rei_bedroom_letter")
m_darkness = false
m_candleItem:set_on_click(function() 
		mItemCandleOptions = ABT();
		m_UseText = m_darkness and "Light" or "Extinguish"
		mItemCandleOptions:setString("useText", m_UseText)
		mItemCandleOptions:addBool(m_UseText)
		mItemCandleOptions:addBool("Drop")

		Render.OptionsBoxAligned("Candle VGUI", mItemCandleOptions, function(options)
			if (options:getBool(options:getString("useText"))) then
				m_darkness = not m_darkness
				if (m_darkness) then
					cw.ce("darkness:1")
				else
					cw.ce("darkness:2")
				end
			end
			if (options:getBool("Drop")) then
				inventory = CWInventory.getinventory("player_inventory")
				inventory:drop(inventory:getSlotOfID("rei_bedroom_candle_item"), cw.Camera.position, "rei_bedroom_candle_item")
				return
			end
		end, Alignment.center)

end)

m_letterItem:set_on_click(function() 
		mItemLetterOptions = ABT();
		mItemLetterOptions:addBool("Read")
		mItemLetterOptions:addBool("Drop")

		Render.OptionsBoxAligned("Letter VGUI", mItemLetterOptions, function(options)
			if (options:getBool("Read")) then
			
				paper_flip:play()
				Render.TextBox("table_inspect", "\"Rei, I left some sweets for you on the table. \nFeel free to have some!\n	-Mom\"", function()
					if (not has_interacted_table) then
						cw.ce("lightning")
						Render.TextBox("table_inspect", "...", function() 
							has_interacted_table = true
						end)	
					end
				end)
				return
			end
			if (options:getBool("Drop")) then
				inventory = CWInventory.getinventory("player_inventory")
				inventory:drop(inventory:getSlotOfID("m_rei_bedroom_letter"), cw.Camera.position, "m_rei_bedroom_letter")
				return
			end
		end, Alignment.center)

end)

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
	Alignment.center)

end)
cw.add_interaction("rei_bedroom_door_exit", function()
	if (not has_interacted_table) then
		Render.TextBox("door_exit_denied", "There's a note on my desk. I haven't read it yet", nil)
	end
	if (not CWInventory.getinventory("player_inventory"):containsID("rei_bedroom_bippo_item") and has_interacted_table) then
		Render.TextBox("door_exit_denied", "I'm too afraid..")
	end
	if (can_enter_bedroom_door and has_interacted_table and CWInventory.getinventory("player_inventory"):containsID("rei_bedroom_bippo_item")) then
		cabinet_door_open:play()
		can_enter_bedroom_door = false
		door_sprite = cw.get_sprite("rei_bedroom_door")
		door_sprite.meta.animate_once = true
		door_sprite.meta.resets = true
		door_sprite.meta:set_on_loop (function() 
			cw.get_player().transform.position = Vector2(1200, 1085)
			can_enter_bedroom_door = true
		end)
	end
end)

cw.add_interaction("rei_bedroom_door_enter", function() 
	cabinet_door_open:play()
	cw.get_player().transform.position = Vector2(434, 570)
end)
cw.re("lightning", function()
	lightning:play()
	cw.run_async(.1, function() 
		cw.ce("darkness:1")
	end)
	cw.run_async(.15, function() 
		cw.ce("darkness:0")
	end)
	cw.run_async(.25, function() 
		cw.ce("darkness:1")
		rei_window_rain_particle.uv = ParticleEffectType.BLOOD.uv
	end)
end)