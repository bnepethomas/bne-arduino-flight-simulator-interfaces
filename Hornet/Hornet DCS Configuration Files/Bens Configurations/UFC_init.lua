dofile(LockOn_Options.common_script_path.."devices_defs.lua")

indicator_type	= indicator_types.COMMON
purposes		= {render_purpose.GENERAL}

dofile(LockOn_Options.common_script_path.."ViewportHandling.lua")
try_find_assigned_viewport("F18_UFC")																 
opacity_sensitive_materials = 
{
	"font_UFC"
}

-------PAGE IDs-------
id_Page =
{
	PAGE_OFF    = 0,
	PAGE_MAIN   = 1
}

id_pagesubset =
{
	MAIN = 0
}

page_subsets = {}
page_subsets[id_pagesubset.MAIN] = LockOn_Options.script_path.."UFC/indicator/UFC_main_page.lua"

----------------------
pages = {}
pages[id_Page.PAGE_OFF]		= {}
pages[id_Page.PAGE_MAIN]	= {id_pagesubset.MAIN}

init_pageID			= id_Page.PAGE_MAIN
use_parser			= false

dynamically_update_geometry = false
dofile(LockOn_Options.common_script_path.."ViewportHandling.lua")
try_find_assigned_viewport("FA_18C_UFC", "UFC")
