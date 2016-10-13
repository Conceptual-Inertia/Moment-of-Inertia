#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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
    if (len_links + 2== used_links){
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
    instrs[links[name + 1].instr_num].par[links[name + 1].par - 1] = bytes_written;
}

void fputu(uint32_t a){
    fputc(a >> 24, out);
    fputc((a >> 16) & 255, out);
    fputc((a >> 8) & 255, out);
    fputc(a & 255, out);
}

void put_instr (uint32_t num){
    fputc(((instrs[num].instr_num & 15) << 4) + ((instrs[num].tpar[0] == '#'? 2: (instrs[num].tpar[0] == 'R' ? 1 : 0)) << 2)
          +((instrs[num].tpar[1] == '#'? 2: (instrs[num].tpar[1] == 'R' ? 1 : 0))), out);
    fputc(((instrs[num].tpar[0] == '#'? 2: (instrs[num].tpar[0] == 'R' ? 1 : 0)) << 6) +
                  (((instrs[num].tpar[0] == 'R'? instrs[num].par[0]:0) & 3) << 4) +
                  (((instrs[num].tpar[1] == 'R'? instrs[num].par[1]:0) & 3) << 2) +
                  (((instrs[num].tpar[2] == 'R'? instrs[num].par[0]:0) & 3)), out);
    fputc((instrs[num].tpar[0] == '@'? instrs[num].par[0]:0) >> 8, out);
    fputc((instrs[num].tpar[0] == '@'? instrs[num].par[0]:0), out);
    fputc((instrs[num].tpar[1] == '@'? instrs[num].par[1]:0) >> 8, out);
    fputc((instrs[num].tpar[1] == '@'? instrs[num].par[1]:0), out);
    fputc((instrs[num].tpar[2] == '@'? instrs[num].par[2]:0) >> 8, out);
    fputc((instrs[num].tpar[2] == '@'? instrs[num].par[2]:0), out);
    if (instrs[num].tpar[0] == '#')fputu(instrs[num].par[0]);
    if (instrs[num].tpar[1] == '#')fputu(instrs[num].par[1]);
    if (instrs[num].tpar[2] == '#')fputu(instrs[num].par[2]);

}

void decodeLine(){
    char buff[15];
    char sub[15];
    uint32_t name;
    fscanf(in, "%s", buff);
    for (int i = 0;buff[i]; i++){//capitalize
        buff[i] = putchar(buff[i]);
    }
    if (toupper(buff[0]) == 'P' && toupper(buff[1]) != 'R') {//goto link
        memcpy(sub, &buff[1], strlen(buff) - 2);
        name = (uint32_t) atoi(sub);
        if (name >= len_links + 1)//not used before
            new_link(bytes_written, 0);//place now
        else make_link_before(name);
        return;
    }

    //name used as instr
    if (strcmp(buff, "ADD")) name = INERTIA_ADD;
    if (strcmp(buff, "DIV")) name = INERTIA_DIV;
    if (strcmp(buff, "MUL")) name = INERTIA_MUL;
    if (strcmp(buff, "LTN")) name = INERTIA_LTN;
    if (strcmp(buff, "EQL")) name = INERTIA_EQL;
    if (strcmp(buff, "AND")) name = INERTIA_AND;
    if (strcmp(buff, "NOT")) name = INERTIA_NOT;
    if (strcmp(buff, "OR")) name = INERTIA_OR;
    if (strcmp(buff, "SHIFTL")) name = INERTIA_SHIFTL;
    if (strcmp(buff, "SHIFTR")) name = INERTIA_SHIFTR;
    if (strcmp(buff, "PRINT")) name = INERTIA_PRINT;
    if (strcmp(buff, "LOAD")) name = INERTIA_LOAD;
    if (strcmp(buff, "GOTO")) name = INERTIA_GOTO;
    if (strcmp(buff, "IF")) name = INERTIA_IF;
    if (strcmp(buff, "RETURN")) name = INERTIA_RETURN;
    if (strcmp(buff, "CALL")) name = INERTIA_CALL;




}

int main(int argc, char *argv[]) {

    instrs = (instr_set *)malloc(len_instrs * sizeof(instr_set));
    links = (go_links *)malloc(len_links * sizeof(go_links));
    if ((!instrs) || (!links)){
        printf("Failed to allocate\n");
        return 1;
    }

    in = fopen(argv[1], "r");
    out = fopen(argv[2], "wb");

    fclose(in);
    fclose(out);
    free(instrs);
    free(links);
    return 0;
}