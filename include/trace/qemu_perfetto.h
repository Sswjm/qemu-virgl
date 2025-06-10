#ifndef QEMU_PERFETTO_H
#define QEMU_PERFETTO_H

// #ifdef CONFIG_PERFETTO
void qemu_perfetto_trace_init(void);

#define TRACE_INIT() qemu_perfetto_trace_init()
#define TRACE_FUNC() TRACE_SCOPE(__func__)

void *qemu_perfetto_trace_begin(const char *scope);
void qemu_perfetto_trace_end(void **scope);

#define TRACE_SCOPE(SCOPE) \
    void *trace_dummy __attribute__((cleanup (qemu_perfetto_trace_end), unused)) = \
    qemu_perfetto_trace_begin(SCOPE)  

#define TRACE_SCOPE_SLOW(SCOPE) TRACE_SCOPE(SCOPE)

#define TRACE_SCOPE_BEGIN(SCOPE) qemu_perfetto_trace_begin(SCOPE)
#define TRACE_SCOPE_END(SCOPE_OBJ)  qemu_perfetto_trace_end(&SCOPE_OBJ)  

/*#else 
#define TRACE_INIT()
#define TRACE_FUNC()
#define TRACE_SCOPE(SCOPE)
#define TRACE_SCOPE_SLOW(SCOPE)
#define TRACE_SCOPE_BEGIN(SCOPE) NULL
#define TRACE_SCOPE_END(SCOPE_OBJ) (void)SCOPE_OBJ
#endif*/ 

#endif
