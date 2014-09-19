#include "Chp8State.h"

#include <iostream>
#include <stdio.h>
#include <SFML/Graphics.hpp>

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

void Chip8State::UnimplementedFunction(std::string name)
{
    std::cout << "Function not implemented " << name;
}

Chip8State::word Chip8State::GetNextOpcode() const
{
    // get the first part of the opcode at address pc and puch it 8 bits
    word address = m_gpMemory[m_programCounter];
    address <<= 8;

    // get the rest of the opcode
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
            // clear screen

            ClearScreen();
            break;
        }

        default:
        {
            UnimplementedOpcode(opcode);
            break;
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
    // vairous set operations
    // 8XYN --> where N is the type of set

    nibble registerX = (opcode & 0x0f00) >> 8;
    nibble registerY = (opcode & 0x00f0) >> 4;

    

    nibble indicatorSwitch = opcode & 0x000f;

    switch (indicatorSwitch)
    {
        case 0x0:
        {
            m_registers[registerX] = m_registers[registerY];
            break;
        }

        case 0x1:
        {
            //  Vx = Vx OR Vy

            m_registers[registerX] |= m_registers[registerY];
            break; 
        }

        case 0x2:
        {
            //  Vx = Vx AND Vy

            m_registers[registerX] &= m_registers[registerY];
            break;
        }

        case 0x3:
        {
            // Vx = Vx XOR Vy
            m_registers[registerX] ^= m_registers[registerY];
            break;
        }

        case 0x4:
        {
            // add Vy to Vx
            // Vf to 1 when carry
            word valueX = m_registers[registerX];
            word valueY = m_registers[registerY];
            

            // TODO
            m_registers[0xf] = valueX > valueY;
            unsigned int total = valueX + valueY;
            word totalWithCarry = valueX + valueY;

            // so if there was a carry
            m_registers[0xf] = total != totalWithCarry;

            m_registers[registerX] = valueX + valueY;

            break;
        }

        case 0x5:
        {
            // substract Vy from Vx
            // Vf to 0 if borrow

            word valueX = m_registers[registerX];
            word valueY = m_registers[registerY];

            m_registers[0xf] = valueX > valueY;

            m_registers[registerX] = valueX - valueY;
            break;
        }

        case 0x6:
        {
            // Shift Vx right by one
            // Set Vf to Vx most significant Bit value

            word currentXValue = m_registers[registerX];

            // The next line will get the value of the least significant bit
            // 0 if it is 0, 1 otherwize
            bool leastSignificantBit = currentXValue & 0x0001;
            m_registers[0xf] = leastSignificantBit;
            m_registers[registerX] >>= 1;
            break;
        }

        case 0x7:
        {
            // Vx = Vy - Vx
            // Vf to 0 if borrow

            word valueX = m_registers[registerX];
            word valueY = m_registers[registerY];

            m_registers[0xf] = valueX > valueY;

            m_registers[registerX] = valueY - valueX;
            break;
        }

        case 0xe:
        {
            // Shift Vx left by one
            // Set Vf to Vx most significant Bit value

            word currentXValue = m_registers[registerX];

            // The next line will get the value of the most significant bit
            // 0 if it is 0, 1 otherwize
            bool mostSignificantBit = (currentXValue & 0x8000) != 0;
            m_registers[0xf] = mostSignificantBit;
            m_registers[registerX] <<= 1;
            break;
        }

        default:
        {
            UnimplementedOpcode(opcode);
            break;
        }
    }
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

    m_registers[registerX] = value & random;
}

void Chip8State::EmulateD(Chip8State::word opcode)
{
    UnimplementedOpcode(opcode);
}

void Chip8State::EmulateE(Chip8State::word opcode)
{
    bool skipNextLine = false;

    nibble registerX = (opcode & 0x0f00) >> 8;
    byte lastByte = opcode & 0x00ff;

    auto key = GetKey(m_registers[registerX]);
    bool keyPressed = sf::Keyboard::isKeyPressed(key);

    if (lastByte == 0x9E)
    {
        // Skip next line if Vx is pressed

        skipNextLine = keyPressed;
    }
    else if (lastByte == 0x0A)
    {
        // Skip next line if Vx is not pressed

        skipNextLine != keyPressed;
    }
    else
    {
        UnimplementedOpcode(opcode);
    }


    if (skipNextLine)
    {
        m_programCounter += 2;
    }
}

void Chip8State::EmulateF(Chip8State::word opcode)
{
    // FXNN -- where NN is the operation

    nibble registerX = (opcode & 0x0f00) >> 8;
    byte operation = opcode & 0x00ff;

    switch (operation)
    {
        case 0x07:
        {
            // set Vx to delay timer value

            m_registers[registerX] = m_delayTimer;
            break;
        }
        case 0x0a:
        {
            // wait for a key, then store in Vx

            UnimplementedOpcode(opcode);
            break;
        }
        case 0x15:
        {
            // set delay timer to Vx value

            m_delayTimer = m_registers[registerX];
            break;
        }
        case 0x18:
        {
            // set sound timer to Vx value

            m_soundTimer = m_registers[registerX];
            break;
        }
        case 0x1e:
        {
            // Add Vx to I

            m_gpMemory[m_addressRegisterI] += m_registers[registerX];
            break;
        }
        case 0x29:
        {
            // set I to location of sprite in Vx

            UnimplementedOpcode(opcode);
            break;
        }
        case 0x33:
        {
            // Lots of things.... see wiki...

            UnimplementedOpcode(opcode);
            break;
        }
        case 0x55:
        {
            // store V0 to Vx in memory starting at address I

            auto lenght = registerX * sizeof(byte);
            std::memcpy(m_gpMemory + m_addressRegisterI, m_registers, lenght);
            break;
        }
        case 0x65:
        {
            // Fills V0 to Vx whit values from memory starting at address I

            auto lenght = registerX * sizeof(byte);
            std::memcpy(m_registers, m_gpMemory + m_addressRegisterI, lenght);
            break;
        }
        default:
        {
            // something went wrong...
            UnimplementedOpcode(opcode);
        }
    }

    UnimplementedOpcode(opcode);
}

void Chip8State::ClearScreen()
{
    UnimplementedFunction("Cls");
}

sf::Keyboard::Key Chip8State::GetKey(byte toConvert) const
{
    /* the chip8 keyboard mapping is like this :
    
    1 2 3 C
    4 5 6 D
    7 8 9 E
    A 0 B F
    
    As a design decision, this will be "reverse"
    mapped to the numpad

    the letters (A to F) will be mapped to the actual
    letters
    */

    // the keyboard have 16 keys 0 to f (included)
    sf::Keyboard::Key keyMapping[16] =
    {
        sf::Keyboard::Numpad0,
        sf::Keyboard::Numpad7,
        sf::Keyboard::Numpad8,
        sf::Keyboard::Numpad9,
        sf::Keyboard::Numpad4,
        sf::Keyboard::Numpad5,
        sf::Keyboard::Numpad6,
        sf::Keyboard::Numpad1,
        sf::Keyboard::Numpad2,
        sf::Keyboard::Numpad3,
        sf::Keyboard::A,
        sf::Keyboard::B,
        sf::Keyboard::C,
        sf::Keyboard::D,
        sf::Keyboard::E,
        sf::Keyboard::F,
    };

    return keyMapping[toConvert];
}