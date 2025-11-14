/*
 * Virtio GPU Adaptive Fence Poll Timer
 *
 * Author:
 *     Yibo Mu <muyibo@loongson.cn>
 */

#ifndef HW_VIRTIO_GPU_POLL_ADAPTIVE_H
#define HW_VIRTIO_GPU_POLL_ADAPTIVE_H

typedef struct AdaptivePoll {
    uint64_t last_poll_time;
    uint64_t avg_render_time;
    int current_interval;
    int min_interval;
    int max_interval;
    double smoothing;
} AdaptivePoll;


void adaptive_poll_init(AdaptivePoll *ap, int min_ms, int max_ms);
void adaptive_poll_update(AdaptivePoll *ap, uint64_t render_time);
int adaptive_poll_next_interval(AdaptivePoll *ap);

#endif