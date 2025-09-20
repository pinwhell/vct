#ifndef VCT_H_
#define VCT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#ifndef VCT_FREESTANDING
#include <stdlib.h>
#endif

#if __STDC_VERSION__ < 202311L
#include <stdbool.h>
#endif

	typedef bool(*tvct_for_each_cb)(void* itm, void* data);

#ifdef VCT_FREESTANDING
	static inline void vct_memcpy(void* dest, const void* src, size_t n) {
		unsigned char* d = dest;
		const unsigned char* s = src;
		for (size_t i = 0; i < n; i++) d[i] = s[i];
	}
	static inline void vct_memset(void* ptr, int value, size_t n) {
		unsigned char* p = ptr;
		for (size_t i = 0; i < n; i++) p[i] = (unsigned char)value;
	}
#else
#include <string.h>
#define vct_memcpy memcpy
#define vct_memset memset
#endif

	enum {
		VCT_SUCCESS = 0,
		VCT_INVALID_PUSH,
		VCT_INVALID_POP,
		VCT_FAILALLOC,
		VCT_INVALID_IDX,
		VCT_INVALID_ARG
	};

	typedef struct vct_allocators {
		void* (*malloc)(size_t sz);
		void* (*realloc)(void* ptr, size_t sz);
		void (*free)(void*);
	} vct_allocators;

	typedef struct vct {
		vct_allocators allocators_stg;
		size_t size;
		size_t stride;
		size_t capacity;
		void* data;
		vct_allocators* allocatorsp;
	} vct;

#define VCT_INIT(v, T, ...) vct_init(v, sizeof(T), __VA_ARGS__)
	int vct_init(vct* v, size_t stride, size_t capacity);
	void vct_deinit(vct* v);
	void vct_set_allocators_ref(vct* v, vct_allocators* ref);
	void vct_set_allocators(vct* v, vct_allocators* allocators);
	static inline size_t vct_size_get(vct* v) { return v->size; }
	int vct_push_any(vct* v, const void* data, size_t size);
	int vct_pop_any(vct* v, void* out, size_t sz);
	int vct_get_any_at(vct* v, size_t idx, void* out, size_t sz);
	void vct_for_each(vct* v, tvct_for_each_cb cb, void* data);

	// Push
	int vct_push_uchar(vct* v, unsigned char obj);
	int vct_push_ushort(vct* v, unsigned short obj);
	int vct_push_uint(vct* v, unsigned int obj);
	int vct_push_ulong(vct* v, unsigned long obj);
	int vct_push_ulonglong(vct* v, unsigned long long obj);
	int vct_push_char(vct* v, char obj);
	int vct_push_short(vct* v, short obj);
	int vct_push_int(vct* v, int obj);
	int vct_push_long(vct* v, long obj);
	int vct_push_longlong(vct* v, long long obj);
	int vct_push_float(vct* v, float obj);
	int vct_push_double(vct* v, double obj);

	// Pop
	int vct_pop_uchar(vct* v, unsigned char* out);
	int vct_pop_ushort(vct* v, unsigned short* out);
	int vct_pop_uint(vct* v, unsigned int* out);
	int vct_pop_ulong(vct* v, unsigned long* out);
	int vct_pop_ulonglong(vct* v, unsigned long long* out);
	int vct_pop_char(vct* v, char* out);
	int vct_pop_short(vct* v, short* out);
	int vct_pop_int(vct* v, int* out);
	int vct_pop_long(vct* v, long* out);
	int vct_pop_longlong(vct* v, long long* out);
	int vct_pop_float(vct* v, float* out);
	int vct_pop_double(vct* v, double* out);

	// Get at index
	int vct_get_uchar_at(vct* v, size_t idx, unsigned char* out);
	int vct_get_ushort_at(vct* v, size_t idx, unsigned short* out);
	int vct_get_uint_at(vct* v, size_t idx, unsigned int* out);
	int vct_get_ulong_at(vct* v, size_t idx, unsigned long* out);
	int vct_get_ulonglong_at(vct* v, size_t idx, unsigned long long* out);
	int vct_get_char_at(vct* v, size_t idx, char* out);
	int vct_get_short_at(vct* v, size_t idx, short* out);
	int vct_get_int_at(vct* v, size_t idx, int* out);
	int vct_get_long_at(vct* v, size_t idx, long* out);
	int vct_get_longlong_at(vct* v, size_t idx, long long* out);
	int vct_get_float_at(vct* v, size_t idx, float* out);
	int vct_get_double_at(vct* v, size_t idx, double* out);

#ifdef VCT_IMPL

	void* vct_calloc(vct* v, size_t count, size_t sz) {
		if (!v->allocatorsp->malloc) return NULL;
		void* ptr = v->allocatorsp->malloc(count * sz);     // allocate memory
		if (ptr) vct_memset(ptr, 0, count * sz);        // zero it
		return ptr;
	}

	void vct_set_allocators_ref(vct* v, vct_allocators* ref)
	{
		v->allocatorsp = ref;
	}

	void vct_set_allocators(vct* v, vct_allocators* allocators)
	{
		vct_memcpy(&v->allocators_stg, allocators, sizeof(vct_allocators));
		v->allocatorsp = &v->allocators_stg;
	}

#ifndef VCT_FREESTANDING
	void vct_set_defallocators(vct* v)
	{
		vct_allocators def_allocats = {
			.malloc = malloc,
			.realloc = realloc,
			.free = free
		};
		vct_set_allocators(v, &def_allocats);
	}
#endif

	int vct_init(vct* v, size_t stride, size_t capacity)
	{
#ifndef VCT_FREESTANDING
		vct_set_defallocators(v);
#endif
		if (!stride) return VCT_INVALID_ARG;
		if (capacity && !(v->data = vct_calloc(v, capacity, stride)))
			return VCT_FAILALLOC;
		else v->data = NULL;
		v->capacity = capacity;
		v->stride = stride;
		v->size = 0;
		return VCT_SUCCESS;
	}

	void vct_deinit(vct* v)
	{
		if (!v) return;
		if (v->data) {
			v->allocatorsp->free(v->data);
			v->data = NULL;
		}
	}

	int vct_push_any(vct* v, const void* data, size_t size)
	{
		if (size > v->stride) return VCT_INVALID_PUSH;
		size_t curr_idx = v->size;
		size_t new_size = v->size + 1;
		if (new_size > v->capacity)
		{
			size_t new_capacity = !v->capacity ? 1 : v->capacity * 2u;
			void* new_data = v->allocatorsp->realloc(v->data, new_capacity * v->stride);
			if (!new_data) return VCT_FAILALLOC;
			v->capacity = new_capacity;
			v->data = new_data;
		}
		void* dst = (char*)v->data + curr_idx * v->stride;
		vct_memcpy(dst, data, size);
		v->size = new_size;
		return VCT_SUCCESS;
	}

	int vct_get_any_at(vct* v, size_t idx, void* out, size_t sz) {
		if (!(idx < v->size)) return VCT_INVALID_IDX;
		vct_memcpy(out, (char*)v->data + v->stride * idx, sz);
		return VCT_SUCCESS;
	}

	void* vct_back(vct* v)
	{
		return (char*)v->data + v->stride * (v->size - 1u);
	}

	int vct_pop_any(vct* v, void* out, size_t sz)
	{
		if (sz > v->stride || v->size < 1u) return VCT_INVALID_POP;
		void* back = vct_back(v);
		if (!back) return VCT_INVALID_POP;
		vct_memcpy(out, back, sz);
		v->size--;
		return VCT_SUCCESS;
	}

	void vct_for_each(vct* v, tvct_for_each_cb cb, void* data)
	{
		if (!v || !v->data || !v->size) return;
		for (size_t i = 0; i < v->size; ++i)
			if (!cb((char*)v->data + i * v->stride, data))
				break;
	}

#define VCT_PUSH_DEF(shorthand, T) int vct_push_##shorthand(vct* v, T obj) { return vct_push_any(v, &obj, sizeof(obj)); }
#define VCT_POP_DEF(shorthand, T) int vct_pop_##shorthand(vct* v, T* out) { return vct_pop_any(v, out, sizeof(T)); }
#define VCT_GET_DEF(shorthand, T) int vct_get_##shorthand##_at(vct* v, size_t idx, T* out) { return vct_get_any_at(v, idx, out, sizeof(T)); }
	VCT_PUSH_DEF(uchar, unsigned char)
		VCT_PUSH_DEF(ushort, unsigned short)
		VCT_PUSH_DEF(uint, unsigned int)
		VCT_PUSH_DEF(ulong, unsigned long)
		VCT_PUSH_DEF(ulonglong, unsigned long long)
		VCT_PUSH_DEF(char, char)
		VCT_PUSH_DEF(short, short)
		VCT_PUSH_DEF(int, int)
		VCT_PUSH_DEF(long, long)
		VCT_PUSH_DEF(longlong, long long)
		VCT_PUSH_DEF(float, float)
		VCT_PUSH_DEF(double, double)
		VCT_POP_DEF(uchar, unsigned char)
		VCT_POP_DEF(ushort, unsigned short)
		VCT_POP_DEF(uint, unsigned int)
		VCT_POP_DEF(ulong, unsigned long)
		VCT_POP_DEF(ulonglong, unsigned long long)
		VCT_POP_DEF(char, char)
		VCT_POP_DEF(short, short)
		VCT_POP_DEF(int, int)
		VCT_POP_DEF(long, long)
		VCT_POP_DEF(longlong, long long)
		VCT_POP_DEF(float, float)
		VCT_POP_DEF(double, double)
		VCT_GET_DEF(uchar, unsigned char)
		VCT_GET_DEF(ushort, unsigned short)
		VCT_GET_DEF(uint, unsigned int)
		VCT_GET_DEF(ulong, unsigned long)
		VCT_GET_DEF(ulonglong, unsigned long long)
		VCT_GET_DEF(char, char)
		VCT_GET_DEF(short, short)
		VCT_GET_DEF(int, int)
		VCT_GET_DEF(long, long)
		VCT_GET_DEF(longlong, long long)
		VCT_GET_DEF(float, float)
		VCT_GET_DEF(double, double)
#endif

#ifdef __cplusplus
}
#endif

#endif