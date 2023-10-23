#ifndef LIST_H_
#define LIST_H_

#include <stdint.h>
#include <stdbool.h>

typedef uint64_t List_id;
typedef List_id* List_h;

typedef struct Node {
		void* data;
		struct Node* next;
} Node;

typedef struct List {
	List_id id;
	Node* head;
	Node* tail;
	Node* cur;
	void (*dataDelete)(void*);
} List;

/*
*	Pravi praznu genericku listu.
*	Svi pokazivaci u listi su inicijanlo NULL.
*	Vraca rucku za upravljanje listom kroz arugment handle.
*/
void list_create(List_h* handle, void (*dataDeleteFunction)(void*));

/*
*	Dodavanje elemnta sa sadrzajem data na pocetak liste.
*/
void list_addHead(List_h handle, void* data);

void ispis2(List_h handle);

/*
*	Dodavanje elemnta sa sadrzajem data na kraj liste.
*/
void list_addTail(List_h handle, void* data);

void addBeforeCurrent2(List_h handle, void* data);

/*
*	Postavlja pokazivac cur na pocetak liste.
*/
void list_start(List_h handle);

/*
*	Prebacuje pokazivac cur na naredni element u listi.
*/
void list_next(List_h handle);

/*
*	Vraca true ako cur nije NULL.
*/
bool list_notEnd(List_h handle);

/*
*	Vraca data deo elementa liste na koji ukazuje cur.
*	Obavezan je cast void pokazivaca pre koriscenja.
*/
void* list_get(List_h handle);

/*
*	Brise element liste na koji pokazuje cur.
*	Prebacuje cur na naredni element.
*/
void deleteCurrent2(List_h handle);

/*
*	Vraca data deo elementa liste na koji ukazuje head.
*	Obavezan je cast void pokazivaca pre koriscenja.
*	Brise element iz liste.
*	Pokazivac head se prebacuje na sledeci element.
*/
void* takeHead2(List_h handle);

/*
*	Vraca data deo elementa liste na koji ukazuje cur.
*	Obavezan je cast void pokazivaca pre koriscenja.
*	Brise element iz liste.
*	Pokazivac cur se prebacuje na sledeci element.
*/
void* takeCurrent2(List_h handle);

/*
*	Vraca data deo elementa liste na koji ukazuje tail.
*	Obavezan je cast void pokazivaca pre koriscenja.
*	Brise element iz liste.
*	Pokazivac tail se prebacuje na sledeci element.
*/
void* takeTail2(List_h handle);

/*
*	Vraca true ako je lista prazna.
*/
bool list_isEmpty(List_h handle);

/*
*	Brise sve elemente liste.
*	Za svaki data deo se poziva dataDelete funkcija ako je navedena.
*/
void list_removeAll(List_h handle); // Sigurno radi

void list_delete(List_h handle);

#endif