#ifndef __NASCenter_Util_List_H__
#define __NASCenter_Util_List_H__ 

typedef struct list List;
typedef struct element Element;

struct element {
	Element *nextElement;
	void *value;
};

struct list {
	Element *start;
	Element *end;
	int size;
};

#ifdef UTIL_LIST_PUBLIC
UTIL_LIST_PUBLIC List *initList();
UTIL_LIST_PUBLIC int addToList(List *list, void *elementValue);
UTIL_LIST_PUBLIC int deleteFromList(List *list, void *elementValue);
UTIL_LIST_PUBLIC int insertIntoList(List *list, int index, void *elementValue);
#else
#define UTIL_LIST_PUBLIC extern
UTIL_LIST_PUBLIC List *initList();
UTIL_LIST_PUBLIC int addToList(List *list, void *elementValue);
UTIL_LIST_PUBLIC int deleteFromList(List *list, void *elementValue);
UTIL_LIST_PUBLIC int insertIntoList(List *list, int index, void *elementValue);
#endif

#endif
