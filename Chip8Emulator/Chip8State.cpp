#include "Chp8State.h"

#include <iostream>
#include <stdio.h>

Chip8State::Chip8State()
    :m_distributor(0x00, 0xff)
{
    // in memory, the program starts at 0x0200
    m_programCounter = 0x200;
    
    // initialize registers
    for (int cptRegisters = 0; cptRegisters < 16; ++cptRegisters)
    {
        m_registers[cptRegisters] = 0;
    }
    m_addressRegisterI = 0;
}

void Chip8State::SetMemory(byte bytes[], int size)
{
    std::memcpy(m_gpMemory + 0x200, bytes, static_cast<size_t>(size));
}

void Chip8State::EmulateOpcode()
{
    word opcode = GetNextOpcode();
    nibble firstNibble = opcode >> 12;

    switch (firstNibble)
    {
        case 0x00:
        {
            Emulate0(opcode);
            break;
        }

        case 0x01:
        {
            Emulate1(opcode);
            break;
        }

        case 0x02:
        {
            Emulate2(opcode);
            break;
        }


        case 0x03:
        {
            Emulate3(opcode);
            break;
        }

        case 0x04:
        {
            Emulate4(opcode);
            break;
        }

        case 0x05:
        {
            Emulate5(opcode);
            break;
        }

        case 0x06:
        {
            Emulate6(opcode);
            break;
        }

        case 0x07:
        {
            Emulate7(opcode);
            break;
        }

        case 0x08:
        {
            Emulate8(opcode);
            break;
        }

        case 0x09:
        {
            Emulate9(opcode);
            break;
        }

        case 0x0a:
        {
            EmulateA(opcode);
            break;
        }

        case 0x0b:
        {
            EmulateB(opcode);
            break;
        }

        case 0x0c:
        {
            EmulateC(opcode);
            break;
        }

        case 0x0d:
        {
            EmulateD(opcode);
            break;
        }

        case 0x0e:
        {
            EmulateE(opcode);
            break;
        }

        case 0x0f:
        {
            EmulateF(opcode);
            break;
        }
        default:
        {
            UnimplementedOpcode(opcode);
            break;
        }
    }
}

void Chip8State::UnimplementedOpcode(Chip8State::word opcode)
{
    std::cout << "Not implemented " << opcode;
}

Chip8State::word Chip8State::GetNextOpcode() const
{
    word address = m_gpMemory[m_programCounter] << 8;
    address += m_gpMemory[m_programCounter + 1];

    return address;
}

void Chip8State::Emulate0(Chip8State::word opcode)
{
    byte lastByte = opcode & 0x00ff;
    switch (lastByte)
    {
        case 0xee:
        {
            // return subroutine
            m_programCounter = m_stackPointer.back();
            m_stackPointer.pop_back();
            break;
        }
        case 0xe0:
        {
            ClearScreen();
            break;
        }

        default:
        {
            UnimplementedOpcode(opcode);
        }
    }

    UnimplementedOpcode(opcode);
}

void Chip8State::Emulate1(Chip8State::word opcode)
{
    // jump
    // 1NNN

    word address = opcode & 0x0fff;
    m_programCounter = address;
}

void Chip8State::Emulate2(Chip8State::word opcode)
{
    // call subroutine
    // 2NNN

    word address = opcode & 0x0fff;
    m_stackPointer.push_back(m_programCounter);
    m_programCounter = address;
}

void Chip8State::Emulate3(Chip8State::word opcode)
{
    // skip next intruction if Vx equals NN
    // 3XNN

    byte value = opcode & 0x00ff;
    nibble comparedRegister = (opcode & 0x0f00) >> 8;

    if (m_registers[comparedRegister] == value)
    {
        m_programCounter += 2;
    }
}

void Chip8State::Emulate4(Chip8State::word opcode)
{
    // skip next intruction if Vx not equals NN
    // 4XNN

    byte value = opcode & 0x00ff;
    nibble comparedRegister = (opcode & 0x0f00) >> 8;

    if (m_registers[comparedRegister] != value)
    {
        m_programCounter += 2;
    }
}

void Chip8State::Emulate5(Chip8State::word opcode)
{
    // skip next intruction if Vx equals Vy
    // 5XY0

    nibble registerX = (opcode & 0x0f00) >> 8;
    nibble registerY = (opcode & 0x00f0) >> 4;

    if (m_registers[registerX] == m_registers[registerY])
    {
        m_programCounter += 2;
    }
}

void Chip8State::Emulate6(Chip8State::word opcode)
{
    // Set Vx to value NN
    // 6XNN

    byte value = opcode & 0x00ff;
    nibble registerX = (opcode & 0x0f00) >> 8;
    
    m_registers[registerX] = value;
}

void Chip8State::Emulate7(Chip8State::word opcode)
{
    // Add NN to Vx
    // 7XNN

    byte value = opcode & 0x00ff;
    nibble registerX = (opcode & 0x0f00) >> 8;

    m_registers[registerX] += value;
}

void Chip8State::Emulate8(Chip8State::word opcode)
{
    UnimplementedOpcode(opcode);
}

void Chip8State::Emulate9(Chip8State::word opcode)
{
    // skip next if Vx not equal Vy
    // 9XY0

    nibble registerX = (opcode & 0x0f00) >> 8;
    nibble registerY = (opcode & 0x00f0) >> 4;

    if (m_registers[registerX] != m_registers[registerY])
    {
        m_programCounter += 2;
    }
}

void Chip8State::EmulateA(Chip8State::word opcode)
{
    // set I to NNN
    // ANNN

    word address = opcode & 0x0fff;
    m_addressRegisterI = address;
}

void Chip8State::EmulateB(Chip8State::word opcode)
{
    // jump NNN + V0
    // BNNN
    
    word address = (opcode & 0x0fff) + m_registers[0];
    m_programCounter = address;
}

void Chip8State::EmulateC(Chip8State::word opcode)
{
    // set X to random AND NN
    // CXNN

    nibble registerX = (opcode & 0x0f00) >> 8;
    byte value = opcode & 0x00ff;

    byte random = static_cast<byte>(m_distributor(m_randomEngine));
    value = value & random;

    m_registers[registerX] = value;
}

void Chip8State::EmulateD(Chip8State::word opcode)
{
    UnimplementedOpcode(opcode);
}

void Chip8State::EmulateE(Chip8State::word opcode)
{
    UnimplementedOpcode(opcode);
}

void Chip8State::EmulateF(Chip8State::word opcode)
{
    UnimplementedOpcode(opcode);
}

void Chip8State::ClearScreen()
{

}