
struct column {
	struct column *next;
	struct column *old_version;
	int version;
	int colnum;
	char *data;
};

struct row{
	int commit_version;
	struct column *value;
};

struct data_set{
	int column_num;
	char *value;
};

struct table_type{
	struct row **table;
	int size;
	int MAX_no_of_cols;
	char *name;
	char **column_names;
};

struct primarykey{
	int id;
	char *col_value;
};
