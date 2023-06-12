dofile(LockOn_Options.script_path.."materials.lua")
dofile(LockOn_Options.common_script_path.."elements_defs.lua")
dofile(LockOn_Options.common_script_path.."tools.lua")

SetScale(1)

-- Texture to bake
local w = 1
local h = 1
local verts 	=	{{-w, h},
					 { w, h},
					 { w,-h},
					 {-w,-h}}
local inds  	= default_box_indices

local picture			 	= CreateElement "ceTexPoly"
picture.name				= RWR_BAKE_MATERIAL .. "_picture"
picture.material		 	= MakeMaterial(RWR_BAKE_MATERIAL,{255,255,255,255})
picture.vertices		 	= verts
picture.indices			 	= inds
picture.additive_alpha   	= true
picture.tex_coords		 	= {{0, 0},{1, 0},{1,1},{0, 1}}
picture.controllers 	 	= {{"brightness", 1.0}}
Add(picture)
