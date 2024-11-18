#define COMPARE_void(x) x
#define __PROXY_RETURN_1 return
#define __PROXY_RETURN_0

// declare
#define ___PROXY_FUNC_DECLARE(rettype, name, ...)                              \
    rettype (*name)(struct __proxy * __VA_OPT__(, ) __VA_ARGS__);

#define __PROXY_FUNC_DECLARE(X) ___PROXY_FUNC_DECLARE X
#define PROXY_FUNC_DECLARE(rettype, name, ...) (rettype, name, __VA_ARGS__)

struct __proxy {
    const void *class;
    union {
        void *self;
        uintptr_t class_offset;
    };
};

#define PROXY_DECLARE(name, ...)                                               \
    struct name {                                                              \
        struct __proxy proxy;                                                  \
        const struct {                                                         \
            MAP(__PROXY_FUNC_DECLARE, __VA_ARGS__)                             \
        } *api;                                                                \
    };

// dispatch

#define __PROXY_RETURN_void

#define ___PROXY_COMBINE(X, Y) X##Y
#define __PROXY_COMBINE(X, Y) ___PROXY_COMBINE(X, Y)

#define __PROXY_FUNC_NAME(func, class_name) __PROXY_COMBINE(func, class_name)

#define ___PROXY_FUNC_HEAD(class_name, rettype, func, ...)                     \
    static rettype __PROXY_FUNC_NAME(__proxy##func, class_name)(               \
        struct __proxy * p __VA_OPT__(, __PROXY_EXPAND) __VA_ARGS__)

#define ___PROXY_FUNC_BODY(class_type, rettype, func, ...)                     \
    {                                                                          \
        __PROXY_COMBINE(__PROXY_RETURN_, NOT_EQUAL(void, rettype))             \
        ((class_type)(p->class))                                               \
            ->func(p->self __VA_OPT__(, __PROXY_EXPAND) __VA_ARGS__);          \
    }

#define __PROXY_EXPAND(...) __VA_ARGS__

#define ___PROXY_ARG_DEFINE(X, Y) X Y
#define __PROXY_ARG_DEFINE(X) ___PROXY_ARG_DEFINE X

#define ___PROXY_FETCH_ARG_NAME(X, Y) Y
#define __PROXY_FETCH_ARG_NAME(X) ___PROXY_FETCH_ARG_NAME X

#define PROXY_FUNC_DISPATCH(class_name, class_type, rettype, func, ...)        \
    ___PROXY_FUNC_HEAD(                                                        \
        class_name, rettype,                                                   \
        func __VA_OPT__(, (MAP_LIST(__PROXY_ARG_DEFINE, __VA_ARGS__))))        \
    ___PROXY_FUNC_BODY(                                                        \
        class_type, rettype,                                                   \
        func __VA_OPT__(, (MAP_LIST(__PROXY_FETCH_ARG_NAME, __VA_ARGS__))))

// define
#define __PROXY_GET_FUNC_NAME(func, class_name)                                \
    .func = &__PROXY_FUNC_NAME(__proxy##func, class_name)

#define PROXY_DEFINE(class_name, dispatch_name, proxy_declare_name, _class,    \
                     ...)                                                      \
    static const struct proxy_declare_name dispatch_name = {                   \
        .api = &((const typeof(*((struct proxy_declare_name *)NULL)->api)){    \
            MAP_LIST_UD(__PROXY_GET_FUNC_NAME, class_name, __VA_ARGS__)}),     \
        .proxy.class = _Generic(_class,                                        \
        uintptr_t: NULL,                                                       \
        default: ((typeof(_class)[]){_class})),                                \
        .proxy.class_offset =                                                  \
            _Generic(_class, uintptr_t: (uintptr_t)_class, default: 0)};

// instantiate
#define PROXY_INSTANTIATE(proxy_declare_name, proxy_dispatch, instance)        \
    (&(struct proxy_declare_name){                                             \
        .proxy.self = instance,                                                \
        .proxy.class =                                                         \
            (proxy_dispatch).proxy.class ? *((void **)(proxy_dispatch)         \
                                                 .proxy.class)                 \
                                         : *(void **)(((char *)instance) +     \
                                                      (proxy_dispatch)         \
                                                          .proxy               \
                                                          .class_offset),      \
        .api = (proxy_dispatch).api,                                           \
    })
