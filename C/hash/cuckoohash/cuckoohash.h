#ifndef _CUCKOOHASH_H_
#define _CUCKOOHASH_H_

#define TABLECOUNT 16

struct hashEntry {
    int key;
    int value;
};

typedef struct hashEntry entry;

enum {MaxLoop = 1000};

struct cuckooHashTable {
    int nCount;             // size of hash table
    entry *table1;          // the first hash table
    entry *table2;          // the second hash table
    //enum {MaxLoop = 1000};  // used to control rehash loop
};

typedef struct cuckooHashTable table;

// 判断一个数是否是质数
int isPrime(int value);

// 得到最小的质数
int getMinPrime(int value);


// 全域散列函数
// h(a,b,k) = ((a*k+b) mod p) mod m
int hashFunc(table *t, int key);

// 除法散列函数
int hashFuncDiv(table *t, int key);

// 乘法散列
// floor表示下取整
int hashFuncFloor(table *t, int key);

// first hash function
// h1(key) = key % size
int hashFunc1(table *t, int key);

// second hash function
// h2(key) = (key / size) % size
int hashFunc2(table *t, int key);

// 初始化hashtable
void initHashTable(table *t, int size);

// 释放hashtable
void freeHashTable(table *t);

// 向hashtable插入数据
int insertHashTable(table *t, entry e);

// 从hashtable删除数据
entry* delEntry(table *t, int key);

// 从hashtable中查找数据
int findValueByKey(table *t, int key);

// 重新计算hash
int reHash(table *t, entry e, int deeps);

// 打印hashtable的内容
void printTable(table *t);

#endif //_CUCKOOHASH_H_