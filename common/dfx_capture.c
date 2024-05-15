#include <stdint.h>
#include "x264.h"
#include "common/common.h"
#include "common/log.h"
#include "common/set.h"
#include "encoder/ratecontrol.h"

void dfx_x264_param(x264_param_t* param)
{
    log_debug("------------------------------------------threads");
    log_debug("CPU flags: %u", param->cpu);
    log_debug("i_threads: %d", param->i_threads);
    log_debug("i_lookahead_threads: %d", param->i_lookahead_threads);
    log_debug("b_sliced_threads: %d", param->b_sliced_threads);
    log_debug("b_deterministic: %d", param->b_deterministic);
    log_debug("b_cpu_independent: %d", param->b_cpu_independent);
    log_debug("i_sync_lookahead: %d", param->i_sync_lookahead);
    log_debug("------------------------------------------basic");
    log_debug("i_width: %d", param->i_width);
    log_debug("i_height: %d", param->i_height);
    log_debug("i_csp: %d", param->i_csp);
    log_debug("i_bitdepth: %d", param->i_bitdepth);
    log_debug("i_level_idc: %d", param->i_level_idc);
    log_debug("i_frame_total: %d", param->i_frame_total);
    log_debug("i_nal_hrd: %d", param->i_nal_hrd);
    log_debug("i_fps_num: %u", param->i_fps_num);
    log_debug("i_fps_den: %u", param->i_fps_den);
    log_debug("i_timebase_num: %u", param->i_timebase_num);
    log_debug("i_timebase_den: %u", param->i_timebase_den);
    log_debug("------------------------------------------vui");
    log_debug("vui.i_sar_height: %d", param->vui.i_sar_height);
    log_debug("vui.i_sar_width: %d", param->vui.i_sar_width);
    log_debug("vui.i_overscan: %d", param->vui.i_overscan);
    log_debug("vui.i_vidformat: %d", param->vui.i_vidformat);
    log_debug("vui.b_fullrange: %d", param->vui.b_fullrange);
    log_debug("vui.i_colorprim: %d", param->vui.i_colorprim);
    log_debug("vui.i_transfer: %d", param->vui.i_transfer);
    log_debug("vui.i_colmatrix: %d", param->vui.i_colmatrix);
    log_debug("vui.i_chroma_loc: %d", param->vui.i_chroma_loc);
    log_debug("------------------------------------------frame-structure");
    log_debug("i_frame_reference: %d", param->i_frame_reference);
    log_debug("i_dpb_size: %d", param->i_dpb_size);
    log_debug("i_keyint_max: %d", param->i_keyint_max);
    log_debug("i_keyint_min: %d", param->i_keyint_min);
    log_debug("i_scenecut_threshold: %d", param->i_scenecut_threshold);
    log_debug("b_intra_refresh: %d", param->b_intra_refresh);
    log_debug("i_bframe: %d", param->i_bframe);
    log_debug("i_bframe_adaptive: %d", param->i_bframe_adaptive);
    log_debug("i_bframe_bias: %d", param->i_bframe_bias);
    log_debug("i_bframe_pyramid: %d", param->i_bframe_pyramid);
    log_debug("b_deblocking_filter: %d", param->b_deblocking_filter);
    log_debug("i_deblocking_filter_alphac0: %d", param->i_deblocking_filter_alphac0);
    log_debug("i_deblocking_filter_beta: %d", param->i_deblocking_filter_beta);
    log_debug("b_cabac: %d", param->b_cabac);
    log_debug("i_cabac_init_idc: %d", param->i_cabac_init_idc);
    log_debug("b_constrained_intra: %d", param->b_constrained_intra);
    log_debug("i_cqm_preset: %d", param->i_cqm_preset);
    log_debug("i_slice_max_size: %d", param->i_slice_max_size);
    log_debug("i_slice_max_mbs: %d", param->i_slice_max_mbs);
    log_debug("i_slice_min_mbs: %d", param->i_slice_min_mbs);
    log_debug("i_slice_count: %d", param->i_slice_count);
    log_debug("i_slice_count_max: %d", param->i_slice_count_max);
    log_debug("------------------------------------------analysis");
    log_debug("analyse.intra: %u", param->analyse.intra);
    log_debug("analyse.inter: %u", param->analyse.inter);
    log_debug("analyse.b_transform_8x8: %d", param->analyse.b_transform_8x8);
    log_debug("analyse.i_weighted_pred: %d", param->analyse.i_weighted_pred);
    log_debug("analyse.b_weighted_bipred: %d", param->analyse.b_weighted_bipred);
    log_debug("analyse.i_direct_mv_pred: %d", param->analyse.i_direct_mv_pred);
    log_debug("analyse.i_chroma_qp_offset: %d", param->analyse.i_chroma_qp_offset);
    log_debug("analyse.i_me_method: %d", param->analyse.i_me_method);
    log_debug("analyse.i_me_range: %d", param->analyse.i_me_range);
    log_debug("analyse.i_mv_range: %d", param->analyse.i_mv_range);
    log_debug("analyse.i_mv_range_thread: %d", param->analyse.i_mv_range_thread);
    log_debug("analyse.i_subpel_refine: %d", param->analyse.i_subpel_refine);
    log_debug("analyse.b_chroma_me: %d", param->analyse.b_chroma_me);
    log_debug("analyse.b_mixed_references: %d", param->analyse.b_mixed_references);
    log_debug("analyse.i_trellis: %d", param->analyse.i_trellis);
    log_debug("analyse.b_fast_pskip: %d", param->analyse.b_fast_pskip);
    log_debug("analyse.b_dct_decimate: %d", param->analyse.b_dct_decimate);
    log_debug("analyse.i_noise_reduction: %d", param->analyse.i_noise_reduction);
    log_debug("analyse.f_psy_rd: %f", param->analyse.f_psy_rd);
    log_debug("analyse.f_psy_trellis: %f", param->analyse.f_psy_trellis);
    log_debug("analyse.b_psy: %d", param->analyse.b_psy);
    log_debug("analyse.b_mb_info: %d", param->analyse.b_mb_info);
    log_debug("analyse.b_mb_info_update: %d", param->analyse.b_mb_info_update);
    log_debug("analyse.i_luma_deadzone[0]: %d", param->analyse.i_luma_deadzone[0]);
    log_debug("analyse.i_luma_deadzone[1]: %d", param->analyse.i_luma_deadzone[1]);
    log_debug("analyse.b_psnr: %d", param->analyse.b_psnr);
    log_debug("analyse.b_ssim: %d", param->analyse.b_ssim);
    log_debug("------------------------------------------rate-control");
    log_debug("rc.i_rc_method: %d", param->rc.i_rc_method);
    log_debug("rc.i_qp_constant: %d", param->rc.i_qp_constant);
    log_debug("rc.i_qp_min: %d", param->rc.i_qp_min);
    log_debug("rc.i_qp_max: %d", param->rc.i_qp_max);
    log_debug("rc.i_qp_step: %d", param->rc.i_qp_step);
    log_debug("rc.i_bitrate: %d", param->rc.i_bitrate);
    log_debug("rc.f_rf_constant: %f", param->rc.f_rf_constant);
    log_debug("rc.f_rf_constant_max: %f", param->rc.f_rf_constant_max);
    log_debug("rc.f_rate_tolerance: %f", param->rc.f_rate_tolerance);
    log_debug("rc.i_vbv_max_bitrate: %d", param->rc.i_vbv_max_bitrate);
    log_debug("rc.i_vbv_buffer_size: %d", param->rc.i_vbv_buffer_size);
    log_debug("rc.f_vbv_buffer_init: %f", param->rc.f_vbv_buffer_init);
    log_debug("rc.f_ip_factor: %f", param->rc.f_ip_factor);
    log_debug("rc.f_pb_factor: %f", param->rc.f_pb_factor);
    log_debug("rc.b_filler: %d", param->rc.b_filler);
    log_debug("rc.i_aq_mode: %d", param->rc.i_aq_mode);
    log_debug("rc.f_aq_strength: %f", param->rc.f_aq_strength);
    log_debug("rc.b_mb_tree: %d", param->rc.b_mb_tree);
    log_debug("rc.i_lookahead: %d", param->rc.i_lookahead);
    log_debug("rc.b_stat_write: %d", param->rc.b_stat_write);
    log_debug("rc.b_stat_read: %d", param->rc.b_stat_read);
    log_debug("rc.f_qcompress: %f", param->rc.f_qcompress);
    log_debug("rc.f_qblur: %f", param->rc.f_qblur);
    log_debug("rc.f_complexity_blur: %f", param->rc.f_complexity_blur);
    log_debug("rc.i_zones: %d", param->rc.i_zones);
    log_debug("------------------------------------------others");
    log_debug("b_annexb: %d", param->b_annexb);
    log_debug("i_sps_id: %d", param->i_sps_id);
    log_debug("b_vfr_input: %d", param->b_vfr_input);
    log_debug("b_pic_struct: %d", param->b_pic_struct);
}

void dfx_x264_encoder_open(struct x264_t* h)
{
    log_debug("------------------------------------------encoder_open");
    log_debug("h->mb.i_mb_width=%d", h->mb.i_mb_width);
    log_debug("h->mb.i_mb_height=%d", h->mb.i_mb_height);
    log_debug("h->mb.i_mb_count=%d", h->mb.i_mb_count);
    log_debug("h->mb.chroma_h_shift=%d", h->mb.chroma_h_shift);
    log_debug("h->mb.chroma_v_shift=%d", h->mb.chroma_v_shift);
    log_debug("h->mb.b_adaptive_mbaff=%d", h->mb.b_adaptive_mbaff);
    log_debug("h->frames.i_delay=%d",  h->frames.i_delay);
    log_debug("h->frames.i_bframe_delay=%d", h->frames.i_bframe_delay);
    log_debug("h->frames.i_max_ref0=%d", h->frames.i_max_ref0);
    log_debug("h->frames.i_max_ref1=%d", h->frames.i_max_ref1);
    log_debug("h->frames.i_max_dpb=%d", h->frames.i_max_dpb);
    log_debug("h->frames.b_have_lowres=%d", h->frames.b_have_lowres);
    log_debug("h->frames.b_have_sub8x8_esa=%d", h->frames.b_have_sub8x8_esa);
    log_debug("h->frames.i_last_idr=%d", h->frames.i_last_idr);
    log_debug("h->frames.i_last_keyframe=%d", h->frames.i_last_keyframe);
}

void dfx_x264_sps(x264_sps_t* sps)
{
    log_debug("------------------------------------------sps");
    log_debug("i_id: %d", sps->i_id);
    log_debug("i_profile_idc: %d", sps->i_profile_idc);
    log_debug("i_level_idc: %d", sps->i_level_idc);
    log_debug("b_constraint_set0: %d", sps->b_constraint_set0);
    log_debug("b_constraint_set1: %d", sps->b_constraint_set1);
    log_debug("b_constraint_set2: %d", sps->b_constraint_set2);
    log_debug("b_constraint_set3: %d", sps->b_constraint_set3);
    log_debug("i_log2_max_frame_num: %d", sps->i_log2_max_frame_num);
    log_debug("i_poc_type: %d", sps->i_poc_type);
    log_debug("i_log2_max_poc_lsb: %d", sps->i_log2_max_poc_lsb);
    log_debug("i_num_ref_frames: %d", sps->i_num_ref_frames);
    log_debug("b_gaps_in_frame_num_value_allowed: %d", sps->b_gaps_in_frame_num_value_allowed);
    log_debug("i_mb_width: %d", sps->i_mb_width);
    log_debug("i_mb_height: %d", sps->i_mb_height);
    log_debug("b_frame_mbs_only: %d", sps->b_frame_mbs_only);
    log_debug("b_mb_adaptive_frame_field: %d", sps->b_mb_adaptive_frame_field);
    log_debug("b_direct8x8_inference: %d", sps->b_direct8x8_inference);
    log_debug("b_crop: %d", sps->b_crop);
    log_debug("crop.i_left: %d", sps->crop.i_left);
    log_debug("crop.i_right: %d", sps->crop.i_right);
    log_debug("crop.i_top: %d", sps->crop.i_top);
    log_debug("crop.i_bottom: %d", sps->crop.i_bottom);
    log_debug("b_vui: %d", sps->b_vui);
    log_debug("vui.b_aspect_ratio_info_present: %d", sps->vui.b_aspect_ratio_info_present);
    log_debug("vui.i_sar_width: %d", sps->vui.i_sar_width);
    log_debug("vui.i_sar_height: %d", sps->vui.i_sar_height);
    log_debug("vui.b_overscan_info_present: %d", sps->vui.b_overscan_info_present);
    log_debug("vui.b_overscan_info: %d", sps->vui.b_overscan_info);
    log_debug("vui.b_signal_type_present: %d", sps->vui.b_signal_type_present);
    log_debug("vui.i_vidformat: %d", sps->vui.i_vidformat);
    log_debug("vui.b_fullrange: %d", sps->vui.b_fullrange);
    log_debug("vui.b_color_description_present: %d", sps->vui.b_color_description_present);
    log_debug("vui.i_colorprim: %d", sps->vui.i_colorprim);
    log_debug("vui.i_transfer: %d", sps->vui.i_transfer);
    log_debug("vui.i_colmatrix: %d", sps->vui.i_colmatrix);
    log_debug("vui.b_chroma_loc_info_present: %d", sps->vui.b_chroma_loc_info_present);
    log_debug("vui.i_chroma_loc_top: %d", sps->vui.i_chroma_loc_top);
    log_debug("vui.i_chroma_loc_bottom: %d", sps->vui.i_chroma_loc_bottom);
    log_debug("vui.b_timing_info_present: %d", sps->vui.b_timing_info_present);
    log_debug("vui.i_num_units_in_tick: %d", sps->vui.i_num_units_in_tick);
    log_debug("vui.i_time_scale: %d", sps->vui.i_time_scale);
    log_debug("vui.b_fixed_frame_rate: %d", sps->vui.b_fixed_frame_rate);
    log_debug("vui.b_nal_hrd_parameters_present: %d", sps->vui.b_nal_hrd_parameters_present);
    log_debug("vui.b_vcl_hrd_parameters_present: %d", sps->vui.b_vcl_hrd_parameters_present);
    log_debug("vui.hrd.i_cpb_cnt: %d", sps->vui.hrd.i_cpb_cnt);
    log_debug("vui.hrd.i_bit_rate_scale: %d", sps->vui.hrd.i_bit_rate_scale);
    log_debug("vui.hrd.i_cpb_size_scale: %d", sps->vui.hrd.i_cpb_size_scale);
    log_debug("vui.hrd.i_bit_rate_value: %d", sps->vui.hrd.i_bit_rate_value);
    log_debug("vui.hrd.i_cpb_size_value: %d", sps->vui.hrd.i_cpb_size_value);
    log_debug("vui.hrd.i_bit_rate_unscaled: %d", sps->vui.hrd.i_bit_rate_unscaled);
    log_debug("vui.hrd.i_cpb_size_unscaled: %d", sps->vui.hrd.i_cpb_size_unscaled);
    log_debug("vui.hrd.b_cbr_hrd: %d", sps->vui.hrd.b_cbr_hrd);
    log_debug("vui.hrd.i_initial_cpb_removal_delay_length: %d", sps->vui.hrd.i_initial_cpb_removal_delay_length);
    log_debug("vui.hrd.i_cpb_removal_delay_length: %d", sps->vui.hrd.i_cpb_removal_delay_length);
    log_debug("vui.hrd.i_dpb_output_delay_length: %d", sps->vui.hrd.i_dpb_output_delay_length);
    log_debug("vui.hrd.i_time_offset_length: %d", sps->vui.hrd.i_time_offset_length);
    log_debug("vui.b_pic_struct_present: %d", sps->vui.b_pic_struct_present);
    log_debug("vui.b_bitstream_restriction: %d", sps->vui.b_bitstream_restriction);
    log_debug("vui.b_motion_vectors_over_pic_boundaries: %d", sps->vui.b_motion_vectors_over_pic_boundaries);
    log_debug("vui.i_max_bytes_per_pic_denom: %d", sps->vui.i_max_bytes_per_pic_denom);
    log_debug("vui.i_max_bits_per_mb_denom: %d", sps->vui.i_max_bits_per_mb_denom);
    log_debug("vui.i_log2_max_mv_length_horizontal: %d", sps->vui.i_log2_max_mv_length_horizontal);
    log_debug("vui.i_log2_max_mv_length_vertical: %d", sps->vui.i_log2_max_mv_length_vertical);
    log_debug("vui.i_num_reorder_frames: %d", sps->vui.i_num_reorder_frames);
    log_debug("vui.i_max_dec_frame_buffering: %d", sps->vui.i_max_dec_frame_buffering);
    log_debug("b_qpprime_y_zero_transform_bypass: %d", sps->b_qpprime_y_zero_transform_bypass);
    log_debug("i_chroma_format_idc: %d", sps->i_chroma_format_idc);
    log_debug("b_avcintra_hd: %d", sps->b_avcintra_hd);
    log_debug("b_avcintra_4k: %d", sps->b_avcintra_4k);
    log_debug("i_cqm_preset: %d", sps->i_cqm_preset);
}

void dfx_x264_pps(x264_pps_t* pps)
{
    log_debug("------------------------------------------pps");
    log_debug("i_id: %d", pps->i_id);
    log_debug("i_sps_id: %d", pps->i_sps_id);
    log_debug("b_cabac: %d", pps->b_cabac);
    log_debug("b_pic_order: %d", pps->b_pic_order);
    log_debug("i_num_slice_groups: %d", pps->i_num_slice_groups);
    log_debug("i_num_ref_idx_l0_default_active: %d", pps->i_num_ref_idx_l0_default_active);
    log_debug("i_num_ref_idx_l1_default_active: %d", pps->i_num_ref_idx_l1_default_active);
    log_debug("b_weighted_pred: %d", pps->b_weighted_pred);
    log_debug("b_weighted_bipred: %d", pps->b_weighted_bipred);
    log_debug("i_pic_init_qp: %d", pps->i_pic_init_qp);
    log_debug("i_pic_init_qs: %d", pps->i_pic_init_qs);
    log_debug("i_chroma_qp_index_offset: %d", pps->i_chroma_qp_index_offset);
    log_debug("b_deblocking_filter_control: %d", pps->b_deblocking_filter_control);
    log_debug("b_constrained_intra_pred: %d", pps->b_constrained_intra_pred);
    log_debug("b_redundant_pic_cnt: %d", pps->b_redundant_pic_cnt);
    log_debug("b_transform_8x8_mode: %d", pps->b_transform_8x8_mode);
}

void dfx_x264_rc_new(x264_ratecontrol_t* rc)
{
    log_debug("------------------------------------------ratecontrol");
    log_debug("b_abr: %d", rc->b_abr);
    log_debug("b_2pass: %d", rc->b_2pass);
    log_debug("b_vbv: %d", rc->b_vbv);
    log_debug("b_vbv_min_rate: %d", rc->b_vbv_min_rate);
    log_debug("fps: %f", rc->fps);
    log_debug("bitrate: %f", rc->bitrate);
    log_debug("rate_tolerance: %f", rc->rate_tolerance);
    log_debug("qcompress: %f", rc->qcompress);
    log_debug("nmb: %d", rc->nmb);
    log_debug("qp_constant[0]: %d", rc->qp_constant[0]);
    log_debug("qp_constant[1]: %d", rc->qp_constant[1]);
    log_debug("qp_constant[2]: %d", rc->qp_constant[2]);

    log_debug("qpm: %f", rc->qpm);
    log_debug("qpa_rc: %f", rc->qpa_rc);
    log_debug("qpa_rc_prev: %f", rc->qpa_rc_prev);
    log_debug("qpa_aq: %d", rc->qpa_aq);
    log_debug("qpa_aq_prev: %d", rc->qpa_aq_prev);
    log_debug("qp_novbv: %f", rc->qp_novbv);

    log_debug("buffer_size: %f", rc->buffer_size);
    log_debug("buffer_fill_final: %lld", (long long)rc->buffer_fill_final);
    log_debug("buffer_fill_final_min: %lld", (long long)rc->buffer_fill_final_min);
    log_debug("buffer_fill: %f", rc->buffer_fill);
    log_debug("buffer_rate: %f", rc->buffer_rate);
    log_debug("vbv_max_rate: %f", rc->vbv_max_rate);
    log_debug("single_frame_vbv: %d", rc->single_frame_vbv);
    log_debug("rate_factor_max_increment: %f", rc->rate_factor_max_increment);

    log_debug("last_satd: %d", rc->last_satd);
    log_debug("last_rceq: %f", rc->last_rceq);
    log_debug("cplxr_sum: %f", rc->cplxr_sum);
    log_debug("expected_bits_sum: %f", rc->expected_bits_sum);
    log_debug("filler_bits_sum: %lld", (long long)rc->filler_bits_sum);
    log_debug("wanted_bits_window: %f", rc->wanted_bits_window);
    log_debug("cbr_decay: %f", rc->cbr_decay);
    log_debug("short_term_cplxsum: %f", rc->short_term_cplxsum);
    log_debug("short_term_cplxcount: %f", rc->short_term_cplxcount);
    log_debug("rate_factor_constant: %f", rc->rate_factor_constant);
    log_debug("ip_offset: %f", rc->ip_offset);
    log_debug("pb_offset: %f", rc->pb_offset);

    log_debug("last_qscale: %f", rc->last_qscale);
    log_debug("last_qscale_for[0]: %f", rc->last_qscale_for[0]);
    log_debug("last_qscale_for[1]: %f", rc->last_qscale_for[1]);
    log_debug("last_qscale_for[2]: %f", rc->last_qscale_for[2]);
    log_debug("last_non_b_pict_type: %d", rc->last_non_b_pict_type);
    log_debug("accum_p_qp: %f", rc->accum_p_qp);
    log_debug("accum_p_norm: %f", rc->accum_p_norm);
    log_debug("last_accum_p_norm: %f", rc->last_accum_p_norm);
    log_debug("lmin[0]: %f", rc->lmin[0]);
    log_debug("lmin[1]: %f", rc->lmin[1]);
    log_debug("lmin[2]: %f", rc->lmin[2]);
    log_debug("lmax[0]: %f", rc->lmax[0]);
    log_debug("lmax[1]: %f", rc->lmax[1]);
    log_debug("lmax[2]: %f", rc->lmax[2]);
    log_debug("lstep: %f", rc->lstep);

    log_debug("mbtree.qpbuf_pos: %d", rc->mbtree.qpbuf_pos);
    log_debug("mbtree.src_mb_count: %d", rc->mbtree.src_mb_count);
    log_debug("mbtree.rescale_enabled: %d", rc->mbtree.rescale_enabled);
    log_debug("mbtree.filtersize[0]: %d", rc->mbtree.filtersize[0]);
    log_debug("mbtree.filtersize[1]: %d", rc->mbtree.filtersize[1]);
    log_debug("mbtree.srcdim[0]: %d", rc->mbtree.srcdim[0]);
    log_debug("mbtree.srcdim[1]: %d", rc->mbtree.srcdim[1]);

    log_debug("frame_size_estimated: %f", rc->frame_size_estimated);
    log_debug("bits_so_far: %f", rc->bits_so_far);
    log_debug("frame_size_maximum: %f", rc->frame_size_maximum);
    log_debug("frame_size_planned: %f", rc->frame_size_planned);
    log_debug("slice_size_planned: %f", rc->slice_size_planned);
    log_debug("bframes: %d", rc->bframes);
    log_debug("bframe_bits: %d", rc->bframe_bits);
    log_debug("------------------------------------------END");
}