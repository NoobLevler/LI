#include "minHeap.h"

#define UP(i) (i-1)/2
#define LEFT(i) 2*i+1
#define RIGHT(i) 2*i+2

typedef struct elem
{
	int key, dataSize;
	void *data;
}*Elem_st;

void swap(Elem *v, int i, int j) {
	Elem aux = v[i];
	v[i] = v[j];
	v[j] = aux;
}

void bubble_up(Elem *h, int i) {
	while (i>0 && h[i]->key < h[UP(i)]->key) {
		swap(h, i, UP(i));
		i = UP(i);
	}
}

void bubble_down(Elem *h, int size) {
	int imin, i = 0;
	while (LEFT(i) < size) {
		imin = LEFT(i);
		if (RIGHT(i)<size && h[RIGHT(i)]->key < h[imin]->key)
			imin = RIGHT(i);
/*		 imin guarda indice do menor filho... */
		if (h[i]->key <= h[imin]->key) break;
		swap(h, i, imin);
		i = imin;
	}
}

struct heap_str{
	int size;
	int used;
	Elem *values;
};
minHeap freeHeap(minHeap h)
{
	int  i;
	for (i = 0; i < h->size; i++)
	{
		free(h->values[i]);
	}
	free(h);
	return NULL;
}
minHeap newHeap(int size) {
	minHeap res;
	if (size <= 0)return NULL;
	res = malloc(sizeof(*res));
	res->size = size;
	res->used = 0;
	res->values = calloc(size, sizeof(Elem));
	return res;
}

int insertHeap(minHeap h, int key, void *data, int dataSize) {
	Elem x;
	if (h->used >= h->size) return 1;
	x = malloc(sizeof(*x));
	x->key = key;
	x->data = data;
	x->dataSize = dataSize;
	h->values[h->used] = x;
	bubble_up(h->values, h->used);
	h->used++;
	return 0;
}

Elem extractMin(minHeap h) {
	Elem x;
	if (h->used <= 0) return NULL;
	x = malloc(sizeof(*x));
	x = h->values[0];
	h->used--;
	h->values[0] = h->values[h->used];
	bubble_down(h->values, h->used);
	return x;
}

int getSize(minHeap h)
{
	return h->size;
}

int getUsed(minHeap h)
{
	return h->used;
}

void* getElemDataAddr(Elem e)
{
	return e->data;
}

int getElemKey(Elem e)
{
	return e->key;
}
