#define VCT_IMPL
#define VCT_FREESTANDING
#include "../include/vct.h"
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
	printf("Freeing " PRIuPTR  "\n", v);
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
	vct v; vct_set_allocators_ref(&v, &allocrs);
	if (VCT_INIT(&v, char, 0u)) return 1u;
	for (char i = '0'; i <= '9'; i++)
		vct_push_char(&v, i);
	vct_for_each(&v, test_cb, NULL);
	vct_deinit(&v);
	return 0u;
}