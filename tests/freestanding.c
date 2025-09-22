#define VCT_IMPL
#define VCT_FREESTANDING
#define MM_FREESTANDING
#define MM_IMPL
#include "../vendor/mm.h"
#include "../include/vct/vct.h"
#include <inttypes.h>
#include <stdlib.h> 
#include <stdio.h>

void* my_malloc(size_t sz)
{
	printf("Allocating %u bytes\n", sz);
	return mm_alloc(sz);
}

void* my_realloc(void* v, size_t sz)
{
	printf("Reallocating %u bytes\n", sz);
	return mm_realloc(v, sz);
}

void my_free(void* v)
{
	printf("Freeing %" PRIuPTR  "\n", v);
	mm_free(v);
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
	char arena[0x1000u];
	mm_add_arena(arena, sizeof(arena));
	vct* v = VCT_ALLOC(char, 0u, &allocrs);
	if (!v) return 1u;
	for (char i = '0'; i <= '9'; i++)
		vct_push_char(v, i);
	vct_for_each(v, test_cb, NULL);
	vct_free(v);
	return 0u;
}