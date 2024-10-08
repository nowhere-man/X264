#ifndef TIMER_H
#define TIMER_H

#include "osdep.h"

typedef struct {
    int64_t base;
    float duration;
} x264_clock_t;

typedef struct {
    x264_clock_t encoder_open;
    struct {
        x264_clock_t total;
        x264_clock_t aq;
        x264_clock_t lower_res_init;
        struct {
            x264_clock_t total;
            x264_clock_t frame_type_decide;
            x264_clock_t scenecut;
            x264_clock_t mb_tree;
            x264_clock_t vbv_lookahead;
            x264_clock_t rc_analyse;
            x264_clock_t weightp;
        } lookahead;
        x264_clock_t build_ref_list;
        x264_clock_t ratecontrol;
        struct {
            x264_clock_t total;
            struct {
                x264_clock_t total;
                x264_clock_t pred;
                x264_clock_t rd;
                x264_clock_t rd_refine;
            } i;
            x264_clock_t p;
            x264_clock_t b;
        } mb_analyse;
        x264_clock_t mb_encode;
    } encoder_encode;
    x264_clock_t encoder_close;
} x264_time_t;

void timer_start(x264_clock_t* clock);
void timer_end(x264_clock_t* clock);
void log_time(x264_time_t* time);

#endif