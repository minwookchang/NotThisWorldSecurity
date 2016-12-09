//Single Linked List Exercise


#include <stdio.h>

typedef struct IntElement {
	struct IntElement *next;
	int data;
}IntElement;

bool insertInFront(IntElement **head, int data) {

	IntElement *newElement = new IntElement;

	//must?
	if (!newElement)
		return false;

	newElement->data = data;
	newElement->next = *head;

	*head = newElement;
	return true;
}

bool deleteElement(IntElement **head, IntElement *deleteMe) {

	IntElement *newElem = *head;

	//if haed
	if (*head == deleteMe) {
		*head = newElem->next;
		delete head;
		return true;
	}

	while (newElem != NULL) {
		if (newElem->next == deleteMe) {
			newElem->next = deleteMe->next;
			delete deleteMe;
			return false;
		}
		newElem = newElem->next;
	}
	return false;
}

void deleteList(IntElement **head) {

	IntElement *deleteMe = *head;

	while (deleteMe) {
		IntElement *next = deleteMe->next;
		delete deleteMe;
		deleteMe = next;
	}
	*head = NULL;
}



void main() {

	printf("hello world");
	return;
}