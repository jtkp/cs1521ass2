
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

int main(void) {
    int i = 647;
    i = i << 16;

    int j = i | 22450;
    printf("%d\n", j);

    j = 0 | 10;

    printf("%c\n", j);

}