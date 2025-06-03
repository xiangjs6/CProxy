#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#include "map.h"
#include "proxy.h"

// declare
PROXY_DECLARE(cal, PROXY_FUNC_DECLARE(int, sub, int, int),
              PROXY_FUNC_DECLARE(int, add, int, int),
              PROXY_FUNC_DECLARE(int, mul, int, int),
              PROXY_FUNC_DECLARE(void, print))

// define
struct test {
    int pad;
    const struct test_class *class;
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

static const struct test_class *class_fn() { return &class; }

PROXY_FUNC_DISPATCH(test_class, typeof(&class), int, sub, (int, a), (int, b))
PROXY_FUNC_DISPATCH(test_class, typeof(class) *, int, mul, (int, a), (int, b))
PROXY_FUNC_DISPATCH(test_class, struct test_class *, int, add, (int, a),
                    (int, b))
PROXY_FUNC_DISPATCH(test_class, struct test_class *, void, print)
PROXY_DEFINE(test_class, test_proxy1, cal, &class, sub, mul, add, print)

PROXY_DEFINE(test_class, test_proxy2, cal, offsetof(struct test, class), sub,
             mul, add, print)
PROXY_DEFINE(test_class, test_proxy3, cal, class_fn, sub, mul, add, print)

void func(struct cal *cal)
{
    printf("%p\n", cal->proxy.class);
    printf("%d\n", cal->api->mul(&cal->proxy, 1, 2));
    printf("%d\n", cal->api->add(&cal->proxy, 1, 2));
    printf("%d\n", cal->api->sub(&cal->proxy, 9, 2));
    cal->api->print(&cal->proxy);
}

int main(void)
{
    struct test t = {.class = &class, .ret = 0};
    func(PROXY_INSTANTIATE(cal, test_proxy1, &t));
    func(PROXY_INSTANTIATE(cal, test_proxy2, &t));
    func(PROXY_INSTANTIATE(cal, test_proxy3, &t));
}
