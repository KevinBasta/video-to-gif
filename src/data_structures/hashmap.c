
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <stdint.h>
#include "main.h"

#include "HashMap.h"
#include "printUtility.h"

/**
 * @brief converts key string to an index
 * @param key       a string representing the key 
 * @param size      size of the map 
 * @return hashed index of the key
 */
static size_t HashFunction(char *key, size_t size) {
    size_t index = 1;

    // create index out of key
    for (int j = 0; key[j] != '\0'; j++)
        index += pow( (u8) key[j] + 2, 3);
    
    index = index % size;
    //printf("index is: %ld\n", index);

    return index;
}

/**
 * @brief creates a hashmap entry
 * @param key   string
 * @param value string
 * @return HashMapEntry 
 */
HashMapEntry *HashMapCreateEntry(char *key, char *value) {
    HashMapEntry *entry = (HashMapEntry*) calloc(1, sizeof(HashMapEntry));
    entry->key   = key;
    entry->value = (char*) malloc(strlen(value) + 1);

    strcpy(entry->key, key);
    strcpy(entry->value, value);

    return entry;
}

/**
 * @brief creates a hashmap
 * @param size total number of entries map can hold
 * @return HashMap
 */
HashMap *HashMapInit(size_t size) {
    HashMap *map = (HashMap*) calloc(1, sizeof(HashMap));

    map->size = size;
    map->currentCount = 0;
    map->entries = (HashMapEntry**) calloc(size, sizeof(HashMapEntry*));

    for (size_t i = 0; i < map->size; i++) {
        map->entries[i] = NULL;
    }

    return map;
}

/**
 * @brief insert a key-value pair in a hashmap
 * @param map   the map to insert in
 * @param key   the key of the new entry
 * @param value the value of the new entry
 * @return OPERATION_SUCCESS or OPERATION_FAILED
 */
STATUS_CODE HashMapInsert(HashMap *map, char *key, char *value) {
    if (map->currentCount == map->size) {
        printf("hashmap full\n");
        return OPERATION_FAILED;
    }

    printf("INSERT OPERATING STARTED %s\n", key);

    size_t index           = HashFunction(key, map->size);
    HashMapEntry *newEntry = HashMapCreateEntry(key, value);

    // collision resolution: Open addressing, quadratic probing
    size_t k = 0;
    while (map->entries[index] != NULL) {
        //printf(">>>>>>>>>>>>>>>>>>>>>>\n");
        //printf("collision at index %ld\n", index);
        //printf("key %s, key at index %s\n", key, map->entries[index]->key);
        k++;
        index = (index + (k*k)) % map->size;
        //printf("new index %ld\n", index);
        //printf("<<<<<<<<<<<<<<<<<<<<<<\n");

        // terminate if probing takes too long
        if (k >= map->size) {
            return OPERATION_FAILED;
        }
    }

    map->entries[index] = newEntry;
    printf("FINAL INDEX: %ld\n", index);

    return OPERATION_SUCCESS;
}

/**
 * @brief search hashmap by key
 * @param map hashmap to search in
 * @param key the key to search for
 * @return the value or null
 */
char *HashMapSearch(HashMap *map, char *key) {
    size_t index        = HashFunction(key, map->size);
    HashMapEntry *entry = map->entries[index];
    char *value         = NULL;
    //printf("%p\n", entry);

    size_t k = 0;
    while (entry != NULL) {
        if (k >= map->size) {
            return NULL;
        }

        if (strcmp(entry->key, key) == 0) {
            value = entry->value;
            printf("search found: %s\n", value);
            break;
        }

        k++;
        index = (index + (k*k)) % map->size;
        entry = map->entries[index];
    }

    return value;
}

char *HashMapCreateKey(char *str, u32 length) {
    char *key = calloc(1, sizeof(length + 1));
    strncpy(key, str, length);
    key[length] = '\0';

    return key;
}


void HashMapPrint(HashMap *map) {
    for (size_t i = 0; i < map->size; i++) {
        if (map->entries[i] != NULL) {
            printf("key: %10s   value: %10s \n", map->entries[i]->key, map->entries[i]->value);
        }
    }
}

void HashMapSearchPrint(HashMap *map, char *key) {
    char *value = HashMapSearch(map, key);

    if (value == NULL) {
        printf("Key [%s] not found\n", key);
    } else {
        printf("Key: [%s] \t Value: [%s]\n", key, value);
    }
}


void freeHashMapEntry(HashMapEntry *entry) {
    free(entry->key);
    free(entry->value);
}

void freeHashMap(HashMap *map) {
    for (size_t i = 0; i < map->size; i++) {
        HashMapEntry *entry = map->entries[i];

        if (entry != NULL) {
            freeHashMapEntry(entry);
        }
    }

    free(map->entries);
    free(map);
}
