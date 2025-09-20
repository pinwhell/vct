
# VCT – C Dynamic Vector Library

A lightweight, allocator-aware, type-generic dynamic array implementation for C.

## Features

-   Generic `vct_push_any`, `vct_pop_any`, `vct_get_any_at`
    
-   Type-safe wrappers for all primitive C types (`int`, `float`, `double`, etc.)
    
-   Custom or default allocators
    
-   Simple iteration with `vct_for_each`
    
-   Minimal dependencies (optional freestanding mode)
    

## Quick Start


```c
#include <vct.h>

int main() {
    vct* v = vct_alloc(sizeof(int), 0);
    vct_push_int(v, 42);
    int x;
    vct_get_int_at(v, 0, &x); // x == 42
    vct_free(v);
}
```
## Push, Pop, Get, Iterate
```c
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

    // Popped: 5

    // Iterate and print
    printf("Remaining elements: ");
    vct_for_each(v, print_int, NULL);
    printf("\n");

    // Remaining elements: 1 2 3 4

    vct_free(v);
    return 0;
}
```
## Float Example
```c
vct* v = vct_alloc(sizeof(float), 0);
vct_push_float(v, 3.14f);
vct_push_float(v, 2.718f);

float f;
vct_pop_float(v, &f); // f == 2.718

vct_free(v);
```
## Generic Any-Type Example
```c
// Example struct
typedef struct {
    int id;
    float value;
} Item;

int main() {
    vct* v = vct_alloc(sizeof(Item), 0);

    // Push a struct
    Item it = { 1, 3.14f };
    vct_push_any(v, &it, sizeof(it));

    // Get the struct back
    Item out;
    vct_get_any_at(v, 0, &out, sizeof(out));

    printf("Item id: %d, value: %.2f\n", out.id, out.value);

    vct_free(v);
    return 0;
}
```
## API Highlights

-   **Allocation**: `vct_alloc(stride, capacity)`, `vct_free`
    
-   **Push/Pop/Get**: `vct_push_T`, `vct_pop_T`, `vct_get_T_at`
    
-   **Iteration**: `vct_for_each(v, cb, user_data)`
    
-   **Custom allocators**: `vct_init_allocators`
## Return Codes

-   `VCT_SUCCESS`
    
-   `VCT_INVALID_PUSH / POP / IDX`
    
-   `VCT_FAILALLOC`
    

## License

MIT
