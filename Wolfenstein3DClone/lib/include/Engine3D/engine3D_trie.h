#ifndef ENGINE3D_TRIE_H
#define ENGINE3D_TRIE_H

#include <stdbool.h>
#include <stdlib.h>

typedef struct engine3D_trie_t {
	bool hasValue;
	int value;
	char *childrenKeys;
	struct engine3D_trie_t *children;
	size_t childrenNextIndex;
	size_t childrenCapacity;
} engine3D_trie_t;

engine3D_trie_t *engine3D_trie_new(void);

void engine3D_trie_init(engine3D_trie_t *const trie);

void engine3D_trie_add(engine3D_trie_t *const trie, const char *const key, int value);

int engine3D_trie_get(const engine3D_trie_t *const trie, const char *const key);

#endif /* ENGINE3D_TRIE_H */
