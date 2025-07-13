dofile(LockOn_Options.common_script_path.."devices_defs.lua")

indicator_type = indicator_types.COMMON

opacity_sensitive_materials = 
{
	"font_IFEI",
	"IFEI_INDICATIONS"
}
color_sensitive_materials = 
{
	"font_IFEI",
	"IFEI_INDICATIONS"
}

LCD_NightLt			= {-0.05, 1}
LCD_DayLt			= {-0.2, 0.8}

-------PAGE IDs-------
id_Page =
{
	PAGE_OFF		= 0,
	PAGE_MAIN		= 1,
	PAGE_SP			= 2,
	PAGE_TIME_SET	= 3
}

id_pagesubset =
{
	MAIN			= 0,
	SP 				= 1,
	TIME_SET 		= 2
}

page_subsets = {}
page_subsets[id_pagesubset.MAIN]	= LockOn_Options.script_path.."IFEI/indicator/IFEI_MAIN_page.lua"
page_subsets[id_pagesubset.SP]		= LockOn_Options.script_path.."IFEI/indicator/IFEI_SP_page.lua"
page_subsets[id_pagesubset.TIME_SET]= LockOn_Options.script_path.."IFEI/indicator/IFEI_TIME_SET_page.lua"

----------------------
pages = {}

pages[id_Page.PAGE_OFF]		= {}
pages[id_Page.PAGE_MAIN]	= {id_pagesubset.MAIN}
pages[id_Page.PAGE_SP]		= {id_pagesubset.SP}
pages[id_Page.PAGE_TIME_SET]= {id_pagesubset.TIME_SET}

init_pageID		= id_Page.PAGE_OFF
use_parser		= false

--- master modes
IFEI_PAGE_OFF		   = 0 
IFEI_PAGE_MAIN    	   = 1 
IFEI_PAGE_SP    	   = 2 
IFEI_PAGE_TIME_SET 	   = 3 

------------------------------------
pages_by_mode                 = {}
clear_mode_table(pages_by_mode, 3, 0, 0)

function get_page_by_mode(master,L2,L3,L4)
	return get_page_by_mode_global(pages_by_mode,init_pageID,master,L2,L3,L4)
end

pages_by_mode[IFEI_PAGE_OFF][0][0][0]			  = id_Page.PAGE_OFF
pages_by_mode[IFEI_PAGE_MAIN][0][0][0]			  = id_Page.PAGE_MAIN
pages_by_mode[IFEI_PAGE_SP][0][0][0]			  = id_Page.PAGE_SP
pages_by_mode[IFEI_PAGE_TIME_SET][0][0][0]		  = id_Page.PAGE_TIME_SET
