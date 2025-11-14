/*
 * Virtio GPU Adaptive Fence Poll Timer
 *
 * Author:
 *     Yibo Mu <muyibo@loongson.cn>
 */

#include "qemu/osdep.h"
#include "qemu/timer.h"

#include "hw/virtio/virtio_gpu_poll_adaptive.h"

void adaptive_poll_init(AdaptivePoll *ap, int min_ms, int max_ms)
{
    ap->last_poll_time = 0;
    ap->avg_render_time = 10;
    ap->current_interval = 10;
    ap->min_interval = min_ms;
    ap->max_interval = max_ms;
    ap->smoothing = 0.5;
}

// TODO: different algorithm Kalman Filter

void adaptive_poll_update(AdaptivePoll *ap, uint64_t render_time)
{
    ap->avg_render_time = (uint64_t)(ap->smoothing * render_time + (1.0 - ap->smoothing) * ap->avg_render_time);
    if (ap->avg_render_time < 3) ap->current_interval = ap->min_interval;
    else if (ap->avg_render_time > 30) ap->current_interval = ap->max_interval;
    else ap->current_interval = (int)ap->avg_render_time;

    if (ap->current_interval < ap->min_interval) ap->current_interval = ap->min_interval;
    if (ap->current_interval > ap->max_interval) ap->current_interval = ap->max_interval;

    ap->last_poll_time = qemu_clock_get_ms(QEMU_CLOCK_VIRTUAL);
}

int adaptive_poll_next_interval(AdaptivePoll *ap)
{
    return ap->current_interval;
}