#include "minHeap.h"

#define UP(i) (i-1)/2
#define LEFT(i) 2*i+1
#define RIGHT(i) 2*i+2

typedef int Elem;

void swap(Elem v[], int i, int j) {
	Elem aux = v[i];
	v[i] = v[j];
	v[j] = aux;
}

void bubble_up(Elem h[], int i) {
	while (i>0 && h[i] < h[UP(i)]) {
		swap(h, i, UP(i));
		i = UP(i);
	}
}

void bubble_down(Elem h[], int size) {
	int imin, i = 0;
	while (LEFT(i) < size) {
		imin = LEFT(i);
		if (RIGHT(i)<size && h[RIGHT(i)] < h[imin])
			imin = RIGHT(i);
		// imin guarda indice do menor filho...
		if (h[i] <= h[imin]) break;
		swap(h, i, imin);
		i = imin;
	}
}

typedef struct heap_str{
	int size;
	int used;
	Elem *values;
} *Heap;

Heap newheap(int size) {
	if (size <= 0) exit(1);

	Heap res = (Heap)malloc(sizeof(struct heap_str));
	res->size = size;
	res->used = 0;
	res->values = (Elem*)calloc(size, sizeof(Elem));
	return res;
}

int insertHeap(Heap h, Elem x) {
	if (h->used >= h->size) return 1;
	h->values[h->used] = x;
	bubble_up(h->values, h->used);
	h->used++;
	return 0;
}

int extractMin(Heap h, Elem *x) {
	if (h->used <= 0) return 1;
	*x = h->values[0];
	h->used--;
	h->values[0] = h->values[h->used];
	bubble_down(h->values, h->used);
	return 0;
}
