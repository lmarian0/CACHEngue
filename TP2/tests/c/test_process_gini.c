#include <assert.h>
#include <stdio.h>

extern int process_gini(double value);

int main(void) {
    assert(process_gini(53.2) == 54);
    assert(process_gini(0.0) == 1);
    assert(process_gini(29.9) == 30);
    assert(process_gini(-1.2) == 0);

    puts("process_gini OK");
    return 0;
}
