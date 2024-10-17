#include <stdio.h>

#include "map.h"
#include "proxy.h"

// declare
PROXY_DECLARE(cal, PROXY_FUNC_DECLARE(int, sub, int, int),
              PROXY_FUNC_DECLARE(int, add, int, int),
              PROXY_FUNC_DECLARE(int, mul, int, int))

// define
struct test {
    const struct test_class {
        int (*add)(struct test *, int, int);
        int (*sub)(struct test *, int, int);
        int (*mul)(struct test *, int, int);
    } *api;
    int ret;
};

#define PROXY_NAME test

PROXY_FUNC_DISPATCH(.api->, int, sub, (int, a), (int, b))
PROXY_FUNC_DISPATCH(.api->, int, mul, (int, a), (int, b))
PROXY_FUNC_DISPATCH(.api->, int, add, (int, a), (int, b))
PROXY_DEFINE(cal, sub, mul, add)

#undef PROXY_NAME

int test_add(struct test *self, int a, int b)
{
    printf("ret: %d\n", self->ret);
    return (self->ret = a + b);
}

int test_mul(struct test *self, int a, int b)
{
    printf("ret: %d\n", self->ret);
    return (self->ret = a * b);
}
int test_sub(struct test *self, int a, int b)
{
    printf("ret: %d\n", self->ret);
    return (self->ret = a - b);
}
const static struct test_class class = {
    .add = &test_add,
    .sub = &test_sub,
    .mul = &test_mul,
};

void func(struct cal *cal)
{
    printf("%d\n", cal->api->mul(cal->self, 1, 2));
    printf("%d\n", cal->api->add(cal->self, 1, 2));
    printf("%d\n", cal->api->sub(cal->self, 1, 2));
}

int main(void)
{
    struct test t = {.api = &class, .ret = 0};
    func(PROXY_INSTANTIATE(cal, test, &t));
}
