import os
import argparse
import json

# A simple script for preparing a 3d scene
"""
[
    {
        "entity": "camera",
        "position": [
            4.238278865814209,
            -0.36461925506591797,
            10.051044464111328
        ],
        "rotation": [
            -0.03255898877978325,
            0.06511641293764114,
            -0.01732342690229416
        ],
        "scale": 1.0,
        "speed": 1.7999999523162842
    },
    {
        "color": [
            0.1303585171699524,
            0.5584049820899963,
            0.8578431606292725,
            1.0
        ],
        "entity": "menger_sponge",
        "lod": 7,
        "position": [
            0.0,
            0.0,
            0.0
        ],
        "rotation": [
            0.0,
            -0.0,
            0.0
        ],
        "scale": 1.0
    }
]
"""

parser = argparse.ArgumentParser()
parser.add_argument('object_name')
parser.add_argument('lod')
parser.add_argument('x_objects')
parser.add_argument('y_objects')
args = parser.parse_args()

out_json = [
    {
        "entity": "camera",
        "position": [3.7846, 4.3198, 9.7219],
        "rotation": [0, 0, 0],
        "scale": 1.0,
        "speed": 1.0
    }
]

x_objects = int(args.x_objects)
y_objects = int(args.y_objects)
lod = int(args.lod)
for y in range(y_objects):
    for x in range(x_objects):
        posX = x % x_objects * 1.5
        posY = y % y_objects * 1.5
        out_json.append({
        "entity": args.object_name,
        "position": [posX, posY, 0],
        "rotation": [0, 0, 0],
        "color": [0.9, 0.9, 0.9, 1],
        "scale": 1.0,
        "lod": lod
        })

json_object = json.dumps(out_json, indent=4)

with open("default.json", "w") as out_file:
    out_file.write(json_object)

