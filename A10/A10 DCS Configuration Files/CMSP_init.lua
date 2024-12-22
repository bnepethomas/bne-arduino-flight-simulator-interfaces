dofile(LockOn_Options.common_script_path.."devices_defs.lua")

indicator_type = indicator_types.COMMON

opacity_sensitive_materials = 
{
	"font_CMSP"
}

-------PAGE IDs-------
id_Page =
{
	PAGE_NULL = 0,
	PAGE_OFF  = 1,
	PAGE_MAIN = 2
}

id_pagesubset =
{
	MAIN   = 0
}

page_subsets = {}
page_subsets[id_pagesubset.MAIN]   = LockOn_Options.script_path.."CMSP/indicator/CMSP_page.lua"
  	
----------------------
pages = {}
pages[id_Page.PAGE_MAIN] = {id_pagesubset.MAIN}

init_pageID     = id_Page.PAGE_MAIN
purposes 	 = {render_purpose.GENERAL}

dofile(LockOn_Options.common_script_path.."ViewportHandling.lua")
try_find_assigned_viewport("ED_A10C_CMSP")

