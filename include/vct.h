#ifndef VCT_H_
#define VCT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

	typedef size_t vct_size_t;
	typedef int vct_bool;
	typedef vct_bool(*tfor_each_cb)(void* itm, void* data);

	inline void vct_memcpy(void* dest, const void* src, vct_size_t n) {
		unsigned char* d = (unsigned char*)dest;
		const unsigned char* s = (const unsigned char*)src;
		for (vct_size_t i = 0; i < n; i++)
			d[i] = s[i];
	}

	inline void vct_memset(void* ptr, int value, vct_size_t n) {
		unsigned char* p = (unsigned char*)ptr;
		for (vct_size_t i = 0; i < n; i++)
			p[i] = (unsigned char)value;
	}

	enum {
		VCT_SUCCESS = 0,
		VCT_INVALID_PUSH,
		VCT_INVALID_POP,
		VCT_FAILALLOC,
		VCT_INVALID_IDX
	};

	typedef struct vct_allocators {
		int initialized;
		void* (*malloc)(vct_size_t sz);
		void* (*free)(void*);
	} vct_allocators;

	typedef struct vct {
		vct_size_t size;
		vct_size_t stride;
		vct_size_t capacity;
		void* data;
	} vct;

	vct* vct_alloc(vct_size_t stride, vct_size_t capacity);
	void vct_free(vct* v);
	int vct_push_any(vct* v, const void* data, vct_size_t size);
	int vct_pop_any(vct* v, void* out, vct_size_t sz);
	int vct_get_any_at(vct* v, vct_size_t idx, void* out, vct_size_t sz);
	void vct_for_each(vct* v, tfor_each_cb cb, void* data);

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
	int vct_get_uchar_at(vct* v, vct_size_t idx, unsigned char* out);
	int vct_get_ushort_at(vct* v, vct_size_t idx, unsigned short* out);
	int vct_get_uint_at(vct* v, vct_size_t idx, unsigned int* out);
	int vct_get_ulong_at(vct* v, vct_size_t idx, unsigned long* out);
	int vct_get_ulonglong_at(vct* v, vct_size_t idx, unsigned long long* out);
	int vct_get_char_at(vct* v, vct_size_t idx, char* out);
	int vct_get_short_at(vct* v, vct_size_t idx, short* out);
	int vct_get_int_at(vct* v, vct_size_t idx, int* out);
	int vct_get_long_at(vct* v, vct_size_t idx, long* out);
	int vct_get_longlong_at(vct* v, vct_size_t idx, long long* out);
	int vct_get_float_at(vct* v, vct_size_t idx, float* out);
	int vct_get_double_at(vct* v, vct_size_t idx, double* out);

#ifdef VCT_IMPL

	vct_allocators allocrs = { 0u };

	void* vct_calloc(size_t count, size_t sz) {
		if (!allocrs.initialized) return NULL;      // ensure allocator is set
		void* ptr = allocrs.malloc(count * sz);     // allocate memory
		if (ptr) vct_memset(ptr, 0, count * sz);        // zero it
		return ptr;
	}

	void* vct_realloc(void* ptr, size_t new_size) {
		if (!allocrs.initialized) return NULL;
		if (!ptr) return allocrs.malloc(new_size);
		void* new_ptr = allocrs.malloc(new_size);
		if (!new_ptr) return NULL;
		vct_memcpy(new_ptr, ptr, new_size); // NOTE: might copy too much if shrinking
		allocrs.free(ptr);
		return new_ptr;
	}

	void vct_init_allocators(vct_allocators* allocators)
	{
		if (allocrs.initialized) return;
		vct_memcpy(&allocrs, allocators, sizeof(vct_allocators));
		allocrs.initialized = 1u;
	}

#ifndef VCT_FREESTANDING
#include <stdlib.h>
	void vct_set_defallocators()
	{
		vct_allocators allocats = {
			.malloc = malloc,
			.free = free
		};
		vct_init_allocators(&allocats);
	}
#endif

	vct* vct_alloc(vct_size_t stride, vct_size_t capacity)
	{
#ifndef VCT_FREESTANDING
		if (!allocrs.initialized) vct_set_defallocators();
#else
		if (!allocrs.initialized) return NULL;
#endif
		if (!stride) return NULL;
		vct* v = allocrs.malloc(sizeof(vct));
		if (!v) return v;
		vct_memset(v, 0, sizeof(vct));
		if (capacity && !(v->data = vct_calloc(capacity, stride))) {
			allocrs.free(v);
			return NULL;
		}
		v->capacity = capacity;
		v->stride = stride;
		v->size = 0;
		return v;
	}

	void vct_free(vct* v)
	{
		if (!v) return;
		if (v->data) allocrs.free(v->data);
		allocrs.free(v);
	}

	int vct_push_any(vct* v, const void* data, vct_size_t size)
	{
		if (size > v->stride) return VCT_INVALID_PUSH;
		vct_size_t curr_idx = v->size;
		vct_size_t new_size = v->size + 1;
		if (new_size > v->capacity)
		{
			vct_size_t new_capacity = !v->capacity ? 1 : v->capacity * 2u;
			void* new_data = vct_realloc(v->data, new_capacity * v->stride);
			if (!new_data) return VCT_FAILALLOC;
			v->capacity = new_capacity;
			v->data = new_data;
		}
		void* dst = (char*)v->data + curr_idx * v->stride;
		vct_memcpy(dst, data, size);
		v->size = new_size;
		return VCT_SUCCESS;
	}

	int vct_get_any_at(vct* v, vct_size_t idx, void* out, vct_size_t sz) {
		if (!(idx < v->size)) return VCT_INVALID_IDX;
		vct_memcpy(out, (char*)v->data + v->stride * idx, sz);
		return VCT_SUCCESS;
	}

	void* vct_back(vct* v)
	{
		return (char*)v->data + v->stride * (v->size - 1u);
	}

	int vct_pop_any(vct* v, void* out, vct_size_t sz)
	{
		if (sz > v->stride || v->size < 1u) return VCT_INVALID_POP;
		void* back = vct_back(v);
		if (!back) return VCT_INVALID_POP;
		vct_memcpy(out, back, sz);
		v->size--;
		return VCT_SUCCESS;
	}

	void vct_for_each(vct* v, tfor_each_cb cb, void* data)
	{
		if (!v || !v->data || !v->size) return;
		for (vct_size_t i = 0; i < v->size; ++i)
			if (!cb((char*)v->data + i * v->stride, data))
				break;
	}

#define VCT_PUSH_DEF(shorthand, T) int vct_push_##shorthand(vct* v, T obj) { return vct_push_any(v, &obj, sizeof(obj)); }
#define VCT_POP_DEF(shorthand, T) int vct_pop_##shorthand(vct* v, T* out) { return vct_pop_any(v, out, sizeof(T)); }
#define VCT_GET_DEF(shorthand, T) int vct_get_##shorthand##_at(vct* v, vct_size_t idx, T* out) { return vct_get_any_at(v, idx, out, sizeof(T)); }
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