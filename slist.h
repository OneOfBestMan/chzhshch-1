#ifndef SLIST_H
#define SLIST_H



// 设计 参考 《深入理解linux内核》 第三版 Doubly linked lists P87

typedef struct List_Entry
{
	List_Entry *prev;
	List_Entry *next;
} *PList_Head, List_Entry;



#define list_entry(addr, typeOfEntry, fieldName) ((typeOfEntry*)((char*)(addr) - (char*)(&((typeOfEntry*)0)->fieldName)))

#define remove_entry(item, listField) item->listField.prev->next = item->listField.next; item->listField.next->prev = item->listField.prev;


void initializeList(PList_Head list);

void append(PList_Head list, List_Entry *item);

List_Entry* remove(List_Entry *item);

bool isEmpty(PList_Head list);

#endif
