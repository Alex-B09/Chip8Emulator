#include <iostream>
#include <cstdint>
#include <fstream>
#include <memory>
#include <string>
#include <iomanip>


using std::cout;
using std::endl;
using std::unique_ptr;
using std::string;

union Opcode
{
    unsigned char bytes[2];
};

struct Data
{
    unique_ptr<char []> bytes;
    int nbOfElement;

    Data(int lenght)
        :nbOfElement(lenght)
    {
        bytes = std::make_unique<char []>(lenght);
        nbOfElement /= 2;
    }

    Opcode operator[](int pos)
    {
        Opcode opcode;
        opcode.bytes[0] = bytes[pos * 2];
        opcode.bytes[1] = bytes[(pos * 2) + 1];

        return opcode;
    }
};


void Disassemble(Opcode opcode, unsigned int pc)
{
    cout << std::hex << std::setfill('0') << std::setw(4);

    cout << pc 
        << "  "
        << std::setfill('0') 
        << std::setw(2)
        << static_cast<unsigned int>(opcode.bytes[0])
        << " "
        << std::setfill('0')
        << std::setw(2)
        << static_cast<unsigned int>(opcode.bytes[1])
        << "   ";

    unsigned char firstNibble = opcode.bytes[0] >> 4;
    unsigned char secondNibble = opcode.bytes[0] & 0x0f;

    uint16_t leftOver = secondNibble << 8;
    leftOver += opcode.bytes[1];


    switch (firstNibble)
    {
        case 0x00:
        {
            switch (opcode.bytes[1])
            {
                case (0xEE) :
                    cout << "RTS";
                    break;

                case(0xe0) :
                    cout << "CLS";
                    break;

                default:
                    cout << "Not available traduction";
            }
            break;
        }

        case 0x01:
        {
            cout << "JUMP     $" << static_cast<unsigned int>(leftOver);
            break;
        }

        case 0x02:
        {
            cout << "CALL     $" << static_cast<unsigned int>(leftOver);
            break;
        }

        case 0x03:
        {
            cout << "SKIP.EQ  V"
                << static_cast<unsigned int>(secondNibble)
                << ", #$"
                << std::setfill('0')
                << std::setw(2)
                << static_cast<unsigned int>(opcode.bytes[1]);
            break;
        }

        case 0x04:
        {
            cout << "SKIP.NE  V"
                << static_cast<unsigned int>(secondNibble)
                << ", #$"
                << std::setfill('0')
                << std::setw(2)
                << static_cast<unsigned int>(opcode.bytes[1]);
            break;
        }

        case 0x05:
        {
            cout << "SKIP.EQ  V"
                << static_cast<unsigned int>(secondNibble)
                << ", V"
                << static_cast<unsigned int>(opcode.bytes[1] >> 4);
            break;
        }

        case 0x06:
        {
            cout << "MVI      V"
                << static_cast<unsigned int>(secondNibble)
                << ", #$"
                << std::setfill('0')
                << std::setw(2)
                << static_cast<unsigned int>(opcode.bytes[1]);

            break;
        }

        case 0x07:
        {
            cout << "ADD      V"
                << static_cast<unsigned int>(secondNibble)
                << ", #$"
                << std::setfill('0')
                << std::setw(2)
                << static_cast<unsigned int>(opcode.bytes[1]);

            break;
        }

        case 0x08:
        {
            unsigned char lastNibble = opcode.bytes[1] & 0x0f;

            switch (lastNibble)
            {
                case 0x00:
                {
                    cout << "Mov      V"
                        << static_cast<unsigned int>(secondNibble)
                        << ", V"
                        << static_cast<unsigned int>(opcode.bytes[1] >> 4);

                    break;
                }
                
                case 0x01:
                {
                    cout << "OR       V"
                        << static_cast<unsigned int>(secondNibble)
                        << ", V"
                        << static_cast<unsigned int>(opcode.bytes[1] >> 4);
                    break;
                }

                case 0x02:
                {
                    cout << "AND      V"
                        << static_cast<unsigned int>(secondNibble)
                        << ", V"
                        << static_cast<unsigned int>(opcode.bytes[1] >> 4);
                    break;
                }

                case 0x03:
                {
                    cout << "XOR      V"
                        << static_cast<unsigned int>(secondNibble)
                        << ", V"
                        << static_cast<unsigned int>(opcode.bytes[1] >> 4);
                    break;
                }

                case 0x04:
                {
                    cout << "ADD.     V"
                        << static_cast<unsigned int>(secondNibble)
                        << ", V"
                        << static_cast<unsigned int>(opcode.bytes[1] >> 4);
                    break;
                }

                case 0x05:
                {
                    cout << "SUB.     V"
                        << static_cast<unsigned int>(secondNibble)
                        << ", V"
                        << static_cast<unsigned int>(opcode.bytes[1] >> 4);
                    break;
                }

                case 0x06:
                {
                    cout << "SHR.     V"
                        << static_cast<unsigned int>(secondNibble);
                    break;
                }

                case 0x07:
                {
                    cout << "SUBB.    V"
                        << static_cast<unsigned int>(secondNibble)
                        << ", V"
                        << static_cast<unsigned int>(opcode.bytes[1] >> 4);
                    break;
                }

                case 0x0E:
                {
                    cout << "SHL.    V"
                        << static_cast<unsigned int>(secondNibble);
                    break;
                }

                default:
                {
                    cout << "BIG ERROR!!!!!!!!!";
                    break;
                }
            }

            break;
        }

        case 0x09:
        {
            cout << "SKIP.NE    V"
                << static_cast<unsigned int>(secondNibble)
                << ", V"
                << static_cast<unsigned int>(opcode.bytes[1] >> 4);
            break;
        }

        case 0x0a:
        {
            cout << "MVI      VI, #$"
                << std::setfill('0')
                << std::setw(2)
                << static_cast<unsigned int>(leftOver);

            break;
        }

        case 0x0b:
        {
            cout << "JUMP       $" << static_cast<unsigned int>(leftOver)
                << "(V0)";
            break;
        }

        case 0x0c:
        {
            cout << "RAND     V" << static_cast<unsigned int>(secondNibble)
                << ", #$"
                << std::setfill('0')
                << std::setw(2)
                << static_cast<unsigned int>(opcode.bytes[1]);
            break;
        }

        case 0x0d:
        {
            cout << "SPRITE   V" << static_cast<unsigned int>(secondNibble)
                << ", V"
                << static_cast<unsigned int>(opcode.bytes[1] >> 4)
                << ", #$"
                << std::setfill('0')
                << std::setw(1)
                << static_cast<unsigned int>(opcode.bytes[1] & 0x0f);
            break;
        }

        case 0x0e:
        {
            if (opcode.bytes[1] == 0x9e)
            { 
                cout << "SKIP.KEY   V"
                    << static_cast<unsigned int>(secondNibble);
                break;
            }
            else if (opcode.bytes[1] == 0xa1)
            {
                cout << "SKIP.NOKEY V"
                    << static_cast<unsigned int>(secondNibble);
                break;
            }
            else
            {
                cout << "BIG PROBLEM!!!!!!";
            }
            break;
        }

        case 0x0f:
        {
            switch (opcode.bytes[1])
            {
                case 0x07:
                {
                    cout << "MOV     V"
                        << static_cast<unsigned int>(secondNibble)
                        << ", DELAY";
                    break;
                }

                case 0x0a:
                {
                    cout << "WAITKEY V"
                        << static_cast<unsigned int>(secondNibble);
                    break;
                }

                case 0x15:
                {
                    cout << "MOV     DELAY, V"
                        << static_cast<unsigned int>(secondNibble);
                    break;
                }

                case 0x18:
                {
                    cout << "MOV     SOUND, V"
                        << static_cast<unsigned int>(secondNibble);
                    break;
                }

                case 0x1e:
                {
                    cout << "ADD      I, V"
                        << static_cast<unsigned int>(secondNibble);
                    break;
                }

                case 0x29:
                {
                    cout << "SPRCHAR V"
                        << static_cast<unsigned int>(secondNibble);
                    break;
                }

                case 0x33:
                {
                    cout << "MOVBCD V"
                        << static_cast<unsigned int>(secondNibble);
                    break;
                }

                case 0x55:
                {
                    cout << "MOVM  (I), V0 - V"
                        << static_cast<unsigned int>(secondNibble);
                    break;
                }

                case 0x65:
                {
                    cout << "MOVM   V0-V"
                        << static_cast<unsigned int>(secondNibble)
                        << ", (I)";
                    break;
                }

                default:
                {
                    cout << "BIG PROBLEM!!!!!";
                    break;
                }
            }
            break;
        }


        default:
        {
            cout << std::setw(1)
                << static_cast<unsigned int>(firstNibble)
                << " Not available traduction";
            break;
        }
    }
    cout << endl;
}

Data ReadFile(string fileName)
{
    using std::ios;

    std::ifstream file;
    file.open(fileName.c_str(), ios::in | ios::binary | ios::ate);

    // tell me the position of the end (since I started at the end (ate))
    int lenght = static_cast<int>(file.tellg());
    Data data(lenght);
    
    file.seekg(0, ios::beg);
    file.read(data.bytes.get(), lenght);

    file.close();

    return data;
}

int main()
{
    auto data = ReadFile("C:\\Programming\\projects\\Chip8Emulator\\Chip8RomPack\\Chip-8 Programs\\Fishie.ch8");
    cout << data.nbOfElement << endl;

    cout << std::hex << std::setfill('0') << std::setw(4);

    unsigned int pc = 0x200;

    for (int i = 0; i < data.nbOfElement; ++i)
    {
        Disassemble(data[i], pc + (2*i));
    }
    cout << endl;
}

