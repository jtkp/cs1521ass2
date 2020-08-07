// COMP1521 20T2 Assignment 2
// SMIPS Emulator
// Written by Justin Pham

#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>

#define     PATTERN 0
#define     COMMAND 1
// defining bit masks
#define     MASK_S  0x03E00000
#define     MASK_T  0x001F0000
#define     MASK_D  0x0000F800
#define     MASK_I  0x0000FFFF
#define     MASK11  0x000007FF
// defining code bit patterns for comparisons
// instructions (first 6 digits)
#define     MUL     0x1C
#define     BEQ     0x4
#define     BNE     0x5
#define     ADDI    0x8
#define     SLTI    0xA
#define     ANDI    0xC
#define     ORI     0xD
#define     LUI     0xF
// if first 6 digits == 0, mask last 11 bits and check the following
#define     ADD     0x20
#define     SUB     0x22
#define     AND     0x24
#define     OR      0x25
#define     SLT     0x2A
#define     SYSCALL 0x0C

void processFile(char* buffer, int** commandList, char* fileName);
void resetBuffer(char* buffer);
bool validPattern(int pattern, int** commandList, int indexList);
void executeProgram(int** commandList, int* registers);

int main(int argc, char** argv) {

    assert(argc == 2);

    char buffer[9] = '\0';
    int commandList[1000][2] = {0};
    for (int i = 0; i < 1000; i++) {
        for (int j = 0; j < 2; j++) {
            commandList[i][j] = -1;
        }
    }
    processFile(buffer, commandList, argv[1]);

    // initialising registers
    int registers[32] = 0;

    executeProgram(commandList, registers);

}

void executeProgram(int** commandList, int* registers) {

    int commandNumber = 0;
    int pattern = commandList[commandNumber][PATTERN];
    int command = commandList[commandNumber][COMMAND];

    while (pattern != -1) {
        int s = (pattern & MASK_S) >> 21;
        int t = (pattern & MASK_T) >> 16;
        int d = (pattern & MASK_D) >> 11;
        int I = pattern & MASK_I;

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
                mul(t, s, I, registers);
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
            default:
                syscall(registers);
        }

        // load next command
        commandNumber++;
        pattern = commandList[commandNumber][PATTERN];
        command = commandList[commandNumber][COMMAND];
    }


}

void processFile(char* buffer, int** commandList, char* fileName) {

    FILE *toOpen = fopen(fileName, 'r');

    int bufferChar = fgetc(toOpen);
    int indexChar = 0;
    int indexList = 0;

    while (bufferChar != EOF) {
        if (bufferChar == '\n') {
            int pattern = atoi(buffer);
            if (!validPattern(pattern, commandList, indexList)) {
                printf("%s:%d: invalid instruction code: %s\n", fileName, indexList, buffer);
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

bool validPattern(int pattern, int** commandList, int indexList) {

    int check6[] = {BEQ, BNE, ADDI, SLTI, ANDI, ORI, LUI};
    int instruction = pattern >> 26;

    for (int i = 0; i < sizeof(check6); i++) {
        if (instruction == check6[i]) {
            commandList[indexList][PATTERN] = pattern;
            commandList[indexList][COMMAND] = check6[i];
            return true;
        }
    }

    // edge cases for MUL, SYSCALL
    if (instruction == MUL) {
        if (pattern & MASK11 == 2) {
            commandList[indexList][PATTERN] = pattern;
            commandList[indexList][COMMAND] = MUL;
            return true;
        }
    } else if (pattern == SYSCALL) {
        commandList[indexList][PATTERN] = pattern;
        commandList[indexList][COMMAND] = SYSCALL;
        return true;
    } else if (instruction != 0) {
        return false;
    }

    int check11[] = {ADD, SUB, AND, OR, SLT};
    int last11 = pattern & MASK11;

    for (int i = 0; i < sizeof(check11); i++) {
        if (last11 == check11[i]) {
            commandList[indexList][PATTERN] = pattern;
            commandList[indexList][COMMAND] = check11[i];
            return true;
        }
    }

    return false;
}