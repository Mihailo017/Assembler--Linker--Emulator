#ifndef LIST_H_
#define LIST_H_

typedef struct Node {
		void* data;
		struct Node* next;
} Node;

typedef struct List {
	Node* head;
	Node* tail;
	Node* cur;
	void (*dataDelete)(void*);
} List;

void initNode(Node* node, void* data);

List* createList();

void setDataDelete(List** list, void (*dataDelete)(void*));

void addHead(List** list, void* data);

void addTail(List** list, void* data);

void addBeforeCurrent(List** list, void* data);

void currentToHead(List** list);

void next(List** list);

int hasCurrent(List** list);

void* getCurrent(List** list);

void deleteCurrent(List** list); // Sigurno radi

void* takeHead(List** list);

void* takeCurrent(List** list);

void* takeTail(List** list);

int isEmpty(List** list);

void emptyList(List** list); // Sigurno radi

void ispis(List** list);

int exists(List** list, int element);
void* find(List** list, int element);

#endif