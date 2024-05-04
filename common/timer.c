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
    log_info("  Adaptive quant time: %.2f ms", time->encoder_encode.aq.duration);
    log_info("  Lowres init time: %.2f ms", time->encoder_encode.lower_res_init.duration);
    log_info("  Lookahead time: %.2f ms", time->encoder_encode.lookahead.duration);
    log_info("  Ratecontrol time: %.2f ms", time->encoder_encode.ratecontrol.duration);
    log_info("  Mb analysis time: %.2f ms", time->encoder_encode.mb_analyse.total.duration);
    log_info("    I frame analysis time: %.2f ms", time->encoder_encode.mb_analyse.i.total.duration);
    log_info("      Prediction time: %.2f ms", time->encoder_encode.mb_analyse.i.pred.duration);
    log_info("      Rd time: %.2f ms", time->encoder_encode.mb_analyse.i.rd.duration);
    log_info("      Rd refine time: %.2f ms", time->encoder_encode.mb_analyse.i.rd_refine.duration);
    log_info("    P frame analysis time: %.2f ms", time->encoder_encode.mb_analyse.p.duration);
    log_info("    B frame analysis time: %.2f ms", time->encoder_encode.mb_analyse.b.duration);
    log_info("  Mb encode time: %.2f ms", time->encoder_encode.mb_encode.duration);
    log_info("Encoder close time: %.2f ms", time->encoder_close.duration);
}