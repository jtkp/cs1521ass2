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

#define     MUL     0x1C
#define     BEQ     0x4
#define     BNE     0x5
#define     ADDI    0x8
#define     SLTI    0xA
#define     ANDI    0xC
#define     ORI     0xD
#define     LUI     0xF

void processFile(FILE *toOpen, char** buffer);
void processCommands(char** buffer, int* commandList);
void resetBuffer(char* buffer);

int main(int argc, char** argv) {

    assert(argc == 2);
    FILE *toOpen = fopen(argv[1], 'r');

    char buffer[1000][9] = '\0';
    processFile(toOpen, buffer);

    int commandList[1000] = -1;
    processCommands(buffer, commandList);

    int instruction = (argv[1][0] >> 26);
    printf("instruction code is: %d\n", instruction);

    // TODO if instruction is invalid, print error message
    if (!validCommand(instruction)) {
        printf("Invalid instruction code");
    }

    // initialising registers
    int registers[32] = 0;

}

void processFile(FILE *toOpen, char** buffer) {

    int toCheck = fgetc(toOpen);
    int indexLine = 0;
    int indexCommand = 0;

    while (toCheck != EOF) {
        if (toCheck == '\n') {
            indexLine++;
            indexCommand = 0;
            toCheck = fgetc(toOpen);
            continue;
        }
        buffer[indexLine][indexCommand] = toCheck;
        indexCommand++;
        toCheck = fgetc(toOpen);
    }

}

void processCommands(char** buffer, int* commandList) {

}


void resetBuffer(char* buffer) {
    for (int i = 0; i < 8; i++) {
        buffer[i] = '\0';
    }
}

bool validCommand(int instruction) {

}