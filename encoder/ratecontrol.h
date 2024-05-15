/*****************************************************************************
 * ratecontrol.h: ratecontrol
 *****************************************************************************
 * Copyright (C) 2003-2024 x264 project
 *
 * Authors: Loren Merritt <lorenm@u.washington.edu>
 *          Laurent Aimar <fenrir@via.ecp.fr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02111, USA.
 *
 * This program is also available under a commercial proprietary license.
 * For more information, contact us at licensing@x264.com.
 *****************************************************************************/

#ifndef X264_ENCODER_RATECONTROL_H
#define X264_ENCODER_RATECONTROL_H

/* Completely arbitrary.  Ratecontrol lowers relative quality at higher framerates
 * and the reverse at lower framerates; this serves as the center of the curve.
 * Halve all the values for frame-packed 3D to compensate for the "doubled"
 * framerate. */
#define BASE_FRAME_DURATION (0.04f / ((h->param.i_frame_packing == 5)+1))

/* Arbitrary limitations as a sanity check. */
#define MAX_FRAME_DURATION (1.00f / ((h->param.i_frame_packing == 5)+1))
#define MIN_FRAME_DURATION (0.01f / ((h->param.i_frame_packing == 5)+1))

#define CLIP_DURATION(f) x264_clip3f(f,MIN_FRAME_DURATION,MAX_FRAME_DURATION)


typedef struct
{
    int pict_type;
    int frame_type;
    int kept_as_ref;
    double qscale;
    int mv_bits;
    int tex_bits;
    int misc_bits;
    double expected_bits; /* total expected bits up to the current frame (current one excluded) */
    double expected_vbv;
    double new_qscale;
    float new_qp;
    int i_count;
    int p_count;
    int s_count;
    float blurred_complexity;
    char direct_mode;
    int16_t weight[3][2];
    int16_t i_weight_denom[2];
    int refcount[16];
    int refs;
    int64_t i_duration;
    int64_t i_cpb_duration;
    int out_num;
} ratecontrol_entry_t;

typedef struct
{
    float coeff_min;
    float coeff;
    float count;
    float decay;
    float offset;
} predictor_t;

struct x264_ratecontrol_t
{
    /* constants */
    int b_abr;
    int b_2pass;
    int b_vbv;
    int b_vbv_min_rate;
    double fps;
    double bitrate;
    double rate_tolerance;
    double qcompress;
    int nmb;                    /* number of macroblocks in a frame */
    int qp_constant[3];

    /* current frame */
    ratecontrol_entry_t *rce;
    float qpm;                  /* qp for current macroblock: precise float for AQ */
    float qpa_rc;               /* average of macroblocks' qp before aq */
    float qpa_rc_prev;
    int   qpa_aq;               /* average of macroblocks' qp after aq */
    int   qpa_aq_prev;
    float qp_novbv;             /* QP for the current frame if 1-pass VBV was disabled. */

    /* VBV stuff */
    double buffer_size;
    int64_t buffer_fill_final;
    int64_t buffer_fill_final_min;
    double buffer_fill;         /* planned buffer, if all in-progress frames hit their bit budget */
    double buffer_rate;         /* # of bits added to buffer_fill after each frame */
    double vbv_max_rate;        /* # of bits added to buffer_fill per second */
    predictor_t *pred;          /* predict frame size from satd */
    int single_frame_vbv;
    float rate_factor_max_increment; /* Don't allow RF above (CRF + this value). */

    /* ABR stuff */
    int    last_satd;
    double last_rceq;
    double cplxr_sum;           /* sum of bits*qscale/rceq */
    double expected_bits_sum;   /* sum of qscale2bits after rceq, ratefactor, and overflow, only includes finished frames */
    int64_t filler_bits_sum;    /* sum in bits of finished frames' filler data */
    double wanted_bits_window;  /* target bitrate * window */
    double cbr_decay;
    double short_term_cplxsum;
    double short_term_cplxcount;
    double rate_factor_constant;
    double ip_offset;
    double pb_offset;

    /* 2pass stuff */
    FILE *p_stat_file_out;
    char *psz_stat_file_tmpname;
    FILE *p_mbtree_stat_file_out;
    char *psz_mbtree_stat_file_tmpname;
    char *psz_mbtree_stat_file_name;
    FILE *p_mbtree_stat_file_in;

    int num_entries;            /* number of ratecontrol_entry_ts */
    ratecontrol_entry_t *entry; /* FIXME: copy needed data and free this once init is done */
    ratecontrol_entry_t **entry_out;
    double last_qscale;
    double last_qscale_for[3];  /* last qscale for a specific pict type, used for max_diff & ipb factor stuff */
    int last_non_b_pict_type;
    double accum_p_qp;          /* for determining I-frame quant */
    double accum_p_norm;
    double last_accum_p_norm;
    double lmin[3];             /* min qscale by frame type */
    double lmax[3];
    double lstep;               /* max change (multiply) in qscale per frame */
    struct
    {
        uint16_t *qp_buffer[2]; /* Global buffers for converting MB-tree quantizer data. */
        int qpbuf_pos;          /* In order to handle pyramid reordering, QP buffer acts as a stack.
                                 * This value is the current position (0 or 1). */
        int src_mb_count;

        /* For rescaling */
        int rescale_enabled;
        float *scale_buffer[2]; /* Intermediate buffers */
        int filtersize[2];      /* filter size (H/V) */
        float *coeffs[2];
        int *pos[2];
        int srcdim[2];          /* Source dimensions (W/H) */
    } mbtree;

    /* MBRC stuff */
    volatile float frame_size_estimated; /* Access to this variable must be atomic: double is
                                          * not atomic on all arches we care about */
    volatile float bits_so_far;
    double frame_size_maximum;  /* Maximum frame size due to MinCR */
    double frame_size_planned;
    double slice_size_planned;
    predictor_t *row_pred;
    predictor_t row_preds[3][2];
    predictor_t *pred_b_from_p; /* predict B-frame size from P-frame satd */
    int bframes;                /* # consecutive B-frames before this P-frame */
    int bframe_bits;            /* total cost of those frames */

    int i_zones;
    x264_zone_t *zones;
    x264_zone_t *prev_zone;

    /* hrd stuff */
    int initial_cpb_removal_delay;
    int initial_cpb_removal_delay_offset;
    double nrt_first_access_unit; /* nominal removal time */
    double previous_cpb_final_arrival_time;
    uint64_t hrd_multiply_denom;
};

#define x264_ratecontrol_new x264_template(ratecontrol_new)
int  x264_ratecontrol_new   ( x264_t * );
#define x264_ratecontrol_delete x264_template(ratecontrol_delete)
void x264_ratecontrol_delete( x264_t * );

#define x264_ratecontrol_init_reconfigurable x264_template(ratecontrol_init_reconfigurable)
void x264_ratecontrol_init_reconfigurable( x264_t *h, int b_init );
#define x264_encoder_reconfig_apply x264_template(encoder_reconfig_apply)
int x264_encoder_reconfig_apply( x264_t *h, x264_param_t *param );

#define x264_adaptive_quant_frame x264_template(adaptive_quant_frame)
void x264_adaptive_quant_frame( x264_t *h, x264_frame_t *frame, float *quant_offsets );
#define x264_macroblock_tree_read x264_template(macroblock_tree_read)
int  x264_macroblock_tree_read( x264_t *h, x264_frame_t *frame, float *quant_offsets );
#define x264_reference_build_list_optimal x264_template(reference_build_list_optimal)
int  x264_reference_build_list_optimal( x264_t *h );
#define x264_thread_sync_ratecontrol x264_template(thread_sync_ratecontrol)
void x264_thread_sync_ratecontrol( x264_t *cur, x264_t *prev, x264_t *next );
#define x264_ratecontrol_zone_init x264_template(ratecontrol_zone_init)
void x264_ratecontrol_zone_init( x264_t * );
#define x264_ratecontrol_start x264_template(ratecontrol_start)
void x264_ratecontrol_start( x264_t *, int i_force_qp, int overhead );
#define x264_ratecontrol_slice_type x264_template(ratecontrol_slice_type)
int  x264_ratecontrol_slice_type( x264_t *, int i_frame );
#define x264_ratecontrol_set_weights x264_template(ratecontrol_set_weights)
void x264_ratecontrol_set_weights( x264_t *h, x264_frame_t *frm );
#define x264_ratecontrol_mb x264_template(ratecontrol_mb)
int  x264_ratecontrol_mb( x264_t *, int bits );
#define x264_ratecontrol_qp x264_template(ratecontrol_qp)
int  x264_ratecontrol_qp( x264_t * );
#define x264_ratecontrol_mb_qp x264_template(ratecontrol_mb_qp)
int  x264_ratecontrol_mb_qp( x264_t *h );
#define x264_ratecontrol_end x264_template(ratecontrol_end)
int  x264_ratecontrol_end( x264_t *, int bits, int *filler );
#define x264_ratecontrol_summary x264_template(ratecontrol_summary)
void x264_ratecontrol_summary( x264_t * );
#define x264_rc_analyse_slice x264_template(rc_analyse_slice)
int  x264_rc_analyse_slice( x264_t *h );
#define x264_threads_distribute_ratecontrol x264_template(threads_distribute_ratecontrol)
void x264_threads_distribute_ratecontrol( x264_t *h );
#define x264_threads_merge_ratecontrol x264_template(threads_merge_ratecontrol)
void x264_threads_merge_ratecontrol( x264_t *h );
#define x264_hrd_fullness x264_template(hrd_fullness)
void x264_hrd_fullness( x264_t *h );

#endif
