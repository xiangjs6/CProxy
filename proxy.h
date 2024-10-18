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
    size_t api_offset;
    bool is_ptr;
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

#define PROXY_DEFINE(proxy_name, object_type, func_member, ...)                \
    const static struct {                                                      \
        const typeof(*((struct proxy_name *)NULL)->api) api;                   \
        size_t api_offset;                                                     \
        bool is_ptr;                                                           \
    } __PROXY_COMBINE(__PROXY_COMBINE(__, proxy_name##_proxy_),                \
                      PROXY_NAME) = {                                          \
        .api = {MAP_LIST(__PROXY_GET_FUNC_NAME, __VA_ARGS__)},                 \
        .api_offset = offsetof(object_type, func_member),                      \
        .is_ptr = _Generic(((object_type *)NULL)->func_member,                 \
        struct PROXY_NAME: false,                                              \
        struct PROXY_NAME *: true,                                             \
        const struct PROXY_NAME *: true)};

#define ___PROXY_FUNC_HEAD(rettype, func, ...)                                 \
    static rettype __PROXY_FUNC_NAME(__proxy##func)(                           \
        struct __proxy * p __VA_OPT__(, __PROXY_EXPAND) __VA_ARGS__)

#define ___PROXY_FUNC_BODY(rettype, func, ...)                                 \
    {                                                                          \
        if (p->is_ptr) {                                                       \
            __PROXY_COMBINE(__PROXY_RETURN_, NOT_EQUAL(void, rettype))         \
            (*(struct PROXY_NAME **)((char *)p->self + p->api_offset))         \
                ->func(p->self __VA_OPT__(, __PROXY_EXPAND) __VA_ARGS__);      \
        }                                                                      \
        __PROXY_COMBINE(__PROXY_RETURN_, NOT_EQUAL(void, rettype))             \
        ((struct PROXY_NAME *)((char *)p->self + p->api_offset))               \
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
#define PROXY_INSTANTIATE(proxy_declare_name, proxy_dispatch_name, instance)   \
    &(struct proxy_declare_name)                                               \
    {                                                                          \
        .proxy.self = instance,                                                \
        .proxy.api_offset =                                                    \
            __PROXY_COMBINE(__PROXY_COMBINE(__, proxy_declare_name),           \
                            _proxy_##proxy_dispatch_name)                      \
                .api_offset,                                                   \
        .proxy.is_ptr =                                                        \
            __PROXY_COMBINE(__PROXY_COMBINE(__, proxy_declare_name),           \
                            _proxy_##proxy_dispatch_name)                      \
                .is_ptr,                                                       \
        .api = &__PROXY_COMBINE(__PROXY_COMBINE(__, proxy_declare_name),       \
                                _proxy_##proxy_dispatch_name)                  \
                    .api                                                       \
    }
