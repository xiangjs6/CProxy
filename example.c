#include <stdio.h>
#include <stdbool.h>

#include "map.h"
#include "proxy.h"

// declare
PROXY_DECLARE(cal, PROXY_FUNC_DECLARE(int, sub, int, int),
              PROXY_FUNC_DECLARE(int, add, int, int),
              PROXY_FUNC_DECLARE(int, mul, int, int),
              PROXY_FUNC_DECLARE(void, print))

// define
struct test {
    int ret;
};

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

const struct test_class {
    int (*add)(struct test *, int, int);
    int (*sub)(struct test *, int, int);
    int (*mul)(struct test *, int, int);
    void (*print)(struct test *);
} class = {
    .add = &test_add,
    .sub = &test_sub,
    .mul = &test_mul,
    .print = &test_print,
};
#define PROXY_NAME test_class

PROXY_FUNC_DISPATCH(int, sub, (int, a), (int, b))
PROXY_FUNC_DISPATCH(int, mul, (int, a), (int, b))
PROXY_FUNC_DISPATCH(int, add, (int, a), (int, b))
PROXY_FUNC_DISPATCH(void, print)
PROXY_DEFINE(test_proxy, cal, &class, sub, mul, add, print)

#undef PROXY_NAME

void func(struct cal *cal)
{
    printf("%d\n", cal->api->mul(&cal->proxy, 1, 2));
    printf("%d\n", cal->api->add(&cal->proxy, 1, 2));
    printf("%d\n", cal->api->sub(&cal->proxy, 9, 2));
    cal->api->print(&cal->proxy);
}

int main(void)
{
    struct test t = {.ret = 0};
    func(PROXY_INSTANTIATE(cal, test_proxy, &t));
}
