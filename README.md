
# VCT – C Dynamic Vector Library

A lightweight, allocator-aware, type-generic dynamic array implementation for C.

## Features

-   Generic `VCT_OP_GEN(shorthand, T)`, `VCT_OP_REF_GEN(shorthand, T)`,`vct_push_any`, `vct_pop_any`, `vct_get_any_at`
    
-   Type-safe wrappers for all primitive C types (`int`, `float`, `double`, etc.)
    
-   Custom or default allocators
    
-   Simple iteration with `vct_for_each`
    
-   Minimal dependencies (optional freestanding mode)
    

## Quick Start


```c
#include <vct/vct.h>

int main() {
    vct v; VCT_INIT(&v, int, 0);
    vct_push_int(&v, 42);
    int x;
    vct_get_int_at(&v, 0, &x); // x == 42
    vct_deinit(&v);
}
```
## Push, Pop, Get, Iterate
```c
#include <stdio.h>
#include <vct/vct.h>

// Callback for vct_for_each
vct_bool print_int(void* itm, void* data) {
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
```
## Float Example
```c
vct v; if (!VCT_INIT(&v, float, 0u)) return 1u;
vct_push_float(&v, 3.14f);
vct_push_float(&v, 2.718f);

float f;
vct_pop_float(&v, &f); // f == 2.718
vct_deinit(&v);
```
## Generic Any-Type Example
```c
#include <vct/vct.h>
#include <stdio.h>

typedef struct {
    int id;
    float value;
} Item;

VCT_OP_REF_GEN(itm, Item)

int main() {
    vct* v = VCT_ALLOC(Item, 0u, NULL);
    if (!v) return 1u;

    // Push a struct
    Item it = { 1, 3.14f };
    vct_push_itmp(v, &it);

    // Get the struct back
    Item out;
    vct_get_itm_at(v, 0, &out);
    printf("Item id: %d, value: %.2f\n", out.id, out.value);
    vct_free(v);
    return 0;
}
```
## Freestanding
```c
#define VCT_IMPL
#define VCT_FREESTANDING
#include "../include/vct/vct.h"
#include <inttypes.h>
#include <stdlib.h> 
#include <stdio.h>

void* my_malloc(size_t sz)
{
	printf("Allocating %u bytes\n", sz);
	return malloc(sz);
}

void* my_realloc(void* v, size_t sz)
{
	printf("Reallocating %u bytes\n", sz);
	return realloc(v, sz);
}

void my_free(void* v)
{
	printf("Freeing %" PRIuPTR  "\n", v);
	free(v);
}

bool test_cb(void* itm, void* data)
{
	(void)data; // Unused
	printf("%c", *(char*)itm);
	return true;
}

vct_allocators allocrs = {
	.malloc = my_malloc,
	.free = my_free,
	.realloc = my_realloc
};

int main()
{
	vct* v = VCT_ALLOC(char, 0u, &allocrs);
	if (!v) return 1u;
	for (char i = '0'; i <= '9'; i++)
		vct_push_char(v, i);
	vct_for_each(v, test_cb, NULL);
	vct_free(v);
	return 0u;
}
```
## API Highlights

-   **Allocation**: `VCT_INIT(v, T, capacity)`, `vct_free`
    
-   **Push/Pop/Get**: `vct_push_T`, `vct_pop_T`, `vct_get_T_at`
    
-   **Iteration**: `vct_for_each(v, cb, user_data)`
    
-   **Custom allocators**: `vct_init_allocators`
## Return Codes

-   `VCT_SUCCESS`
    
-   `VCT_INVALID_PUSH / POP / IDX`
    
-   `VCT_FAILALLOC`
    

## License

MIT
