dofile(LockOn_Options.common_script_path.."elements_defs.lua")

function addStrPoly(name, position, align, controllers, size)
	position[3] = 0.0
	size = size or {VerticalSize, HorizontalSize}
	size[3] = 0.0
	size[4] = 0.0

	local Str				= CreateElement "ceStringPoly"
	Str.name				= name
	Str.material			= "font_IFEI"
	Str.init_pos			= position
	Str.alignment			= align
	Str.controllers			= controllers
	Str.stringdefs			= size
	Str.use_mipfilter		= true
	Add(Str)
	return Str
end

--	name - Element name. Only unique
--  size - Polygon size. size[1] = width; size[2] = height
--  text_lb_angle - left bottom corner coordinates on texture in pixels. text_lb_angle[1] = x, text_lb_angle[2] = y 
--	part_size - size of the choosen element on texture in pixels. part_size[1] = width, part_size[2] = height
--	position - position in game
function addTextPoly(name, size, text_lb_angle, part_size, position, controllers, align, texture_size)
	position[3] = 0.0
	local text_size = texture_size or 1024
	local Vertices-- = {0, 0, size[1], size[2]}
	local text_coords = {0, 0, 1, 1}
	local Offset = {0, 0}

	if align == "LeftCenter" then
		Vertices = {0, -size[2] / 2, size[1], size[2] / 2}
		Offset = {0, 0.5}
	else
		Vertices = {0, 0, size[1], size[2]}
	end

	local TexturePartWidth = part_size[1] / text_size
	local TexturePartHeight = part_size[2] / text_size

	text_coords[1] = ((text_lb_angle[1]+ part_size[1] * Offset[1]) / size[1]) / (text_size / size[1])
	text_coords[2] = -(((text_size - text_lb_angle[2] + part_size[2] * Offset[2]) / size[2]) / (text_size / size[2]))
	text_coords[3] = TexturePartWidth / size[1]
	text_coords[4] = TexturePartHeight / size[2]

	local Texture			= CreateElement "ceTexPoly"
	Texture.name			= name
	Texture.vertices		= {{Vertices[1],Vertices[2]}, {Vertices[1],Vertices[4]}, {Vertices[3],Vertices[4]}, {Vertices[3],Vertices[2]}}
	Texture.indices			= default_box_indices
	Texture.init_pos		= position
	Texture.material		= "IFEI_INDICATIONS"
	Texture.tex_params		= text_coords
	Texture.controllers		= controllers
	Texture.use_mipfilter	= use_mipfilter
	Add(Texture)
end

function drawDD()	-- DD - double dot
	addStrPoly("txt_DD_1", {XPositionClock - OffsetToDoubleDot, YPositionClock}, "RightCenter", {{"txt_DOUBLE_DOT"}}, {VerticalSizeClock, HorizontalSizeClock})
	addStrPoly("txt_DD_2", {XPositionClock - OffsetToDoubleDot * 2 - OffsetToNextNumeral, YPositionClock}, "RightCenter", {{"txt_DOUBLE_DOT"}}, {VerticalSizeClock, HorizontalSizeClock})
	addStrPoly("txt_DD_3", {XPositionClock - OffsetToDoubleDot, YPositionTimer}, "RightCenter", {{"txt_DOUBLE_DOT"}}, {VerticalSizeClock, HorizontalSizeClock})
	addStrPoly("txt_DD_4", {XPositionClock - OffsetToDoubleDot * 2 - OffsetToNextNumeral, YPositionTimer}, "RightCenter", {{"txt_DOUBLE_DOT"}}, {VerticalSizeClock, HorizontalSizeClock})
end