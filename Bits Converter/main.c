#include <stdio.h>
#include <stdlib.h>
#include "bits.h"

struct Node
{
  //linked list node structure
  unsigned int inputNum;
  unsigned int binaryOrig;
  unsigned int binaryMirror;
  unsigned int count;
  struct Node *next;
};

void push(struct Node** head_ref, unsigned int new_data)
{
  //add a new node to linkedlist without sorting
    struct Node* new_node = (struct Node*) malloc(sizeof(struct Node));
  
    new_node->binaryMirror = BinaryMirror(new_data);
    new_node->binaryOrig = BinaryMirror(new_node->binaryMirror);
    new_node->inputNum = new_data;
    new_node->count = SequenceCount(new_data);
    new_node->next = (*head_ref);

    (*head_ref)    = new_node;
}

void deleteHeadNode(struct Node **head_ref) 
{ 
   // If linked list is empty 
   if (*head_ref == NULL) 
      return; 
  
   // Store head node 
   struct Node* temp = *head_ref; 
  
   
    *head_ref = temp->next;   // Change head 
    free(temp);               // free old head 
    return; 
    
}  

void swap(struct Node *a, struct Node *b)
{
    unsigned int temp = a->binaryMirror;
    a->binaryMirror = b->binaryMirror;
    b->binaryMirror = temp;

    temp = a->binaryOrig;
    a->binaryOrig = b->binaryOrig;
    b->binaryOrig = temp;

    temp = a->inputNum;
    a->inputNum = b->inputNum;
    b->inputNum = temp;

    temp = a->count;
    a->count = b->count;
    b->count = temp;
}

int isNumber(char k){
    if (k!='0'&&k!='1'&&k!='2'&&k!='3'&&k!='4'&&k!='5'&&k!='6'&&k!='7'&&k!='8'&&k!='9')
        return 0;
    else
        return 1;
}


void bubbleSort(struct Node *start)
{
  //sort according to the ASCII values of the output
    int swapped, i, k = 0;
    struct Node *ptr1;
    struct Node *lptr = NULL;
    char str1[20];
    char str2[20];

    if (start == NULL)
        return;

    do
    {
        swapped = 0;
        ptr1 = start;

        while (ptr1->next != lptr)
        {
            int done = 1;
            k=0;
            //take decimal numbers as string to compare each char of numbers easily
            sprintf(str1, "%u", ptr1->binaryMirror);
            sprintf(str2, "%u", ptr1->next->binaryMirror);

            //continue loop if any of the two numbers has a digit left not calculated
            while((isNumber(str1[k]) || isNumber(str2[k])) && done){
	      
                if (str1[k] > str2[k])
                {
                    swap(ptr1, ptr1->next);
                    swapped = 1;
                    done=0;

                }else if(str1[k]==str2[k]){
                    k++;
                }else if(str1[k] < str2[k]){
                    done=0;
                }

		else if (!isNumber(str1[k]) || !isNumber(str2[k])){
		  //a case for numbers such as 12345678 vs. 1234567 
                  //1234567 comes first alphabetically 
                    if(isNumber(str1[k])){
                    swap(ptr1, ptr1->next);
                    swapped = 1;
                    done=0;} else if(isNumber(str2[k])){done=0;}                  
		    }
            }


            ptr1 = ptr1->next;
        }
        lptr = ptr1;
    }
    while (swapped);
}

void printList(struct Node *start, FILE *fout)
{
  //prints the linkedlist to output file
    struct Node *temp = start;
    while (temp!=NULL)
    {
        fprintf(fout, "%-15u %-15u\n", temp->binaryMirror, temp->count);
        temp = temp->next;
    }
}

int main(int argc, char *argv[])
{

    if (argc!=3)
        printf("Please enter the input file name and the output file name.");
    else{

        FILE *fp;
        FILE * fout;

        fp = fopen(argv[1], "r"); // read mode
        int i=0;
        struct Node* head = NULL;


        if (fp == NULL)
        {
          perror("Error while opening the input file.\n");
          exit(EXIT_FAILURE);
        }
        else{
	  //read and add every decimal number from file to linked list
            fscanf (fp, "%u", &i);
            push(&head,i);

            while (fgetc(fp) != EOF)
            {

                fscanf (fp, "%u", &i);
                push(&head,i);

            }
            fclose(fp);
            //last number is included twice because of the loop so delete head and fix that
            deleteHeadNode(&head);
            bubbleSort(head);
            fout = fopen(argv[2], "w");

            if(fout == NULL)
            {

                printf("Unable to create file.\n");
                exit(EXIT_FAILURE);
            }
            printList(head, fout);

        }

    }
    return 0;

}
