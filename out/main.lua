
__window = cw:get_window()
__window.name = "Einoki"
__window.debug = true
__window.debug_window = true
_player = Sprite.new("sprites/sprites.png")
_player.transform.position.x = 0
_player.transform.position.y = 0
_player.centered = true
_player.enabled = true

uvpsx = 0
uvpsy = 347
uvpw = 24
uvph = 32
_player.transform.scale.x = uvpw * 4
_player.transform.scale.y = uvph * 4
_player.tile.x = 1
_player.tile.y = 4
_player.uv = ctypes.SDL_Rect(uvpsx, uvpsy, uvpw, uvph)

cw.Camera.target(_player.transform.position)
playerpos = _player.transform.position
sprite_frame_offset = 0.0
sprite_frame_multiplier = 0.2
sprite_frame_amt = 0
cw.on("key_down",
	function(key)
	end
)
speed_normal = 0.15
speed_sprint = 0.2
speed = speed_normal
cw.add_start("ui_generator", function()
printf("%d, %d", _player.pixel_sz.x, _player.pixel_sz.y)
	
end)

tick_sprite_tile = function(amt)
	sprite_frame_offset = sprite_frame_offset + ( (amt) * Time.DeltaTime / 128)
	if (sprite_frame_offset >= 4) then
		sprite_frame_amt = sprite_frame_amt + 1
		sprite_frame_offset = 0
		if (sprite_frame_amt >= 4) then
			sprite_frame_amt = 0
		end
	end

end
cw.add_update("player_controller", function() 
	move_amt = Time.DeltaTime*speed
	if input.key_pressed(input.K_q) then
		printf("Player position: %d, %d|Camera position: %d %d|\n", playerpos.x, playerpos.y, cw.Camera.position.x, cw.Camera.position.y)
		printf("Player UV: %d, %d, %d, %d :: %d, %d", _player.uv_final.x, _player.uv_final.y, _player.uv_final.w, _player.uv_final.h, _player.tile.x, _player.tile.y)
	end
	if input.key_down(input.K_lshift) then
		speed = speed_sprint
	else
		speed = speed_normal
	end
	if input.key_down(input.K_a) then
		tick_sprite_tile(move_amt)
		_player.tile.x = sprite_frame_amt
		_player.tile.y = 4
	elseif input.key_down(input.K_d) then
		tick_sprite_tile(move_amt)
		_player.tile.x = sprite_frame_amt
		_player.tile.y = 2
	elseif input.key_down(input.K_w) then
		tick_sprite_tile(move_amt)
		_player.tile.x = sprite_frame_amt
		_player.tile.y = 1
	elseif input.key_down(input.K_s) then
		tick_sprite_tile(move_amt)
		_player.tile.x = sprite_frame_amt
		_player.tile.y = 3
	end
	if input.key_down(input.K_s) and not Raycast.any_cone(_player.center_position, 45, 90, 35, c_solid) then
		_player.transform.position.y = move_amt + _player.transform.position.y
	end
	if input.key_down(input.K_w) and not Raycast.any_cone(_player.center_position, 45, 270, 35, c_solid) then
		_player.transform.position.y = _player.transform.position.y - move_amt
	end
	if input.key_down(input.K_d) and not Raycast.any_cone(_player.center_position, 56, 0, 35, c_solid) then
		_player.transform.position.x = move_amt + _player.transform.position.x
	end
	if input.key_down(input.K_a) and not Raycast.any_cone(_player.center_position, 56, 180, 35, c_solid) then
		_player.transform.position.x = _player.transform.position.x - move_amt
	end
end)
RectCollider.new(math.random(0, 128), math.random(0, 128), math.random(0, 128), math.random(0, 128), c_solid)


