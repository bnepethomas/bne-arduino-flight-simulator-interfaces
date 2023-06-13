dofile(LockOn_Options.common_script_path.."elements_defs.lua")

UFC_OptionDisplay_font = {0.00755, 0.0083, 0.00257, 0.0}
UFC_ScratchPadStringDisplay_font = {0.00755, 0.0083, -0.0053, 0.0}
UFC_ScratchPadNumberDisplay_font = {0.00678, 0.0076, -0.00237, 0.0}
UFC_Comm1Display_font = {0.0077, 0.0086, -0.0052, 0.0}
UFC_Comm2Display_font = {0.0073, 0.0083, -0.0055, 0.0}

-- size of indicator
x_size  = 0.1775
y_size  = 0.1155
x_size_05 = x_size * 0.5
y_size_05 = y_size * 0.5

option_display_pos_x = 0.021;
option_display_pos_y0 = 0.03894;
option_display_pos_dy = 0.02;
option_cueing_pos_x = 0.018;
scratch_pad_string1_display_pos_x = -0.06115;
scratch_pad_string2_display_pos_x = -0.05;
scratch_pad_string_display_pos_y = 0.0415;
scratch_pad_number_display_pos_x = -0.0046;
scratch_pad_number_display_pos_y = 0.0413;
comm_1_display_pos_x = -0.0808;
comm_1_display_pos_y = -0.04097;
comm_2_display_pos_x = 0.0728;
comm_2_display_pos_y = -0.0405;






mip_filter_used = true

function use_mipfilter(object)
	if mip_filter_used then
		object.use_mipfilter = true
	end
end

