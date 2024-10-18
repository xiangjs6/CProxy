#define COMPARE_void(x) x
#define __PROXY_RETURN_1 return
#define __PROXY_RETURN_0

// declare
#define ___PROXY_FUNC_DECLARE(rettype, name, ...)                              \
    rettype (*name)(struct __proxy * __VA_OPT__(, ) __VA_ARGS__);
#define __PROXY_FUNC_DECLARE(X) ___PROXY_FUNC_DECLARE X
#define PROXY_FUNC_DECLARE(rettype, name, ...) (rettype, name, __VA_ARGS__)

struct __proxy {
    void *self;
    const void *class;
};

#define PROXY_DECLARE(proxy_name, ...)                                         \
    struct proxy_name {                                                        \
        struct __proxy proxy;                                                  \
        const struct {                                                         \
            MAP(__PROXY_FUNC_DECLARE, __VA_ARGS__)                             \
        } *api;                                                                \
    };

// dispatch

#define __PROXY_RETURN_void

#define ___PROXY_COMBINE(X, Y) X##Y
#define __PROXY_COMBINE(X, Y) ___PROXY_COMBINE(X, Y)

#define __PROXY_FUNC_NAME(func) __PROXY_COMBINE(func, PROXY_NAME)

#define __PROXY_GET_FUNC_NAME(func) .func = &__PROXY_FUNC_NAME(__proxy##func)

#define PROXY_DEFINE(proxy_name, proxy_declare_name, classname, ...)           \
    const static struct {                                                      \
        const typeof(*((struct proxy_declare_name *)NULL)->api) api;           \
        const void *class;                                                     \
    } proxy_name = {.api = {MAP_LIST(__PROXY_GET_FUNC_NAME, __VA_ARGS__)},     \
                    .class = classname};

#define ___PROXY_FUNC_HEAD(rettype, func, ...)                                 \
    static rettype __PROXY_FUNC_NAME(__proxy##func)(                           \
        struct __proxy * p __VA_OPT__(, __PROXY_EXPAND) __VA_ARGS__)

#define ___PROXY_FUNC_BODY(rettype, func, ...)                                 \
    {                                                                          \
        __PROXY_COMBINE(__PROXY_RETURN_, NOT_EQUAL(void, rettype))             \
        ((struct PROXY_NAME *)(p->class))                                      \
            ->func(p->self __VA_OPT__(, __PROXY_EXPAND) __VA_ARGS__);          \
    }

#define __PROXY_EXPAND(...) __VA_ARGS__

#define ___PROXY_ARG_DEFINE(X, Y) X Y
#define __PROXY_ARG_DEFINE(X) ___PROXY_ARG_DEFINE X

#define ___PROXY_FETCH_ARG_NAME(X, Y) Y
#define __PROXY_FETCH_ARG_NAME(X) ___PROXY_FETCH_ARG_NAME X

#define PROXY_FUNC_DISPATCH(rettype, func, ...)                                \
    ___PROXY_FUNC_HEAD(                                                        \
        rettype,                                                               \
        func __VA_OPT__(, (MAP_LIST(__PROXY_ARG_DEFINE, __VA_ARGS__))))        \
    ___PROXY_FUNC_BODY(                                                        \
        rettype,                                                               \
        func __VA_OPT__(, (MAP_LIST(__PROXY_FETCH_ARG_NAME, __VA_ARGS__))))

// instantiate
#define PROXY_INSTANTIATE(proxy_declare_name, proxy_dispatch, instance)        \
    &(struct proxy_declare_name)                                               \
    {                                                                          \
        .proxy.self = instance, .proxy.class = proxy_dispatch.class,           \
        .api = &proxy_dispatch.api                                             \
    }
