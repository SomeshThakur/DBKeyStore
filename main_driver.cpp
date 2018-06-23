#include"api.h"
int main()
{
	table_type **tables = (table_type**)malloc(sizeof(table_type*)*MAX);
	primarykey **primary_keys = (primarykey**)malloc(sizeof(primarykey*)*MAX);
	char *buff = (char*)malloc(sizeof(char)*MAX);
	int index = 0;
	int no_of_cols = 0;
	int ch = 0;
	int id = 0, pi = 0, p_index = 0;
	int current_version = 1;
	struct data_set **data;
	struct column *list;
	struct row *tmp;
	int it;
	int MAX_no_of_cols = 0;

	do{
		system("cls");
		printf("\n\n\t1. Create Table\n");
		printf("\t2. Delete Table\n");
		printf("\t3. PUT/INSERT\n");
		printf("\t4. GET\n");
		printf("\t5. DELETE ROW\n");
		printf("\t6. DUMP TABLE\n");
		printf("\t-1. Exit\n");
		printf("    Enter your choice : ");
		scanf("%d", &ch);
		switch (ch)
		{
		case 1:
			system("cls");
			printf("\n\tEnter Table Name : ");
			scanf("%s", buff);
			tables[index] = create_table(buff);
			printf("\n\tEnter intial Number of cols : ");
			scanf("%d", &(tables[index]->MAX_no_of_cols));
			printf("\n\tEnter Names of cols (Add -p to make it primary key)\n\tExample username-p\n");
			for (int i = 0; i < tables[index]->MAX_no_of_cols; i++)
			{
				printf("\t%d Column Name :", i + 1);
				scanf("%s", tables[index]->column_names[i]);
				if (is_primary_key(tables[index]->column_names[i])){
					strip(tables[index]->column_names[i], '-');
					pi = i;
				}
			}
			index++;
			printf("\n\tTable created successfully!\n");
			system("pause");
			break;
		case 2:
			printf("\n\tEnter table name : ");
			scanf("%s", buff);
			it = get_table_index(tables, buff, index);
			if (it >= 0){
				tables[it] = NULL;
				printf("\n\tTable deleted sucessfully!\n");
			}
			else {
				printf("\n\tTable doesnt exits\n");
				system("pause");
			}
			break;
		case 3:

			// PUT

			system("cls");
			printf("\n\tEnter Table Name : ");
			scanf("%s", buff);
			it = get_table_index(tables, buff, index);
			if (it >= 0)
			{
				printf("\n\tSchema of table %s(", tables[it]->name);
				for (int i = 0; i < tables[it]->MAX_no_of_cols; i++){
					printf("index=%d : %s,", i + 1, tables[it]->column_names[i]);
				}
				printf(")\n");
			re_scan:
				printf("\n\tEnter Number of columns you want to insert : ");
				scanf("%d", &no_of_cols);
				if (no_of_cols >= tables[it]->MAX_no_of_cols)
				{
					printf("\n\tEnter less then %d columns only!!\n\n", tables[it]->MAX_no_of_cols);
					goto re_scan;
				}
				data = (data_set**)malloc(sizeof(data_set)*no_of_cols);

				// Primary Key

				data[0] = (data_set*)malloc(sizeof(data_set));
				printf("\n\tEnter %s Primary Column Value : ", tables[it]->column_names[pi]);
				data[0]->value = (char*)malloc(sizeof(char)*MAX);
				scanf("%s", (data[0]->value));
				data[0]->column_num = pi;
				primary_keys[p_index] = (primarykey*)malloc(sizeof(primarykey));
				primary_keys[p_index]->id = id;
				primary_keys[p_index]->col_value = (char*)malloc(sizeof(char)*MAX);
				strcpy(primary_keys[p_index]->col_value, data[0]->value);


				// Asking for data 
				for (int i = 1; i <= no_of_cols; i++)
				{
					data[i] = (data_set*)malloc(sizeof(data_set));
				up:
					printf("\n\tEnter Column Index : ");
					scanf("%d", &(data[i]->column_num));
					data[i]->column_num -= 1;
					if (data[i]->column_num == pi){
						printf("\n\tCan't write primary again in single query!\n");
						goto up;
					}
					printf("\tEnter Column value : ", i + 1);
					data[i]->value = (char*)malloc(sizeof(char)*MAX);
					scanf("%s", data[i]->value);
				}


				current_version = get_current_version(tables[it], id);
				put(tables[it], primary_keys[p_index]->id, no_of_cols, data, current_version);
				id++;
				p_index++;
				current_version++;
			}
			else {
				printf("\n\tTable doesnt exits\n");
				system("pause");
			}
			break;
		case 4:
			system("cls");
			printf("\n\tEnter Table Name : ");
			scanf("%s", buff);
			it = get_table_index(tables, buff, index);
			if (it >= 0){
				printf("\n\tEnter primary key value ( %s ) : ", tables[it]->column_names[pi]);
				scanf("%s", buff);
				id = get_primary_key(primary_keys, buff, p_index);
				tmp = get(tables[it], id);
				if (tmp == NULL)
					printf("\n\tColumn Does Not Exits \n");
				else{
					list = tmp->value;
					system("cls");
					print_column(list, tables[it]->column_names);
				}system("pause");
			}
			else {
				printf("\n\tTable doesnt exits\n");
				system("pause");
			}
			break;
		case 5:
			system("cls");
			printf("\n\tEnter Table Name : ");
			scanf("%s", buff);
			it = get_table_index(tables, buff, index);
			if (it >= 0){
				printf("\n\tEnter primary key value ( %s ) : ", tables[it]->column_names[pi]);
				scanf("%s", buff);
				id = get_primary_key(primary_keys, buff, p_index);
				tmp = get(tables[it], id);
				if (tmp == NULL)
					printf("\n\tColumn Does Not Exits \n");
				else
				{
					delete_row(tmp->value);
					tables[it]->table[id] = NULL;
					printf("\n\tDeleted successfully\n");
					system("pause");
				}
			}
			else {
				printf("\n\tTable doesnt exits\n");
				system("pause");
			}
			break;
		case 6:
			system("cls");
			printf("\n\tEnter Table Name : ");
			scanf("%s", buff);
			it = get_table_index(tables, buff, index);
			if (it >= 0){
				dump(tables[it], primary_keys, p_index);
			}
			else {
				printf("\n\tTable doesnt exits\n");
				system("pause");
			}
		default:
			break;
		}
	} while (ch != -1);
	return 0;
}
