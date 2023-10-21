
#include <stdio.h>
#include <string.h>
#include <ncurses.h>

#define true 1
#define false 0

#define OPERATION_TABLE_LENGTH 256
#define ZERO_PAGE_MAX 256
//#define LINE_SIZE 40
#define CHAR_NUM 256
#define MEMORY_SIZE 0x10000

#define IRQ_VECTOR 0xfffe
#define RESET_VECTOR 0xfffc

#define KEY_I_ADDRESS 0xc000
#define KEY_CLEAR_ADDRESS 0xc010

#define RAM_SIZE 0xbfff

#define ROM_START_ADDRESS 0xd000
#define ROM_SIZE 12288

// SYSTEM_FILE
//#define MEMORY_FILE_NAME "SYSTEM_DISK.bin"
#define ROM_FILE_NAME "ROM.bin"

//DISPLAY_MEMORY_AREA
#define DISPLAY_MEM0 0x400
#define DISPLAY_MEM1 0x480
#define DISPLAY_MEM2 0x500
#define DISPLAY_MEM3 0x580
#define DISPLAY_MEM4 0x600
#define DISPLAY_MEM5 0x680
#define DISPLAY_MEM6 0x700
#define DISPLAY_MEM7 0x780
#define DISPLAY_MEM8 0x428
#define DISPLAY_MEM9 0x4a8
#define DISPLAY_MEM10 0x528
#define DISPLAY_MEM11 0x5a8
#define DISPLAY_MEM12 0x628
#define DISPLAY_MEM13 0x6a8
#define DISPLAY_MEM14 0x728
#define DISPLAY_MEM15 0x7a8
#define DISPLAY_MEM16 0x450
#define DISPLAY_MEM17 0x4d0
#define DISPLAY_MEM18 0x550
#define DISPLAY_MEM19 0x5d0
#define DISPLAY_MEM20 0x650
#define DISPLAY_MEM21 0x6d0
#define DISPLAY_MEM22 0x750
#define DISPLAY_MEM23 0x7d0

#define DISPLAY_MEM_WIDTH 40

int memoryDestination = 0;
int displayMemoryAddress = 0;
int displayAreaAddress = 0;
int displayHCount = 0;

//testcode
int promptLIne = 0;
int lineCount = 0;
int charLIneNum = 0;
int debugFlg = 0;

enum{
  memory,
  display,
  keyboard
};


enum {
    accumulator,
    immediate,
    zeroPage,
    zeroPageX,
    zeroPageY,
    absolute,
    absoluteX,
    absoluteY,
    indirect,
    indirectX,
    indirectY,
    relative,
    implied
};

enum{
    adc,
    and,
    asl,
    bcc,
    bcs,
    beq,
    bit,
    bmi,
    bne,
    bpl,
    brk,
    bvc,
    bvs,
    clc,
    cld,
    cli,
    clv,
    cmp,
    cpx,
    cpy,
    dec,
    dex,
    dey,
    eor,
    inc,
    inx,
    iny,
    jmp,
    jsr,
    lda,
    ldx,
    ldy,
    lsr,
    nop,
    ora,
    pha,
    php,
    pla,
    plp,
    rol,
    ror,
    rti,
    rts,
    sbc,
    sec,
    sed,
    sei,
    sta,
    stx,
    sty,
    tax,
    tay,
    tsx,
    txa,
    txs,
    tya
};

typedef struct{
    unsigned char opcode;
    int opcodeName;
    int addressingMode;
}t_operationTable;

t_operationTable operationTable[OPERATION_TABLE_LENGTH] = {
    /* opcode   opcodeName  addressingMode**/
    { 0x69, adc, immediate },
    { 0x65, adc, zeroPage },
    { 0x75, adc, zeroPageX },
    { 0x6d, adc, absolute },
    { 0x7d, adc, absoluteX },
    { 0x79, adc, absoluteY },
    { 0x61, adc, indirectX },
    { 0x71, adc, indirectY },
    { 0x29, and, immediate },
    { 0x25, and, zeroPage },
    { 0x35, and, zeroPageX },
    { 0x2d, and, absolute },
    { 0x3d, and, absoluteX },
    { 0x39, and, absoluteY },
    { 0x21, and, indirectX },
    { 0x31, and, indirectY },
    { 0x0a, asl, accumulator },
    { 0x06, asl, zeroPage },
    { 0x16, asl, zeroPageX },
    { 0x0e, asl, absolute },
    { 0x1e, asl, absoluteX },
    { 0x90, bcc, relative },
    { 0xb0, bcs, relative },
    { 0xf0, beq, relative },
    { 0x24, bit, zeroPage },
    { 0x2c, bit, absolute },
    { 0x30, bmi, relative },
    { 0xd0, bne, relative },
    { 0x10, bpl, relative },
    { 0x00, brk, implied },
    { 0x50, bvc, relative },
    { 0x70, bvs, relative },
    { 0x18, clc, implied },
    { 0xd8, cld, implied },
    { 0x58, cli, implied },
    { 0xb8, clv, implied },
    { 0xc9, cmp, immediate },
    { 0xc5, cmp, zeroPage },
    { 0xd5, cmp, zeroPageX },
    { 0xcd, cmp, absolute },
    { 0xdd, cmp, absoluteX },
    { 0xd9, cmp, absoluteY },
    { 0xc1, cmp, indirectX },
    { 0xd1, cmp, indirectY },
    { 0xe0, cpx, immediate },
    { 0xe4, cpx, zeroPage },
    { 0xdc, cpx, absolute },
    { 0xc0, cpy, immediate },
    { 0xc4, cpy, zeroPage },
    { 0xcc, cpy, absolute },
    { 0xc6, dec, zeroPage },
    { 0xd6, dec, zeroPageX },
    { 0xce, dec, absolute },
    { 0xde, dec, absoluteX },
    { 0xca, dex, implied },
    { 0x88, dey, implied },
    { 0x49, eor, immediate },
    { 0x45, eor, zeroPage },
    { 0x55, eor, zeroPageX },
    { 0x4d, eor, absolute },
    { 0x5d, eor, absoluteX },
    { 0x59, eor, absoluteY },
    { 0x41, eor, indirectX },
    { 0x51, eor, indirectY },
    { 0xe6, inc, zeroPage },
    { 0xf6, inc, zeroPageX },
    { 0xee, inc, absolute },
    { 0xfe, inc, absoluteX },
    { 0xe8, inx, implied },
    { 0xc8, iny, implied },
    { 0x4c, jmp, absolute },
    { 0x6c, jmp, indirect },
    { 0x20, jsr, absolute },
    { 0xa9, lda, immediate },
    { 0xa5, lda, zeroPage },
    { 0xb5, lda, zeroPageX },
    { 0xad, lda, absolute },
    { 0xbd, lda, absoluteX },
    { 0xb9, lda, absoluteY },
    { 0xa1, lda, indirectX },
    { 0xb1, lda, indirectY },
    { 0xa2, ldx, immediate },
    { 0xa6, ldx, zeroPage },
    { 0xb6, ldx, zeroPageY},
    { 0xae, ldx, absolute },
    { 0xbe, ldx, absoluteY },
    { 0xa0, ldy, immediate },
    { 0xa4, ldy, zeroPage },
    { 0xb4, ldy, zeroPageX },
    { 0xac, ldy, absolute },
    { 0xbc, ldy, absoluteX },
    { 0x4a, lsr, accumulator },
    { 0x46, lsr, zeroPage },
    { 0x56, lsr, zeroPageX },
    { 0x4e, lsr, absolute },
    { 0x5e, lsr, absoluteX },
    { 0xea, nop, implied },
    { 0x09, ora, immediate },
    { 0x05, ora, zeroPage },
    { 0x15, ora, zeroPageX },
    { 0x0d, ora, absolute },
    { 0x1d, ora, absoluteX },
    { 0x19, ora, absoluteY },
    { 0x01, ora, indirectX },
    { 0x11, ora, indirectY },
    { 0x48, pha, implied },
    { 0x08, php, implied },
    { 0x68, pla, implied },
    { 0x28, plp, implied },
    { 0x2a, rol, accumulator },
    { 0x26, rol, zeroPage },
    { 0x36, rol, zeroPageX },
    { 0x2e, rol, absolute },
    { 0x3e, rol, absoluteX },
    { 0x6a, ror, accumulator },
    { 0x66, ror, zeroPage },
    { 0x76, ror, zeroPageX },
    { 0x6e, ror, absolute },
    { 0x7e, ror, absoluteX},
    { 0x40, rti, implied },
    { 0x60, rts, implied },
    { 0xe9, sbc, immediate },
    { 0xe5, sbc, zeroPage },
    { 0xf5, sbc, zeroPageX },
    { 0xed, sbc, absolute },
    { 0xfd, sbc, absoluteX },
    { 0xf9, sbc, absoluteY },
    { 0xe1, sbc, indirectX },
    { 0xf1, sbc, indirectY },
    { 0x38, sec, implied },
    { 0xf8, sed, implied },
    { 0x78, sei, implied },
    { 0x85, sta, zeroPage },
    { 0x95, sta, zeroPageX },
    { 0x8d, sta, absolute },
    { 0x9d, sta, absoluteX },
    { 0x99, sta, absoluteY },
    { 0x81, sta, indirectX },
    { 0x91, sta, indirectY },
    { 0x86, stx, zeroPage },
    { 0x96, stx, zeroPageY },
    { 0x8e, stx, absolute },
    { 0x84, sty, zeroPage },
    { 0x94, sty, zeroPageX },
    { 0x8c, sty, absolute },
    { 0xaa, tax, implied },
    { 0xa8, tay, implied },
    { 0xba, tsx, implied },
    { 0x8a, txa, implied },
    { 0x9a, txs, implied },
    { 0x98, tya, implied }
};


typedef struct{
    unsigned int code;
    char printChar;
}t_convertTbl;


t_convertTbl convertTbl[CHAR_NUM] = {
    {0x00, '@'},
    {0x01, 'A'},
    {0x02, 'B'},
    {0x03, 'C'},
    {0x04, 'D'},
    {0x05, 'E'},
    {0x06, 'F'},
    {0x07, 'G'},
    {0x08, 'H'},
    {0x09, 'I'},
    {0x0a, 'J'},
    {0x0b, 'K'},
    {0x0c, 'L'},
    {0x0d, 'M'},
    {0x0e, 'N'},
    {0x0f, 'O'},
    {0x10, 'P'},
    {0x11, 'Q'},
    {0x12, 'R'},
    {0x13, 'S'},
    {0x14, 'T'},
    {0x15, 'U'},
    {0x16, 'V'},
    {0x17, 'W'},
    {0x18, 'X'},
    {0x19, 'Y'},
    {0x1a, 'Z'},
    {0x1b, '['},
    {0x1c, '\\'},
    {0x1d, ']'},
    {0x1e, '^'},
    {0x1f, '_'},
    {0x20, ' '},
    {0x21, '!'},
    {0x22, '"'},
    {0x23, '#'},
    {0x24, '$'},
    {0x25, '%'},
    {0x26, '&'},
    {0x27, '\''},
    {0x28, '('},
    {0x29, ')'},
    {0x2a, '*'},
    {0x2b, '+'},
    {0x2c, ','},
    {0x2d, '-'},
    {0x2e, '.'},
    {0x2f, '/'},
    {0x30, '0'},
    {0x31, '1'},
    {0x32, '2'},
    {0x33, '3'},
    {0x34, '4'},
    {0x35, '5'},
    {0x36, '6'},
    {0x37, '7'},
    {0x38, '8'},
    {0x39, '9'},
    {0x3a, ':'},
    {0x3b, ';'},
    {0x3c, '<'},
    {0x3d, '='},
    {0x3e, '>'},
    {0x3f, '?'},
    {0x40, '@'},
    {0x41, 'A'},
    {0x42, 'B'},
    {0x43, 'C'},
    {0x44, 'D'},
    {0x45, 'E'},
    {0x46, 'F'},
    {0x47, 'G'},
    {0x48, 'H'},
    {0x49, 'I'},
    {0x4a, 'J'},
    {0x4b, 'K'},
    {0x4c, 'L'},
    {0x4d, 'M'},
    {0x4e, 'N'},
    {0x4f, 'O'},
    {0x50, 'P'},
    {0x51, 'Q'},
    {0x52, 'R'},
    {0x53, 'S'},
    {0x54, 'T'},
    {0x55, 'U'},
    {0x56, 'V'},
    {0x57, 'W'},
    {0x58, 'X'},
    {0x59, 'Y'},
    {0x5a, 'Z'},
    {0x5b, '['},
    {0x5c, '\\'},
    {0x5d, ']'},
    {0x5e, '^'},
    {0x5f, '_'},
    {0x60, '|'},
    {0x61, '!'},
    {0x62, '"'},
    {0x63, '#'},
    {0x64, '$'},
    {0x65, '%'},
    {0x66, '&'},
    {0x67, '\''},
    {0x68, '('},
    {0x69, ')'},
    {0x6a, '*'},
    {0x6b, '+'},
    {0x6c, ','},
    {0x6d, '-'},
    {0x6e, '.'},
    {0x6f, '/'},
    {0x70, '0'},
    {0x71, '1'},
    {0x72, '2'},
    {0x73, '3'},
    {0x74, '4'},
    {0x75, '5'},
    {0x76, '6'},
    {0x77, '7'},
    {0x78, '8'},
    {0x79, '9'},
    {0x7a, ':'},
    {0x7b, ';'},
    {0x7c, '<'},
    {0x7d, '='},
    {0x7e, '>'},
    {0x7f, '?'},
    {0x80, '@'},
    {0x81, 'A'},
    {0x82, 'B'},
    {0x83, 'C'},
    {0x84, 'D'},
    {0x85, 'E'},
    {0x86, 'F'},
    {0x87, 'G'},
    {0x88, 'H'},
    {0x89, 'I'},
    {0x8a, 'J'},
    {0x8b, 'K'},
    {0x8c, 'L'},
    {0x8d, 'M'},
    {0x8e, 'N'},
    {0x8f, 'O'},
    {0x90, 'P'},
    {0x91, 'Q'},
    {0x92, 'R'},
    {0x93, 'S'},
    {0x94, 'T'},
    {0x95, 'U'},
    {0x96, 'V'},
    {0x97, 'W'},
    {0x98, 'X'},
    {0x99, 'Y'},
    {0x9a, 'Z'},
    {0x9b, '['},
    {0x9c, '\\'},
    {0x9d, ']'},
    {0x9e, '^'},
    {0x9f, '_'},
    {0xa0, ' '},
    {0xa1, '!'},
    {0xa2, '"'},
    {0xa3, '#'},
    {0xa4, '$'},
    {0xa5, '%'},
    {0xa6, '&'},
    {0xa7, '\''},
    {0xa8, '('},
    {0xa9, ')'},
    {0xaa, '*'},
    {0xab, '+'},
    {0xac, ','},
    {0xad, '-'},
    {0xae, '.'},
    {0xaf, '/'},
    {0xb0, '0'},
    {0xb1, '1'},
    {0xb2, '2'},
    {0xb3, '3'},
    {0xb4, '4'},
    {0xb5, '5'},
    {0xb6, '6'},
    {0xb7, '7'},
    {0xb8, '8'},
    {0xb9, '9'},
    {0xba, ':'},
    {0xbb, ';'},
    {0xbc, '<'},
    {0xbd, '='},
    {0xbe, '>'},
    {0xbf, '?'},
    {0xc0, '@'},
    {0xc1, 'A'},
    {0xc2, 'B'},
    {0xc3, 'C'},
    {0xc4, 'D'},
    {0xc5, 'E'},
    {0xc6, 'F'},
    {0xc7, 'G'},
    {0xc8, 'H'},
    {0xc9, 'I'},
    {0xca, 'J'},
    {0xcb, 'K'},
    {0xcc, 'L'},
    {0xcd, 'M'},
    {0xce, 'N'},
    {0xcf, 'O'},
    {0xd0, 'P'},
    {0xd1, 'Q'},
    {0xd2, 'R'},
    {0xd3, 'S'},
    {0xd4, 'T'},
    {0xd5, 'U'},
    {0xd6, 'V'},
    {0xd7, 'W'},
    {0xd8, 'X'},
    {0xd9, 'Y'},
    {0xda, 'Z'},
    {0xdb, '['},
    {0xdc, '\\'},
    {0xdd, ']'},
    {0xde, '^'},
    {0xdf, '_'},
    {0xe0, ' '},
    {0xe1, '!'},
    {0xe2, '"'},
    {0xe3, '#'},
    {0xe4, '$'},
    {0xe5, '%'},
    {0xe6, '&'},
    {0xe7, '\''},
    {0xe8, '('},
    {0xe9, ')'},
    {0xea, '*'},
    {0xeb, '+'},
    {0xec, ','},
    {0xed, '-'},
    {0xee, '.'},
    {0xef, '/'},
    {0xf0, '0'},
    {0xf1, '1'},
    {0xf2, '2'},
    {0xf3, '3'},
    {0xf4, '4'},
    {0xf5, '5'},
    {0xf6, '6'},
    {0xf7, '7'},
    {0xf8, '8'},
    {0xf9, '9'},
    {0xfa, ':'},
    {0xfb, ';'},
    {0xfc, '<'},
    {0xfd, '='},
    {0xfe, '>'},
    {0xff, '?'}
};


typedef struct{
    int addressingMode;
    int opcode;
}t_decoded;


typedef struct{
    int opcode;
    int useRegister;
    unsigned char *operand;
    unsigned char *operand2;
    int useOperandFlg;
}t_inst;


typedef struct{
    unsigned int programCounter;
    unsigned char accumulator;
    unsigned char indexX;
    unsigned char indexY;
    unsigned char stackPointer;
} t_regist;


typedef struct{
   unsigned int negative : 1;
   unsigned int overFlow : 1;
   unsigned int dummy : 1;
   unsigned int breakCommand : 1;
   unsigned int decimalMode : 1;
   unsigned int interaputDisable : 1;
   unsigned int zero : 1;
   unsigned int carry : 1;
}t_statusBits;


typedef union{
    unsigned char buffer;
    t_statusBits bits;
} t_statusRegist;


unsigned char getOpecode( unsigned int address, unsigned char *memory ){
    unsigned char opcode = memory[address];
    return opcode;
}


t_decoded decode( unsigned char opcode ){
    t_decoded decoded = { 0 };
    
    for(int i = 0; OPERATION_TABLE_LENGTH > i; i++){
        if(operationTable[i].opcode == opcode){
            decoded.opcode = operationTable[i].opcodeName;
            decoded.addressingMode = operationTable[i].addressingMode;
            break;
        }
    }
    
    return decoded;
}


int isDisplayMemoryArea( unsigned int address ){
    int result = false;
    
    if(( address >= DISPLAY_MEM0 ) && ( address < (DISPLAY_MEM0 + DISPLAY_MEM_WIDTH))){
        displayMemoryAddress = address;
        displayAreaAddress = address - DISPLAY_MEM0;
        displayHCount = 0;
        result = true;
    }
    else if(( address >= DISPLAY_MEM1 ) && ( address < ( DISPLAY_MEM1 + DISPLAY_MEM_WIDTH ))){
        displayMemoryAddress = address;
        displayAreaAddress = address - DISPLAY_MEM1;
        displayHCount = 1;
        result = true;
    }
    else if(( address >= DISPLAY_MEM2 ) && ( address < ( DISPLAY_MEM2 + DISPLAY_MEM_WIDTH ))){
        displayMemoryAddress = address;
        displayAreaAddress = address - DISPLAY_MEM2;
        displayHCount = 2;
        result = true;
    }
    else if(( address >= DISPLAY_MEM3 ) && ( address < ( DISPLAY_MEM3 + DISPLAY_MEM_WIDTH ))){
        displayMemoryAddress = address;
        displayAreaAddress = address - DISPLAY_MEM3;
        displayHCount = 3;
        result = true;
    }
    else if(( address >= DISPLAY_MEM4 ) && ( address < ( DISPLAY_MEM4 + DISPLAY_MEM_WIDTH ))){
        displayMemoryAddress = address;
        displayAreaAddress = address - DISPLAY_MEM4;
        displayHCount = 4;
        result = true;
    }
    else if(( address >= DISPLAY_MEM5 ) && ( address < ( DISPLAY_MEM5 + DISPLAY_MEM_WIDTH ))){
        displayMemoryAddress = address;
        displayAreaAddress = address - DISPLAY_MEM5;
        displayHCount = 5;
        result = true;
    }
    else if(( address >= DISPLAY_MEM6 ) && ( address < ( DISPLAY_MEM6 + DISPLAY_MEM_WIDTH ))){
        displayMemoryAddress = address;
        displayAreaAddress = address - DISPLAY_MEM6;
        displayHCount = 6;
        result = true;
    }
    else if(( address >= DISPLAY_MEM7 ) && ( address < ( DISPLAY_MEM7 + DISPLAY_MEM_WIDTH ))){
        displayMemoryAddress = address;
        displayAreaAddress = address - DISPLAY_MEM7;
        displayHCount = 7;
        result = true;
    }
    else if(( address >= DISPLAY_MEM8 ) && ( address < ( DISPLAY_MEM8 + DISPLAY_MEM_WIDTH ))){
        displayMemoryAddress = address;
        displayAreaAddress = address - DISPLAY_MEM8;
        displayHCount = 8;
        result = true;
    }
    else if(( address >= DISPLAY_MEM9 ) && ( address < ( DISPLAY_MEM9 + DISPLAY_MEM_WIDTH ))){
        displayMemoryAddress = address;
        displayAreaAddress = address - DISPLAY_MEM9;
        displayHCount = 9;
        result = true;
    }
    else if(( address >= DISPLAY_MEM10 ) && ( address < ( DISPLAY_MEM10 + DISPLAY_MEM_WIDTH ))){
        displayMemoryAddress = address;
        displayAreaAddress = address - DISPLAY_MEM10;
        displayHCount = 10;
        result = true;
    }
    else if(( address >= DISPLAY_MEM11 ) && ( address < ( DISPLAY_MEM11 + DISPLAY_MEM_WIDTH ))){
        displayMemoryAddress = address;
        displayAreaAddress = address - DISPLAY_MEM11;
        displayHCount = 11;
        result = true;
    }
    else if(( address >= DISPLAY_MEM12 ) && ( address < ( DISPLAY_MEM12 + DISPLAY_MEM_WIDTH ))){
        displayMemoryAddress = address;
        displayAreaAddress = address - DISPLAY_MEM12;
        displayHCount = 12;
        result = true;
    }
    else if(( address >= DISPLAY_MEM13 ) && ( address < ( DISPLAY_MEM13 + DISPLAY_MEM_WIDTH ))){
        displayMemoryAddress = address;
        displayAreaAddress = address - DISPLAY_MEM13;
        displayHCount = 13;
        result = true;
    }
    else if(( address >= DISPLAY_MEM14 ) && ( address < ( DISPLAY_MEM14 + DISPLAY_MEM_WIDTH ))){
        displayMemoryAddress = address;
        displayAreaAddress = address - DISPLAY_MEM14;
        displayHCount = 14;
        result = true;
    }
    else if(( address >= DISPLAY_MEM15 ) && ( address < ( DISPLAY_MEM15 + DISPLAY_MEM_WIDTH ))){
        displayMemoryAddress = address;
        displayAreaAddress = address - DISPLAY_MEM15;
        displayHCount = 15;
        result = true;
    }
    else if(( address >= DISPLAY_MEM16 ) && ( address < ( DISPLAY_MEM16 + DISPLAY_MEM_WIDTH ))){
        displayMemoryAddress = address;
        displayAreaAddress = address - DISPLAY_MEM16;
        displayHCount = 16;
        result = true;
    }
    else if(( address >= DISPLAY_MEM17 ) && ( address < ( DISPLAY_MEM17 + DISPLAY_MEM_WIDTH ))){
        displayMemoryAddress = address;
        displayAreaAddress = address - DISPLAY_MEM17;
        displayHCount = 17;
        result = true;
    }
    else if(( address >= DISPLAY_MEM18 ) && ( address < ( DISPLAY_MEM18 + DISPLAY_MEM_WIDTH ))){
        displayMemoryAddress = address;
        displayAreaAddress = address - DISPLAY_MEM18;
        displayHCount = 18;
        result = true;
    }
    else if(( address >= DISPLAY_MEM19 ) && ( address < ( DISPLAY_MEM19 + DISPLAY_MEM_WIDTH ))){
        displayMemoryAddress = address;
        displayAreaAddress = address - DISPLAY_MEM19;
        displayHCount = 19;
        result = true;
    }
    else if(( address >= DISPLAY_MEM20 ) && ( address < ( DISPLAY_MEM20 + DISPLAY_MEM_WIDTH ))){
        displayMemoryAddress = address;
        displayAreaAddress = address - DISPLAY_MEM20;
        displayHCount = 20;
        result = true;
    }
    else if(( address >= DISPLAY_MEM21 ) && ( address < ( DISPLAY_MEM21 + DISPLAY_MEM_WIDTH ))){
        displayMemoryAddress = address;
        displayAreaAddress = address - DISPLAY_MEM21;
        displayHCount = 21;
        result = true;
    }
    else if(( address >= DISPLAY_MEM22 ) && ( address < ( DISPLAY_MEM22 + DISPLAY_MEM_WIDTH ))){
        displayMemoryAddress = address;
        displayAreaAddress = address - DISPLAY_MEM22;
        displayHCount = 22;
        result = true;
    }
    else if(( address >= DISPLAY_MEM23 ) && ( address < ( DISPLAY_MEM23 + DISPLAY_MEM_WIDTH ))){
        displayMemoryAddress = address;
        displayAreaAddress = address - DISPLAY_MEM23;
        displayHCount = 23;
        result = true;
    }
    
    return result;
}


void switchMemoryDestination( unsigned int address, unsigned char *mem ){
    
    if( isDisplayMemoryArea( address )){
        memoryDestination = display;        
    }
    else if( address == KEY_CLEAR_ADDRESS ){
        mem[KEY_I_ADDRESS] &= 0x7f;
    }
    else{
        memoryDestination = memory;
    }

}


void getInst( t_regist *regist, unsigned char *memory, t_inst *inst ){
    //t_inst inst = { 0 };
    unsigned int address = 0;
    unsigned int preAddress = 0;
    unsigned int preAddress2 = 0;
    unsigned int operand = 0;
    
    // get opcode
    unsigned char opcode = getOpecode( regist->programCounter, memory );
        
    // addressing
    t_decoded decoded = decode( opcode );
    inst->opcode = decoded.opcode;
    
    switch( decoded.addressingMode ){
        case accumulator:
            inst->operand = &regist->accumulator;
            inst->useOperandFlg = true;
            regist->programCounter += 1;
            break;
            
        case immediate:
            inst->operand = &memory[regist->programCounter + 1];
            switchMemoryDestination( regist->programCounter + 1, memory );
            inst->useOperandFlg = true;
            regist->programCounter += 2;
            break;
            
        case zeroPage:
            address = memory[regist->programCounter + 1] & 0xff;
            inst->operand = &memory[address];
            switchMemoryDestination( address, memory );
            inst->useOperandFlg = true;
            regist->programCounter += 2;
            break;
            
        case zeroPageX:
            preAddress = memory[regist->programCounter + 1] + regist->indexX;
            address = preAddress & 0xff;
            inst->operand = &memory[address];
            switchMemoryDestination( address,memory );
            inst->useOperandFlg = true;
            regist->programCounter += 2;
            break;
            
        case zeroPageY:
            preAddress = memory[regist->programCounter + 1] + regist->indexY;
            address = preAddress & 0xff;
            inst->operand = &memory[address];
            switchMemoryDestination( address,memory );
            inst->useOperandFlg = true;
            regist->programCounter += 2;
            break;
            
        case absolute:
            if( (inst->opcode == jsr) || (inst->opcode == jmp) ){
                inst->operand = &memory[regist->programCounter + 1];
                inst->operand2 = &memory[regist->programCounter + 2];
                switchMemoryDestination( regist->programCounter + 1, memory );
                //inst->intOperand = &address;
                //testInst->intOperand = &address;
                inst->useOperandFlg = false;
            }
            else{
                address = memory[regist->programCounter + 1] & 0xff;
                address |= ((memory[regist->programCounter + 2] << 8) & 0xff00);
                inst->operand = &memory[address];
                switchMemoryDestination( address,memory );
                inst->useOperandFlg = true;
                regist->programCounter += 3;
            }
            break;
            
        case absoluteX:
            address = memory[regist->programCounter + 1] & 0xff;
            address |= ((memory[regist->programCounter + 2] << 8) & 0xff00);
            address += (regist->indexX & 0xff);
            inst->operand = &memory[address];
            switchMemoryDestination( address,memory );
            inst->useOperandFlg = true;
            regist->programCounter += 3;
            break;
            
        case absoluteY:
            address = memory[regist->programCounter + 1] & 0xff;
            address |= ((memory[regist->programCounter + 2] << 8) & 0xff00);
            address += (regist->indexY & 0xff);
            inst->operand = &memory[address];
            switchMemoryDestination( address,memory );
            inst->useOperandFlg = true;
            regist->programCounter += 3;
            break;
            
        case indirect:
            preAddress = memory[regist->programCounter + 1] & 0xff;
            preAddress |= ((memory[regist->programCounter + 2] << 8) & 0xff00);
            inst->operand = &memory[preAddress];
            inst->operand2 = &memory[preAddress + 1];
            switchMemoryDestination( preAddress,memory );
            //inst->operand = &memory[address];
            inst->useOperandFlg = false;
            regist->programCounter += 3;
            break;
            
        case indirectX:
            operand = memory[regist->programCounter + 1];
            //preAddress = memory[operand] & 0xff;
            preAddress = operand + regist->indexX;
            preAddress &= 0xff;            
            preAddress2 = (preAddress + 1) & 0xff;
            address = memory[preAddress] & 0xff;
            address |= ((memory[preAddress2] << 8) & 0xff00);
            inst->operand = &memory[address];
            switchMemoryDestination( address,memory );
            inst->useOperandFlg = true;
            regist->programCounter += 2;
            break;
            
        case indirectY:
            preAddress = memory[regist->programCounter + 1] & 0xff;
            preAddress2 = ( preAddress + 1) & 0xff;
            address = memory[preAddress];
            address |= (( memory[preAddress2] << 8) & 0xff00);
            address += regist->indexY;
            inst->operand = &memory[address];
            switchMemoryDestination( address,memory );
            inst->useOperandFlg = true;
            regist->programCounter += 2;
            break;
            
        case relative:
            inst->operand = &memory[regist->programCounter + 1];
            switchMemoryDestination( regist->programCounter + 1,memory );
            inst->useOperandFlg = true;
            regist->programCounter += 2;
            break;
            
        case implied:
            break;
            
        default:
            break;
    }
    
    //return inst;
}

void updateCarry( t_statusRegist *statusRegist, unsigned int tmp){
    if(((tmp & 0x100) >> 8) == 1){
        statusRegist->bits.carry = true;
    }
    else{
        statusRegist->bits.carry = false;
    }
    
}

void updateCarry2( t_statusRegist *statusRegist, unsigned int tmp){
    if(((tmp & 0x100) >> 8) == 1){
        statusRegist->bits.carry = false;
    }
    else{
        statusRegist->bits.carry = true;
    }
}


void updateOverFlow( t_statusRegist *statusRegist,unsigned int preTmp){    
    //pre
    unsigned int preUp = ( preTmp & 0x80 ) >> 7;

    if(( statusRegist->bits.carry == false ) && (preUp == true)){
        statusRegist->bits.overFlow = true;
    }
    else{
        statusRegist->bits.overFlow = false;
    }
}


void updateOverFlow2( t_statusRegist *statusRegist,unsigned int preTmp){
    unsigned int preUp = ( preTmp & 0x80 ) >> 7;

    if(( statusRegist->bits.carry == true ) && (preUp == true)){
        statusRegist->bits.overFlow = true;
    }
    else{
        statusRegist->bits.overFlow = false;
    }
}


void updateNegative( t_statusRegist *statusRegist, unsigned int tmp){
    
    if(((tmp & 0x80) >> 7) == 1){
        statusRegist->bits.negative = true;
    }
    else{
        statusRegist->bits.negative = false;
    }
    
}

void updateZero( t_statusRegist *statusRegist, unsigned int tmp){
    tmp &= 0xff;
    if( tmp == 0 ){
        statusRegist->bits.zero = true;
    }
    else{
        statusRegist->bits.zero = false;
    }
}

void pushToStack( unsigned int programCounter, t_statusRegist statusRegist, unsigned char *stackPointer, unsigned char *memory){
    unsigned int stackAddress = ((*stackPointer) | 0x100) & 0x1ff;
    
    //memory[stackAddress--] = statusRegist.buffer;
    memory[stackAddress--] = (programCounter & 0xff00) >> 8;
    memory[stackAddress--] = programCounter & 0xff;
    memory[stackAddress--] = statusRegist.buffer;
    switchMemoryDestination( stackAddress,memory );
    
    *stackPointer = stackAddress & 0xff;
}


void pushToStack2( unsigned int programCounter, unsigned char *stackPointer, unsigned char *memory){
    unsigned int stackAddress = ((*stackPointer) | 0x100) & 0x1ff;
 
    memory[stackAddress--] = (programCounter & 0xff00) >> 8;
    memory[stackAddress--] = programCounter & 0xff;
    switchMemoryDestination( stackAddress,memory );
    
    *stackPointer = stackAddress & 0xff;
}

void pushToStack3( unsigned char data, unsigned char *stackPointer, unsigned char *memory){
    unsigned int stackAddress = ((*stackPointer) | 0x100) & 0x1ff;
    
    memory[stackAddress--] = data;
    switchMemoryDestination( stackAddress,memory );
    
    *stackPointer = stackAddress & 0xff;
}




unsigned int pullFromStack2( unsigned char *stackPointer, unsigned char *memory){
    unsigned int data;
    unsigned int stackAddress = ((*stackPointer) | 0x100) & 0x1ff;

    data = memory[++stackAddress] & 0xff;
    data |= ((memory[++stackAddress] << 8 ) & 0xff00);
    *stackPointer = stackAddress & 0xff;
    switchMemoryDestination( stackAddress,memory );
    
    return data;
}


unsigned char pullFromStack3( unsigned char *stackPointer, unsigned char *memory){
    unsigned char data;
    unsigned int stackAddress = ((*stackPointer) | 0x100) & 0x1ff;
    
    data = memory[++stackAddress];
    *stackPointer = stackAddress & 0xff;
    switchMemoryDestination( stackAddress,memory );
    
    return data;
}

void execute(t_inst inst, t_regist *regist, t_statusRegist *statusRegist, unsigned char *memory){
    unsigned int tmp = 0;
    unsigned int tmp2 =0;
    unsigned int preTmp = 0;
    int decimalAcumLow = 0;
    int decimalAcumHigh = 0;
    int decimalAcum = 0;
    int decimalOpLow = 0;
    int decimalOpHigh = 0;
    int decimalOp = 0;
    
    switch( inst.opcode ){
        case adc:
            if(statusRegist->bits.decimalMode == false){
                //pre
                preTmp = ( regist->accumulator & 0x7f ) + ( *inst.operand & 0x7f ) + statusRegist->bits.carry;
                tmp = regist->accumulator + *inst.operand + statusRegist->bits.carry;
                updateCarry(statusRegist, tmp);
                //updateOverFlow(statusRegist, *inst.operand, regist->accumulator, tmp);
                updateOverFlow(statusRegist, preTmp );
                updateNegative(statusRegist, tmp);
                updateZero(statusRegist, tmp);
                //*inst.operand = tmp;
                regist->accumulator = tmp;
            }
            else{
                decimalAcumLow = regist->accumulator & 0xff;
                decimalAcumHigh = (regist->accumulator & 0xff00) >> 8;
                decimalAcum = (decimalAcumHigh * 10) + decimalAcumLow;
                
                decimalOpLow = *inst.operand & 0xff;
                decimalOpHigh = (*inst.operand & 0xff00) >> 8;
                decimalOp = (decimalOpHigh * 10) + decimalOpLow;
                
                tmp = decimalAcum + decimalOp + statusRegist->bits.carry;

                updateCarry(statusRegist, tmp);
                //updateOverFlow(statusRegist, *inst.operand, regist->accumulator, tmp);
                updateNegative(statusRegist, tmp);
                updateZero(statusRegist, tmp);
                //*inst.operand = tmp;
                regist->accumulator = tmp;
            }
            
            break;
            
        case and:
            //tmp = regist->accumulator & *inst.operand;
            regist->accumulator &= *inst.operand;
            //tmp &= 0xff;
            updateNegative(statusRegist, regist->accumulator );
            updateZero(statusRegist, regist->accumulator );
            //*inst.operand = tmp;
            //regist->accumulator = tmp;
            break;
            
        case asl:
            statusRegist->bits.carry = (*inst.operand & 0x80) >> 7;
            //*inst.operand = ((*inst.operand) << 1) & 0xff;
            *inst.operand *= 2;
            updateZero(statusRegist, *inst.operand);
            updateNegative(statusRegist, *inst.operand);
            break;
            
        case bcc:
            if(statusRegist->bits.carry == false){
                if( *inst.operand & 0x80 ){
                    regist->programCounter += ((*inst.operand) | 0xff00);
                    regist->programCounter &= 0xffff;
                }
                else{
                    regist->programCounter += (*inst.operand) & 0xff;
                }
                
            }
            else{
                //regist->programCounter += 2;
            }
            break;
            
        case bcs:
            if(statusRegist->bits.carry == true){
                if( *inst.operand & 0x80 ){
                    //tmp = *inst.operand & 0x7e;
                    regist->programCounter += ((*inst.operand) | 0xff00);
                    regist->programCounter &= 0xffff;
                }
                else{
                    regist->programCounter += (*inst.operand) & 0xff;
                }
                
            }
            else{
                //regist->programCounter += 2;
            }
            break;
            
        case beq:
            if(statusRegist->bits.zero == true){
                if( *inst.operand & 0x80 ){
                    regist->programCounter += ((*inst.operand) | 0xff00);
                    regist->programCounter &= 0xffff;
                }
                else{
                    regist->programCounter += (*inst.operand) & 0xff;
                }
                
            }
            else{
                //regist->programCounter += 2;
            }
            break;
            
        case bit:
            tmp = (*inst.operand) & regist->accumulator;
            updateZero(statusRegist, tmp );
            statusRegist->bits.negative = (tmp & 0x80) >> 7;
            statusRegist->bits.negative |= ((*inst.operand) & 0x80) >> 7;
            statusRegist->bits.overFlow = ((*inst.operand) & 0x40) >> 6;
            //statusRegist->bits.overFlow = (tmp & 0x40) >> 6;
            break;
            
        case bmi:
            if(statusRegist->bits.negative == true){
                if( *inst.operand & 0x80 ){
                    regist->programCounter += ((*inst.operand) | 0xff00);
                    regist->programCounter &= 0xffff;
                }
                else{
                    regist->programCounter += (*inst.operand) & 0xff;
                }
                    
            }
            else{
                //regist->programCounter += 2;
            }
            break;
            
        case bne:
            if(statusRegist->bits.zero == false){
                if( *inst.operand & 0x80 ){
                    regist->programCounter += ((*inst.operand) | 0xff00);
                    regist->programCounter &= 0xffff;
                }
                else{
                    regist->programCounter += (*inst.operand) & 0xff;
                }
            }
            else{
                //regist->programCounter += 2;
            }
            break;
            
        case bpl:
            if(statusRegist->bits.negative == false){
                if( *inst.operand & 0x80 ){
                    regist->programCounter += ((*inst.operand) | 0xff00);
                    regist->programCounter &= 0xffff;
                }
                else{
                    regist->programCounter += (*inst.operand) & 0xff;
                }
                    
            }
            else{
                //regist->programCounter += 2;
            }
            break;
            
        case brk:
            statusRegist->bits.breakCommand = true;
            pushToStack(regist->programCounter + 2, *statusRegist,
                        &regist->stackPointer, memory);
            regist->programCounter = memory[IRQ_VECTOR] & 0xff;
            regist->programCounter |= ((memory[IRQ_VECTOR + 1] << 8) & 0xff00);
            break;
            
        case bvc:
            if(statusRegist->bits.overFlow == false){
                if( *inst.operand & 0x80 ){
                    regist->programCounter += ((*inst.operand) | 0xff00);
                    regist->programCounter &= 0xffff;
                }
                else{
                    regist->programCounter += (*inst.operand) & 0xff;
                }
            }
            else{
                //regist->programCounter += 2;
            }
            break;
            
        case bvs:
            if(statusRegist->bits.overFlow == true){
                if( *inst.operand & 0x80 ){
                    regist->programCounter += ((*inst.operand) | 0xff00);
                    regist->programCounter &= 0xffff;
                }
                else{
                    regist->programCounter += (*inst.operand) & 0xff;
                }
            }
            else{
                //regist->programCounter += 2;
            }
            break;
            
        case clc:
            statusRegist->bits.carry = false;
            regist->programCounter += 1;
            break;
            
        case cld:
            statusRegist->bits.decimalMode = false;
            regist->programCounter += 1;
            break;
            
        case cli:
            statusRegist->bits.interaputDisable = false;
            regist->programCounter += 1;
            break;
            
        case clv:
            statusRegist->bits.overFlow = false;
            regist->programCounter += 1;
            break;
            
        case cmp:
            tmp = regist->accumulator - *inst.operand;
            updateZero(statusRegist, tmp );
            updateNegative(statusRegist, tmp );
            //updateCarry(statusRegist, tmp);
            updateCarry2(statusRegist, tmp);
            //statusRegist->bits.carry = !statusRegist->bits.negative;
            break;
            
        case cpx:
            tmp = regist->indexX - *inst.operand;
            updateZero(statusRegist, tmp);
            updateNegative(statusRegist, tmp);
            //updateCarry(statusRegist, tmp);
            updateCarry2(statusRegist, tmp);
            //statusRegist->bits.carry = !statusRegist->bits.negative;
            break;
            
        case cpy:
            tmp = regist->indexY - *inst.operand;
            updateZero(statusRegist, tmp);
            updateNegative(statusRegist, tmp);
            //updateCarry(statusRegist, tmp);
            updateCarry2(statusRegist, tmp);
            //statusRegist->bits.carry = !statusRegist->bits.negative;
            break;
            
        case dec:
            *inst.operand -= 1;
            updateZero(statusRegist, *inst.operand );
            updateNegative(statusRegist, *inst.operand );
            //regist->programCounter += 1;
            break;
            
        case dex:
            regist->indexX -= 1;
            updateZero(statusRegist, regist->indexX );
            updateNegative(statusRegist, regist->indexX );
            regist->programCounter += 1;
            break;
            
        case dey:
            regist->indexY -= 1;
            updateZero(statusRegist, regist->indexY );
            updateNegative(statusRegist, regist->indexY );
            regist->programCounter += 1;
            break;
            
        case eor:
            regist->accumulator = (*inst.operand) ^ regist->accumulator;
            updateZero(statusRegist, regist->accumulator );
            updateNegative(statusRegist, regist->accumulator );
            break;
            
        case inc:
            (*inst.operand) += 1;
            updateZero(statusRegist, *inst.operand );
            updateNegative(statusRegist, *inst.operand );
            break;
            
        case inx:
            regist->indexX += 1;
            updateZero(statusRegist, regist->indexX );
            updateNegative(statusRegist, regist->indexX );
            regist->programCounter += 1;
            break;
            
        case iny:
            regist->indexY += 1;
            updateZero(statusRegist, regist->indexY );
            updateNegative(statusRegist, regist->indexY );
            regist->programCounter += 1;
            break;
            
        case jmp:
            if( inst.useOperandFlg ){
                regist->programCounter = (*inst.operand) & 0xff;
            }
            else{
                regist->programCounter = (*inst.operand) & 0xff;
                regist->programCounter |= (((*inst.operand2) << 8) & 0xff00);
                //regist->programCounter = *inst.intOperand & 0xffff;
            }
            break;
            
        case jsr:
            pushToStack2(regist->programCounter + 2, &regist->stackPointer, memory);
            //regist->programCounter = *inst.intOperand & 0xffff;
            //regist->programCounter = *testInst.intOperand & 0xffff;
            regist->programCounter = (*inst.operand) & 0xff;
            regist->programCounter |= (((*inst.operand2) << 8) & 0xff00);
            break;
            
        case lda:
            regist->accumulator = *inst.operand;
            updateZero(statusRegist, regist->accumulator );
            updateNegative(statusRegist, regist->accumulator );
            break;
            
        case ldx:
            regist->indexX = *inst.operand;
            updateZero(statusRegist, regist->indexX );
            updateNegative(statusRegist, regist->indexX );
            break;
            
        case ldy:
            regist->indexY = *inst.operand;
            updateZero(statusRegist, regist->indexY );
            updateNegative(statusRegist, regist->indexY );
            break;
            
        case lsr:
            statusRegist->bits.carry = *inst.operand & 0x01;
            *inst.operand >>= 1;
            statusRegist->bits.negative = false;
            updateZero(statusRegist, *inst.operand );
            break;
            
        case nop:
            regist->programCounter += 1;
            break;
            
        case ora:
            regist->accumulator |= *inst.operand;
            updateZero(statusRegist, regist->accumulator );
            updateNegative(statusRegist, regist->accumulator );
            break;
            
        case pha:
            pushToStack3(regist->accumulator, &regist->stackPointer, memory);
            regist->programCounter += 1;
            break;
            
        case php:
            pushToStack3(statusRegist->buffer, &regist->stackPointer, memory);
            regist->programCounter += 1;
            break;
            
        case pla:
            regist->accumulator = pullFromStack3(&regist->stackPointer, memory);
            regist->programCounter += 1;
            updateZero(statusRegist, regist->accumulator);
            updateNegative(statusRegist, regist->accumulator);

            break;
            
        case plp:
            statusRegist->buffer = pullFromStack3(&regist->stackPointer, memory);
            regist->programCounter += 1;
            break;
            
        case rol:
            tmp = statusRegist->bits.carry & 0x01;
            statusRegist->bits.carry = (*inst.operand & 0x80) >> 7;
            *inst.operand <<= 1;
            *inst.operand |= tmp;
            updateZero(statusRegist, *inst.operand );
            updateNegative(statusRegist, *inst.operand );
            break;
            
        case ror:
            tmp = (*inst.operand) & 0x01;
            *inst.operand >>= 1;
            *inst.operand |= ((statusRegist->bits.carry << 7) & 0x80);
            statusRegist->bits.carry = tmp;
            updateZero(statusRegist, *inst.operand);
            updateNegative(statusRegist, *inst.operand);
            break;
            
        case rti:
            statusRegist->buffer = pullFromStack3(&regist->stackPointer, memory);
            regist->programCounter = pullFromStack2(&regist->stackPointer, memory);
            break;
            
        case rts:
            regist->programCounter = pullFromStack2(&regist->stackPointer, memory);
            regist->programCounter += 1;
            break;
            
        case sbc:
            if(statusRegist->bits.decimalMode == false){
                preTmp = ( regist->accumulator & 0x7f ) - ( *inst.operand & 0x7f ) - (!statusRegist->bits.carry);
                tmp = regist->accumulator - *inst.operand - (!statusRegist->bits.carry);
                updateZero(statusRegist, tmp );
                updateNegative(statusRegist, tmp);
                //updateCarry(statusRegist, tmp);
                updateCarry2(statusRegist, tmp);
                updateOverFlow2( statusRegist, preTmp );
                //*inst.operand = tmp;
                regist->accumulator = tmp;
            }
            else{
                decimalAcumLow = regist->accumulator & 0xff;
                decimalAcumHigh = (regist->accumulator & 0xff00) >> 8;
                decimalAcum = (decimalAcumHigh * 10) + decimalAcumLow;
                
                decimalOpLow = *inst.operand & 0xff;
                decimalOpHigh = (*inst.operand & 0xff00) >> 8;
                decimalOp = (decimalOpHigh * 10) + decimalOpLow;

                tmp = regist->accumulator - *inst.operand - (!statusRegist->bits.carry);
                updateZero(statusRegist, tmp);
                updateNegative(statusRegist, tmp);
                updateCarry(statusRegist, tmp);
                //updateOverFlow(statusRegist, *inst.operand, regist->accumulator, tmp);
                //*inst.operand = tmp;
                regist->accumulator = tmp;
            }
            break;
            
        case sec:
            statusRegist->bits.carry = true;
            regist->programCounter += 1;
            break;
            
        case sed:
            statusRegist->bits.decimalMode = true;
            regist->programCounter += 1;
            break;
            
        case sei:
            statusRegist->bits.interaputDisable = true;
            regist->programCounter += 1;
            break;
            
        case sta:
            *inst.operand = regist->accumulator;
            break;
            
        case stx:
            *inst.operand = regist->indexX;
            break;
            
        case sty:
            *inst.operand = regist->indexY;
            break;
            
        case tax:
            regist->indexX = regist->accumulator;
            updateZero(statusRegist, regist->indexX);
            updateNegative(statusRegist, regist->indexX);
            regist->programCounter += 1;
            break;
            
        case tay:
            regist->indexY = regist->accumulator;
            updateZero(statusRegist, regist->indexY);
            updateNegative(statusRegist, regist->indexY);
            regist->programCounter += 1;
            break;
            
        case tsx:
            regist->indexX = regist->stackPointer;
            updateZero(statusRegist, regist->indexX);
            updateNegative(statusRegist, regist->indexX);
            regist->programCounter += 1;
            break;
            
        case txa:
            regist->accumulator = regist->indexX;
            updateZero(statusRegist, regist->accumulator);
            updateNegative(statusRegist, regist->accumulator);
            regist->programCounter += 1;
            break;
            
        case txs:
            regist->stackPointer = regist->indexX;
            regist->programCounter += 1;
            break;
            
        case tya:
            regist->accumulator = regist->indexY;
            updateZero(statusRegist, regist->accumulator);
            updateNegative(statusRegist, regist->accumulator);
            regist->programCounter += 1;
            break;
            
        default:
            break;
    }
}


int getKeyInput( unsigned char *memory ){
    int reset = false;
    
    //char inputBuf[2] = {'\0'};
    unsigned int inputBuf = 0;
    
    //scanf("%s", inputBuf);
    inputBuf = getch();
    
    if( inputBuf != -1 ){

        switch(inputBuf){
            case '\n':
                inputBuf = 0x8d;
                break;

            case '^':
                inputBuf = 0x83;
                break;

            case 'r':
             reset = true;
             break;   

            default:
                break;
        }

        if(!reset){
            inputBuf |= 0x80;
            memory[KEY_I_ADDRESS] = inputBuf & 0xff;
        }
    }
       
    return reset;
}


//testcode
void testKeyinput( int *count, unsigned char *memory){   
    char buffer[5] = "F666G";
    char buffer2[12] = "800:LDA #$31";

    if( (*count) <= 4 ){
        if( ((memory[KEY_I_ADDRESS] & 0x80) >> 7) == 0 ){
            memory[KEY_I_ADDRESS] = buffer[(*count)++];
            memory[KEY_I_ADDRESS] |= 0x80;
        }
    }
    else if( ((*count) == 5) || ( (*count) == 18)) {
        if( ((memory[KEY_I_ADDRESS] & 0x80) >> 7) == 0 ){
            memory[KEY_I_ADDRESS] = 0x8d;
            (*count)++;
        }
    }
    else if( (*count) >= 6){
        if( ((memory[KEY_I_ADDRESS] & 0x80) >> 7) == 0 ){
            memory[KEY_I_ADDRESS] = buffer2[((*count)++) - 6 ];
            memory[KEY_I_ADDRESS] |= 0x80;
        }

    }

}


int initMemory( unsigned char *memory ){
    FILE *fp;
    int ready = false;

    unsigned char initCont = 0x00;
    memset(memory, initCont, MEMORY_SIZE);

    fp =fopen(ROM_FILE_NAME, "rb");

    if( fp != NULL ){
        fread(&memory[ROM_START_ADDRESS], sizeof(char), ROM_SIZE, fp);
        fclose(fp);
        ready = true;
    }
    else{
        ready = false;
        fclose(fp);
    }

    return ready;
}


void initRegist( t_regist *regist, unsigned char *memory ){
    
    regist->stackPointer = 0xfd;
    unsigned int entryPoint = memory[RESET_VECTOR] & 0xff;
    entryPoint |= ((memory[RESET_VECTOR + 1] << 8) & 0xff00);
    regist->programCounter = entryPoint;
    
    regist->accumulator = 0;
    regist->indexX = 0;
    regist->indexY = 0;
    
}


char convertCharCode( unsigned char code){
    unsigned char printChar = '\0';

    for(int i = 0; CHAR_NUM > i ;i++){
        if(convertTbl[i].code == ( code & 0xff)){
            printChar = convertTbl[i].printChar;
            break;
        }
    }
    
    return printChar;
}


void printLine( int lineNum, unsigned char *memory ){
    char printChar;
    char printBuffer[DISPLAY_MEM_WIDTH + 1] = "";
    printBuffer[DISPLAY_MEM_WIDTH] = '\0';
    int i = 0;
    for(i = 0; DISPLAY_MEM_WIDTH > i; i++){
        if(memory[i] != '\0'){
            printBuffer[i] = convertCharCode(memory[i]);
        }
    }
    mvprintw(lineNum, 0, printBuffer);    
}


void monitor( unsigned char *memory ){
    int lineNum = 0;
    char printChar[2];

    if( memoryDestination == display ){
        lineCount = 0;
        charLIneNum = 0;
        
        if( memory[displayMemoryAddress] != '\0'){
            printChar[0] = convertCharCode( memory[displayMemoryAddress]);
            printChar[1] = '\0';
            mvprintw(displayHCount, displayAreaAddress, printChar);
        }
        //clear();  
        
        //printLine( lineNum++, &memory[0x400]);
        //printLine( lineNum++, &memory[0x480]);
        //printLine( lineNum++, &memory[0x500]);
        //printLine( lineNum++, &memory[0x580]);
        //printLine( lineNum++, &memory[0x600]);
        //printLine( lineNum++, &memory[0x680]);
        //printLine( lineNum++, &memory[0x700]);
        //printLine( lineNum++, &memory[0x780]);
        //printLine( lineNum++, &memory[0x428]);
        //printLine( lineNum++, &memory[0x4a8]);
        //printLine( lineNum++, &memory[0x528]);
        //printLine( lineNum++, &memory[0x5a8]);
        //printLine( lineNum++, &memory[0x628]);
        //printLine( lineNum++, &memory[0x6a8]);
        //printLine( lineNum++, &memory[0x728]);
        //printLine( lineNum++, &memory[0x7a8]);
        //printLine( lineNum++, &memory[0x450]);
        //printLine( lineNum++, &memory[0x4d0]);
        //printLine( lineNum++, &memory[0x550]);
        //printLine( lineNum++, &memory[0x5d0]);
        //printLine( lineNum++, &memory[0x650]);
        //printLine( lineNum++, &memory[0x6d0]);
        //printLine( lineNum++, &memory[0x750]);
        //printLine( lineNum++, &memory[0x7d0]);
        
        refresh();
    }    
}


void debugMonitor( t_regist regist, t_statusRegist statusRegist, char *memory ){
    
    int accum = regist.accumulator & 0xff;
    int indeX = regist.indexX & 0xff;
    int indeY = regist.indexY & 0xff;
    int sp = regist.stackPointer & 0xff;
    
    int n = statusRegist.bits.negative & 0x01;
    int z = statusRegist.bits.zero & 0x01;
    int c = statusRegist.bits.carry & 0x01;
    int v = statusRegist.bits.overFlow & 0x01;
    int mem = memory[0x36] & 0xff;
    int mem2 = memory[0x37] & 0xff;
    
    printf("PC :%x\n", regist.programCounter);
    //printf("inst :%d\n", inst.opcode);
    printf("ACUM :%x\n", accum);
    printf("indexX :%x\n", indeX);
    printf("indexY :%x\n", indeY);
    printf("SP :%x\n", sp);
    printf("N :%d\n", n);
    printf("Z :%d\n", z);
    printf("C :%d\n", c);
    printf("V :%d\n", v);
    printf("LOC36 :%d\n", mem);
    printf("LOC37 :%d\n\n", mem2);
}


int main(int argc, const char * argv[]) {
    t_regist regist = { 0 };
    t_statusRegist statusRegist = { 0 };
    t_inst inst = { 0 };
    
    unsigned char memory[MEMORY_SIZE];
    
    int kill = false;
    int reset = false;

    //ncurses
    initscr();
    noecho();
    curs_set(0);
    cbreak();
    timeout(0);

    //emurator initialize
    initMemory(memory);
    initRegist( &regist, memory );
        
    clear();
    while(!kill){
        //fetch
        getInst( &regist, memory, &inst );
                
        //execute
        execute(inst, &regist, &statusRegist, memory);
        
        monitor( memory );
        
        reset = getKeyInput( memory );
        if(reset){
            initRegist( &regist, memory );
        }        
    }
    
    endwin();
    
    return 0;
}
