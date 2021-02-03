#ifndef _SIMPLE_HASH_H_
#define _SIMPLE_HASH_H_

struct hash_entry {
    int key;
    char *value;
    int flag;
};

typedef struct hash_entry entry;

struct hash_table {
    int count;
    int cap;
    entry *table;
};


typedef struct hash_table table;

#define MAX_ENTRY_NUMS 11
#define MAX_STR_LEN 10

char *data_table[MAX_ENTRY_NUMS];

//int index[MAX_ENTRY_NUMS];



// 初始化 hashtable
table* init_hash_table();

// 从 hashtable 查询数据
int find_hash_table(table *t);

// 向 hashtable 插入数据
int insert_hash_table(table *t, int key, char *value, int len);

// 计算 hash key
int hash_key(int key);

void print_hash(table *t, int len);

#endif //_SIMPLE_HASH_H_