#ifndef CHIP8_STATE__H
#define CHIP8_STATE__H

#include <cstdint>
#include <memory>
#include <random>
#include <vector>

class Chip8State
{
public:
    using nibble = unsigned char;
    using byte = uint8_t;
    using word = uint16_t;

private:
    byte m_registers[16];
    word m_addressRegisterI;
    std::vector<word> m_stackPointer;
    word m_programCounter;
    
    byte m_soundTimer;
    byte m_delayTimer;

    byte m_gpMemory[4 * 1024];
    byte m_screenData[64][32];


    // not "machine" attributs
    std::default_random_engine m_randomEngine;
    std::uniform_int_distribution<int> m_distributor;

public:
    Chip8State();
    
    void SetMemory(byte romBytes[], int size);
    void EmulateOpcode();   

private:
    void UnimplementedOpcode(word opcode);
    word GetNextOpcode() const;

    // y aller avec std::function et vecteur...plus simple
    // opcodes
    void Emulate0(word opcode);
    void Emulate1(word opcode);
    void Emulate2(word opcode);
    void Emulate3(word opcode);
    void Emulate4(word opcode);
    void Emulate5(word opcode);
    void Emulate6(word opcode);
    void Emulate7(word opcode);
    void Emulate8(word opcode);
    void Emulate9(word opcode);
    void EmulateA(word opcode);
    void EmulateB(word opcode);
    void EmulateC(word opcode);
    void EmulateD(word opcode);
    void EmulateE(word opcode);
    void EmulateF(word opcode);


    // helper
    void ClearScreen();
};


#endif