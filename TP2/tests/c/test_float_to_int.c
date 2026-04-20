#include <assert.h>
#include <stdio.h>

extern long float_to_int(double value);

int main(void) {
    assert(float_to_int(53.2) == 53);
    assert(float_to_int(0.0) == 0);
    assert(float_to_int(-1.2) == -1);
    assert(float_to_int(29.9) == 29);

    puts("float_to_int OK");
    return 0;
}
