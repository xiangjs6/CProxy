// declare
#define ___PROXY_FUNC_DECLARE(rettype, name, ...)                              \
    rettype (*name)(void *self, __VA_ARGS__);
#define __PROXY_FUNC_DECLARE(X) ___PROXY_FUNC_DECLARE X
#define PROXY_FUNC_DECLARE(rettype, name, ...) (rettype, name, __VA_ARGS__)

#define PROXY_DECLARE(proxy_name, ...)                                         \
    struct proxy_name {                                                        \
        const struct {                                                         \
            MAP(__PROXY_FUNC_DECLARE, __VA_ARGS__)                             \
        } *api;                                                                \
        void *self;                                                            \
    };

// dispatch

#ifndef container_of
#define container_of(ptr, type, member)                                        \
    ((type *)((char *)(1 ? (ptr) : &((type *)0)->member) -                     \
              offsetof(type, member)))
#endif

#define __PROXY_FUNC_NAME(func) PROXY_NAME(func)

#define __PROXY_GET_FUNC_NAME(func) .func = &__PROXY_FUNC_NAME(__proxy##func)

#define PROXY_DEFINE(proxy_name, ...)                                          \
    typeof(*((struct proxy_name *)NULL)->api) PROXY_NAME(                      \
        proxy_name##_proxy_) = {MAP_LIST(__PROXY_GET_FUNC_NAME, __VA_ARGS__)};

#define ___PROXY_FUNC_HEAD(rettype, func, ...)                                 \
    static rettype __PROXY_FUNC_NAME(__proxy##func)(                           \
        void *p, __PROXY_EXPAND __VA_ARGS__)

#define ___PROXY_FUNC_BODY(member, rettype, func, ...)                         \
    {                                                                          \
        return (*(struct PROXY_NAME() *)p)member func(                         \
            p, __PROXY_EXPAND __VA_ARGS__);                                    \
    }

#define __PROXY_EXPAND(...) __VA_ARGS__

#define ___PROXY_ARG_DEFINE(X, Y) X Y
#define __PROXY_ARG_DEFINE(X) ___PROXY_ARG_DEFINE X

#define ___PROXY_FETCH_ARG_NAME(X, Y) Y
#define __PROXY_FETCH_ARG_NAME(X) ___PROXY_FETCH_ARG_NAME X

#define PROXY_FUNC_DISPATCH(member, rettype, func, ...)                        \
    ___PROXY_FUNC_HEAD(rettype, func,                                          \
                       (MAP_LIST(__PROXY_ARG_DEFINE, __VA_ARGS__)))            \
    ___PROXY_FUNC_BODY(member, rettype, func,                                  \
                       (MAP_LIST(__PROXY_FETCH_ARG_NAME, __VA_ARGS__)))
