{
  "autoexec": "main.lua",
  "layout": [
    {
      "id": "",
      "schema": [
        "mesh"
      ],
      "asset_data": {
        "layer": "solid",
        "lines": [
          [
            [ 512, 512 ],
            [ 632, 632 ]
          ]
        ]
      }

    },
    {
      "id": "world_sprite",
      "schema": [
        "image__"
      ],
      "asset_data": {
        "filename": "sprites/world.png",
        "transform": {
          "w": "400%",
          "h": "400%"
        }
      }
    },
    {
      "id": "world_door_darkness_overlay_0",
      "schema": [
        "image"
      ],
      "asset_data": {
        "layer": "top",
        "filename": "sprites/world.png",
        "transform": {
          "x": 375,
          "y": 600,
          "w": "500%",
          "h": "500%"
        },
        "uv": [
          [ 0, 412 ],
          [ 26, 70 ]
        ]
      }
    },
    {
      "id": "overlay_ui_shadow",
      "schema": [
        "ui",
        "image__"
      ],
      "asset_data": {
        "filename": "sprites/sprites.png",
        "transform": {
          "w": 720,
          "h": 720
        },
        "uv": [
          [ 0, 91 ],
          [ 256, 256 ]
        ]
      }
    }
  ]
}