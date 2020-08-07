// COMP1521 20T2 Assignment 2
// SMIPS Emulator
// Written by Justin Pham

#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>

// defining bit masks
#define     MASK_S  0x03E00000
#define     MASK_T  0x001F0000
#define     MASK_D  0x0000F800
#define     MASK_I  0x0000FFFF
#define     MASK11  0x000007FF
// defining code bit patterns for comparisons
// if first 6 digits == 0, mask last 11 bits and check the following
#define     ADD     0x20
#define     SUB     0x22
#define     AND     0x24
#define     OR      0x25
#define     SLT     0x2A
#define     SYSCALL 0x0C
// instructions (first 6 digits)
#define     MUL     0x1C
#define     BEQ     0x4
#define     BNE     0x5
#define     ADDI    0x8
#define     SLTI    0xA
#define     ANDI    0xC
#define     ORI     0xD
#define     LUI     0xF

void processFile(char* buffer, int* commandList, char* fileName);
int processCommand(char* buffer);
bool validCommand(int command);

int main(int argc, char** argv) {

    assert(argc == 2);

    char buffer[9] = '\0';
    int commandList[1000] = -1;
    processFile(buffer, commandList, argv[1]);

    // int instruction = (argv[1][0] >> 26);
    // printf("instruction code is: %d\n", instruction);

    // // TODO if instruction is invalid, print error message
    // if (!validCommand(instruction)) {
    //     printf("Invalid instruction code");
    // }

    // // initialising registers
    // int registers[32] = 0;

}

void processFile(char* buffer, int* commandList, char* fileName) {

    FILE *toOpen = fopen(fileName, 'r');

    int buffer_char = fgetc(toOpen);
    int index = 0;
    int indexList = 0;

    while (buffer_char != EOF) {
        if (buffer_char == '\n') {
            int command = atoi(buffer);
            if (!validCommand(command)) {
                printf("%s:%d: invalid instruction code: %s\n", fileName, indexList, buffer);
                exit(1);
            }
            resetBuffer(buffer);

            index = 0;
            buffer_char = fgetc(toOpen);
            continue;
        }
        buffer[index] = buffer_char;
        index++;
        buffer_char = fgetc(toOpen);
    }

}

void resetBuffer(char* buffer) {
    for (int i = 0; i < 9; i++) {
        buffer[i] = '\0';
    }
}

bool validCommand(int command) {

    int check6[] = {MUL, BEQ, BNE, ADDI, SLTI, ANDI, ORI, LUI};
    int instruction = command >> 26;

    for (int i = 0; i < sizeof(check6); i++) {
        if (instruction == check6[i]) {
            return true;
        }
    }

    if (instruction != 0) {
        return false;
    }

    int check11[] = {ADD, SUB, AND, OR, SLT, SYSCALL};
    int last11 = command & MASK11;

    for (int i = 0; i < sizeof(check11); i++) {
        if (last11 == check11[i]) {
            return true;
        }
    }

    return false;
}