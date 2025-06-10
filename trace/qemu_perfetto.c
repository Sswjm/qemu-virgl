/*  
 *  QEMU Perfetto Implementation
 */

#include "trace/qemu_perfetto.h"

#include <vperfetto-min.h>
#include <stdbool.h>
#include <stddef.h>

static void on_tracing_state_change(bool enabled)
{
  // TODO: add this function  
}

void qemu_perfetto_trace_init(void)
{
    struct vperfetto_min_config config = {
        .on_tracing_state_change = on_tracing_state_change,
        .init_flags = VPERFETTO_INIT_FLAG_USE_SYSTEM_BACKEND,
        .filename = NULL,
        .shmem_size_hint_kb = 32 * 1024,
    };

    vperfetto_min_startTracing(&config);
}

void *qemu_perfetto_trace_begin(const char *scope)
{
    vperfetto_min_beginTrackEvent_VMM(scope);
    return NULL;
}

void qemu_perfetto_trace_end(void **dummy)
{
    (void)dummy;
    vperfetto_min_endTrackEvent_VMM();
}
