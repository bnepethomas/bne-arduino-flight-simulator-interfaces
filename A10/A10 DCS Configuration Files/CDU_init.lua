dofile(LockOn_Options.common_script_path.."devices_defs.lua")

indicator_type = indicator_types.COMMON

dofile(LockOn_Options.script_path.."CDU/indicator/CDU_pages.lua")
init_pageID	= id_CDU_Pages.CDU_PAGE_OFF

CDU_preinit_files_path = LockOn_Options.script_path.."CDU/Indicator/Preinit/"
page_subsets = {}
dofile(LockOn_Options.script_path.."CDU/indicator/CDU_subsets_init.lua")

----------------------
pages = {}
pages_by_mode = {}
clear_mode_table(pages_by_mode, 10, 10, 29)

function get_page_by_mode(master, L2, L3, L4)
	return get_page_by_mode_global(pages_by_mode,init_pageID,master,L2,L3,L4)
end

dofile(LockOn_Options.script_path.."CDU/indicator/CDU_pages_init.lua")

use_parser		= false


dofile(LockOn_Options.common_script_path.."devices_defs.lua")

indicator_type = indicator_types.COMMON
used_render_mask = "interleave.dds"

opacity_sensitive_materials = 
{
	"font_CDU",
}

dofile(LockOn_Options.common_script_path.."ViewportHandling.lua")
try_find_assigned_viewport("ED_A10C_CDU")


