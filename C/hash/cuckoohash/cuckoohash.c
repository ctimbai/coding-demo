#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <assert.h>

#include "cuckoohash.h"

// 全域散列函数
// h(a,b,k) = ((a*k+b) mod p) mod m
int hashFunc(table *t, int key)
{
    int a, b, p;
    p = 101; //一个足够大的质数

    a = rand() % (p - 1) + 1;
    b = rand() % p;

    return ((a * key + b) % p) % t->nCount;
}

// 除法散列函数
int hashFuncDiv(table *t, int key)
{
    return key % t->nCount;
}

// 乘法散列
// floor表示下取整
int hashFuncFloor(table *t, int key)
{
    int a, p;

    p = 101; //一个足够大的质数
    a = rand() % (p - 1) + 1;
    return floor(t->nCount * (a * key % 1));
}

// 判断一个数是否是质数
int isPrime(int value)
{
    int i;

    if (value <= 0) {
        return 0;
    }

    for (i = 2; i <= sqrt((double)value); i ++) {
        if (value % i == 0) {
            return 0;
        }
    }

    return 1;
}

// 得到最小的质数
int getMinPrime(int value)
{
    while (isPrime(value) == 0) {
        value ++;
    }

    return value;
}

// first hash function
// h1(key) = key % size
int hashFunc1(table *t, int key)
{
    return key % t->nCount;
}

// second hash function
// h2(key) = (key / size) % size
int hashFunc2(table *t, int key)
{
    return (key / t->nCount) % t->nCount;
}

// 初始化hashtable
void initHashTable(table *t, int nSize)
{
    t->nCount = getMinPrime(nSize);

    t->table1 = malloc(t->nCount * sizeof(entry));
    memset(t->table1, 0, sizeof(entry) * t->nCount);

    t->table2 = malloc(t->nCount * sizeof(entry));
    memset(t->table2, 0, sizeof(entry) * t->nCount);
}

// 释放hashtable
void freeHashTable(table *t)
{
    if (t->table1) {
        free(t->table1);
    }
    if (t->table2) {
        free(t->table2);
    }
}

// 向hashtable插入数据
int insertHashTable(table *t, entry e)
{
    int hashKey1, hashKey2;

    if (NULL == t) {
        return -1;
    }

    hashKey1 = hashFunc1(t, e.key);
    hashKey2 = hashFunc2(t, e.key);

    if (t->table1[hashKey1].key == 0) {
        t->table1[hashKey1].key = e.key;
        t->table1[hashKey1].value = e.value;
        return 1;
    }
    else if (t->table2[hashKey2].key == 0) {
        t->table2[hashKey2].key = e.key;
        t->table2[hashKey2].value = e.value;
        return 1;
    }
    else {
        if (reHash(t, t->table1[hashKey1], MaxLoop)) {
            t->table1[hashKey1].key = e.key;
            t->table1[hashKey1].value = e.value;
            return 1;
        }
        else if (reHash(t, t->table2[hashKey2], MaxLoop)) {
            t->table2[hashKey2].key = e.key;
            t->table2[hashKey2].value = e.value;
            return 1;
        }
    }
    return 0;
}

// 从hashtable删除数据
entry* delEntry(table *t, int key)
{
    // todo
}

// 从hashtable中查找数据
int findValueByKey(table *t, int key)
{
    int hashKey1, hashKey2;

    hashKey1 = hashFunc1(t, key);
    hashKey2 = hashFunc2(t, key);

    if (t->table1 && t->table1[hashKey1].key == key) {
        return 1;
    }
    if (t->table2 && t->table2[hashKey2].key == key) {
        return 1;
    }
    return 0;
}

// 重新计算hash
int reHash(table *t, entry e, int deeps)
{
    int hashKey1, hashKey2;

    if (deeps <= 0) {
        return 0;
    }

    hashKey1 = hashFunc1(t, e.key);
    hashKey2 = hashFunc2(t, e.key);

    if (e.key == t->table1[hashKey1].key) {
        if (t->table2[hashKey2].key == 0) {
            t->table2[hashKey2].key = e.key;
            t->table2[hashKey2].value = e.value;
            return 1;
        }
        else {
            if (reHash(t, t->table2[hashKey2], deeps - 1)) {
                t->table2[hashKey2].key = e.key;
                t->table2[hashKey2].value = e.value;
                return 1;
            }
        }
    }
    else if (e.key == t->table2[hashKey2].key) {
        if (t->table1[hashKey1].key == 0) {
            t->table1[hashKey1].key = e.key;
            t->table1[hashKey1].value = e.value;
            return 1;
        }
        else {
            if (reHash(t, t->table1[hashKey1], deeps - 1)) {
                t->table1[hashKey1].key = e.key;
                t->table1[hashKey1].value = e.value;
                return 1;
            }
        }
    }
    return 0;
}

// 打印hashtable的内容
void printTable(table *t)
{
    int i;

    printf("hashtable1:\n");
    for (i = 0; i < t->nCount; i ++) {
        if (t->table1[i].key) {
            printf("index: %d, key: %d, value: %d\n", i, t->table1[i].key, t->table1[i].value);
        }
    }
    printf("hashtable2:\n");
    for (i = 0; i < t->nCount; i ++) {
        if (t->table2[i].key) {
            printf("index: %d, key: %d, value: %d\n", i, t->table2[i].key, t->table2[i].value);
        }
    }
}

int main()
{
    int i;
    table *t;

    t = malloc(sizeof(table));
    int key[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14};
    int value[] = {20, 50, 53, 75, 100, 67, 105, 3, 36, 39, 50, 40, 90, 103};

    //int key[] = {1,2,3,4,5,6,7,8,9,10,11};
    //int value[] = {20, 50, 53, 75, 100, 67, 105, 3, 36, 39, 50};
    initHashTable(t, 12);
    for (i = 0; i < sizeof(key)/sizeof(key[0]); i ++) {
        entry e;
        e.key = key[i];
        e.value = value[i];
        insertHashTable(t, e);
    }

    printTable(t);

    return 0;
}
