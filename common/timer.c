#include "osdep.h"
#include "timer.h"
#include "log.h"

void timer_start(x264_clock_t* clk)
{
    clk->base = x264_mdate();
}

void timer_end(x264_clock_t* clk)
{
    int64_t now = x264_mdate();
    clk->duration += (now - clk->base) / 1000.f;
}

void log_time(x264_time_t* time)
{
    float total_time = time->encoder_open.duration + time->encoder_encode.total.duration + time->encoder_close.duration;
    log_info("Total time : %.2f ms", total_time);
    log_info("Encoder open time: %.2f ms", time->encoder_open.duration);
    log_info("Encoder encode time: %.2f ms", time->encoder_encode.total.duration);
    log_info("Encoder close time: %.2f ms", time->encoder_close.duration);
}