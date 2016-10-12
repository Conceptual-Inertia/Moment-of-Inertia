#include <stdio.h>
#include <stdlib.h>

#define INERTIA_ADD 0x0 // Addition
#define INERTIA_DIV 0x1 // Division
#define INERTIA_MUL 0x2 // Multiplication

#define INERTIA_LTN 0x3 // Less Than
#define INERTIA_EQL 0x4 // Equal To
#define INERTIA_AND 0x5 // Bitwise AND
#define INERTIA_NOT 0x6 // Bitwise NOT
#define INERTIA_OR 0x7 //  Bitwise OR
#define INERTIA_SHIFTL 0x8 //Bitwise SHIFTL
#define INERTIA_SHIFTR 0x9 //Bitwise SHIFTR

#define INERTIA_PRINT 0xA // Print to stdout
#define INERTIA_LOAD 0xB // Load value
#define INERTIA_GOTO 0xC //goto
#define INERTIA_IF 0xD //if par1 == false, skip to par2
#define INERTIA_RETURN 0xE //return
#define INERTIA_CALL 0xF // call function

typedef struct instr_set{
    uint32_t instr_num;
    char tpar[3];
    uint32_t par[3];
}instr_set;

typedef struct go_links{
    uint32_t instr_num;//which instruction ask for it
    uint32_t par;//which instr par
}go_links;

FILE *in;
FILE *out;

uint32_t bytes_written;

uint32_t len_instrs = 1024;
uint32_t used_instrs;
instr_set *instrs;

uint32_t len_links = 1024;
uint32_t used_links;
go_links *links;

void new_instr(uint32_t instr_num, char tpar1, uint32_t par1, char tpar2, uint32_t par2, char tpar3, uint32_t par3){
    if (len_instrs == used_instrs){
        len_instrs <<= 1;
        instrs = (instr_set *)realloc(instrs, len_instrs * sizeof(instr_set));
        if (!instrs){
            printf("Failed to allocate\n");
            exit(1);
        }
    }

    instrs[used_instrs].instr_num = instr_num;
    instrs[used_instrs].tpar[0] = tpar1;
    instrs[used_instrs].tpar[1] = tpar2;
    instrs[used_instrs].tpar[2] = tpar3;
    instrs[used_instrs].par[0] = par1;
    instrs[used_instrs].par[1] = par2;
    instrs[used_instrs].par[2] = par3;
    bytes_written += (2 + (tpar1 == '#') + (tpar2 == '#') + (tpar3 == '#'));
    used_instrs ++;
}

void new_link(uint32_t instr_num, uint32_t par){
    if (len_links == used_links){
        len_links <<= 1;
        links = (go_links *)realloc(links, len_links * sizeof(go_links));
        if (!links){
            printf("Failed to allocate\n");
            exit(1);
        }
    }

    links[used_links].instr_num = instr_num;
    links[used_links].par = par;
    used_links++;
}

void make_link_before(uint32_t name){
    instrs[links[name + 2].instr_num].par[links[name + 2].par - 1] = bytes_written;
}

void make_link_after(uint32_t name){

}

int main(int argc, char *argc[]) {
    instrs = (instr_set *)malloc(len_instrs * sizeof(instr_set));
    links = (go_links *)malloc(len_links * sizeof(go_links));
    if ((!instrs) || (!links)){
        printf("Failed to allocate\n");
        return 1;
    }



    free(instrs);
    free(links);
    return 0;
}