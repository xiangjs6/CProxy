#include <stdio.h>

#include "map.h"
#include "proxy.h"

// declare
PROXY_DECLARE(cal, PROXY_FUNC_DECLARE(int, sub, int, int),
              PROXY_FUNC_DECLARE(int, add, int, int),
              PROXY_FUNC_DECLARE(int, mul, int, int),
              PROXY_FUNC_DECLARE(void, print))

// define
struct test {
    const struct test_class {
        int (*add)(struct test *, int, int);
        int (*sub)(struct test *, int, int);
        int (*mul)(struct test *, int, int);
        void (*print)(struct test *);
    } *api;
    int ret;
};

#define PROXY_NAME test

PROXY_FUNC_DISPATCH(.api->, int, sub, (int, a), (int, b))
PROXY_FUNC_DISPATCH(.api->, int, mul, (int, a), (int, b))
PROXY_FUNC_DISPATCH(.api->, int, add, (int, a), (int, b))
PROXY_FUNC_DISPATCH(.api->, void, print)
PROXY_DEFINE(cal, sub, mul, add, print)

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
void test_print(struct test *self) { printf("ret: %d\n", self->ret); }
const static struct test_class class = {
    .add = &test_add,
    .sub = &test_sub,
    .mul = &test_mul,
    .print = &test_print,
};

void func(struct cal *cal)
{
    printf("%d\n", cal->api->mul(cal->self, 1, 2));
    printf("%d\n", cal->api->add(cal->self, 1, 2));
    printf("%d\n", cal->api->sub(cal->self, 9, 2));
    cal->api->print(cal->self);
}

int main(void)
{
    struct test t = {.api = &class, .ret = 0};
    func(PROXY_INSTANTIATE(cal, test, &t));
}
