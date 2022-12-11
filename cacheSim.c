#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include "cache.h"

/*argv = program sets blocks block-size (no-)write-allocate
> write-through(or back) replacement tracefile*/
int main(int argc, char* argv[]){
    CacheSet cset;
    FILE* fp = NULL;
    int mode, WRallo = 0, WRway = 0, policy = 0;
    
    //arguments number muse be 8
    if(argc != 8){
        printf("error : too few or many arguments!!\n");
        exit(0);
    }
	//open trace file
	printf("%s file will be open\n", argv[7]);
    if((fp = fopen(argv[7],"r")) == NULL){
        printf("file do not exist\n");
        exit(0);
    }
	//set argumetns
    if(strcmp(argv[4], "write-allocate")) {
        WRallo = 1;
        printf("Start with no-write-allocate & ");
    } else { printf("Start with write-allocate & "); }

    if(strcmp(argv[5], "write-through")){
        WRway = 1;
        printf("write-back & ");
    } else { printf("write-through & "); }

    if(strcmp(argv[6], "fifo") == 0){
        policy = 1;
        printf("fifo &");
    } else if(strcmp(argv[6], "random") == 0){
        policy = 2;
        printf("random eviction &");
    } else { printf("lru & "); }
    
    //set mode and create place for caching
    mode = createSet(&cset, argv);
    switch(mode){
        case 0:     //direct-mapped mode
            printf("Direct Mapped\n");
            break;
        case 1:     //set-associative mode
            printf("Set Associative\n");
            break;
        case 2:     //fully associative mode
            printf("Fully Associative\n");
            break;
        default :   //if arguments violate the rules, exit the program
            return 0;
    }
	//start caching
    caching(fp, &cset, WRallo, WRway, policy);

    dis_stats(cset.stats);
	fclose(fp);
    return 0;
}