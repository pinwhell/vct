#include <stdio.h>
#include <vct.h>

// Callback for vct_for_each
vct_bool print_int(void* itm, void* data) {
    (void)data; // unused
    printf("%d ", *(int*)itm);
    return 1;  // continue iteration
}

int main() {
    vct* v = vct_alloc(sizeof(int), 0);

    // Push integers
    for (int i = 1; i <= 5; i++)
        vct_push_int(v, i);

    // Pop last element
    int val;
    vct_pop_int(v, &val);
    printf("Popped: %d\n", val);

    // Iterate and print
    printf("Remaining elements: ");
    vct_for_each(v, print_int, NULL);
    printf("\n");

    vct_free(v);
    return 0;
}