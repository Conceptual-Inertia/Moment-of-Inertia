#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>

#define INERTIA_ADD 0x0 // Addition
#define INERTIA_DIV 0x1 // Division
#define INERTIA_MUL 0x2 // Multiplication

#define INERTIA_LTN 0x3 // Less Than
#define INERTIA_EQL 0x4 // Equal To
#define INERTIA_AND 0x5 // Bitwise AND
#define INERTIA_NOT 0x6 // Bitwise NOT
#define INERTIA_OR 0x7  // Bitwise OR
#define INERTIA_INC 0x8 // Increase by 1
#define INERTIA_DEC 0x9 // Decrease by 1

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

void new_instr(uint32_t instr_num, char tpar[3], uint32_t par[3]){
    if (len_instrs == used_instrs){
        len_instrs <<= 1;
        instrs = (instr_set *)realloc(instrs, len_instrs * sizeof(instr_set));
        if (!instrs){
            printf("Failed to allocate\n");
            exit(1);
        }
    }

    instrs[used_instrs].instr_num = instr_num;
    instrs[used_instrs].tpar[0] = tpar[0];
    instrs[used_instrs].tpar[1] = tpar[1];
    instrs[used_instrs].tpar[2] = tpar[2];
    instrs[used_instrs].par[0] = par[0];
    instrs[used_instrs].par[1] = par[1];
    instrs[used_instrs].par[2] = par[2];
    bytes_written += (2 + (tpar[0] == '#') + (tpar[1] == '#') + (tpar[2] == '#'));
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
        links[used_links].instr_num = instr_num;
        links[used_links].par = par;
        used_links ++;
    }

    links[used_links].instr_num = instr_num;
    links[used_links].par = par;
    used_links++;
}

void make_link_before(uint32_t name){
    instrs[links[name - 1].instr_num].par[links[name - 1].par - 1] = bytes_written;
}

void fputu(uint32_t a){
    fputc((a >> 24), out);
    fputc((a >> 16) & 255, out);
    fputc((a >> 8) & 255, out);
    fputc(a & 255, out);
}

void put_instr (uint32_t num){
    fputc(((instrs[num].instr_num & 15) << 4) + ((instrs[num].tpar[0] == '#'? 2: (instrs[num].tpar[0] == 'R' ? 1 : 0)) << 2)
          +((instrs[num].tpar[1] == '#'? 2: (instrs[num].tpar[1] == 'R' ? 1 : 0))), out);
    fputc(((instrs[num].tpar[2] == '#'? 2: (instrs[num].tpar[2] == 'R' ? 1 : 0)) << 6) +
                  (((instrs[num].tpar[0] == 'R'? instrs[num].par[0]:0) & 3) << 4) +
                  (((instrs[num].tpar[1] == 'R'? instrs[num].par[1]:0) & 3) << 2) +
                  (((instrs[num].tpar[2] == 'R'? instrs[num].par[2]:0) & 3)), out);
    fputc((instrs[num].tpar[0] == '@'? instrs[num].par[0]:0) >> 8, out);
    fputc((instrs[num].tpar[0] == '@'? instrs[num].par[0]:0), out);
    fputc((instrs[num].tpar[1] == '@'? instrs[num].par[1]:0) >> 8, out);
    fputc((instrs[num].tpar[1] == '@'? instrs[num].par[1]:0), out);
    fputc((instrs[num].tpar[2] == '@'? instrs[num].par[2]:0) >> 8, out);
    fputc((instrs[num].tpar[2] == '@'? instrs[num].par[2]:0), out);
    if (instrs[num].tpar[0] == '#')fputu(instrs[num].par[0]);
    if (instrs[num].tpar[1] == '#')fputu(instrs[num].par[1]);
    if (instrs[num].tpar[2] == '#')fputu(instrs[num].par[2]);
    
    printf("%d, %d %c%d %c%d %c%d\n", num, instrs[num].instr_num, instrs[num].tpar[0], instrs[num].par[0], instrs[num].tpar[1], instrs[num].par[1], instrs[num].tpar[2], instrs[num].par[2]);

}

void decode_add(char *tpar, uint32_t *par, uint32_t n){
    char c;
    uint32_t t;
    if (fscanf(in, " %c%u", &c,&t) == EOF) return;
    printf("READADD: %c %u\n", c,t);
    c = toupper(c);
    if (c == 'P'){
        if (t > ((int32_t)used_links)) {//not used before
            new_link(used_instrs, n);
            *par = 0;
            *tpar = '#';
        }
        else{//used before
            *par = links[t - 1].instr_num;
            if (links[t].par != 0) printf("Warning: goto link incorrect");
            *tpar = '#';
        }
    }
    else {
        *tpar = c;
        *par = t;
    }
}

int decode_line(){
    char buff[15];
    uint32_t name = 0;
    if (fscanf(in, "%s", buff) == EOF) return EOF;
    printf("READ: %s\n", buff);

    for (int i = 0;buff[i]; i++){//capitalize
        buff[i] = toupper(buff[i]);
    }
    if (buff[0] >= '0' && buff[0] <= '9') {//goto link
        name = atoi(buff);
        printf("name: %u", name);
        if (name > ((int32_t)used_links)) {//not used before
            new_link(bytes_written, 0);//place now
        }
        else {
            make_link_before(name);
        }
        return 1;
    }

    //name used as instr
    if (!strcmp(buff, "ADD")) name = INERTIA_ADD;
    if (!strcmp(buff, "DIV")) name = INERTIA_DIV;
    if (!strcmp(buff, "MUL")) name = INERTIA_MUL;
    if (!strcmp(buff, "LTN")) name = INERTIA_LTN;
    if (!strcmp(buff, "EQL")) name = INERTIA_EQL;
    if (!strcmp(buff, "AND")) name = INERTIA_AND;
    if (!strcmp(buff, "NOT")) name = INERTIA_NOT;
    if (!strcmp(buff, "OR")) name = INERTIA_OR;
    if (!strcmp(buff, "INC")) name = INERTIA_INC;
    if (!strcmp(buff, "DEC")) name = INERTIA_DEC;
    if (!strcmp(buff, "PRINT")) name = INERTIA_PRINT;
    if (!strcmp(buff, "LOAD")) name = INERTIA_LOAD;
    if (!strcmp(buff, "GOTO")) name = INERTIA_GOTO;
    if (!strcmp(buff, "IF")) name = INERTIA_IF;
    if (!strcmp(buff, "RETURN")) name = INERTIA_RETURN;
    if (!strcmp(buff, "CALL")) name = INERTIA_CALL;

    char tpar[3] = {'@', '@', '@'};
    uint32_t par[3] = {0, 0, 0};

    int times;
    switch (name){
        case 0 ... 5: // three par
        case 7:
            decode_add(&tpar[0], &par[0], 1);
            decode_add(&tpar[1], &par[1], 2);
            decode_add(&tpar[2], &par[2], 3);
            break;
        case 6: //two par
        case 11:
        case 13:
            decode_add(&tpar[0], &par[0], 1);
            decode_add(&tpar[1], &par[1], 2);
            break;
        case 8 ... 10://one par
        case 12:
        case 15:
            decode_add(&tpar[0], &par[0], 1);
            break;
        //no par, do nothing
    }

    new_instr(name, tpar, par);

    return 1;
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
    if ((!in) || (!out)){
        printf("Failed to read file\n");
        if (out) {
            fclose(out);
            remove(argv[2]);
        }
        return 1;
    }


    while (decode_line() != EOF){};

    fputu(bytes_written);
    for (uint32_t i = 0; i < used_instrs; i ++){
        put_instr(i);
    }

    fclose(in);
    fclose(out);
    free(instrs);
    free(links);
    return 0;
}
