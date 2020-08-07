// COMP1521 20T2 Assignment 2
// SMIPS Emulator
// Written by Justin Pham

#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

#define     LINES   1000
#define     ARGS    2
#define     PATTERN 0
#define     COMMAND 1
// defining bit masks
#define     MASK_S  0x03E00000
#define     MASK_T  0x001F0000
#define     MASK_D  0x0000F800
#define     MASK_I  0x0000FFFF
#define     MASK6   0xFC000000
#define     MASK11  0x000007FF
// defining code bit patterns for comparisons
#define     MUL     0x70000002
#define     BEQ     0x10000000
#define     BNE     0x14000000
#define     ADDI    0x20000000
#define     SLTI    0x28000000
#define     ANDI    0x30000000
#define     ORI     0x34000000
#define     LUI     0x3C000000
// if not part of the above, mask for last 11 bits and check the following
#define     ADD     0x20
#define     SUB     0x22
#define     AND     0x24
#define     OR      0x25
#define     SLT     0x2A
#define     SYSCALL 0x0C

void processFile(char* buffer, int commandList[LINES][ARGS], char* fileName);
void resetBuffer(char* buffer);
bool validPattern(int pattern, int commandList[LINES][ARGS], int indexList);
void executeProgram(int commandList[LINES][ARGS], int* registers);
void printCommand(int pattern, int command, int indexList);
void printRegisters(int* registers);
// --------------------- MIPS COMMAND IMPLEMENTATION ---------------------
void add(int d, int s, int t, int* registers);
void sub(int d, int s, int t, int* registers);
void and(int d, int s, int t, int* registers);
void or(int d, int s, int t, int* registers);
void slt(int d, int s, int t, int* registers);
void mul(int d, int s, int t, int* registers);
int beq(int t, int s, int I, int* registers);
int bne(int t, int s, int I, int* registers);
void addi(int t, int s, int I, int* registers);
void slti(int t, int s, int I, int* registers);
void andi(int t, int s, int I, int* registers);
void ori(int t, int s, int I, int* registers);
void lui(int t, int I, int* registers);
int syscall(int* registers);


int main(int argc, char** argv) {

    assert(argc == 2);

    char buffer[9] = {'\0'};
    int commandList[LINES][ARGS] = {0};
    for (int i = 0; i < LINES; i++) {
        for (int j = 0; j < ARGS; j++) {
            commandList[i][j] = -1;
        }
    }
    printf("Program\n");
    processFile(buffer, commandList, argv[1]);

    // initialising registers
    int registers[32] = {0};

    printf("Output\n");
    executeProgram(commandList, registers);

    printf("Registers After Execution\n");
    printRegisters(registers);

}

void executeProgram(int commandList[LINES][ARGS], int* registers) {

    int commandNumber = 0;
    int pattern = 0;
    int command = 0;

    while (pattern != -1 && commandNumber >= 0 && commandNumber < LINES) {
        pattern = commandList[commandNumber][PATTERN];
        command = commandList[commandNumber][COMMAND];
        // printf("commandNumber: %d\n", commandNumber);

        int s = (pattern & MASK_S) >> 21;
        int t = (pattern & MASK_T) >> 16;
        int d = (pattern & MASK_D) >> 11;
        int16_t I = (int16_t) (pattern);

        switch (command) {
            
        case ADD:
            add(d, s, t, registers);
            break;
        case SUB:
            sub(d, s, t, registers);
            break;
        case AND:
            and(d, s, t, registers);
            break;
        case OR:
            or(d, s, t, registers);
            break;
        case SLT:
            slt(d, s, t, registers);
            break;
        case MUL:
            mul(d, s, t, registers);
            break;
        case BEQ:
            // if condition is met, jumps to branchCommandNumber-1, else unchanged
            commandNumber += beq(t, s, I, registers);
            break;
        case BNE:
            // if condition is met, jumps to branchCommandNumber-1, else unchanged
            commandNumber += bne(t, s, I, registers);
            break;
        case ADDI:
            addi(t, s, I, registers);
            break;
        case SLTI:
            slti(t, s, I, registers);
            break;
        case ANDI:
            andi(t, s, I, registers);
            break;
        case ORI:
            ori(t, s, I, registers);
            break;
        case LUI:
            lui(t, I, registers);
            break;
        case SYSCALL:
            if (syscall(registers) == -1) {
                return;
            }
            break;
        }

        // load next command
        commandNumber++;
    }

    return;


}

void processFile(char* buffer, int commandList[LINES][ARGS], char* fileName) {

    FILE *toOpen = fopen(fileName, "r");

    int bufferChar = fgetc(toOpen);
    int indexChar = 0;
    int indexList = 0;

    while (bufferChar != EOF) {
        if (bufferChar == '\n') {
            int pattern = (int) strtol(buffer, NULL, 16);
            // printf("buffer is: %s\n", buffer);
            // printf("pattern is: %x\n", pattern);
            if (validPattern(pattern, commandList, indexList) == false) {
                printf("%s:%d: invalid instruction code: %08x\n", fileName, indexList, pattern);
                exit(1);
            }
            resetBuffer(buffer);
            indexList++;

            indexChar = 0;
            bufferChar = fgetc(toOpen);
            continue;
        }
        buffer[indexChar] = bufferChar;
        indexChar++;
        bufferChar = fgetc(toOpen);
    }

}

void resetBuffer(char* buffer) {
    for (int i = 0; i < 9; i++) {
        buffer[i] = '\0';
    }
}

bool validPattern(int pattern, int commandList[LINES][ARGS], int indexList) {

    int check6[] = {BEQ, BNE, ADDI, SLTI, ANDI, ORI, LUI};
    int first6 = pattern & MASK6;
    // printf("first6 is: %x\n", first6);

    for (int i = 0; i < 7; i++) {
        if (first6 == check6[i]) {
            commandList[indexList][PATTERN] = pattern;
            commandList[indexList][COMMAND] = check6[i];
            printCommand(pattern, check6[i], indexList);
            return true;
        }
    }

    // edge cases for MUL, SYSCALL
    if ((pattern & 0xFC0007FF) == MUL) {
        commandList[indexList][PATTERN] = pattern;
        commandList[indexList][COMMAND] = MUL;
        printCommand(pattern, MUL, indexList);
        return true;
    } else if (pattern == SYSCALL) {
        commandList[indexList][PATTERN] = pattern;
        commandList[indexList][COMMAND] = SYSCALL;
        printCommand(pattern, SYSCALL, indexList);
        return true;
    } else if (first6 != 0) {
        return false;
    }

    int check11[] = {ADD, SUB, AND, OR, SLT};
    int last11 = pattern & MASK11;

    for (int i = 0; i < 5; i++) {
        if (last11 == check11[i]) {
            commandList[indexList][PATTERN] = pattern;
            commandList[indexList][COMMAND] = check11[i];
            printCommand(pattern, check11[i], indexList);
            return true;
        }
    }

    return false;
}

void printCommand(int pattern, int command, int indexList) {
    
    int s = (pattern & MASK_S) >> 21;
    int t = (pattern & MASK_T) >> 16;
    int d = (pattern & MASK_D) >> 11;
    int I = (int) ((int16_t) pattern);

    // printf("I = %d\n", I);

    switch (command) {
            
    case ADD:
        printf("%3d: add  $%d, $%d, $%d\n", indexList, d, s, t);
        break;
    case SUB:            
        printf("%3d: sub  $%d, $%d, $%d\n", indexList, d, s, t);
        break;
    case AND:
        printf("%3d: and  $%d, $%d, $%d\n", indexList, d, s, t);
        break;
    case OR:
        printf("%3d: or   $%d, $%d, $%d\n", indexList, d, s, t);
        break;
    case SLT:
        printf("%3d: slt  $%d, $%d, $%d\n", indexList, d, s, t);
        break;
    case MUL:
        printf("%3d: mul  $%d, $%d, $%d\n", indexList, d, s, t);
        break;
    case BEQ:
        printf("%3d: beq  $%d, $%d, %d\n", indexList, s, t, I);
        break;
    case BNE:
        printf("%3d: bne  $%d, $%d, %d\n", indexList, s, t, I);
        break;
    case ADDI:
        printf("%3d: addi $%d, $%d, %d\n", indexList, t, s, I);
        break;
    case SLTI:
        printf("%3d: slti $%d, $%d, %d\n", indexList, t, s, I);
        break;
    case ANDI:
        printf("%3d: andi $%d, $%d, %d\n", indexList, t, s, I);
        break;
    case ORI:
        printf("%3d: ori  $%d, $%d, %d\n", indexList, t, s, I);
        break;
    case LUI:
        printf("%3d: lui  $%d, %d\n", indexList, t, I);
        break;
    case SYSCALL:
        printf("%3d: syscall\n", indexList);
        break;
    }

}

void printRegisters(int* registers) {
    for (int i = 0; i < 32; i++) {
        if (registers[i] != 0) {
            printf("$%-3d= %d\n", i, registers[i]);
        }
    }
}

// --------------------- MIPS COMMAND IMPLEMENTATION ---------------------
void add(int d, int s, int t, int* registers) {
    // printf("entered add\n");
    if (d == 0) {
        return;
    }
    registers[d] = registers[s] + registers[t];
}

void sub(int d, int s, int t, int* registers) {
    // printf("entered sub\n");
    if (d == 0) {
        return;
    }
    registers[d] = registers[s] - registers[t];
}

void and(int d, int s, int t, int* registers) {
    // printf("entered and\n");
    if (d == 0) {
        return;
    }
    registers[d] = registers[s] & registers[t];
}

void or(int d, int s, int t, int* registers) {
    // printf("entered or\n");
    if (d == 0) {
        return;
    }
    registers[d] = registers[s] | registers[t];
}

void slt(int d, int s, int t, int* registers) {
    // printf("entered slt\n");
    if (d == 0) {
        return;
    }
    if (registers[s] < registers[t]) {
        registers[d] = 1;
    } else {
        registers[d] = 0;
    }
}

void mul(int d, int s, int t, int* registers) {
    // printf("entered mul\n");
    if (d == 0) {
        return;
    }
    registers[d] = registers[s] * registers[t];
}

int beq(int t, int s, int I, int* registers) {
    // printf("entered beq\n");
    if (registers[s] == registers[t]) {
        // printf("jumping back to %d-1\n", I);
        return I-1;
    }
    return 0;
}

int bne(int t, int s, int I, int* registers) {
    // printf("entered bne\n");
    if (registers[s] != registers[t]) {
        // printf("jumping back to %d-1\n", I);
        return I-1;
    }
    return 0;
}

void addi(int t, int s, int I, int* registers) {
    // printf("entered addi\n");
    if (t == 0) {
        return;
    }
    registers[t] = registers[s] + I;
}

void slti(int t, int s, int I, int* registers) {
    // printf("entered slti\n");
    if (t == 0) {
        return;
    }
    registers[t] = registers[s] < I;
}

void andi(int t, int s, int I, int* registers) {
    // printf("entered andi\n");
    if (t == 0) {
        return;
    }
    registers[t] = registers[s] & I;
}

void ori(int t, int s, int I, int* registers) {
    // printf("entered ori\n");
    if (t == 0) {
        return;
    }
    registers[t] = registers[s] | I;
}

void lui(int t, int I, int* registers) {
    // printf("entered lui\n");
    if (t == 0) {
        return;
    }
    registers[t] = I << 16;
}

int syscall(int* registers) {
    // printf("entered print syscall\n");
    int code = registers[2];
    int toPrint = registers[4];
    // printf("%d", toPrint);

    if (code == 1) {
        printf("%d", toPrint);
        return 0;
    } else if (code == 11) {
        toPrint = toPrint & 0xFF;
        printf("%c", toPrint);
        return 0;
    } else if (code == 10) {
        return -1;
    } else {
        printf("Unknown system call: %d\n", code);
        return -1;
    }
}