#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define SIZE 1000
#define MAX 1000
#include<stdio.h>
#include<conio.h>
#include<string.h>
#include<stdlib.h>
#include "structs.h"

unsigned long hash(char *str)
{
	unsigned long hash = 5381;
	int c;

	while (c = *str++)
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

	return hash%SIZE;
}

struct column * create_column(int column_num, char *data){
	struct column *p = (column*)malloc(sizeof(column));
	p->colnum = column_num;
	p->data = (char*)malloc(sizeof(char)*MAX);
	strcpy(p->data, data);
	p->version = 1;
	p->next = NULL;
	p->old_version = NULL;
	return p;
}

struct row *get(struct table_type *table_t, int id)
{
	if (id < 0 || table_t->table[id] == NULL)return NULL;
	return table_t->table[id];
}

void put(struct table_type *table_t, int id, int num_of_col, struct data_set **data, int current_version)
{
	int flag = 1;
	// Row doesnt exits
	if (table_t->table[id] == NULL)
	{
		table_t->table[id] = (row*)malloc(sizeof(row));
		table_t->table[id]->commit_version = current_version;
		table_t->table[id]->value = NULL;

		for (int i = 0; i <= num_of_col; i++)
		{
			if (table_t->table[id]->value == NULL)
			{
				table_t->table[id]->value = create_column(data[i]->column_num, data[i]->value);
			}
			else
			{
				goto update;
			}
		}
	}
	//Row exits
	else
	{
		if (current_version < table_t->table[id]->commit_version)return;
		// Updating data
		if (current_version >= table_t->table[id]->commit_version || current_version == -1)
		{
			table_t->table[id]->commit_version = current_version;
		update:
			// column Exits flag
			int found_col = 0;
			struct column *p = table_t->table[id]->value;
			struct column *q = NULL;
			for (int i = 0; i <= num_of_col; i++)
			{
				q = NULL;
				p = table_t->table[id]->value;
				found_col = 0;
				while (!found_col&&p != NULL)
				{
					if (data[i]->column_num == p->colnum)
					{
						//Column Exits

						found_col = 1;
						struct column *tmp = create_column(data[i]->column_num, data[i]->value);
						if (q)q->next = tmp;
						else {
							table_t->table[id]->value = tmp; break;
						}
						tmp->version = p->version + 1;
						tmp->next = p->next;
						// Moving to old version
						tmp->old_version = p;
						break;
					}
					q = p;
					p = p->next;
				}
				if (!found_col)
				{
					// Column doesnot exits
					//table_t->MAX_no_of_cols++;
					//Traversing till last
					//p = table_t->table[id]->value;
					//while (p->next != NULL)
					//p = p->next;

					// Creating column
					struct column *tmp = create_column(data[i]->column_num, data[i]->value);
					// Updating in table
					q->next = tmp;
				}
			}
			//if (g_flag)goto come_back;
		}
	}
	table_t->size++;
}

void delete_row(struct column *list)
{
	/*if (current == NULL)return;
	delete_row(current->next);
	free(current);*/
	if (list == NULL)return;
	delete_row(list->old_version);
	delete_row(list->next);
	free(list->data);
	free(list->old_version);
	list->old_version = NULL;
	free(list);
}

void print_column(struct column *list, char** cols){
	printf(" Column : Data\n");
	while (list != NULL){
		if (list->colnum >= 0)
			printf(" %s : %s\n", cols[list->colnum], list->data);
		list = list->next;
	}
}

void init(struct table_type *table_t)
{
	table_t->table = (row **)malloc(sizeof(row*)*MAX);
	for (int i = 0; i < MAX; i++)
		table_t->table[i] = NULL;
}

int get_current_version(table_type *table_t, int id){
	return	table_t->table[id] == NULL ? 1 : table_t->table[id]->commit_version;
}

struct column * get_column(table_type *table_t, int id, int colnum){
	struct column *tmp = table_t->table[id]->value;
	for (int i = 0; i < table_t->MAX_no_of_cols; i++){
		if (tmp == NULL)break;
		if (tmp->colnum == colnum){
			return tmp;
		}
		tmp = tmp->next;
	}
	return NULL;
}

int get_table_index(table_type **tables, char *table_name, int tables_count){
	for (int i = 0; i < tables_count; i++){
		if (!strcmp(tables[i]->name, table_name))return i;
	}
	return -1;
}

void dump(table_type *table_t, primarykey **primary_keys, int primary_keys_count){
	FILE *fp = NULL;
	struct column *tmp;
	int flag = 1;
	for (int cur_col = 0; cur_col < table_t->MAX_no_of_cols; cur_col++){
		fp = fopen(table_t->column_names[cur_col], "w");
		for (int i = 0; i < primary_keys_count; i++){
			tmp = get_column(table_t, primary_keys[i]->id, cur_col);
			if (tmp != NULL){
				fputs(tmp->data, fp);
				fputs("\n", fp);
			}
		}
		fclose(fp);
	}
}

int is_primary_key(char *buff){
	for (int i = 0; buff[i] != '\0'; i++){
		if (buff[i] == '-')return 1;
	}
	return 0;
}

void strip(char *text, char delim){
	size_t i = 0;
	for (i = 0; text[i] != delim&&i < strlen(text); i++);
	text[i] = '\0';
}

int get_primary_key(primarykey **primar_keys, char *col_val, int primar_key_count){
	for (int i = 0; i < primar_key_count; i++){
		if (!strcmp(primar_keys[i]->col_value, col_val))return i;
	}
	return -1;
}

struct table_type * create_table(char*buff){
	struct table_type *tmp = (table_type*)malloc(sizeof(table_type));
	tmp->name = (char*)malloc(sizeof(char)*MAX);
	tmp->MAX_no_of_cols = 0;
	init(tmp);
	strcpy(tmp->name, buff);
	tmp->size = 0;
	tmp->column_names = (char**)malloc(sizeof(char*)*MAX);
	for (int i = 0; i < MAX; i++){
		tmp->column_names[i] = (char*)malloc(sizeof(char)*MAX);
	}
	return tmp;
}
