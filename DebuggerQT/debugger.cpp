#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QTimer>
#include <QLineEdit>
#include <QGroupBox>
#include <QGridLayout>
#include <QDebug>
#include "../main.cpp"
#include "../Bus.cpp"
#include "../Memory.cpp"
#include "../Instruction.cpp"
#include <QLineEdit>
#include <QDebug> 
#include <windows.h>
#include <fstream> 
#include <QString> 

class DebuggerApp : public QWidget
{
    Q_OBJECT // Enable the use of signals and slots in this class

        public : DebuggerApp(QWidget *parent = nullptr)
        : QWidget(parent), memory(0x10000), bus(memory), cpu()
    {
        // Main layout
        QVBoxLayout *mainLayout = new QVBoxLayout(this);

        // --- Control Section ---
        QHBoxLayout *controlLayout = new QHBoxLayout;
        stepButton = new QPushButton("Step", this);
        runToAddressField = new QLineEdit(this);
        runToAddressField->setPlaceholderText("Enter Address (hex)");
        runToAddressButton = new QPushButton("Run to Address", this);
        controlLayout->addWidget(stepButton);
        controlLayout->addWidget(runToAddressField);
        controlLayout->addWidget(runToAddressButton);
        mainLayout->addLayout(controlLayout);

        // --- Registers and Flags Section ---
        QHBoxLayout *registersAndFlagsLayout = new QHBoxLayout;

        // Registers
        registerGroup = new QGroupBox("Registers");
        QGridLayout *registerLayout = new QGridLayout;
        initializeRegisterLabels(registerLayout);
        registerGroup->setLayout(registerLayout);

        // Flags
        flagsGroup = new QGroupBox("Flags");
        QGridLayout *flagsLayout = new QGridLayout;
        initializeFlagLabels(flagsLayout);
        flagsGroup->setLayout(flagsLayout);

        // Add Registers and Flags to the horizontal layout
        registersAndFlagsLayout->addWidget(registerGroup);
        registersAndFlagsLayout->addWidget(flagsGroup);

        mainLayout->addLayout(registersAndFlagsLayout);

        // --- Memory Display Section ---
        memoryDisplay = new QTextEdit(this);
        memoryDisplay->setReadOnly(true);
        mainLayout->addWidget(memoryDisplay);

        // --- Mnemonic Display ---
        Mnemonic = new QLabel("Mnemonic: ", this);
        mainLayout->addWidget(Mnemonic);

        // Connect signals and slots
        connect(stepButton, &QPushButton::clicked, this, &DebuggerApp::step);
        connect(runToAddressButton, &QPushButton::clicked, this, &DebuggerApp::runToAddress);

        // Initialize CPU and load ROM
        bus.loadROM("C:\\Users\\eelip\\Downloads\\48.bin");
        cpu.reset(&bus);

        // Connect cpuStateChanged signal to updateState slot
        connect(this, &DebuggerApp::cpuStateChanged, this, &DebuggerApp::updateState);

        // Set up the UI window
        setWindowTitle("Z80 Debugger");
        setGeometry(100, 100, 600, 400);
    }

signals:
    void cpuStateChanged(); // Signal to notify when the CPU state changes

private slots:
    // Slot triggered by clicking the "Step" button
    void step()
    {
        cpu.execute(bus.read(cpu.PC)); // Execute one step in the CPU
        emit cpuStateChanged();
    }

    

    void runToAddress()
    {
        char userInput = '\0';                                     // Variable to track user input
        std::ofstream outFile("execution_log.txt", std::ios::out); // Open a file for writing

        if (!outFile.is_open())
        {
            qDebug() << "Failed to open file for logging!";
            return; // Exit if the file can't be opened
        }

        // Writing header to the file
        outFile << "Execution Log: PC and Mnemonics\n";
        outFile << "--------------------------------\n";

        do
        {
            // Print the current program counter (PC)
            QString pcString = QString::number(cpu.PC, 16).toUpper();
            qDebug() << "Current PC: 0x" << pcString;

            // Write the current PC to the file
            outFile << "Current PC: 0x" << pcString.toStdString() << "\n";
            std::string mnemonic = getMnemonic();
            // Execute one instruction
            cpu.execute(bus.read(cpu.PC));

            // Get the mnemonic for the current instruction (this could be a method of your CPU class)
            // Assuming getMnemonic() returns a string

            // Write the mnemonic to the file
            outFile << "Mnemonic: " << mnemonic << "\n";

            // Process pending events to keep the application responsive
            QCoreApplication::processEvents();

            // Check if the "s" key is pressed to stop the loop (Windows-specific check)
            if (GetAsyncKeyState('S') & 0x8000) // Check if the "s" key is pressed
            {
                userInput = 's'; // Set userInput to 's' to exit the loop
            }

        } while (userInput != 's'); // Exit the loop when "s" is pressed

        // Log when execution is stopped
        qDebug() << "Execution stopped by user.";
        outFile << "Execution stopped by user.\n"; // Write that execution was stopped

        emit cpuStateChanged(); // Emit the state change signal
    }

    void updateState()
    {
        decodeAndSetMnemonic();
        updateRegisterLabels();
        displayMemoryAroundPC();
    }

    void decodeAndSetMnemonic()
    {
        uint8_t opcode;
        switch (bus.read(cpu.PC))
        {
        case 0xDD:
            if (bus.read(cpu.PC + 1) == 0xCB)
            {
                // Read the opcode and lookup in the DDCB instruction table
                opcode = bus.read(cpu.PC + 2);
                Mnemonic->setText(QString("Mnemonic: %1")
                                      .arg(QString::fromStdString(cpu.instructionTableDDCB[opcode].getMnemonic())));
            }
            else
            {
                // Read the opcode and lookup in the DD instruction table
                opcode = bus.read(cpu.PC + 1);
                auto it = cpu.instructionTableDD.find(opcode);
                if (it != cpu.instructionTableDD.end())
                {
                    Mnemonic->setText(QString("Mnemonic: %1")
                                          .arg(QString::fromStdString(it->second.getMnemonic())));
                }
                else
                {
                    Mnemonic->setText("Unknown Instruction");
                }
            }
            break;
        case 0xFD:
            if (bus.read(cpu.PC + 1) == 0xCB)
            {
                // Read the opcode and lookup in the DDCB instruction table
                opcode = bus.read(cpu.PC + 2);
                Mnemonic->setText(QString("Mnemonic: %1")
                                      .arg(QString::fromStdString(cpu.instructionTableFDCB[opcode].getMnemonic())));
            }
            else
            {
                // Read the opcode and lookup in the DD instruction table
                auto opcode = bus.read(cpu.PC + 1);
                auto it = cpu.instructionTableFD.find(opcode);
                if (it != cpu.instructionTableFD.end())
                {
                    Mnemonic->setText(QString("Mnemonic: %1")
                                          .arg(QString::fromStdString(it->second.getMnemonic())));
                }
                else
                {
                    Mnemonic->setText("Unknown Instruction");
                }
            }
            break;
        case 0xED:
            // Read the opcode and lookup in the DDCB instruction table
            opcode = bus.read(cpu.PC + 1);
            Mnemonic->setText(QString("Mnemonic: %1")
                                  .arg(QString::fromStdString(cpu.instructionTableED[opcode].getMnemonic())));
            break;
        case 0xCB:
            opcode = bus.read(cpu.PC + 1);
            Mnemonic->setText(QString("Mnemonic: %1")
                                  .arg(QString::fromStdString(cpu.instructionTableCB[opcode].getMnemonic())));
            break;
        default:
            opcode = bus.read(cpu.PC);
            auto it = cpu.instructionTable.find(opcode);
            if (it != cpu.instructionTable.end())
            {
                Mnemonic->setText(QString("Mnemonic: %1")
                                      .arg(QString::fromStdString(it->second.getMnemonic())));
            }
            break;
        }
    }
    std::string getMnemonic()
    {
        uint8_t opcode;
        switch (bus.read(cpu.PC))
        {
        case 0xDD:
            if (bus.read(cpu.PC + 1) == 0xCB)
            {
                // Read the opcode and lookup in the DDCB instruction table
                opcode = bus.read(cpu.PC + 2);
                return cpu.instructionTableDDCB[opcode].getMnemonic();
            }
            else
            {
                // Read the opcode and lookup in the DD instruction table
                opcode = bus.read(cpu.PC + 1);
                auto it = cpu.instructionTableDD.find(opcode);
                if (it != cpu.instructionTableDD.end())
                {
                    return cpu.instructionTableDD[opcode].getMnemonic();
                }
                else
                {
                    return "Empty";
                }
            }
            break;
        case 0xFD:
            if (bus.read(cpu.PC + 1) == 0xCB)
            {
                // Read the opcode and lookup in the DDCB instruction table
                opcode = bus.read(cpu.PC + 2);
                return cpu.instructionTableFDCB[opcode].getMnemonic();
            }
            else
            {
                // Read the opcode and lookup in the DD instruction table
                auto opcode = bus.read(cpu.PC + 1);
                auto it = cpu.instructionTableFD.find(opcode);
                if (it != cpu.instructionTableFD.end())
                {
                    return cpu.instructionTableFD[opcode].getMnemonic();
                }
                else
                {
                    return "empty";
                }
            }
            break;
        case 0xED:
            // Read the opcode and lookup in the DDCB instruction table
            opcode = bus.read(cpu.PC + 1);
            return cpu.instructionTableED[opcode].getMnemonic();
            break;
        case 0xCB:
            opcode = bus.read(cpu.PC + 1);
            return cpu.instructionTableCB[opcode].getMnemonic();
            break;
        default:
            opcode = bus.read(cpu.PC);
            auto it = cpu.instructionTable.find(opcode);
            if (it != cpu.instructionTable.end())
            {
                return "Empty";
            }
            break;
        }
    }

private:
    void initializeRegisterLabels(QGridLayout *layout)
    {
        pcLabel = new QLabel("PC: 0x0000");
        aRegLabel = new QLabel("A: 0x00");
        bRegLabel = new QLabel("B: 0x00");
        cRegLabel = new QLabel("C: 0x00");
        dRegLabel = new QLabel("D: 0x00");
        eRegLabel = new QLabel("E: 0x00");
        hRegLabel = new QLabel("H: 0x00");
        lRegLabel = new QLabel("L: 0x00");
        BCRegLabel = new QLabel("BC: 0x0000");
        DERegLabel = new QLabel("DE: 0x0000");
        HLRegLabel = new QLabel("HL: 0x0000");
        AFRegLabel = new QLabel("AF: 0x0000");

        layout->addWidget(pcLabel, 0, 0, 1, 2);
        layout->addWidget(aRegLabel, 1, 0);
        layout->addWidget(bRegLabel, 1, 1);
        layout->addWidget(cRegLabel, 2, 0);
        layout->addWidget(dRegLabel, 2, 1);
        layout->addWidget(eRegLabel, 3, 0);
        layout->addWidget(hRegLabel, 3, 1);
        layout->addWidget(lRegLabel, 4, 0);
        layout->addWidget(BCRegLabel, 4, 1);
        layout->addWidget(DERegLabel, 5, 0);
        layout->addWidget(HLRegLabel, 5, 1);
        layout->addWidget(AFRegLabel, 6, 0, 1, 2);
    }

    void initializeFlagLabels(QGridLayout *layout)
    {
        C = new QLabel("C: 0");
        N = new QLabel("N: 0");
        P = new QLabel("P: 0");
        H = new QLabel("H: 0");
        S = new QLabel("S: 0");
        Z = new QLabel("Z: 0");

        layout->addWidget(C, 0, 0);
        layout->addWidget(N, 0, 1);
        layout->addWidget(P, 1, 0);
        layout->addWidget(H, 1, 1);
        layout->addWidget(S, 2, 0);
        layout->addWidget(Z, 2, 1);
    }

    void displayMemoryAroundPC()
    {
        memoryDisplay->clear();

        for (int i = 0x4000; i <= 0x57FF; i++)
        {

            memoryDisplay->append(QString("0x%1: 0x%2")
                                      .arg(i, 4, 16, QChar('0'))
                                      .arg(bus.read(i), 2, 16, QChar('0')));
        }
    }

    void updateRegisterLabels()
    {
        pcLabel->setText(QString("PC: 0x%1").arg(cpu.PC, 4, 16, QChar('0')));
        aRegLabel->setText(QString("A: 0x%1").arg(cpu.A, 2, 16, QChar('0')));
        bRegLabel->setText(QString("B: 0x%1").arg(cpu.B, 2, 16, QChar('0')));
        cRegLabel->setText(QString("C: 0x%1").arg(cpu.C, 2, 16, QChar('0')));
        dRegLabel->setText(QString("D: 0x%1").arg(cpu.D, 2, 16, QChar('0')));
        eRegLabel->setText(QString("E: 0x%1").arg(cpu.E, 2, 16, QChar('0')));
        hRegLabel->setText(QString("H: 0x%1").arg(cpu.H, 2, 16, QChar('0')));
        lRegLabel->setText(QString("L: 0x%1").arg(cpu.L, 2, 16, QChar('0')));
        BCRegLabel->setText(QString("BC: 0x%1").arg(cpu.getBC(), 2, 16, QChar('0')));
        DERegLabel->setText(QString("DE: 0x%1").arg(cpu.getDE(), 2, 16, QChar('0')));
        HLRegLabel->setText(QString("HL: 0x%1").arg(cpu.getHL(), 2, 16, QChar('0')));
        AFRegLabel->setText(QString("Af: 0x%1").arg(cpu.getAF(), 2, 16, QChar('0')));
        C->setText(QString("C: %1").arg(cpu.isFlagSet(z80::C_flag), 2, 16));
        N->setText(QString("N: %1").arg(cpu.isFlagSet(z80::N), 2, 16));
        P->setText(QString("P: %1").arg(cpu.isFlagSet(z80::P), 2, 16));
        H->setText(QString("H: %1").arg(cpu.isFlagSet(z80::H_flag), 2, 16));
        S->setText(QString("S: %1").arg(cpu.isFlagSet(z80::S), 2, 16));
        Z->setText(QString("Z: %1").arg(cpu.isFlagSet(z80::Z), 2, 16));
    }

    QGroupBox *registerGroup;
    QGroupBox *flagsGroup;
    QTextEdit *memoryDisplay;
    QLabel *Mnemonic;
    QPushButton *stepButton;
    QPushButton *runToAddressButton;
    QLineEdit *runToAddressField;
    QLabel *pcLabel, *aRegLabel, *bRegLabel, *cRegLabel, *dRegLabel, *eRegLabel;
    QLabel *hRegLabel, *lRegLabel, *BCRegLabel, *DERegLabel, *HLRegLabel, *AFRegLabel;
    QLabel *C, *N, *P, *H, *S, *Z;
    z80 cpu;
    Memory memory;
    Bus bus;
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    DebuggerApp window;
    window.show();

    return app.exec();
}
#include "debugger.moc"
