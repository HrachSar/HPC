#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

typedef struct node
{
	struct node *next;
	void *val;
} node;


node *head;


void init_list(int val){
	head = (node *)malloc(sizeof(node));
	int *data = (int *)malloc(sizeof(int));
	*data = val;
	(head)->val = (void *)(data);
	(head)->next = NULL;
}

void add_node(node **head, node *next){
	if(next == NULL)
		return;
	if (*head == NULL){
		*head = next;
	}
	else{
		next->next = *head;
		*head = next;
	}
}

void print_list(node **head){
	if(*head == NULL)
		return;

	node *curr = *head;
	while (curr != NULL)
	{
		printf("%d ", *((int *)(curr->val)));
		curr = curr->next;
	}
	printf("\n");

}
void free_list(node **head){

	if(*head == NULL)
		return;

	node *curr = *head;
	node *prev = NULL;

	while(curr != NULL){
		prev = curr;
		curr = curr->next;
		free(prev);
	}
}

int main(){
	init_list(5);
	printf("value of node = %d\n", *((int *)((head)->val)) );
	int val1 = 3;
	int val2 = 4;
	int val3 = 7;

	node *n1 = (node *)malloc(sizeof(node));
	n1->val = (void *)(&val1);
	n1->next = NULL;


	node *n2 = (node *)malloc(sizeof(node));
	n2->val = (void *)(&val2);
	n2->next = NULL;


	node *n3 = (node *)malloc(sizeof(node));
	n3->val = (void *)(&val3);
	n3->next = NULL;

	add_node(&head, n1);
	add_node(&head, n2);
	add_node(&head, n3);

	print_list(&head);

	free_list(&head);
	return 0;
}
