#include <stdio.h>
#include <vct.h>

// Callback for vct_for_each
bool print_int(void* itm, void* data) {
    (void)data; // unused
    printf("%d ", *(int*)itm);
    return 1;  // continue iteration
}

int main() {
    vct v; if (VCT_INIT(&v, int, 0u)) return 1u;

    // Push integers
    for (int i = 1; i <= 5; i++)
        vct_push_int(&v, i);

    // Pop last element
    int val;
    vct_pop_int(&v, &val);
    printf("Popped: %d\n", val);

    // Iterate and print
    printf("Remaining elements: ");
    vct_for_each(&v, print_int, NULL);
    printf("\n");

    vct_deinit(&v);
    return 0;
}