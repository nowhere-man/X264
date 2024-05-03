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
        x264_clock_t lookahead;
        x264_clock_t ratecontrol;
        x264_clock_t mb_analyse;
        x264_clock_t mb_encode;
    } encoder_encode;
    x264_clock_t encoder_close;
} x264_time_t;

void timer_start(x264_clock_t* clock);
void timer_end(x264_clock_t* clock);
void log_time(x264_time_t* time);

#endif