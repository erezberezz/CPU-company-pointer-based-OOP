#define _CRT_SECURE_NO_WARNINGS
#define NAME_LENGTH 200
#define CURR 1
#define PREV 0
#define NEXT 2
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct cpu
{
	char name[NAME_LENGTH];
	int copies;
	struct cpu *next;
}CPU;

//input: name of the cpu (string), number of copies, pointer to our head cpu//
//return: cpu pointer//
//used in order to sort this list in the desired order//
CPU* add_node(char* name, int copies, CPU* head){
	CPU* newcpu = (CPU *)malloc(sizeof(CPU));
	CPU *prev = NULL, *curr = head;
	//mem check
	if (newcpu == NULL) {
		printf("Memory allocation failed\n"); return 1;
	}

	strcpy(newcpu->name, name);
	newcpu->copies = copies;
	if (curr == NULL) {
		newcpu->next = NULL;
		head = newcpu;
		return newcpu;
	}
	if (strcmp(newcpu->name, curr->name) < 0)
	{
		newcpu->next = curr;
		return newcpu;
	}
	while (curr != NULL && strcmp(newcpu->name, curr->name) > 0)
	{
		prev = curr;
		curr = curr->next;
	}
	prev->next = newcpu;
	newcpu->next = curr;
	return head;
}

//input: String//
//return: Nothing(void)//
//removes the spaces leading up to and trailing in the string - used in order to compare between them later.//
void clean_str(char *str){
	int  i, j;
	for (i = 0; str[i] == ' ' || str[i] == '\t'; i++);
	for (j = 0; str[i]; i++)
		str[j++] = str[i];
	str[j] = '\0';
	for (i = 0; str[i] != '\0'; i++) {
		if (str[i] != ' '&& str[i] != '\t')
			j = i;
	}
	str[j + 1] = '\0';
}


//input: head of linked CPU list,name, pos//
//return: CPU pointer//
//goes through our linked list and returns a pointer depending on the POS value we choose//
//CURR for the target CPU, PREV for the one before it and NEXT for the one after it//
CPU* iter_list(CPU* cpu_list, char* name, int pos) {
	CPU* temp = cpu_list, *prev=NULL;
	while (temp != NULL){
		if (strcmp(temp->name, name) == 0)
			switch (pos) {
			case CURR:
				return temp;
			case PREV:
				return prev;
			case NEXT:
				return temp->next;
			}
		prev = temp;
		
		temp = temp->next;
	}
	return NULL;
}
//input: name, copies, pointer to our list
//return: returns our list
//works by searching the list for our bad cpu, if found - deducts the number of bad copies, if not - does nothing.
CPU* fatal_malf(char *bad_name, int bad_copies, CPU* cpu_list){
	CPU* target = iter_list(cpu_list, bad_name,CURR);
	if (target != NULL){
		if (target->copies < bad_copies)
			target->copies = 0;
		else 
			target->copies -= bad_copies;
		return cpu_list;
	}
	return cpu_list;
}
//input: name, copies, pointer to list
//return: returns our list
//searches list for our returned cpu, if found - adds to its number of copies, if not - adds it in the required order
CPU* return_from_cus(char* cpu_name, int copies, CPU* cpu_list) {
	CPU* target = iter_list(cpu_list, cpu_name, CURR);
	if (target != NULL)
		target->copies += copies;
	else
		return add_node(cpu_name, copies, cpu_list);	
	return cpu_list;
}
//input: all of our files and list pointer
//return: the head of our linked list
//reads our cpus file, cleans up the names for future comparsion, and creating the list in the required order
CPU* init(FILE *f_cpus, FILE *f_actions, FILE *f_output, CPU *cpu_list){
	CPU *new = NULL, *head = NULL;
	char cpu[NAME_LENGTH], *elim;
	int copies;
	//initing the head node
	fgets(cpu, NAME_LENGTH, f_cpus);
	if (feof(f_cpus)) //used in case of empty cpus.txt
		return cpu_list;
	elim = strtok(cpu, "$");
	clean_str(elim);
	copies = atoi(strtok(NULL, "$"));
	head = add_node(elim, copies, NULL);
	while (!feof(f_cpus)) {
		fgets(cpu, NAME_LENGTH, f_cpus);
		elim = strtok(cpu, "$");
		clean_str(elim);
		copies = atoi(strtok(NULL, "$"));
		new = add_node(elim, copies,head);
		if(strcmp(elim,head->name) < 0);
			head = new;
	}
	return new;
}
//input: head of our list
//output: nothing (void)
//goes one by one through our nodes and frees each one untill we reach the head (temp->next=NULL)
void free_list(CPU* cpu_list) {
	CPU* temp = cpu_list, *prev;
	while (temp != NULL) {
		prev = temp;
		temp = temp->next;
		free(prev);
	}
}

//input: file pointer and linked list//
//return: Nothing(void)//
//prints our linked list to file f_output//
void print_output(CPU* cpu_list, FILE *f_output){
	CPU* temp = cpu_list;
	while (temp->next != NULL){
		fprintf(f_output, "%s $$$ %d\n", temp->name,temp->copies);
		temp = temp->next;
	}
	fprintf(f_output, "%s $$$ %d", temp->name, temp->copies);
	//^used in order to print the last line without an extra \n
}

void read_actions(FILE *f_cpus, FILE *f_actions, FILE *f_output, CPU *cpu_list){
	char line[NAME_LENGTH], cpu_name[NAME_LENGTH] = { NULL }, new_name[NAME_LENGTH] = { NULL }, *elim = NULL;
	CPU *target, *prev, *next;
	int cpu_copies = 0;
	//Input: all of our files
	//returns: None (void)
	//reads our actions file line by line, strips "$$$" chars and acts upon actions 
	while (!feof(f_actions))
	{
		fgets(line, NAME_LENGTH, f_actions); 
		elim = strtok(line, "$");
		while (elim != NULL)
		{
			if (strcmp("Initialize\n", elim) == 0)
				cpu_list = init(f_cpus, f_actions, f_output, cpu_list);

			if (strcmp("Fatal_malfunction ", elim) == 0){
				strcpy(cpu_name, strtok(NULL, "$"));
				clean_str(cpu_name);
				cpu_copies = atoi(strtok(NULL, "$"));
				fatal_malf(cpu_name, cpu_copies,cpu_list);
			}
			if (strcmp("Returned_from_customer ", elim) == 0){
				strcpy(cpu_name, strtok(NULL, "$"));
				clean_str(cpu_name);
				cpu_copies = atoi(strtok(NULL, "$"));
				cpu_list=return_from_cus(cpu_name, cpu_copies, cpu_list);
			}
			if (strcmp("Rename ", elim) == 0){
				//here we are finding the prev and the next CPU to our target in our list, linking the prev cpu to the next cpu//
				//and then freeing our target cpu and re-insterting it with the new name and copies so it would be inserted correctly by alphabet.//
				strcpy(cpu_name, strtok(NULL, "$"));
				strcpy(new_name, strtok(NULL, "$\n"));
				clean_str(new_name);  
				clean_str(cpu_name);
				target = iter_list(cpu_list, cpu_name,CURR);
				if (target != NULL) {
					cpu_copies = target->copies;
					prev = iter_list(cpu_list, cpu_name, PREV);
					next = iter_list(cpu_list, cpu_name, NEXT);
					prev->next = next;
					free(target);
					add_node(new_name, cpu_copies, cpu_list);
				}
				}
				if (strcmp("Finalize", elim) == 0){
				//write all to files, free list, close flies//
				print_output(cpu_list,f_output);
				free_list(cpu_list);
				fclose(f_actions); fclose(f_cpus); fclose(f_output);
				return 0;
				}
				elim = strtok(NULL, "$");
		}
	}
	fclose(f_actions);
}

int main(int argc, char *argv[]){
	FILE *f_actions = NULL; FILE *f_cpus = NULL; FILE *f_output = NULL;
	CPU *cpu_list = NULL;
	if (argc != 4)
	{
		printf("Error: invalid number of arguments (<%d> instead of 3)\n", (argc - 1));
		return 1;
	}
	//our argv order is 0- program name 1-cpus 2-actions 3-updated_cpus 4- NULL//
	//opening files://
	f_cpus = fopen(argv[1], "r"); 
	f_actions = fopen(argv[2], "r");
	f_output = fopen(argv[3], "w");
	//checking if they have succesfully opened//
	if (f_cpus == NULL)
	{
		printf("Error: opening %s failed\n", argv[1]);
		return 1;
	}
	if (f_actions == NULL)
	{
		printf("Error: opening %s failed\n", argv[2]);
		return 1;
	}
	if (f_output == NULL)
	{
		printf("Error: opening %s failed\n", argv[3]);
		return 1;
	}
	read_actions(f_cpus, f_actions, f_output, cpu_list);
}