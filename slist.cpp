#include "stdafx.h"
#include "slist.h"


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


List_Entry* remove(List_Entry *item)
{
	item->next->prev = item->prev;
	item->prev->next = item->next;
	return item;
}


bool isEmpty(PList_Head list)
{
	return list->next == list;
}


int debugCounter = 0;