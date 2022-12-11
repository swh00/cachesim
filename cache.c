#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "cache.h"
#define MAX_BUF 17

char buffer[MAX_BUF];
//2의 제곱수인지를 확인하는 함수
int isTwo(int num){ return num && !((num-1) & num); }
//2의 제곱수에 대한 승수를 구하는 함수
int power2(int num){
    int power = 0, tmp = num;
    while(tmp > 1){
        power++;
        tmp = tmp >> 1;
    }
    return power;
}
//제곱수를 구하는 함수
int power(int num, int iter){
	int n = num;
	int result = n;
	int max = iter - 1;
	for(int i = 0; i < max; i++)
		result = result * n;
	return result;
}
//캐시구조를 보여주는 함수
void dis_cache(CacheSet cset){
    CacheSet ctmp = cset;
    int sets = ctmp.sets;
    int blocks = ctmp.blocks;
    Cache** cache = ctmp.caches;
    int* tmp = NULL;
    int index = 1;

    printf("------- cache -------\n");
    for(int i = 0; i < sets; i++){
        printf("set.%d | ", i);
        for(int k = 0; k < blocks; k++){
            printf("block.[%d] : %d %c | ", k, cache[i][k].block, cache[i][k].dirty);
        }
        printf("\n");
    }
}
//캐싱 상태를 보여주는 함수
void dis_stats(Stats stats){
    Stats tmp = stats;
    printf("------- stats -------\n");
    printf("Total loads  = %d\n", tmp.loads);
    printf("Total stores = %d\n", tmp.stores);
    printf("Load hits    = %d\n", tmp.l_hits);
    printf("Load misses  = %d\n", tmp.l_misses);
    printf("Store hits   = %d\n", tmp.s_hits);
    printf("Store misses = %d\n", tmp.s_misses);
    printf("Total cycles = %d\n", tmp.cycles);
}
//캐시를 위해 할당된 공간을 보여주는 함수
void toString(CacheSet cset){
    CacheSet tmp = cset;
    printf("------ Format ------\n");
    printf("cache.sets   = %d\n", tmp.sets);
    printf("cache.blocks = %d\n", tmp.blocks);
    printf("block.size   = %d\n", tmp.size);
    //printf("cache.policy = %d\n", cset.policy);
    dis_stats(tmp.stats);
    dis_cache(cset);
}
//캐시를 만드는 함수
Cache** createCache(CacheSet cset){
    int sets = cset.sets;
    int blocks = cset.blocks;
    int size = cset.size; //size만큼 추가로 저장을 의미
    Cache** caches = (Cache**)malloc(sizeof(Cache*)*sets);

    for(int row = 0; row < sets; row++){
        caches[row] = (Cache*)malloc(sizeof(Cache)*blocks);
        for(int col = 0; col < blocks; col++){
            caches[row][col].valid = 'n';
            caches[row][col].dirty = 'n';
            caches[row][col].time = 0;
            caches[row][col].block = 0;     
            caches[row][col].time = 0;
        }
    }    
    return caches;
}
//캐시를 위한 구조를 만드는 함수
int createSet(CacheSet* cset, char* argv[]){
    int mode;
    cset->sets = atoi(argv[1]);         //function : change string to intger
    cset->blocks = atoi(argv[2]);
	cset->size = atoi(argv[3]); //number of data can be stored
    mode = setMode(*cset);
	
    if(cset == NULL){
        printf("there is no caches\n");
        return -1;
    } else if(mode < 0) {
        printf("argument's range error!\n");
        return -1;
    }

    Cache** caches = createCache((*cset));

    if(cset == NULL){
        printf("there is no caches\n");
        exit(0);
    }
    
    cset->stats.loads = 0;
    cset->stats.stores = 0;
    cset->stats.l_hits = 0;
    cset->stats.l_misses = 0;
    cset->stats.s_hits = 0;
    cset->stats.s_misses = 0;
    cset->stats.cycles = 0;
    cset->caches = caches;
    
    return mode;
}
//동적할당된 캐시를 해제해주는 함수
void freeCache(CacheSet* cset){
    for(int row = 0; row < cset->sets; row++)
        free(cset->caches[row]);
    free(cset->caches);
}
//어떤 캐시방법인지 확인
int setMode(CacheSet cset){
    int sets = cset.sets;
    int blocks = cset.blocks;
    int size = cset.size;
    //if command-line parameters violates the rules, exit the program
    if(size >= 4) {  //each flags 0, 1, 2 are direct-mapped, set-associative, fully associative
        if(isTwo(sets) || isTwo(blocks) || isTwo(size)) {
            if((sets >= 1) && (blocks == 1)) return 0;
            else if((sets > 1) && (blocks > 1)) return 1;
            else return 2;
        } 
        else return -1; 
    } 
    else return -1;
}
//찾고자 하는 데이터가 캐시안에 있는지 확인하는 함수
int findCache(CacheSet* cset, unsigned int address, int WRway, int policy, int time, char type){
    int sets = cset->sets;
    int blocks = cset->blocks;
    int bsize = cset->size;      //number of storing memory
    int bbit = power2(bsize);   //valid bit number
    unsigned int index_bit = power2(sets);
    unsigned int offset = address % (int) power(2, bbit);  //offset is block's size
    unsigned int tag_ind = address >> bbit;      //get tag+index field
    unsigned int index = tag_ind % power(2, index_bit);		//index field
    unsigned int tag = tag_ind / power(2, index_bit);		//tag field
    int k, tmp, check = 1;
	
	if(sets == 1 && index == 1)	return 0;		//if cache has a set, only can store 0 except 1
	
    Cache* caches = cset->caches[index];
	Cache ctmp;
	
    for(k = 0; k < blocks; k++){
		ctmp = caches[k];
        if(ctmp.valid == 'o'){
            if(ctmp.tag == tag){
                if(ctmp.block >= offset){
                    if(policy == 0)
                        cset->caches[index][k].time = time;		//update time for lru
					if(type == 's' && WRway == 1){
						cset->caches[index][k].dirty = 'o'; 	//set dirty-bit for write-back
						return 2;
					}
                    return 1;
                }
            }
        }   
    }
    return 0;
}
//캐시안에 데이터를 집어 넣는 함수
int putCache(CacheSet* cset, unsigned int address, int policy, int time){
    int sets = cset->sets;
    int blocks = cset->blocks;
    int bsize = cset->size;      //number of storing memory
    int bbit = power2(bsize);   //valid bit number
    unsigned int index_bit = power2(sets);
    unsigned int offset = address % power(2, bsize);  //offset is block's size
    unsigned int tag_ind = address >> bbit;      //get tag+index field
    unsigned int index = tag_ind % power(2, index_bit);
    unsigned int tag = tag_ind / power(2, index_bit);
    int k, tmp, check = 1, min = time;
 	
	if(sets == 1 && index == 1)	return 0;		//if cache has a set, only can store 0 except 1
	
	//empty space in cache will be fill first
    for(k = 0; k < blocks; k++){
        if(cset->caches[index][k].valid == 'n'){
            cset->caches[index][k].valid = 'o';
            cset->caches[index][k].tag = tag;            
            cset->caches[index][k].time = time;         
            cset->caches[index][k].block = bsize - 1;
            return 1;
        }
    }
   	//if cache is not empty, change by cache policy
    switch(policy){
        case 0: //lru       
        case 1: //fifo          //it was divided at find data in cache...
            for(k = 0; k < blocks; k++){		//find minimum time
                if(cset->caches[index][k].time <= min){
                    tmp = k;
                    min = cset->caches[index][k].time;
                }
            }
            cset->caches[index][tmp].tag = tag;
            cset->caches[index][tmp].time = time;
            cset->caches[index][tmp].block = bsize - 1;
			if(cset->caches[index][tmp].dirty == 'o'){	//if write through and instruction's type is store
				cset->caches[index][tmp].dirty = 'n';
				return 2;
			}
            return 1;
        case 2: //random evictions
            tmp = rand() % sets;
            cset->caches[index][tmp].tag = tag;
            cset->caches[index][tmp].time = time;
            cset->caches[index][tmp].block = bsize - 1;
			if(cset->caches[index][tmp].dirty == 'o'){	//if write through and instruction's type is store
				cset->caches[index][tmp].dirty = 'n';
				return 2;
			}
        	return 1;
    }
    return 0;
}
//캐싱을 수행할 함수
void caching(FILE* fp, CacheSet* cset, int WRallo, int WRway, int policy){
    int tag, time = 0, i = 0;
	int isWR, isAlloc;
	int mem_num = cset->size / 4;
    Stats stats = {0, 0, 0, 0, 0, 0, 0};
    unsigned int address;
    char type;
    char* line = NULL;
    char tmp[9];
    tmp[8] = '\0';
	//read file a line
    while(!feof(fp)){
        line = fgets(buffer, MAX_BUF, fp);
        if(!line) break;	//if line is empty, break loop 
        type = line[0];
		//set variables for caching
        for(i = 4; i < 12; i++){
            tmp[i - 4] = line[i];
        }
        address = strtol(tmp, NULL, 16); //function : change hex-string to unsigned interger
		
        if((isWR = findCache(cset, address, WRway, policy, time, type)) > 0){		//when hits
            if(type == 'l'){
                stats.l_hits++;
                stats.loads++;
				stats.cycles = stats.cycles + 1;
            } else {
                stats.s_hits++;
                stats.stores++;
				if(isWR == 1){		//in write through, update single cache and memory 
					stats.cycles = stats.cycles + 101;
				} else {				//in write back allocation, only update cache
					stats.cycles = stats.cycles + 1;
				}
            }
        } else {						//when misses
            if(type == 'l'){
                stats.l_misses++;
                stats.loads++;
				isWR = putCache(cset, address, policy, time);
				if(isWR == 1){			//in write through...
					stats.cycles = stats.cycles + 101 * mem_num + 1;		//load from memory, and update caches
				} else {				//in write back, have to update memory
					stats.cycles = stats.cycles + 200 * mem_num + 1;		//data in cache was changed, so update memory and cache
				}
            } else {
                stats.s_misses++;
                stats.stores++;
				if(WRallo == 0){
					isWR = putCache(cset, address, policy, time);
					if(isWR == 1){
						stats.cycles = stats.cycles + 101 * mem_num + 1;	//load into cache from memory
					} else {
						stats.cycles = stats.cycles + 201 * mem_num + 1;	//set cache and update memory
						
					}
					
				} else {	//no-write-allocate
					stats.cycles = stats.cycles + 100;	//just write at memory, except cache
				}
            }
        }
        
        time++;		//set time
    }
	cset->stats = stats; //update stats
}