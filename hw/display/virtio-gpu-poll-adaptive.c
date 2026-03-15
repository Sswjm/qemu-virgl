/*
 * Virtio GPU Adaptive Fence Poll Timer
 *
 * Author:
 *     Yibo Mu <muyibo@loongson.cn>
 */


#include "qemu/osdep.h"
#include "hw/virtio/virtio_gpu_dynpoll_kalman.h"
#include <math.h>

static inline int clamp_i(int v, int lo, int hi)
{
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

static void win_push(VGPUDynPollKalman *s, bool success)
{
    int W = s->win_size;
    if (W <= 0 || W > 31) return;

    s->win_bits = ((s->win_bits << 1) | (success ? 1u : 0u)) & ((1u << W) - 1u);
    if (s->win_count < W) s->win_count++;
}

static double win_rate(const VGPUDynPollKalman *s)
{
    if (s->win_count <= 0) return 1.0;
    uint32_t b = s->win_bits;
    int ones = 0;
    for (int i = 0; i < s->win_count; i++) {
        ones += (b >> i) & 1u;
    }
    return (double)ones / (double)s->win_count;
}

void vgpu_dynpoll_kalman_init(VGPUDynPollKalman *s,
                              int init_ms, int min_ms, int max_ms,
                              int dec_ms, int inc_ms,
                              double init_x, double init_P,
                              double Q, double R,
                              int win_size)
{
    s->x = init_x;
    s->P = init_P;
    s->Q = Q;
    s->R = R;

    s->interval_ms = clamp_i(init_ms, min_ms, max_ms);
    s->min_ms = min_ms;
    s->max_ms = max_ms;
    s->dec_ms = dec_ms;
    s->inc_ms = inc_ms;

    s->win_bits = 0;
    s->win_size = win_size;
    s->win_count = 0;
}

static void kalman_update(VGPUDynPollKalman *s, double z)
{
    /* predict: x_pred = x, P_pred = P + Q */
    double P_pred = s->P + s->Q;
    double K = P_pred / (P_pred + s->R);
    s->x = s->x + K * (z - s->x);
    s->P = (1.0 - K) * P_pred;
}

void vgpu_dynpoll_kalman_update(VGPUDynPollKalman *s,
                                double observed_ms,
                                bool success,
                                int cmdq_len,
                                int fenceq_len)
{
    /* 1) update Kalman estimate */
    if (observed_ms < 0) observed_ms = 0;
    kalman_update(s, observed_ms);

    /* 2) push success to window */
    win_push(s, success);
    double p = win_rate(s);

    /* 3) Patent-like recursion with “pressure” term */
    int pressure = cmdq_len + fenceq_len;
    double rho = 0.0;
    /* rho grows when estimate large or pressure high */
    rho += fmin(1.0, s->x / 20.0);       /* normalize by 20ms */
    rho += fmin(1.0, pressure / 32.0);   /* normalize by 32 items */
    rho = fmin(2.0, rho);                /* cap */

    int t = s->interval_ms;

    if (success) {
        /* success: shorten interval a bit */
        t = t - s->dec_ms;
    } else {
        /* timeout: enlarge interval with rho */
        int inc = (int)lrint((double)s->inc_ms * (1.0 + rho));
        t = t + inc;
    }

    /* 4) completion-rate multiplicative adjust (patent spirit) */
    if (s->win_count >= s->win_size) {
        if (p <= 0.20) {
            t = (int)lrint(t * 1.10); /* frequently timeout -> relax */
        } else if (p >= 0.80) {
            t = (int)lrint(t * 0.90); /* mostly success -> tighten */
        }
    }

    s->interval_ms = clamp_i(t, s->min_ms, s->max_ms);
}

int vgpu_dynpoll_kalman_get_interval(const VGPUDynPollKalman *s)
{
    return s->interval_ms;
}

double vgpu_dynpoll_kalman_get_est(const VGPUDynPollKalman *s)
{
    return s->x;
}

double vgpu_dynpoll_kalman_get_success_rate(const VGPUDynPollKalman *s)
{
    return win_rate(s);
}

// #include "qemu/osdep.h"
// #include "qemu/timer.h"

// #include "hw/virtio/virtio_gpu_poll_adaptive.h"

// void adaptive_poll_init(AdaptivePoll *ap, int min_ms, int max_ms)
// {
//     ap->last_poll_time = 0;
//     ap->avg_render_time = 10;
//     ap->current_interval = 10;
//     ap->min_interval = min_ms;
//     ap->max_interval = max_ms;
//     ap->smoothing = 0.5;
// }

// // TODO: different algorithm Kalman Filter

// void adaptive_poll_update(AdaptivePoll *ap, uint64_t render_time)
// {
//     ap->avg_render_time = (uint64_t)(ap->smoothing * render_time + (1.0 - ap->smoothing) * ap->avg_render_time);
//     if (ap->avg_render_time < 3) ap->current_interval = ap->min_interval;
//     else if (ap->avg_render_time > 30) ap->current_interval = ap->max_interval;
//     else ap->current_interval = (int)ap->avg_render_time;

//     if (ap->current_interval < ap->min_interval) ap->current_interval = ap->min_interval;
//     if (ap->current_interval > ap->max_interval) ap->current_interval = ap->max_interval;

//     ap->last_poll_time = qemu_clock_get_ms(QEMU_CLOCK_VIRTUAL);
// }

// int adaptive_poll_next_interval(AdaptivePoll *ap)
// {
//     return ap->current_interval;
// }