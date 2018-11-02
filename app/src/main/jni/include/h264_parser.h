#include "math.h"
#include <stdio.h>
#include <stdlib.h>

#define H264_GET_NAL_TYPE(c)  ( (c) & 0x1F )

#define H264_FU_FLAG		28							// H264 FU

/*Nal Type Define*/
#define NAL_TYPE_SLICE      1
#define NAL_TYPE_IDR        5
#define NAL_TYPE_SEI        6
#define NAL_TYPE_SPS        7
#define NAL_TYPE_PPS        8
#define NAL_TYPE_SEQ_END    9
#define NAL_TYPE_STREAM_END 10
#define NALU_TYPE_EOSTREAM  11
#define NALU_TYPE_FILL      12


/*slice_type*/
#define P_slice1  0
#define B_slice1  1
#define I_slice1  2
#define SP_slice1 3
#define SI_slice1 4
#define P_slice2  5
#define B_slice2  6
#define I_slice2  7
#define SP_slice2 8
#define SI_slice2 9

typedef struct sps_rbsp{
	int profile_idc;
	int constraint_set0_flag;
	int constraint_set1_flag;
	int constraint_set2_flag;
	int constraint_set3_flag;
	int reserved_zero_4bits;
	int level_idc;
	int seq_parameter_set_id;

	int chroma_format_idc;
	int separate_colour_plane_flag;
	int bit_depth_luma_minus8;
	int bit_depth_chroma_minus8;
	int qpprime_y_zero_transform_bypass_flag;
	int seq_scaling_matrix_present_flag;
	int seq_scaling_list_present_flag[12];

	int log2_max_frame_num_minus4;

	int pic_order_cnt_type;
	int log2_max_pic_order_cnt_lsb_minus4;
	int delta_pic_order_always_zero_flag;
	int offset_for_non_ref_pic;
	int offset_for_top_to_bottom_field;
	int num_ref_frames_in_pic_order_cnt_cycle;
	int offset_for_ref_frame[256];

	int num_ref_frames;
	int gaps_in_frame_num_value_allowed_flag;
	int pic_width_in_mbs_minus1;
	int pic_height_in_map_units_minus1;

	int frame_mbs_only_flag;
	int mb_adaptive_frame_field_flag;

	int direct_8x8_interface_flag;

	int frame_cropping_flag;
	int frame_crop_left_offset;
	int frame_crop_right_offset;
	int frame_crop_top_offset;
	int frame_crop_bottom_offset;

	int vui_parameters_presnt_flag;
} sps_rbsp;

typedef struct pps_rbsp{
	int pic_parameter_set_id;
	int seq_parameter_set_id;
	int entropy_coding_mode_flag;
	int pic_order_in_frame_present_flag;

	int num_slice_groups_minus1;
	int slice_group_map_type;
	int run_length_minus1[256];
	int top_left[256];
	int bottom_right[256];
	int slice_group_change_direction_flag;
	int slice_group_change_rate_minus1;
	int pic_size_in_map_units_minus1;
	int slice_group_id[256];

	int num_ref_idx_l0_active_minus1;
	int num_ref_idx_l1_active_minus1;
	int weighted_pred_flag;
	int weighted_bipred_idc;
	int pic_init_qp_minus26;
	int pic_init_qs_minus26;
	int chroma_qp_index_offset;
	int deblocking_filter_control_present_flag;
	int constrained_intra_pred_flag;
	int redundant_pic_cnt_present_flag;
} pps_rbsp;

typedef struct slice_header{
	int first_mb_in_slice;
	int slice_type;
	int pic_parameter_set_id;
	int colour_plane_id;
	int frame_num;

	int field_pic_flag;
	int bottom_field_flag;

	int idr_pic_id;

	int pic_order_cnt_lsb;
	int delta_pic_order_cnt_bottom;
	int delta_pic_order_cnt[2];

	int redundant_pic_cnt;

	int direct_spatial_mv_pred_flag;

	int num_ref_idx_active_override_flag;
	int num_ref_idx_l0_active_minus1;
	int num_ref_idx_l1_active_minus1;
	
	/**ref_pic_list_reordering()**/
	int ref_pic_list_reordering_flag_l0;
	int ref_pic_list_reordering_flag_l1;
	int reordering_of_pic_nums_idc;
	int abs_diff_pic_num_minus1;
	int long_term_pic_num;
	
	/**pred_weight_table()**/
	int luma_log2_weight_denom;
	int chroma_log2_weight_denom;
	int luma_weight_l0_flag;
	int luma_weight_l0[256];
	int luma_offset_l0[256];
	int chroma_weight_l0_flag;
	int chroma_weight_l0[256][2];
	int chroma_offset_l0[256][2];
	int luma_weight_l1_flag;
	int luma_weight_l1[256];
	int luma_offset_l1[256];
	int chroma_weight_l1_flag;
	int chroma_weight_l1[256][2];
	int chroma_offset_l1[256][2];
	
	/**dec_ref_pic_marking()**/
	int no_output_of_prior_pics_flag;
	int long_term_reference_flag;
	int memory_management_control_operation;
	int adaptive_ref_pic_marking_mode_flag;
	int difference_of_pic_nums_minus1;
	int long_term_frame_idx;
	int max_long_tewrm_frame_idx_plus1;

	int cabac_init_idc;

	int slice_qp_delta;
	int sp_for_switch_flag;
	int slice_qs_delta;
	
	int disable_deblocking_filter_idc;
	int slice_alpha_c0_offset_div2;
	int slice_beta_offset_div2;
}slice_header;

int h264_decode_seq_parameter_set(unsigned char * buf, unsigned long len, sps_rbsp* sps,  int *width, int *height);
int h264_decode_pic_parameter_set(unsigned char * buf, unsigned long len, pps_rbsp* pps);
int h264_decode_slice_header(unsigned char * buf, unsigned long len, sps_rbsp* sps, pps_rbsp* pps, slice_header* hdr);
