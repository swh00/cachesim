#ifndef _CACHE_
#define _CACHE_
//캐싱의 결과를 보여주는 구조체
typedef struct _Stats{
    int loads, stores;
    int l_hits, l_misses;
    int s_hits, s_misses;
    int cycles;
}Stats;
//캐시의 구조체
typedef struct _cache{
    char valid;	//valid bit
    char dirty;	//dirty bit
    unsigned int tag;	//tag
    unsigned int block;	//offset or block
    int time;	//for lru & fifo
}Cache;
//캐시의 정보를 위한 구조체
typedef struct _cacheSet{
    int sets;
    int blocks;
    int size;
    Stats stats;
    Cache** caches;
}CacheSet;

int isTwo(int);
int power2(int);
int power(int, int);
int str2int(int);

void dis_cache(CacheSet);
void dis_stats(Stats);
void toString(CacheSet);

Cache** createCache(CacheSet cset);
int createSet(CacheSet*, char**);
void freeCache(CacheSet*);
int setMode(CacheSet);
int findCache(CacheSet*, unsigned int, int, int, int, char);
int putCache(CacheSet*, unsigned int, int, int);

void caching(FILE*, CacheSet*, int, int, int);

#endif