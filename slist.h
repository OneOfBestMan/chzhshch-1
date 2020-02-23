
#ifndef SLIST_H
#define SLIST_H

// 设计 参考 《深入理解linux内核》 第三版 Doubly linked lists P87

typedef struct List_Entry
{
	List_Entry *prev;
	List_Entry *next;
} *PList_Head, List_Entry;

void initializeList(PList_Head list)
{
	list->prev = list->next = list;
}

void append(PList_Head list, List_Entry *item)
{
	list->prev->next = item;
	item->prev = list->prev;
	item->next = list;
	list->prev = item;
}

#define list_entry(addr, typeOfEntry, fieldName) ((typeOfEntry*)((char*)(addr) - (char*)(&((typeOfEntry*)0)->fieldName)))

List_Entry* remove(List_Entry *item)
{
	item->next->prev = item->prev;
	item->prev->next = item->next;
	return item;
}

bool isEmpty(PList_Head list)
{
	return list->next == list->prev;
}


#endif