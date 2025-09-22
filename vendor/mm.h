#ifndef MM_H_
#define MM_H_

#include <stddef.h>

enum {
	MM_SUCCESS = 0,
	MM_ARENA_NO_SPACE,
	MM_ARENA_ALREADY_EXISTS
};

typedef struct mm_block_head {
	size_t size;
	int free;
	struct mm_block_head* next;
} mm_block_head;

typedef struct {
	void* base;
	size_t size;
	mm_block_head* first;
} mm_arena;

int mm_add_arena(void* base, size_t size);
void* mm_alloc(size_t size);
void mm_free(void* ptr);
void* mm_realloc(void* old_ptr, size_t new_sz);

#ifdef MM_IMPL
void* mm_memcpy(void* dest, const void* src, size_t n) {
	unsigned char* d = dest;
	const unsigned char* s = src;
	while (n--) {
		*d++ = *s++;
	}
	return dest;
}

#ifndef MM_FREESTANDING
#include <string.h>
#define mm_memcpy memcpy
#endif

#ifndef ARENA_ARR_SZ 
#define ARENA_ARR_SZ 16u
#endif

mm_arena arenas[ARENA_ARR_SZ];
int arena_idx = 0u;

int mm_add_arena(void* base, size_t size)
{
	if (!(arena_idx < ARENA_ARR_SZ)) return MM_ARENA_NO_SPACE;
	for (size_t i = 0; i < arena_idx; ++i)
		if (arenas[i].base == base) return MM_ARENA_ALREADY_EXISTS;
	mm_arena* arena = &arenas[arena_idx];
	arena->base = base;
	arena->size = size;
	arena->first = (mm_block_head*)base;
	arena->first->size = size - sizeof(mm_block_head);
	arena->first->free = 1u;
	arena->first->next = NULL;
	arena_idx++;
	return MM_SUCCESS;
}

// O(N)
void mm_arena_defrag(mm_arena* arena) {
	if (!arena || !arena->first) return;

	mm_block_head* cur = arena->first;
	while (cur && cur->next) {
		if (cur->free && cur->next->free) {
			// merge cur and next
			cur->size += sizeof(mm_block_head) + cur->next->size;
			cur->next = cur->next->next;
			// stay on cur to catch more free neighbors
		}
		else {
			cur = cur->next;
		}
	}
}

// O(N)
mm_arena* mm_arena_from_blockhead(mm_block_head* head)
{
	for (size_t i = 0; i < arena_idx; i++)
	{
		mm_arena* a = &arenas[i];
		char* start = a->base;
		char* end = (char*)a->base + a->size;
		if ((char*)head >= start && (char*)head < end) return a;
	}
	return NULL;
}

mm_block_head* mm_arena_find_fit(mm_arena* a, size_t size) {
	mm_block_head* cur = a->first;
	while (cur) {
		if (cur->free && cur->size >= size) return cur;
		cur = cur->next;
	}
	return NULL;
}

mm_block_head* mm_arenas_find_fit(size_t size)
{
	for (size_t i = 0; i < arena_idx; i++)
	{
		mm_block_head* head = mm_arena_find_fit(&arenas[i], size);
		if (!head) continue;
		return head;
	}
	return NULL;
}

void* mm_alloc(size_t size)
{
	size = (size + 7) & ~7; // 8-byte align
	mm_block_head* b = mm_arenas_find_fit(size);
	if (b == NULL) return NULL;
	size_t left_over = b->size - size;
	if (left_over >= sizeof(mm_block_head) + sizeof(void*)) // Split
	{
		mm_block_head* newb = (mm_block_head*)((char*)b + sizeof(mm_block_head) + size);
		newb->size = b->size - size - sizeof(mm_block_head);
		newb->next = b->next;
		newb->free = 1u;
		b->next = newb;
		b->size = size;
	}
	b->free = 0u;
	return (char*)b + sizeof(mm_block_head);
}

void mm_free(void* ptr)
{
	if (!ptr) return;
	mm_block_head* b = (mm_block_head*)((char*)ptr - sizeof(mm_block_head));
	mm_arena* a = mm_arena_from_blockhead(b);
	if (!a) return;
	b->free = 1;
	mm_arena_defrag(a);
}

int mm_block_head_try_grow(mm_block_head* h, size_t to_new_size)
{
	to_new_size = (to_new_size + 7) & ~7; // 8-byte align

	if (to_new_size <= h->size) {
		// Already big enough
		return 1;
	}

	size_t needed = to_new_size - h->size;

	mm_block_head* old_nextp = h->next;

	if (!old_nextp) return 0u;

	mm_block_head old_next = *h->next;

	// Cannot expand if next is missing, used, or too small
	if (!old_nextp->free || old_nextp->size < needed)
		return 0;

	// Merge part or all of the next block
	if (old_next.size > needed + sizeof(mm_block_head)) {
		// Split the next block
		mm_block_head* new_next = (mm_block_head*)((char*)h->next + needed);
		new_next->size = old_next.size - needed - sizeof(mm_block_head);
		new_next->free = 1;
		new_next->next = old_next.next;

		h->next = new_next;
		h->size += needed;
	}
	else {
		// Take whole next block
		h->size += sizeof(mm_block_head) + old_next.size;
		h->next = old_next.next;
	}

	return 1; // Expanded successfully
}

void* mm_realloc(void* old_ptr, size_t new_sz)
{
	new_sz = (new_sz + 7) & ~7; // 8-byte align
	if (!old_ptr) return mm_alloc(new_sz);
	mm_block_head* b = (mm_block_head*)((char*)old_ptr - sizeof(mm_block_head));
	if (b->size >= new_sz) return old_ptr; // Fits
	if (mm_block_head_try_grow(b, new_sz)) return old_ptr;
	void* new_ptr = mm_alloc(new_sz);
	if (new_ptr)
	{
		mm_memcpy(new_ptr, old_ptr, b->size);
		mm_free(old_ptr);
	}
	return new_ptr;
}
#endif

#endif