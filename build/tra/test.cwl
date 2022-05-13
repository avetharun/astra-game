{
	"layout": [
		{
			"id": "tex_test",
			"schema": [
				"texture"
			],
			"asset_data": {
				"filename": "sprites/sprites.png",
				"transform": {
					"x": 0,
					"y": 10,
					"w": "100%",
					"h": "100%"
				},
				"layer": "top"
			}
		},
		{
			"id": "interactable_test",
			"schema": [
				"gameobject"
			],
			"asset_data": {
				"type": "collider",
				"layer": "wall",
				"onCollide": "scripts/collision_test",
				"transform": {
					"x": 0,
					"y": 10,
					"w": "100",
					"h": "100"
				}
			}
		}
	]
}