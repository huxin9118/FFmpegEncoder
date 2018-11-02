#include "h264_parser.h"

static int is_h264_iframe(unsigned char *data)
{
	unsigned long hdr_type = H264_GET_NAL_TYPE(*data);

	if(hdr_type == NAL_TYPE_IDR || hdr_type == NAL_TYPE_SPS || hdr_type == NAL_TYPE_PPS
		|| (hdr_type== NAL_TYPE_SLICE  && *(data+1)==0x88))
		return 1;

	return 0;
}

static unsigned long Ue(unsigned char *buf, unsigned long len, unsigned long *start_bit)
{
	unsigned long zero_num = 0;
	unsigned long i;
	while ((*start_bit) < len * 8)
	{
		if (buf[(*start_bit) / 8] & (0x80 >> ((*start_bit) % 8)))
		{
			break;
		}
		zero_num++;
		(*start_bit)++;
	}
	(*start_bit)++;


	{
	unsigned long ret = 0;
	for ( i = 0; i<zero_num; i++)
	{
		ret <<= 1;
		if (buf[(*start_bit) / 8] & (0x80 >> ((*start_bit) % 8)))
		{
			ret += 1;
		}
		(*start_bit)++;
	}
	return (1 << zero_num) - 1 + ret;
	}
}


static int Se(unsigned char *buf, unsigned long len, unsigned long *start_bit)
{


	int val = Ue(buf, len, start_bit);
	double k = val;
	int nValue = (int)ceil(k / 2);
	if (val % 2 == 0)
		nValue = -nValue;
	return nValue;


}

static unsigned long u(unsigned long bit_count, unsigned char * buf, unsigned long *start_bit)
{
	unsigned long ret = 0;
	unsigned long i;
	for ( i = 0; i<bit_count; i++)
	{
		ret <<= 1;
		if (buf[(*start_bit) / 8] & (0x80 >> ((*start_bit) % 8)))
		{
			ret += 1;
		}
		(*start_bit) = (*start_bit)+1;
	}
	return ret;
}

int h264_decode_seq_parameter_set(unsigned char * buf, unsigned long len, sps_rbsp* sps,  int *width, int *height)
{
	//PJ_LOG(3, (THIS_FILE, "nal_unit_header: %x %x",*buf,*(buf+1)));
	int i = 0;
	unsigned long start_bit = 0;
	int forbidden_zero_bit = u(1, buf, &start_bit);
	int nal_ref_idc = u(2, buf, &start_bit);
	int nal_unit_type = u(5, buf, &start_bit);
	if (nal_unit_type == H264_FU_FLAG){
		int S = u(1, buf, &start_bit);
		int E = u(1, buf, &start_bit);
		int R = u(1, buf, &start_bit);
		if(S==1 && E==0){
			nal_unit_type = u(5, buf, &start_bit);
			//PJ_LOG(3, (THIS_FILE, "FU-A: S=%d E=%d R=%d nal_unit_type=%d",S,E,R,nal_unit_type));
		}
		else{
			int FU_A_nal_unit_type = u(5, buf, &start_bit);
			//PJ_LOG(3, (THIS_FILE, "FU-A: S=%d E=%d R=%d FU_A_nal_unit_type=%d",S,E,R,FU_A_nal_unit_type));
		}
	}
	if (nal_unit_type == NAL_TYPE_SPS)
	{
		//PJ_LOG(3, (THIS_FILE, "Signal: nal_unit_type=%d",nal_unit_type));
		sps->profile_idc = u(8, buf, &start_bit);
		sps->constraint_set0_flag = u(1, buf, &start_bit);//(buf[1] & 0x80)>>7;
		sps->constraint_set1_flag = u(1, buf, &start_bit);//(buf[1] & 0x40)>>6;
		sps->constraint_set2_flag = u(1, buf, &start_bit);//(buf[1] & 0x20)>>5;
		sps->constraint_set3_flag = u(1, buf, &start_bit);//(buf[1] & 0x10)>>4;
		sps->reserved_zero_4bits = u(4, buf, &start_bit);
		sps->level_idc = u(8, buf, &start_bit);
		sps->seq_parameter_set_id = Ue(buf, len, &start_bit);

		if (sps->profile_idc == 100 ||
			sps->profile_idc == 110 ||
			sps->profile_idc == 122 ||
			sps->profile_idc == 244 ||
			sps->profile_idc ==  44 ||
			sps->profile_idc ==  83 ||
			sps->profile_idc ==  86 ||
			sps->profile_idc == 118 ||
			sps->profile_idc == 128 ||
			sps->profile_idc == 138 ||
			sps->profile_idc == 139 ||
			sps->profile_idc == 134)
		{
			sps->chroma_format_idc = Ue(buf, len, &start_bit);
			if (sps->chroma_format_idc == 3){
				sps->separate_colour_plane_flag = u(1, buf, &start_bit);
			}
			sps->bit_depth_luma_minus8 = Ue(buf, len, &start_bit);
			sps->bit_depth_chroma_minus8 = Ue(buf, len, &start_bit);
			sps->qpprime_y_zero_transform_bypass_flag = u(1, buf, &start_bit);
			sps->seq_scaling_matrix_present_flag = u(1, buf, &start_bit);

			if (sps->seq_scaling_matrix_present_flag)
			{
				for ( i = 0; i < ((sps->chroma_format_idc != 3)?8:12); i++) {
					sps->seq_scaling_list_present_flag[i] = u(1, buf, &start_bit);
				}
			}
		}

		sps->log2_max_frame_num_minus4 = Ue(buf, len, &start_bit);

		sps->pic_order_cnt_type = Ue(buf, len, &start_bit);
		if (sps->pic_order_cnt_type == 0) {
			sps->log2_max_pic_order_cnt_lsb_minus4 = Ue(buf, len, &start_bit);
		}else if (sps->pic_order_cnt_type == 1){
			sps->delta_pic_order_always_zero_flag = u(1, buf, &start_bit);
			sps->offset_for_non_ref_pic = Se(buf, len, &start_bit);
			sps->offset_for_top_to_bottom_field = Se(buf, len, &start_bit);
			sps->num_ref_frames_in_pic_order_cnt_cycle = Ue(buf, len, &start_bit);
			for ( i = 0; i < sps->num_ref_frames_in_pic_order_cnt_cycle; i++){
				sps->offset_for_ref_frame[i] = Se(buf, len, &start_bit);
			}
		}

		sps->num_ref_frames = Ue(buf, len, &start_bit);
		sps->gaps_in_frame_num_value_allowed_flag = u(1, buf, &start_bit);
		sps->pic_width_in_mbs_minus1 = Ue(buf, len, &start_bit);
		sps->pic_height_in_map_units_minus1 = Ue(buf, len, &start_bit);

		*width = (sps->pic_width_in_mbs_minus1 + 1) * 16;
		*height = (sps->pic_height_in_map_units_minus1 + 1) * 16;

		sps->frame_mbs_only_flag = u(1, buf, &start_bit);
		if(!sps->frame_mbs_only_flag){
			sps->mb_adaptive_frame_field_flag = u(1, buf, &start_bit);
		}

		sps->direct_8x8_interface_flag = u(1, buf, &start_bit);

		sps->frame_cropping_flag = u(1, buf, &start_bit);
		if(sps->frame_cropping_flag){
			sps->frame_crop_left_offset = Ue(buf, len, &start_bit);
			sps->frame_crop_right_offset = Ue(buf, len, &start_bit);
			sps->frame_crop_top_offset = Ue(buf, len, &start_bit);
			sps->frame_crop_bottom_offset = Ue(buf, len, &start_bit);


			unsigned int crop_unit_x;
			unsigned int crop_unit_y;
			if (0 == sps->chroma_format_idc) // monochrome
			{
				crop_unit_x = 1;
				crop_unit_y = 2 - sps->frame_mbs_only_flag;
			}
			else if (1 == sps->chroma_format_idc) // 4:2:0
			{
				crop_unit_x = 2;
				crop_unit_y = 2 * (2 - sps->frame_mbs_only_flag);
			}
			else if (2 == sps->chroma_format_idc) // 4:2:2
			{
				crop_unit_x = 2;
				crop_unit_y = 2 - sps->frame_mbs_only_flag;
			}
			else if (3 == sps->chroma_format_idc)   // 4:4:4
			{
				crop_unit_x = 1;
				crop_unit_y = 2 - sps->frame_mbs_only_flag;
			}
			else{
				crop_unit_x = 2;
				crop_unit_y = 2 * (2 - sps->frame_mbs_only_flag);
			}

			//PJ_LOG(3, (THIS_FILE, "L=%d R=%d T=%d B=%d",frame_crop_left_offset,frame_crop_right_offset,frame_crop_top_offset,frame_crop_bottom_offset));
			////PJ_LOG(3, (THIS_FILE, "idc=%d flag=%d",chroma_format_idc,frame_mbs_only_flag));

			*width  -= crop_unit_x * (sps->frame_crop_left_offset + sps->frame_crop_right_offset);
			*height -= crop_unit_y * (sps->frame_crop_top_offset  + sps->frame_crop_bottom_offset);
		}

		sps->vui_parameters_presnt_flag = u(1, buf, &start_bit);

		//PJ_LOG(3, (THIS_FILE, "profile_idc:%d\t render_size_change_cb:%d x %d",profile_idc,*width,*height));
		return 0;
	}
	else{
		return -1;
	}
}

int h264_decode_pic_parameter_set(unsigned char * buf, unsigned long len, pps_rbsp* pps)
{
	int i = 0;
	unsigned long start_bit = 0;
	int seq_scaling_list_present_flag[8];
	int forbidden_zero_bit = u(1, buf, &start_bit);
	int nal_ref_idc = u(2, buf, &start_bit);
	int nal_unit_type = u(5, buf, &start_bit);
	if (nal_unit_type == H264_FU_FLAG){
		int S = u(1, buf, &start_bit);
		int E = u(1, buf, &start_bit);
		int R = u(1, buf, &start_bit);
		if(S==1 && E==0){
			nal_unit_type = u(5, buf, &start_bit);
		}
		else{
			int FU_A_nal_unit_type = u(5, buf, &start_bit);
		}
	}
	if (nal_unit_type == NAL_TYPE_PPS)
	{
		pps->pic_parameter_set_id = Ue(buf, len, &start_bit);
		pps->seq_parameter_set_id = Ue(buf, len, &start_bit);
		pps->entropy_coding_mode_flag = u(1, buf, &start_bit);
		pps->pic_order_in_frame_present_flag = u(1, buf, &start_bit);

		pps->num_slice_groups_minus1 = Ue(buf, len, &start_bit);
		if(pps->num_slice_groups_minus1 > 0){
			pps->slice_group_map_type = Ue(buf, len, &start_bit);
			if(pps->slice_group_map_type == 0){
				for(i = 0; i <= pps->num_slice_groups_minus1; i++){
					pps->run_length_minus1[i] = Ue(buf, len, &start_bit);
				}
			}else if(pps->slice_group_map_type == 2){
				for(i = 0; i <= pps->num_slice_groups_minus1; i++){
					pps->top_left[i] = Ue(buf, len, &start_bit);
					pps->bottom_right[i] = Ue(buf, len, &start_bit);
				}
			}else if(pps->slice_group_map_type == 3 || pps->slice_group_map_type == 4 || pps->slice_group_map_type == 5){
				pps->slice_group_change_direction_flag = u(1, buf, &start_bit);
				pps->slice_group_change_rate_minus1 = Ue(buf, len, &start_bit);
			}else if(pps->slice_group_map_type == 6){
				pps->pic_size_in_map_units_minus1 = Ue(buf, len, &start_bit);
				for(i = 0; i <= pps->pic_size_in_map_units_minus1; i++){
					pps->slice_group_id[i] = u(1, buf, &start_bit);
				}
			}
		}

		pps->num_ref_idx_l0_active_minus1 = Ue(buf, len, &start_bit);
		pps->num_ref_idx_l1_active_minus1 = Ue(buf, len, &start_bit);
		pps->weighted_pred_flag = u(1, buf, &start_bit);
		pps->weighted_bipred_idc = u(2, buf, &start_bit);
		pps->pic_init_qp_minus26 = Se(buf, len, &start_bit);
		pps->pic_init_qs_minus26 = Se(buf, len, &start_bit);
		pps->chroma_qp_index_offset = Se(buf, len, &start_bit);
		pps->deblocking_filter_control_present_flag = u(1, buf, &start_bit);
		pps->constrained_intra_pred_flag = u(1, buf, &start_bit);
		pps->redundant_pic_cnt_present_flag = u(1, buf, &start_bit);
		return 0;
	}
	else{
		return -1;
	}
}

int h264_decode_slice_header(unsigned char * buf, unsigned long len, sps_rbsp* sps, pps_rbsp* pps, slice_header* hdr){
	int i = 0, j = 0;
	unsigned long start_bit = 0;
	int seq_scaling_list_present_flag[8];
	int forbidden_zero_bit = u(1, buf, &start_bit);
	int nal_ref_idc = u(2, buf, &start_bit);
	int nal_unit_type = u(5, buf, &start_bit);
	if (nal_unit_type == H264_FU_FLAG){
		int S = u(1, buf, &start_bit);
		int E = u(1, buf, &start_bit);
		int R = u(1, buf, &start_bit);
		if(S==1 && E==0){
			nal_unit_type = u(5, buf, &start_bit);
		}
		else{
			int FU_A_nal_unit_type = u(5, buf, &start_bit);
		}
	}
	if (nal_unit_type == NAL_TYPE_IDR || nal_unit_type == NAL_TYPE_SLICE){
		hdr->first_mb_in_slice = Ue(buf, len, &start_bit);
		hdr->slice_type = Ue(buf, len, &start_bit);
		hdr->pic_parameter_set_id = Ue(buf, len, &start_bit);
		if(sps->separate_colour_plane_flag){
			hdr->colour_plane_id = u(2, buf, &start_bit);
		}
		hdr->frame_num = u(sps->log2_max_frame_num_minus4 + 4, buf, &start_bit);

		if(!sps->frame_mbs_only_flag){
			hdr->field_pic_flag = u(1, buf, &start_bit);
			if(hdr->field_pic_flag){
				hdr->bottom_field_flag = u(1, buf, &start_bit);
			}
		}

		if(nal_unit_type == NAL_TYPE_IDR){
			hdr->idr_pic_id = Ue(buf, len, &start_bit);
		}

		if(sps->pic_order_cnt_type == 0){
			hdr->pic_order_cnt_lsb = u(sps->log2_max_pic_order_cnt_lsb_minus4 + 4, buf, &start_bit);
			if(pps->pic_order_in_frame_present_flag && !hdr->field_pic_flag){
				hdr->delta_pic_order_cnt_bottom = Se(buf, len, &start_bit);
			}
		}
		if(sps->pic_order_cnt_type == 1 && !sps->delta_pic_order_always_zero_flag){
			hdr->delta_pic_order_cnt[0] = Se(buf, len, &start_bit);
			if(pps->pic_order_in_frame_present_flag && !hdr->field_pic_flag){
				hdr->delta_pic_order_cnt[1] = Se(buf, len, &start_bit);
			}
		}

		if(pps->redundant_pic_cnt_present_flag){
			hdr->redundant_pic_cnt = Ue(buf, len, &start_bit);
		}

		if(hdr->slice_type==B_slice1 || hdr->slice_type==B_slice2){
			hdr->direct_spatial_mv_pred_flag = u(1, buf, &start_bit);
		}

		if(hdr->slice_type==P_slice1 || hdr->slice_type==P_slice2 ||
		   hdr->slice_type==SP_slice1 || hdr->slice_type==SP_slice2 ||
		   hdr->slice_type==B_slice1 || hdr->slice_type==B_slice2){
			hdr->num_ref_idx_active_override_flag = u(1, buf, &start_bit);
			if(hdr->num_ref_idx_active_override_flag){
				hdr->num_ref_idx_l0_active_minus1 = Ue(buf, len, &start_bit);
				if(hdr->slice_type==B_slice1 || hdr->slice_type==B_slice2){
					hdr->num_ref_idx_l1_active_minus1 = Ue(buf, len, &start_bit);
				}
			}
		}
		
		/**ref_pic_list_reordering()**/
		if((hdr->slice_type != I_slice1 && hdr->slice_type != I_slice2) &&
		   (hdr->slice_type != SI_slice1 && hdr->slice_type != SI_slice2)){
		   	hdr->ref_pic_list_reordering_flag_l0 = u(1, buf, &start_bit);
		   	if(hdr->ref_pic_list_reordering_flag_l0){
		   		do{
		   			hdr->reordering_of_pic_nums_idc = Ue(buf, len, &start_bit);
	   				if(hdr->reordering_of_pic_nums_idc==0 || hdr->reordering_of_pic_nums_idc==1){
	   					hdr->abs_diff_pic_num_minus1 = Ue(buf, len, &start_bit);
					}
					if(hdr->reordering_of_pic_nums_idc==2){
	   					hdr->long_term_pic_num = Ue(buf, len, &start_bit);
					}
				}while(hdr->reordering_of_pic_nums_idc != 3);
			}
		}
		
		if(hdr->slice_type==B_slice1 || hdr->slice_type==B_slice2){
			hdr->ref_pic_list_reordering_flag_l1 = u(1, buf, &start_bit);
			if(hdr->ref_pic_list_reordering_flag_l1){
		   		do{
		   			hdr->reordering_of_pic_nums_idc = Ue(buf, len, &start_bit);
	   				if(hdr->reordering_of_pic_nums_idc==0 || hdr->reordering_of_pic_nums_idc==1){
	   					hdr->abs_diff_pic_num_minus1 = Ue(buf, len, &start_bit);
					}
					if(hdr->reordering_of_pic_nums_idc==2){
	   					hdr->long_term_pic_num = Ue(buf, len, &start_bit);
					}
				}while(hdr->reordering_of_pic_nums_idc != 3);
			}
		}
		
		if(pps->weighted_pred_flag &&
		  (hdr->slice_type==P_slice1 || hdr->slice_type==P_slice2 || hdr->slice_type==SP_slice1 || hdr->slice_type==SP_slice2 || (pps->weighted_bipred_idc==1 && (hdr->slice_type==B_slice1 || hdr->slice_type==B_slice2)))){
		  	/**pred_weight_table()**/
		  	hdr->luma_log2_weight_denom = Ue(buf, len, &start_bit);
		  	hdr->chroma_log2_weight_denom = Ue(buf, len, &start_bit);
		  	for(i = 0; i <= pps->num_ref_idx_l0_active_minus1; i++){
		  		hdr->luma_weight_l0_flag = u(1, buf, &start_bit);
		  		if(	hdr->luma_weight_l0_flag){
		  			hdr->luma_weight_l0[i] = Se(buf, len, &start_bit);
		  			hdr->luma_offset_l0[i] = Se(buf, len, &start_bit);
				}
				hdr->chroma_weight_l0_flag  = u(1, buf, &start_bit);
		  		if(	hdr->chroma_weight_l0_flag){
				    for(j = 0; j < 2; j++){
					    hdr->chroma_weight_l0[i][j] = Se(buf, len, &start_bit);
			  			hdr->chroma_offset_l0[i][j] = Se(buf, len, &start_bit);
					}
				}
		  	}
		  	
			if(hdr->slice_type==B_slice1 || hdr->slice_type==B_slice2){
				for(i = 0; i <= pps->num_ref_idx_l1_active_minus1; i++){
					hdr->luma_weight_l1_flag = u(1, buf, &start_bit);
			  		if(	hdr->luma_weight_l1_flag){
			  			hdr->luma_weight_l1[i] = Se(buf, len, &start_bit);
			  			hdr->luma_offset_l1[i] = Se(buf, len, &start_bit);
					}
					hdr->chroma_weight_l1_flag  = u(1, buf, &start_bit);
			  		if(	hdr->chroma_weight_l1_flag){
					    for(j = 0; j < 2; j++){
						    hdr->chroma_weight_l1[i][j] = Se(buf, len, &start_bit);
				  			hdr->chroma_offset_l1[i][j] = Se(buf, len, &start_bit);
						}
					}
				}
			}
		}

		if(nal_ref_idc!=0){
			/**dec_ref_pic_marking()**/
			if(nal_unit_type == NAL_TYPE_IDR){
				hdr->no_output_of_prior_pics_flag = u(1, buf, &start_bit);
				hdr->long_term_reference_flag = u(1, buf, &start_bit);
			}else{
				hdr->adaptive_ref_pic_marking_mode_flag = u(1, buf, &start_bit);
				if(hdr->adaptive_ref_pic_marking_mode_flag){
					do{
						hdr->memory_management_control_operation = Ue(buf, len, &start_bit);
						if(hdr->memory_management_control_operation == 1 && hdr->memory_management_control_operation == 3){
							hdr->difference_of_pic_nums_minus1 = Ue(buf, len, &start_bit);
						}
						if(hdr->memory_management_control_operation == 2){
							hdr->long_term_pic_num  = Ue(buf, len, &start_bit);
						}
						if(hdr->memory_management_control_operation == 3 && hdr->memory_management_control_operation == 4){
							hdr->long_term_frame_idx  = Ue(buf, len, &start_bit);
						}
						if(hdr->memory_management_control_operation == 4){
							hdr->max_long_tewrm_frame_idx_plus1  = Ue(buf, len, &start_bit);
						}
					}while(hdr->memory_management_control_operation != 0);
				}
			}
		}

		if(pps->entropy_coding_mode_flag &&
		  (hdr->slice_type != I_slice1 && hdr->slice_type != I_slice2) &&
		  (hdr->slice_type != SI_slice1 && hdr->slice_type != SI_slice2)){
			hdr->cabac_init_idc = Ue(buf, len, &start_bit);
		}

		hdr->slice_qp_delta = Se(buf, len, &start_bit);
		if(hdr->slice_type==SP_slice1 || hdr->slice_type==SP_slice2 ||
		   hdr->slice_type==SI_slice1 || hdr->slice_type==SI_slice2){
		   	if(hdr->slice_type==SP_slice1 || hdr->slice_type==SP_slice2){
		   		hdr->sp_for_switch_flag =u(1, buf, &start_bit);
			}
			hdr->slice_qs_delta = Se(buf, len, &start_bit);
		}

		if(pps->deblocking_filter_control_present_flag){
			hdr->disable_deblocking_filter_idc = Ue(buf, len, &start_bit);
			if(hdr->disable_deblocking_filter_idc != 1){
				hdr->slice_alpha_c0_offset_div2 = Se(buf, len, &start_bit);
				hdr->slice_beta_offset_div2 = Se(buf, len, &start_bit);
			}
		}
	   	return 0;
	}
	else{
		return -1;
	}
}

