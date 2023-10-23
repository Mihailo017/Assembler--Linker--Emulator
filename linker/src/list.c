#include "list.h"
#include <stdio.h>
#include <stdlib.h>

void initNode(Node* node, void* data) {
    node->data = data;
    node->next = 0;
}

List* createList() {
	List* list = malloc(sizeof(List));
    list->head = list->tail = list->cur = 0;
	//list->dataDelete = NULL;
	return list;
}

/*void setDataDelete(List** list, void (*dataDelete)(void*)) {
	(*list)->dataDelete = dataDelete;
}*/

void addHead(List** list, void* data) { // Sigurno radi
	Node* temp = malloc(sizeof(Node));
    initNode(temp, data);

	List* l = *list;

	temp->next = l->head;
	l->head = temp;
	if (!l->tail) l->tail = l->head;
}

void addTail(List** list, void* data) { // Sigurno radi
	List* l = *list;
    if (!l->head) {
        l->tail = l->head =  malloc(sizeof(Node));
        initNode(l->head, data);
    }
    else {
        l->tail = l->tail->next =  malloc(sizeof(Node));
        initNode(l->tail, data);
    }
}

void addBeforeCurrent(List** list, void* data) { // Sigurno radi
	Node* temp;
	List* l = *list;
	if (l->cur) {
		temp =  malloc(sizeof(Node));
        initNode(temp, data);
		temp->next = l->cur->next;
		temp->data = l->cur->data;
		l->cur->next = temp;
		l->cur->data = data;
		if (l->tail == l->cur) l->tail = temp;
		l->cur = l->cur->next;
	} else {
		if (!l->head) l->head = l->tail = l->cur =  malloc(sizeof(Node));
        initNode(l->head, data);
    }
}

void currentToHead(List** list) {
	(*list)->cur = (*list)->head;
}

void next(List** list) {
	if ((*list)->cur) (*list)->cur = (*list)->cur->next;
}

int hasCurrent(List** list) {
	return (*list)->cur != 0;
}

void* getCurrent(List** list) {
	return (*list)->cur ? (*list)->cur->data : 0;
}

void* takeHead(List** list) { // Sigurno radi
	void* ret = 0;
	List* l = *list;
	if (l->head) {
		Node* temp = l->head;
		l->head = temp->next;
		ret = temp->data;
		if (l->cur == temp) l->cur = l->head;
		if (!l->head) l->tail = 0;
		free(temp);
	}
	return ret;
}

void* takeCurrent(List** list) { // Sigurno radi
	void* ret = 0;
	List* l = *list;
	if (l->cur) {
		ret = l->cur->data;
		deleteCurrent(list);
	}
	return ret;
}

void* takeTail(List** list) { // Sigurno radi
	List* l = *list;
	Node *temp1 = l->cur, *temp2 = l->tail;
	l->cur = l->tail;
	void* ret = takeCurrent(list);
	if (temp1 != temp2) l->cur = temp1;
	return ret;
}

int isEmpty(List** list) {return (*list)->head == 0;}

void deleteCurrent(List** list) { // Sigurno radi
	List* l = *list;
	if (!l->cur) return;
	Node* temp;
	if (l->cur->next) {
		temp = l->cur->next;
		l->cur->data = temp->data;
		l->cur->next = temp->next;
		if (l->tail == temp) l->tail = l->cur;
		free(temp);
	} else {
		temp = l->head;
		while (temp->next != l->tail) {
			temp = temp->next;
			if (!temp) break;
		}
		if (temp) {
			l->tail = temp;
			temp->next = 0;
		} else l->head = l->tail = 0;
		free(l->cur);

		//l->cur = 0;
		// ja sam promenio ovo
		l->cur = l->head;
	}
}

void emptyList(List** list) { // Sigurno radi
	List* l = *list;
	while (l->head) {
		l->tail = l->head;
		l->head = l->head->next;
		//free(l->tail->data);
        free(l->tail);
	}
	l->head = l->tail = l->cur = 0;
}

void ispis(List** list) { // Samo za integer
	List* l = *list;
	l->cur = l->head;
	if (!l->cur) printf("Lista je prazna!");
	while (l->cur) {
		printf("%d\n", *(int*)((l->cur)->data));
		l->cur = (l->cur)->next;
	}
}

int exists(List** list, int element) {
	for (currentToHead(list); hasCurrent(list); next(list))
		if (*((int*)(getCurrent(list))) == element) return 1;
	return 0;
}

void* find(List** list, int element) {
	void* ret;
	for (currentToHead(list); hasCurrent(list); next(list))
		if (*((int*)(ret = getCurrent(list))) == element) return ret;
	return NULL;
}