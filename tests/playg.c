
#include <vct/vct.h>
#include <stdio.h>

bool test_cb(void* c, void* data) {
	printf("%c", *(char*)c);
	return true;
}

bool print_int(void* itm, void* data) {
	(void)data; // unused
	printf("%d ", *(int*)itm);
	return true;  // continue iteration
}

int main()
{
	vct v = { 0 }; if (VCT_INIT(&v, char, 0u)) return 1u;
	vct_push_char(&v, 'H');
	vct_push_char(&v, 'i');
	vct_push_char(&v, '\0');
	vct_for_each(&v, test_cb, NULL);
	char c;
	vct_get_char_at(&v, 0u, &c);
	printf("%s%c", (char*)v.data, c);
	vct_deinit(&v);

	if (!VCT_INIT(&v, float, 0u)) return 1u;
	vct_push_float(&v, 3.14f);
	vct_push_float(&v, 2.718f);

	float f;
	vct_pop_float(&v, &f); // f == 2.718

	vct_deinit(&v);
}