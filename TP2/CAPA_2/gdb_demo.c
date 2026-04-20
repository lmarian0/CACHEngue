#include <stdio.h>

extern int float_to_int(double value);
extern int gini_plus_one(double value);

static void stack_demo(double value)
{
    volatile int left = 11;
    volatile int right = 22;
    volatile int frame_data[4] = {1, 2, 3, 4};

    int truncated = float_to_int(value);
    int incremented = gini_plus_one(value);

    printf("value=%.2f truncated=%d incremented=%d locals=%d,%d,%d\n",
           value,
           truncated,
           incremented,
           left,
           right,
           frame_data[0]);
}

int main(void)
{
    double input = 7.75;

    puts("before stack_demo");
    stack_demo(input);
    puts("after stack_demo");

    return 0;
}