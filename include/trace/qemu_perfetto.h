#ifndef QEMU_PERFETTO_H
#define QEMU_PERFETTO_H

#ifdef CONFIG_PERFETTO
void trace_init(void);

#define TRACE_INIT() trace_init()
#define TRACE_FUNC() TRACE_SCOPE(__func__)

void *trace_begin(const char *scope);
void trace_end(void **scope);

#define TRACE_SCOPE(SCOPE) \
    void *trace_dummy __attribute__((cleanup (trace_end), unused)) = \
    trace_begin(SCOPE)  

#define TRACE_SCOPE_SLOW(SCOPE) TRACE_SCOPE(SCOPE)

#define TRACE_SCOPE_BEGIN(SCOPE) trace_begin(SCOPE)
#define TRACE_SCOPE_END(SCOPE_OBJ)  trace_end(&SCOPE_OBJ)  

#else /* CONFIG_PERFETTO */
#define TRACE_INIT()
#define TRACE_FUNC()
#define TRACE_SCOPE(SCOPE)
#define TRACE_SCOPE_SLOW(SCOPE)
#define TRACE_SCOPE_BEGIN(SCOPE) NULL
#define TRACE_SCOPE_END(SCOPE_OBJ) (void)SCOPE_OBJ
#endif /* CONFIG_PERFETTO */

#endif
