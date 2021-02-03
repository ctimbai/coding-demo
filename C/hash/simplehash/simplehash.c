#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "simplehash.h"

// 初始化 hashtable
table * init_hash_table()
{
    int i;

    table *t = (table *)malloc(sizeof(table));
    if (t == NULL) {
        printf("init hash table is failed!\n");
        return NULL;
    }

    t->cap = MAX_ENTRY_NUMS;
    t->count = 0;
    for (i = 0; i < MAX_ENTRY_NUMS; i ++) {
        t->table = (entry *)malloc(MAX_ENTRY_NUMS * sizeof(entry));
        if (t->table != NULL) {
            memset(t->table, 0, MAX_ENTRY_NUMS * sizeof(entry));
        }
    }

    for (i = 0; i < MAX_ENTRY_NUMS; i ++) {
        data_table[i] = malloc(MAX_STR_LEN);
        memset(data_table[i], 0, MAX_STR_LEN);
    }
    return t;
}

// 从 hashtable 查询数据
int find_hash_table(table *t)
{

}

// 向 hashtable 插入数据
int insert_hash_table(table *t, int key, char *value, int len)
{
    int index, pos;

    if (NULL == t) {
        return 0;
    }

    if (t->count >= MAX_ENTRY_NUMS) {
        printf("hash table is full!\n");
        return 0;
    }

    index = hash_key(key);
    if (t->table[index].key == key) {
        printf("index: %d, key is exit!, orikey: %d, nowkey: %d\n", index, t->table[index].key, key);
        return 0;
    }

    pos = index;
    while (t->table[pos].flag == 1) {
        pos = (++ pos) % MAX_ENTRY_NUMS;
    }

    t->table[pos].key = key;
    memcpy(data_table[pos], value, len);
    t->table[pos].value = data_table[pos];
    t->table[pos].flag = 1;
    t->count ++;
    return 1;
}

// 计算 hash key
int hash_key(int key)
{
    return (key % MAX_ENTRY_NUMS);
}

void print_hash(table *t, int len)
{
    int i;
    printf("count: %d\n", t->count);
    for (i = 0; i <= len; i ++) {
        printf("key: %d, value: %s\n", t->table[i].key, t->table[i].value);
    }

    printf("table:\n");
    for (i = 0; i <= len; i ++) {
        printf("value: %s\n", data_table[i]);
    }
}

int main(void)
{
    int i;
    table *t = init_hash_table();
    //print_hash(t, 5);
    char *value[5] = {"ab", "bc", "cd", "de", "ef"};
    for (i = 1; i <= 5; i ++) {
        insert_hash_table(t, i, value[i-1], 2);
    }

    print_hash(t, 5);

    return 0;
}