#include <stdio.h>
#include <stdint.h>
#include <omp.h>

typedef int32_t int_ __attribute__((aligned(64)));
int_ x, y, r1, r2;

static void __attribute__((noinline)) f(const int id)
{
    if (0 == id % 2) {
        x = 1;
        /*
         * here is the store-load barrier: force a load targeting
         * memory modified by the previous store, but prevent the
         * CPU to forward the store by asking for a bigger load.
         * It would also work with an unaligned access, eg.
         * (void)*(volatile int32_t *)((char *)&x + 2);
         */
        (void)*(volatile int64_t *)&x;
        asm volatile ("":::"memory");
        r1 = y;
    } else {
        y = 1;
        (void)*(volatile int64_t *)&y;
        asm volatile ("":::"memory");
        r2 = x;
    }
}

int main(void)
{
    int err = 0;
#pragma omp parallel num_threads(2)
    {
        const int id = omp_get_thread_num();
        for (int i=1;;i++) {
#pragma omp barrier
            f(id);
#pragma omp barrier
#pragma omp single
            {
                if (r1 == 0 && r2 == 0) {
                    err++;
                    printf("%i errors in %i iterations\n", err, i);
                }
                x = y = 0;
            }
        }
    }
    return 0;
}
