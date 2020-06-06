#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#define STRING_MAX 256

typedef struct Item Item;

struct Item {
	char id[STRING_MAX];
	char name[STRING_MAX];
	float price;
	int quantity;
};

void Item_Free(void *item) {
	free(item);
	item=NULL;
}

typedef struct List List;

struct List {
	void *data;
	List *next;
};

List *head=NULL;

List *List_New(void *data,List *next) {
	List *list=malloc(sizeof(List));
	if(list) {
		list->data=data;
		list->next=next;
	}
	return list;
}

void List_Free(List **head,void (*Data_Free)(void*)) {
	List *curr=*head;
	List *next;
	while(curr) {
		next=curr->next;
		Data_Free(curr->data);
		free(curr);
		curr=next;
	}
	*head=NULL;
}

int List_Length(List *head) {
	int length=0;
	List *iter=head;
	while(iter) {
		length++;
		iter=iter->next;
	}
	return length;
}

void List_PushFront(List **head,void *data) {
	(*head)=List_New(data,*head);
}

void List_PushBack(List **head,void *data) {
	List *curr=*head;
	List *newNode=List_New(data,NULL);
	if(curr==NULL) {
		*head=newNode;
	} else {
		while(curr->next) curr=curr->next;
		curr->next=newNode;
	}
}

List *List_Get(List *head,int n) {
	int i=0;
	List *iter=head;
	while(iter) {
		if(n==i) return iter;
		i++;
		iter=iter->next;
	}
	return NULL;
}

void List_Set(List **head,int n,void *data,void (*Data_Free)(void*)) {
	List *item=List_Get(*head,n);
	if(item) {
		Data_Free(item->data);
		item->data=data;
	}
}

List *List_RemoveAt(List **head,int n) {
	List *before=List_Get(*head,n-1);
	if(before) {
		List *item=before->next;
		before->next=item->next;
		item->next=NULL;
		return item;
	} else {
		List *item=*head;
		if(item) {
			(*head)=(*head)->next;
			item->next=NULL;
			return item;
		}
	}
	return NULL;
}

void List_PrintItems(List *head) {
	List *iter=head;
	printf("/---------|----------------------------------|------------|------\\\n");
	printf("| %-7s | %-32s | %-10s | %-4s |\n","ID","Name","Price","Qty");
	printf("|---------|----------------------------------|------------|------|\n");
	while(iter) {
		Item *item=iter->data;
		printf("| %7s | %-32s | %10.2f | %4d |\n",item->id,item->name,item->price,item->quantity);
		iter=iter->next;
	}
	printf("\\---------|----------------------------------|------------|------/\n\n");
}

void getString(char str[]) {
	fgets(str,STRING_MAX-1,stdin);
	char *p=strchr(str,'\n');
	if(p) *p='\0';
}

void flush() {
	int ch;
	while((ch=getchar())!='\n' && ch!=EOF);
}

int menu() {
	int choice;
	printf(
		"--- Menu ---\n"
		"1. List Items\n"
		"2. Add Item\n"
		"3. Edit Item\n"
		"4. Search Item\n"
		"5. Remove Item\n"
		"6. Clear All Items\n"
		"7. Save\n"
		"8. Load\n"
		"9. Quit\n"
	);
	printf("Choice: ");
	scanf("%d",&choice);
	flush();
	return choice;
}

void listAllItems() {
	List_PrintItems(head);
}

void addItem() {
	Item *item=malloc(sizeof(Item));
	printf("Enter item id: "); getString(item->id);
	printf("Enter item name: "); getString(item->name);
	printf("Enter item price: "); scanf("%f",&item->price); flush();
	printf("Enter item quantity: "); scanf("%d",&item->quantity); flush();
	List_PushBack(&head,item);
	printf("Item Added.\n");
}

void editItem() {
	Item *item1=malloc(sizeof(Item));
	Item *item2;
	int i=0;

	printf("Enter item id to edit: "); getString(item1->id);

	List *iter=head;
	while(iter) {
		item2=iter->data;
		if(!strcmp(item1->id,item2->id)) {
			break;
		}
		i++;
		iter=iter->next;
	}

	if(iter) {
		printf("Enter Item id [%s]: ",item2->id); getString(item1->id);
		printf("Enter Item name [%s]: ",item2->name); getString(item1->name);
		printf("Enter Item price [%.2f]: ",item2->price); scanf("%f",&item1->price); flush();
		printf("Enter Item quantity [%d]: ",item2->quantity); scanf("%d",&item1->quantity); flush();

		List_Set(&head,i,item1,Item_Free);

		printf("Item edited.\n");
	} else {
		printf("Item not found.\n");
	}

}

void searchItem() {
	Item *item1=malloc(sizeof(Item));
	Item *item2;

	printf("Enter item id to search: "); getString(item1->id);
	List *iter=head;
	while(iter) {
		item2=iter->data;
		if(!strcmp(item1->id,item2->id)) {
			break;
		}
		iter=iter->next;
	}

	if(iter) {
		printf("Item id: %s\n",item2->id);
		printf("Item name: %s\n",item2->name);
		printf("Item price: %.2f\n",item2->price);
		printf("Item quantity: %d\n",item2->quantity);
	} else {
		printf("Item not found.\n");
	}

	Item_Free(item1);
}

void removeItem() {
	Item *item1=malloc(sizeof(Item));
	Item *item2;
	int i=0;

	printf("Enter item id to remove: "); getString(item1->id);
	List *iter=head;
	while(iter) {
		item2=iter->data;
		if(!strcmp(item1->id,item2->id)) {
			break;
		}
		i++;
		iter=iter->next;
	}

	if(iter) {
		List_RemoveAt(&head,i);
		printf("Item removed.\n");
	} else {
		printf("Item not found.\n");
	}

	Item_Free(item1);
}

void clearAllItems() {
	List_Free(&head,Item_Free);
	printf("Items Cleared.\n");
}

bool ask(char *q) {
	char ans;
	do {
		printf(q);
		ans=tolower(getchar()); flush();
	} while(ans!='y' && ans!='n');
	return ans=='y';
}

void saveFile() {
	char filename[STRING_MAX];
	printf("Save to file: "); getString(filename);
	FILE *fout=fopen(filename,"w");
	List *iter=head;
	while(iter) {
		Item *item=iter->data;
		fprintf(fout,"%s,%s,%.2f,%d\n",item->id,item->name,item->price,item->quantity);
		iter=iter->next;
	}
	fclose(fout);
	printf("Items saved.\n");
}

void loadFile() {
	char filename[STRING_MAX];
	printf("Load from file: "); getString(filename);
	FILE *fin=fopen(filename,"r");
	if(!fin) {
		printf("Error opening file.\n");
		return;
	}
	List_Free(&head,Item_Free);
	while(true) {
		Item *item=malloc(sizeof(Item));
		if(fscanf(fin,"%[^,],%[^,],%f,%d\n",item->id,item->name,&item->price,&item->quantity)==4) {
			List_PushBack(&head,item);
		} else {
			free(item);
			item=NULL;
			break;
		}
	}
	fclose(fin);
	printf("Items loaded.\n");
}

int main(void) {
	bool quit=false;
	int choice;

	while(!quit) {
		choice=menu();
		switch(choice) {
			case 1: listAllItems(); break;
			case 2: addItem(); break;
			case 3: editItem(); break;
			case 4: searchItem(); break;
			case 5: removeItem(); break;
			case 6: if(ask("Are you sure to clear all items (y/n)? ")) clearAllItems(); break;
			case 7: saveFile(); break;
			case 8: loadFile(); break;
			case 9: quit=true; break;
			default: printf("Invalid choice.\n");
		}
	}

	printf("Bye!\n");

	List_Free(&head,Item_Free);

	return 0;
}

