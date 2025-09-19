
#include <vct.h>
#include <stdio.h>

vct_bool test_cb(void* c, void* data) {
	printf("%c", *(char*)c);
	return 1u;
}

int main()
{
	vct* v = vct_alloc(sizeof(char), 0u);

	if (!v) return 1u;

	vct_push_char(v, 'H');
	vct_push_char(v, 'i');
	vct_push_char(v, '\0');

	vct_for_each(v, test_cb, NULL);

	char c;
	vct_get_char_at(v, 0u, &c);

	printf("%s%c", (char*)v->data, c);

	vct_free(v);
}