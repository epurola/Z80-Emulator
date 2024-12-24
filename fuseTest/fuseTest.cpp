#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <utility>
#include <cstdint>
#include <stdexcept>
#include <iomanip>
#include "../main.cpp"
#include "../Bus.cpp"
#include "../Memory.cpp"
#include "../Instruction.cpp"

struct Test
{
    std::string description;
    uint16_t AF;
    uint16_t BC;
    uint16_t DE;
    uint16_t HL;
    uint16_t AF1;
    uint16_t BC1;
    uint16_t DE1;
    uint16_t HL1;
    uint16_t IX;
    uint16_t IY;
    uint16_t SP;
    uint16_t PC;
    uint16_t MEMPTR;
    uint16_t I, R, IFF1, IFF2, IM;                                      // CPU flags
    bool halted;                                                        // Halted flag
    uint32_t tstates;                                                   // Number of tstates
    std::vector<std::pair<uint16_t, std::vector<uint8_t>>> memorySetup; // Memory writes
};
struct Expected
{
    std::string description;
    uint16_t AF;
    uint16_t BC;
    uint16_t DE;
    uint16_t HL;
    uint16_t AF1;
    uint16_t BC1;
    uint16_t DE1;
    uint16_t HL1;
    uint16_t IX;
    uint16_t IY;
    uint16_t SP;
    uint16_t PC;
    uint16_t MEMPTR;
    uint16_t I;
    uint16_t R;
    uint16_t IFF1;
    uint16_t IFF2;
    uint16_t IM;                                                        // CPU flags
    bool halted;                                                        // Halted flag
    uint32_t tstates;                                                   // Number of tstates
    std::vector<std::pair<uint16_t, std::vector<uint8_t>>> memorySetup; // Memory writes
};

std::vector<Test> parseTestsIn(const std::string &filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        throw std::runtime_error("Could not open file: " + filename);
    }

    std::vector<Test> tests;
    Test currentTest = {};
    std::string line;

    int lineType = 1;

    while (std::getline(file, line))
    {
        // Skip blank lines
        if (line.empty())
        {
            lineType = 1;
            continue;
        }

        int spaceCount = 0;

        // Count the number of spaces
        for (char ch : line)
        {
            if (ch == ' ')
            {
                spaceCount++;
            }
        }

        // Start of a new test (description line)
        if (lineType == 1)
        {
            if (!currentTest.description.empty())
            {
                // Push the current test to the list before starting a new one
                tests.push_back(currentTest);
                currentTest = Test();
            }
            currentTest.description = line;
            lineType++;
        }

        // Parse CPU state
        else if (lineType == 2)
        {
            std::istringstream iss(line);
            iss >> std::hex >> currentTest.AF >> currentTest.BC >> currentTest.DE >> currentTest.HL >> currentTest.AF1 >> currentTest.BC1 >> currentTest.DE1 >> currentTest.HL1 >> currentTest.IX >> currentTest.IY >> currentTest.SP >> currentTest.PC >> currentTest.MEMPTR;
            lineType++;
        }

        else if (lineType == 3)
        {

            std::istringstream iss(line);

            iss >> std::hex >> currentTest.I >> currentTest.R >> currentTest.IFF1 >> currentTest.IFF2 >> currentTest.IM >> currentTest.halted >> currentTest.tstates;
            lineType++;
        }
        // Parse memory setup
        else if (lineType == 4)
        {

            std::istringstream iss(line);
            uint16_t address;
            iss >> std::hex >> address;

            std::vector<uint8_t> bytes;
            uint16_t byte;
            while (iss >> std::hex >> byte)
            {
                bytes.push_back(static_cast<uint8_t>(byte));
            }
            if (!bytes.empty() && bytes.back() == 0xFF)
            {
                // Remove the last byte if it's 0xFF
                bytes.pop_back();
            }

            currentTest.memorySetup.emplace_back(address, bytes);
            lineType++;
        }
        else if (lineType == 5)
        {

            if (line == "-1")
            {
                continue;
            }

            std::istringstream iss(line);
            uint16_t address;
            iss >> std::hex >> address;

            std::vector<uint8_t> bytes;
            uint16_t byte;
            while (iss >> std::hex >> byte)
            {
                bytes.push_back(static_cast<uint8_t>(byte));
            }
            if (!bytes.empty() && bytes.back() == 0xFF)
            {
                // Remove the last byte if it's 0xFF
                bytes.pop_back();
            }

            currentTest.memorySetup.emplace_back(address, bytes);
        }

        if (currentTest.description == "c4_2")
        {
            std::cout << "SpaceCount" << spaceCount << std::endl;
        }
    }

    // Push the last test
    if (!currentTest.description.empty())
    {
        tests.push_back(currentTest);
    }

    file.close();
    return tests;
}

std::vector<Expected> parseExpected(const std::string &filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        throw std::runtime_error("Could not open file: " + filename);
    }

    std::vector<Expected> expected;
    Expected currentTestExpected = {};
    std::string line;

    while (std::getline(file, line))
    {
        // Skip blank lines
        if (line.empty())
            continue;

        int spaceCount = 0;

        // Count the number of spaces
        for (char ch : line)
        {
            if (ch == ' ')
            {
                spaceCount++;
            }
        }

        if (line.find("MC") != std::string::npos ||
            line.find("MR") != std::string::npos ||
            line.find("MW") != std::string::npos ||
            line.find("PR") != std::string::npos ||
            line.find("PW") != std::string::npos ||
            line.find("PC") != std::string::npos)
            continue;

        if (line.find(" ") == std::string::npos && line != "-1")
        {
            if (!currentTestExpected.description.empty())
            {

                expected.push_back(currentTestExpected);
                currentTestExpected = Expected();
            }
            currentTestExpected.description = line;
        }
      
        else if (spaceCount >= 12 && spaceCount != 17)
        {
            std::istringstream iss(line);
            iss >> std::hex >> currentTestExpected.AF >> currentTestExpected.BC >> currentTestExpected.DE >> currentTestExpected.HL >> currentTestExpected.AF1 >> currentTestExpected.BC1 >> currentTestExpected.DE1 >> currentTestExpected.HL1 >> currentTestExpected.IX >> currentTestExpected.IY >> currentTestExpected.SP >> currentTestExpected.PC >> currentTestExpected.MEMPTR;
        }
     
        else if (spaceCount == 10 || spaceCount == 6 || spaceCount == 8 || spaceCount == 9 && currentTestExpected.tstates == 0)
        {
            std::istringstream iss(line);
            uint8_t halted;

            iss >> std::hex >> currentTestExpected.I >> currentTestExpected.R >> currentTestExpected.IFF1 >> currentTestExpected.IFF2 >> currentTestExpected.IM >> halted >> currentTestExpected.tstates;
        }
      
        else if (currentTestExpected.memorySetup.empty())
        {

            std::istringstream iss(line);
            uint16_t address;
            iss >> std::hex >> address;

            std::vector<uint8_t> bytes;
            uint16_t byte;
            while (iss >> std::hex >> byte)
            {
                bytes.push_back(static_cast<uint8_t>(byte));
            }
            if (!bytes.empty() && bytes.back() == 0xFF)
            {
                // Remove the last byte if it's 0xFF
                bytes.pop_back();
            }

            currentTestExpected.memorySetup.emplace_back(address, bytes);
        }
        else if (!currentTestExpected.memorySetup.empty())
        {

            if (line == "-1")
            {
                continue;
            }

            std::istringstream iss(line);
            uint16_t address;
            iss >> std::hex >> address;

            std::vector<uint8_t> bytes;
            uint16_t byte;
            while (iss >> std::hex >> byte)
            {
                bytes.push_back(static_cast<uint8_t>(byte));
            }
            if (!bytes.empty() && bytes.back() == 0xFF)
            {
              
                bytes.pop_back();
            }

            currentTestExpected.memorySetup.emplace_back(address, bytes);
        }
        if (currentTestExpected.description == "c4_2")
        {
            std::cout << "SpaceCountE" << spaceCount << std::endl;
        }
    }

    // Push the last test
    if (!currentTestExpected.description.empty())
    {
        expected.push_back(currentTestExpected);
    }

    file.close();
    return expected;
}

int main()
{
    z80 cpu;
    Memory memory(0x10000);
    Bus bus(memory);
    cpu.reset(&bus);
    int passed = 0;
    int failed = 0;
    int error = 0;
    std::string filePath = "C:\\Users\\eelip\\Downloads\\tests.expected";
    std::string filePath1 = "C:\\Users\\eelip\\Downloads\\tests.in";
    std::vector<Expected> expected = parseExpected(filePath);
    std::vector<Test> tests = parseTestsIn(filePath1);

    try
    {

        for (size_t i = 0; i < tests.size(); ++i)
        {

            if (tests[i].description == "ed48" || tests[i].description == "ed50" || tests[i].description == "ed40"

                || tests[i].description == "ed58" || tests[i].description == "ed60" || tests[i].description == "ed68" || tests[i].description == "ed70" || tests[i].description == "ed78" || tests[i].description == "edb2" || tests[i].description == "edb2_1" || tests[i].description == "edb3" || tests[i].description == "edb3_1" || tests[i].description == "eda2" || tests[i].description == "edaa" || tests[i].description == "edab" || tests[i].description == "edba" || tests[i].description == "edbb" || tests[i].description == "edba_1" || tests[i].description == "edbb_1" || tests[i].description == "edaa_01" || tests[i].description == "edaa_02" || tests[i].description == "edaa_03" || tests[i].description == "edab_01" || tests[i].description == "edab_02" || tests[i].description == "edab_03" || tests[i].description == "eda2_01" || tests[i].description == "eda2_02" || tests[i].description == "eda2_03" || tests[i].description == "eda3_01" || tests[i].description == "eda3_02" || tests[i].description == "eda3_03" || tests[i].description == "eda3_04" || tests[i].description == "eda3_05" || tests[i].description == "eda3_06" || tests[i].description == "eda3_07" || tests[i].description == "eda3_08" || tests[i].description == "eda3_09" || tests[i].description == "eda3_10" || tests[i].description == "eda3_11" || tests[i].description == "eda3")
            {
                continue;
            }

            const Test &test = tests[i];
            const Expected &exp = expected[i]; 

            std::cout << "Test Description: " << test.description
                      << " Expected Description: " << exp.description << "\n";

            cpu.A = (test.AF & 0xFF00) >> 8;
            cpu.F = (test.AF & 0x00FF);
            cpu.B = (test.BC & 0xFF00) >> 8;
            cpu.C = (test.BC & 0x00FF);
            cpu.D = (test.DE & 0xFF00) >> 8;
            cpu.E = (test.DE & 0x00FF);
            cpu.H = (test.HL & 0xFF00) >> 8;
            cpu.L = (test.HL & 0x00FF);
            cpu.IX = test.IX;
            cpu.IY = test.IY;
            cpu.SP = test.SP;
            cpu.PC = test.PC;
            cpu.setAF1(test.AF1);
            cpu.setBC1(test.BC1);
            cpu.setDE1(test.DE1);
            cpu.setHL1(test.HL1);
            cpu.setAF(test.AF);

            test.halted == 1 ? cpu.halted = true : cpu.halted = false;

            cpu.MPTR = test.MEMPTR;
            cpu.IFF2 = test.IFF2;
            cpu.I = (uint8_t)test.I;
            cpu.R = (uint8_t)test.R;

            if (!test.memorySetup.empty())
            {
                for (const auto &[startAddress, bytes] : test.memorySetup)
                {
                    uint16_t currentAddress = startAddress; // Start at the given address

                    // Loop over each byte in the vector and write it to the bus
                    for (uint8_t byte : bytes)
                    {
                        bus.write(currentAddress, byte); // Write each byte to the bus
                        currentAddress++;                // Increment the address for the next byte
                    }
                }
            }

            uint16_t runToAddress = exp.PC;

            bool memoryOk = true;

            do
            {
                cpu.execute(bus.read(cpu.PC));
            } while (cpu.PC < runToAddress);

            if (exp.memorySetup.empty())
            {
                memoryOk = true;
            }

            if (!exp.memorySetup.empty())
            {
                for (const auto &[startAddress, bytes] : exp.memorySetup)
                {
                    uint16_t currentAddress = startAddress;

                    for (uint8_t byte : bytes)
                    {
                        uint8_t value = bus.read(currentAddress);
                        if ((int)value != (int)byte)
                        {
                            memoryOk = false;
                            std::cout << "Mismatch in MEMORY: Expected " << byte << ", Got " << (int)value << std::endl;
                            break;
                        }

                        currentAddress++;
                    }
                }
            }

            // Now compare with the expected values
            if ((cpu.A == (exp.AF & 0xFF00) >> 8) &&
                (cpu.F == (exp.AF & 0x00FF)) &&
                (cpu.B == (exp.BC & 0xFF00) >> 8) &&
                (cpu.C == (exp.BC & 0x00FF)) &&
                (cpu.D == (exp.DE & 0xFF00) >> 8) &&
                (cpu.E == (exp.DE & 0x00FF)) &&
                (cpu.H == (exp.HL & 0xFF00) >> 8) &&
                (cpu.L == (exp.HL & 0x00FF)) &&
                (cpu.IX == exp.IX) &&
                (cpu.IY == exp.IY) &&
                (cpu.SP == exp.SP) &&
                (cpu.PC == exp.PC) &&
                (cpu.I == (uint8_t)exp.I) &&
                (cpu.R == (uint8_t)exp.R) &&
                cpu.getAF1() == exp.AF1 &&
                (cpu.getBC1() == exp.BC1) &&
                (cpu.getDE1() == exp.DE1) &&
                (cpu.getHL1() == exp.HL1) &&
                cpu.getAF() == exp.AF &&
                cpu.getBC() == exp.BC &&
                cpu.getDE() == exp.DE &&
                cpu.getHL() == exp.HL &&

                memoryOk)
            // cpu.MPTR == exp.MEMPTR &&
            {
                passed++;
                std::cout << "Test passed! " << static_cast<int>(passed) << std::endl;
            }
            else
            {
                failed++;
                std::cout << "Test failed!" << std::endl;
                std::cout << "Name: " << test.description << std::endl;

#include <iomanip>

                if (cpu.A != (exp.AF >> 8))
                    std::cout << "Mismatch in A: Expected " << std::hex << (exp.AF >> 8) << ", Got " << std::hex << (int)cpu.A << std::endl;
                if (cpu.F != (exp.AF & 0xFF))
                    std::cout << "Mismatch in F: Expected " << std::hex << (exp.AF & 0xFF) << ", Got " << std::hex << (int)cpu.F << std::endl;
                if (cpu.B != (exp.BC >> 8))
                    std::cout << "Mismatch in B: Expected " << std::hex << (exp.BC >> 8) << ", Got " << std::hex << (int)cpu.B << std::endl;
                if (cpu.C != (exp.BC & 0xFF))
                    std::cout << "Mismatch in C: Expected " << std::hex << (exp.BC & 0xFF) << ", Got " << std::hex << (int)cpu.C << std::endl;
                if (cpu.D != (exp.DE >> 8))
                    std::cout << "Mismatch in D: Expected " << std::hex << (exp.DE >> 8) << ", Got " << std::hex << (int)cpu.D << std::endl;
                if (cpu.E != (exp.DE & 0xFF))
                    std::cout << "Mismatch in E: Expected " << std::hex << (exp.DE & 0xFF) << ", Got " << std::hex << (int)cpu.E << std::endl;
                if (cpu.H != (exp.HL >> 8))
                    std::cout << "Mismatch in H: Expected " << std::hex << (exp.HL >> 8) << ", Got " << std::hex << (int)cpu.H << std::endl;
                if (cpu.L != (exp.HL & 0xFF))
                    std::cout << "Mismatch in L: Expected " << std::hex << (exp.HL & 0xFF) << ", Got " << std::hex << (int)cpu.L << std::endl;
                if (cpu.IX != exp.IX)
                    std::cout << "Mismatch in IX: Expected " << std::hex << exp.IX << ", Got " << std::hex << cpu.IX << std::endl;
                if (cpu.IY != exp.IY)
                    std::cout << "Mismatch in IY: Expected " << std::hex << exp.IY << ", Got " << std::hex << cpu.IY << std::endl;
                if (cpu.SP != exp.SP)
                    std::cout << "Mismatch in SP: Expected " << std::hex << exp.SP << ", Got " << std::hex << cpu.SP << std::endl;
                if (cpu.PC != exp.PC)
                    std::cout << "Mismatch in PC: Expected " << std::hex << exp.PC << ", Got " << std::hex << cpu.PC << std::endl;
                if (cpu.I != exp.I)
                    std::cout << "Mismatch in I: Expected " << std::hex << exp.I << ", Got " << std::hex << (int)cpu.I << std::endl;
                if (cpu.R != exp.R)
                    std::cout << "Mismatch in R: Expected " << std::hex << exp.R << ", Got " << std::hex << (int)cpu.R << std::endl;
                // if (cpu.MPTR != exp.MEMPTR)
                //  std::cout << "Mismatch in MPTR: Expected " << std::hex << exp.R << ", Got " << std::hex << (int)cpu.MPTR << std::endl;

                std::cout << "Memory setup during the test:" << std::endl;
                for (const auto &[startAddress, bytes] : test.memorySetup)
                {
                    std::cout << "Start Address: " << std::hex << startAddress << " Data: ";
                    for (uint8_t byte : bytes)
                    {
                        std::cout << std::hex << (int)byte << " ";
                    }
                    std::cout << std::endl;
                }
                std::cout << "Memory setup during the eval:" << std::endl;
                for (const auto &[startAddress, bytes] : exp.memorySetup)
                {
                    std::cout << "Start Address: " << std::hex << startAddress << " Data: ";
                    for (uint8_t byte : bytes)
                    {
                        std::cout << std::hex << (int)byte << " ";
                    }
                    std::cout << std::endl;
                }

                break;
            }
        }
    }
    catch (const std::exception &ex)
    {
        std::cerr << "Error: " << ex.what() << "\n";
        error++;
    }

    system("pause");

    return 0;
}
