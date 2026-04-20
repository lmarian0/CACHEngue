#include <assert.h>
#include <stdio.h>

extern long gini_plus_one(double value);

int main(void) {
    assert(gini_plus_one(53.2) == 54);
    assert(gini_plus_one(0.0) == 1);
    assert(gini_plus_one(-1.2) == 0);
    assert(gini_plus_one(29.9) == 30);

    puts("gini_plus_one OK");
    return 0;
}
