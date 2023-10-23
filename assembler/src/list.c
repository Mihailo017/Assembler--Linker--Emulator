#include "list.h"

#include <stdio.h>
#include <stdlib.h>

static Node* createNode(void* data)
{
	Node* node = calloc(1, sizeof(Node));
	node->data = data;
	return node;
}

static uint64_t nextListId = 0;

void list_create(List_h* handle, void (*dataDeleteFunction)(void*))
{
	List* l = calloc(1, sizeof(List));
	l->id = ++nextListId;
	l->dataDelete = dataDeleteFunction;
	*handle = &l->id;
}

void list_addHead(List_h handle, void* data)
{
	Node* temp = createNode(data);
	List* l = (List*) handle;

	temp->next = l->head;
	l->head = temp;
	if (!l->tail) l->tail = l->head;

}

void list_addTail(List_h handle, void* data)
{
	List* l = (List*) handle;
    if (!l->head) {
        l->tail = l->head =  createNode(data);
    }
    else {
        l->tail = l->tail->next = createNode(data);
    }
}

void addBeforeCurrent2(List_h handle, void* data)
{
	Node* temp;
	List* l = (List*) handle;

	if (l->cur) {
		temp =  createNode(data);
		temp->next = l->cur->next;
		temp->data = l->cur->data;

		l->cur->next = temp;
		l->cur->data = data;

		if (l->tail == l->cur) l->tail = temp;

		l->cur = l->cur->next;
	} else {
		if (!l->head) l->head = l->tail = l->cur =  createNode(data);
    }
}

void list_start(List_h handle) {
	List* l = (List*) handle;
	l->cur = l->head;
}

void list_next(List_h handle) {
	List* l = (List*) handle;
	if (l->cur) l->cur = l->cur->next;
}

bool list_notEnd(List_h handle) {
	List* l = (List*) handle;
	return l->cur != 0;
}

void* list_get(List_h handle) {
	List* l = (List*) handle;
	return l->cur ? l->cur->data : 0;
}

void* takeHead2(List_h handle) {

	List* l = (List*) handle;
	void* ret = NULL;

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

void* takeCurrent2(List_h handle) {
	List* l = (List*) handle;
	void* ret = 0;

	if (l->cur) {
		ret = l->cur->data;
		deleteCurrent2(handle);
	}
	return ret;
}

void* takeTail2(List_h handle)
{
	List* l = (List*) handle;

	Node *temp1 = l->cur, *temp2 = l->tail;
	l->cur = l->tail;
	void* ret = takeCurrent2(handle);
	if (temp1 != temp2) l->cur = temp1;
	return ret;
}

bool list_isEmpty(List_h handle)
{
	return ((List*)handle)->head == NULL;
}

void deleteCurrent2(List_h handle)
{
	List* l = (List*) handle;

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

void list_removeAll(List_h handle) {
	List* l = (List*) handle;
	while (l->head) {
		l->tail = l->head;
		l->head = l->head->next;
		if (l->dataDelete != NULL) {
			l->dataDelete(l->tail->data);
		}
		free(l->tail->data);
        free(l->tail);
	}
	l->head = l->tail = l->cur = 0;
}

void ispis2(List_h handle) { // Samo za integer
	List* l = (List*) handle;
	l->cur = l->head;
	if (!l->cur) printf("Lista je prazna!");
	while (l->cur) {
		printf("%d\n", *(int*)((l->cur)->data));
		l->cur = (l->cur)->next;
	}
}

void list_delete(List_h handle)
{
	list_removeAll(handle);
	free((List*) handle);
}