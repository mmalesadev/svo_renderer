import os
import argparse

# A simple script for generating SVOs at different levels
# Usage: ./prep_octree_file.py <mesh_file_path> <svo_builder_dir>

parser = argparse.ArgumentParser()
parser.add_argument('mesh_file_path')
parser.add_argument('svo_builder_dir')
args = parser.parse_args()

svo_builder_path = os.path.join(args.svo_builder_dir, "svo_builder.exe")
tri_convert_path = os.path.join(args.svo_builder_dir, "tri_convert.exe")

os.system(f"{tri_convert_path} -f {args.mesh_file_path}")

mesh_name = os.path.splitext(os.path.basename(args.mesh_file_path))[0]
mesh_file_name = mesh_name + ".tri"
mesh_path = os.path.join(args.svo_builder_dir, mesh_file_name)

def prep_octree(level):
    target = r'..\data\svo'
    os.system(f"{svo_builder_path} -f {mesh_path} -s {level} -c model")
    os.system(f"mkdir {target}\{mesh_name}")
    os.system(f"move {args.svo_builder_dir}\{mesh_name}{level}_1.octree {target}\{mesh_name}\{mesh_name}_{level}.octree")
    os.system(f"move {args.svo_builder_dir}\{mesh_name}{level}_1.octreedata {target}\{mesh_name}\{mesh_name}_{level}.octreedata")
    os.system(f"move {args.svo_builder_dir}\{mesh_name}{level}_1.octreenodes {target}\{mesh_name}\{mesh_name}_{level}.octreenodes")

prep_octree(512)
prep_octree(256)
prep_octree(128)
prep_octree(64)
prep_octree(32)
prep_octree(16)
prep_octree(8)
prep_octree(4)
prep_octree(2)