#ifndef DFX_CAPTURE
#define DFX_CAPTURE

#include "x264.h"

void dfx_x264_param(x264_param_t* param);
void dfx_x264_encoder_open(struct x264_t* h);
void dfx_x264_sps(x264_sps_t* sps);
void dfx_x264_pps(x264_pps_t* pps);
void dfx_x264_rc_init(x264_ratecontrol_t* h);
#endif