
__window = cw:get_window()
__window.name = "Einoki"
if not cdecl.NDEBUG then
	__window.debug = true
	__window.debug_window = true
else 
	__window.debug = false
	__window.debug_window = false
end
_player = Sprite.new("sprites/sprites.png")
_player.transform.position.x = 0
_player.transform.position.y = 0
_player.centered = true
_player.enabled = true
_player.id = "player"

cw.associate_player(_player)



uvpsx = 0
uvpsy = 347

uvpsx_p = 0
uvpsy_p = 508


uvpw = 24
uvph = 32
_player.transform.scale.x = uvpw * 4
_player.transform.scale.y = uvph * 4
_player.tile.x = 1
_player.tile.y = 4
m_PJ_UV = SDL_Rect(uvpsx_p, uvpsy_p, uvpw, uvph)
m_DF_UV = SDL_Rect(uvpsx, uvpsy, uvpw, uvph)
_player.uv = m_PJ_UV

cw.Camera.target(_player.transform.position)
playerpos = _player.transform.position
sprite_frame_offset = 0.0
sprite_frame_amt = 0
cw.on("key_down",
	function(key)
	end
)
speed_normal = 0.15
speed_sprint = 0.2
speed = speed_normal
tick_sprite_tile = function(amt)
	sprite_frame_offset = sprite_frame_offset + amt
	if (sprite_frame_offset >= 4) then
		sprite_frame_amt = sprite_frame_amt + 1
		sprite_frame_offset = 0
		if (sprite_frame_amt >= 4) then
			sprite_frame_amt = 0
		end
	end

end
direction = "up"
cw.add_update("player_controller", function()
	if (__window.is_editor) then
		if (input.mouse.middle) then
			_player.transform.position.x = input.mouse.delta.x + _player.transform.position.x
			_player.transform.position.y = input.mouse.delta.y + _player.transform.position.y
		end
		_player.enabled = false
	elseif not __window.debug_window and not __window.is_editor then
		_player.enabled = true
		if input.key_pressed(CWSettings.keybinds.use) then 
			if direction == "down" and Raycast.any_cone_except(_player.center_position + Vector2(0,16), 12, 90, 40, c_player) then
				if (cw.m_runInteraction(RaycastHit.id)) then 
				end
			end
			if direction == "up" and Raycast.any_cone_except(_player.center_position + Vector2(0,16), 12, 270, 40, c_player) then
				if (cw.m_runInteraction(RaycastHit.id)) then 
				end
			end
			if direction == "right" and Raycast.any_cone_except(_player.center_position + Vector2(0,16), 12, 0, 40, c_player) then
				if (cw.m_runInteraction(RaycastHit.id)) then 
				end
			end
			if direction == "left" and Raycast.any_cone_except(_player.center_position + Vector2(0,16), 12, 180, 40, c_player) then
				if (cw.m_runInteraction(RaycastHit.id)) then 
				end
			end
		end
		if (not cw.is_interacting) then
			move_amt = Time.DeltaTime*speed
			if input.key_down(input.K_lshift) then
				speed = speed_sprint
			else
				speed = speed_normal
			end
			if input.key_down(CWSettings.keybinds.down) then
				if not Raycast.cone(_player.center_position + Vector2(0,16), 45, 90, 25, c_solid) or __window.noclip then
					_player.transform.position.y = move_amt + _player.transform.position.y
				end
				direction = "down"

			end
			if input.key_down(CWSettings.keybinds.up) then
				if not Raycast.cone(_player.center_position + Vector2(0,16), 45, 270, 25, c_solid) or __window.noclip then
					_player.transform.position.y = _player.transform.position.y - move_amt
				end
				direction = "up"
			end
			if input.key_down(CWSettings.keybinds.right) then
				if not Raycast.cone(_player.center_position + Vector2(0,16), 45, 0, 25, c_solid) or __window.noclip then
					_player.transform.position.x = move_amt + _player.transform.position.x
				end
				direction = "right"
			end
			if input.key_down(CWSettings.keybinds.left) then
				if not Raycast.cone(_player.center_position + Vector2(0,16), 45, 180, 25, c_solid) or __window.noclip then
					_player.transform.position.x = _player.transform.position.x - move_amt
				end
				direction = "left"
			end
			if input.key_down(CWSettings.keybinds.left) then
				tick_sprite_tile(Time.DeltaTime * 0.03)
				_player.tile.x = sprite_frame_amt
				_player.tile.y = 4
			elseif input.key_down(CWSettings.keybinds.right) then
				tick_sprite_tile(Time.DeltaTime * 0.03)
				_player.tile.x = sprite_frame_amt
				_player.tile.y = 2
			elseif input.key_down(CWSettings.keybinds.up) then
				tick_sprite_tile(Time.DeltaTime * 0.03)
				_player.tile.x = sprite_frame_amt
				_player.tile.y = 1
			elseif input.key_down(CWSettings.keybinds.down) then
				tick_sprite_tile(Time.DeltaTime * 0.03)
				_player.tile.x = sprite_frame_amt
				_player.tile.y = 3
			end
		end
	end
	if input.key_pressed(input.K_grave) then
		__window.debug_window = not __window.debug_window
	end
	if input.key_pressed(input.K_pause) then
		__window.debug = not __window.debug
		if (not __window.debug) then 
			__window.debug_window = false
		end
	end
end)


cw.re("clothes_pajamas", function() 
	_player.uv = m_PJ_UV
end)
cw.re("clothes_default", function() 
	_player.uv = m_DF_UV
end)
cw.re("scene_transition_blackout", function()
	
end)