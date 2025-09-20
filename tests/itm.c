#include <stdio.h>
#include <vct.h>

typedef struct {
    int id;
    float value;
} Item;

int main() {
    vct* v = VCT_ALLOC(Item, 0u, NULL);
    if (!v) return 1u;

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