#define UTIL_LIST_PUBLIC
#include "NASCenter_Util_List.h"
#include <stdlib.h>

List *initList() {
	List *list;
	list = (List *) malloc(sizeof(List));
	list->start = NULL;
	list->end = NULL;
	list->size=0;
	return list;
}

int addToList(List *list, void *elementValue){
	Element *endElement = NULL;
	Element * element = malloc(sizeof(Element));
	if(element!=NULL){
		element->nextElement=NULL;
		element->value = elementValue;
		if(list->start==NULL){
			list->start=element;
		} else{
			endElement = list->end;
			endElement->nextElement = element;
		}
		list->end = element;
		list->size++;
	} else {
		return -1;
	}
	return 0;
}

int deleteFromList(List *list, void *elementValue){
	Element *current, *nextElement;
	Element *previousElement = NULL;
	current = list->start;
	if(current== NULL){
		return -1;
	}
	nextElement=list->start->nextElement;
	while(current){
		if(current->value==elementValue){
			if(previousElement){
				previousElement->nextElement = nextElement;
			}
			if(current==list->start){
				list->start = nextElement;
			}
			if(current==list->end){
				list->end = previousElement;
			}
			list->size--;
			free(current);
			current=NULL;
			return 1;
		}
		previousElement = current;
		current = nextElement;
		nextElement = nextElement->nextElement;
	} // End of while(current)
}

int insertIntoList(List *list, int index, void *elementValue){
	Element *current, *newElement;
	int i=0;
	if(index>=list->size){
		return -1;
	}
	current = list->start;
	for(i=0;i<index;i++){
		current = current->nextElement;
	}
	newElement = (Element * ) malloc(sizeof(Element));
	if(newElement) {
		newElement->value = elementValue;
		newElement->nextElement = current->nextElement;
		current->nextElement = newElement;
	}else {
		return -1;
	}
	return 0;
}

/*main() {
	List *list;
	Element *temp;
	char *e1 = "Element1";
	char *e2 = "Element2";
	char *e3 = "Element3";
	list = initList();
	addToList(list,e1);
	addToList(list,e2);
	addToList(list,e3);
	printf("List size %d\n" , list->size);
	temp = list->start;
	while(temp){
		printf("List val %s\n",temp->value);
		temp = temp->nextElement;
	}
	deleteFromList(list,e2);
	printf("List size %d\n" , list->size);
	temp = list->start;
	while(temp){
		printf("List val %s\n",temp->value);
		temp = temp->nextElement;
	}
	insertIntoList(list,0,e2);
	temp = list->start;
	while(temp){
		printf("List val %s\n",temp->value);
		temp = temp->nextElement;
	}
}*/


