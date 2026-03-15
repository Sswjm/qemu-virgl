/*
 * Virtio GPU Adaptive Fence Poll Timer
 *
 * Author:
 *     Yibo Mu <muyibo@loongson.cn>
 */

#ifndef HW_VIRTIO_GPU_POLL_ADAPTIVE_H
#define HW_VIRTIO_GPU_POLL_ADAPTIVE_H

#include "qemu/osdep.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct VGPUDynPollKalman {
    /* Kalman (scalar) */
    double x;      /* estimated time scale (ms) */
    double P;      /* covariance */
    double Q;      /* process noise var */
    double R;      /* measurement noise var */

    /* Patent-like adaptive timer */
    int interval_ms;
    int min_ms;
    int max_ms;
    int dec_ms;    /* Δ_down */
    int inc_ms;    /* Δ_up base */

    /* sliding window success rate (W=10) */
    uint32_t win_bits; /* last W results as bits */
    int win_size;      /* W */
    int win_count;     /* <= W */
} VGPUDynPollKalman;

void vgpu_dynpoll_kalman_init(VGPUDynPollKalman *s,
                              int init_ms, int min_ms, int max_ms,
                              int dec_ms, int inc_ms,
                              double init_x, double init_P,
                              double Q, double R,
                              int win_size /* e.g. 10 */);

void vgpu_dynpoll_kalman_update(VGPUDynPollKalman *s,
                                double observed_ms,
                                bool success,
                                int cmdq_len,
                                int fenceq_len);

int vgpu_dynpoll_kalman_get_interval(const VGPUDynPollKalman *s);
double vgpu_dynpoll_kalman_get_est(const VGPUDynPollKalman *s);
double vgpu_dynpoll_kalman_get_success_rate(const VGPUDynPollKalman *s);

// typedef struct AdaptivePoll {
//     uint64_t last_poll_time;
//     uint64_t avg_render_time;
//     int current_interval;
//     int min_interval;
//     int max_interval;
//     double smoothing;
// } AdaptivePoll;


// void adaptive_poll_init(AdaptivePoll *ap, int min_ms, int max_ms);
// void adaptive_poll_update(AdaptivePoll *ap, uint64_t render_time);
// int adaptive_poll_next_interval(AdaptivePoll *ap);

#endif