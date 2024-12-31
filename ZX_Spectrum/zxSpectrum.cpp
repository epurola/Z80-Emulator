#include <QApplication>
#include <QWidget>
#include <QPainter>
#include <QTimer>
#include "../main.cpp"
#include "../Bus.cpp"
#include "../Memory.cpp"
#include "../Instruction.cpp"
#include <windows.h>
#include <QKeyEvent>

const int SCREEN_WIDTH = 256;
const int SCREEN_HEIGHT = 192;
const int BORDER_WIDTH = 48;            
const int BORDER_HEIGHT = 56;                           
const int TOTAL_WIDTH = SCREEN_WIDTH + 2 * BORDER_WIDTH;   
const int TOTAL_HEIGHT = SCREEN_HEIGHT + 2 * BORDER_HEIGHT; 

QRgb borderColor = qRgb(0, 0, 0);

bool screenReady = false;
QImage _screenBuffer;
QImage _fullBuffer;
int line = 0;
int startAddress = 0x4000;
int userInput;
int pixelLine = 0;
int screenLine = 0;
int lineRendered = 0;
bool uninitialized = true;
int frameCounter = 0;
int foreground;
int background;
int brightness;

class ZXScreen : public QWidget
{
    Q_OBJECT

public:
    ZXScreen(Bus &bus, z80 &cpu, QWidget *parent = nullptr) : QWidget(parent), bus(bus), cpu(cpu)
    {
        setFixedSize(2 * TOTAL_WIDTH, 2 * TOTAL_HEIGHT);
        _fullBuffer = QImage(TOTAL_WIDTH, TOTAL_HEIGHT, QImage::Format_RGB888);
        _fullBuffer.fill(borderColor);
        _screenBuffer = QImage(SCREEN_WIDTH, SCREEN_HEIGHT, QImage::Format_RGB888);
        _screenBuffer.fill(Qt::black);
        _keyLine.fill(0xFF);
    }

    std::array<uint8_t, 8> UpdateInput()
    {
        return _keyLine;
    }

    void updateScreenBuffer()
    {

        uint16_t address = startAddress;

        for (int character = 0x4000; character <= 0x57FF; ++character)
        {
            uint8_t byte = bus.read(character);
            if (byte != 0)
            {
                uninitialized = false;
            }

            for (int bitPos = 0; bitPos < 8; ++bitPos)
            {
                bool bit = 0x1 & (byte >> (7 - bitPos));
                int x = (address & 0x1F) * 8 + bitPos;

                int blockRow = (character >> 11) & 0x3;  // Which block of rows (0-2)
                int rowInBlock = (character >> 5) & 0x7; // Which row within the block (0-7)
                int pixelLine = (character >> 8) & 0x7;  // Vertical offset within the row
                int y = blockRow * 64 + rowInBlock * 8 + pixelLine;
                int attributeAddress = 0x5800 + ((y / 8) * 32 + (x / 8));
                uint8_t color = bus.read(attributeAddress);
                foreground = color & 0b00000111;
                background = (color & 0b00111000) >> 3;
                brightness = (color & 0b01000000) >> 6;
                int flash = (color & 0b10000000) >> 7;
                _screenBuffer.setPixel(x, y, getColor(bit, flash, foreground, background, brightness, frameCounter));
            }
            address++;
        }

        getBorderColor();
        QPainter painter(&_fullBuffer);
        painter.drawImage(BORDER_WIDTH, BORDER_HEIGHT, _screenBuffer);
        painter.end();

        screenReady = true;
        frameCounter++;
        if (frameCounter == 32)
        {
            frameCounter = 0;
        }
        repaint();
        bus.interrupt = true;
    }

    QRgb getColor(bool value, bool flash, int foreground, int background, int brightness, int &frameCounter)
    {

        if (flash)
        {
            if (frameCounter < 16) // First half of the 32 frames, use one color
            {
                return value ? color(background, brightness) : color(foreground, brightness);
            }
            else // Second half of the 32 frames, use the other color
            {
                return value ? color(foreground, brightness) : color(background, brightness);
            }
        }
        else
        {
            // If flash is not enabled, just use the value to determine the color
            return value ? color(foreground, brightness) : color(background, brightness);
        }
    }

    QRgb color(int color, int brightness)
    {
        if (brightness)
        {
            switch (color)
            {
            case 0:
                return qRgb(0, 0, 0); // Black
                break;
            case 1:
                return qRgb(0, 0, 128); // Dark Blue
                break;
            case 2:
                return qRgb(128, 0, 0); // Dark Red
                break;
            case 3:
                return qRgb(128, 0, 128); // Dark Magenta
                break;
            case 4:
                return qRgb(0, 128, 0); // Dark Green
                break;
            case 5:
                return qRgb(0, 128, 128); // Dark Cyan
                break;
            case 6:
                return qRgb(128, 128, 0); // Dark Yellow
                break;
            default:
                return qRgb(96, 96, 96); // Dark Grey
                break;
            }
        }
        else
        {
            switch (color)
            {
            case 0:
                return qRgb(0, 0, 0); // Black
                break;
            case 1:
                return qRgb(0, 0, 255); // Light Gray
                break;
            case 2:
                return qRgb(255, 0, 0); // Green
                break;
            case 3:
                return qRgb(255, 0, 255);
                break;
            case 4:
                return qRgb(0, 255, 0); // Blue
                break;
            case 5:
                return qRgb(0, 255, 255); // Yellow
                break;
            case 6:
                return qRgb(255, 255, 0); // Cyan
                break;
            default:
                return qRgb(207, 207, 207);
                break;
            }
        }
    }

    void getBorderColor()
    {
        int value = bus.readBorder(0x00FE); 

        value = value & 0b00000111; 

        QRgb newBorderColor;

        switch (value)
        {
        case 0:
            newBorderColor = qRgb(0, 0, 0); // Black
            break;
        case 1:
            newBorderColor = qRgb(0, 0, 255); // Blue
            break;
        case 2:
            newBorderColor = qRgb(255, 0, 0); // Red
            break;
        case 3:
            newBorderColor = qRgb(255, 0, 255); // magenta
            break;
        case 4:
            newBorderColor = qRgb(0, 255, 0); // Green
            break;
        case 5:
            newBorderColor = qRgb(0, 255, 255); // Yellow
            break;
        case 6:
            newBorderColor = qRgb(255, 255, 0); // Yellow
            break;
        default:
            newBorderColor = qRgb(207, 207, 207); 
            break;
        }

        if (newBorderColor != borderColor)
        {
            borderColor = newBorderColor;


            QPainter painter(&_fullBuffer);
            painter.fillRect(0, 0, TOTAL_WIDTH, BORDER_HEIGHT, borderColor);                                                          // Top border
            painter.fillRect(0, TOTAL_HEIGHT - BORDER_HEIGHT, TOTAL_WIDTH, BORDER_HEIGHT, borderColor);                               // Bottom border
            painter.fillRect(0, BORDER_HEIGHT, BORDER_WIDTH, TOTAL_HEIGHT - 2 * BORDER_HEIGHT, borderColor);                          // Left border
            painter.fillRect(TOTAL_WIDTH - BORDER_WIDTH, BORDER_HEIGHT, BORDER_WIDTH, TOTAL_HEIGHT - 2 * BORDER_HEIGHT, borderColor); // Right border
            painter.end();

          
            repaint();
        }
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        QPainter painter(this);

        // Get the widget's current size
        int widgetWidth = width();
        int widgetHeight = height();

      
        double aspectRatio = static_cast<double>(TOTAL_WIDTH) / TOTAL_HEIGHT;
        int scaledWidth = widgetWidth;
        int scaledHeight = static_cast<int>(widgetWidth / aspectRatio);

        if (scaledHeight > widgetHeight)
        {
            scaledHeight = widgetHeight;
            scaledWidth = static_cast<int>(widgetHeight * aspectRatio);
        }

        // Center the scaled image in the widget
        int xOffset = (widgetWidth - scaledWidth) / 4;
        int yOffset = (widgetHeight - scaledHeight) / 4;

        painter.drawImage(QRect(xOffset, yOffset, scaledWidth, scaledHeight), _fullBuffer);
    }
    void keyPressEvent(QKeyEvent *event) override
    {
        updateKeyLine(event, true);
        for (int i = 0; i < 8; ++i)
        {
            bus.KeyMatrix[i] = _keyLine[i];
        }
    }

    void keyReleaseEvent(QKeyEvent *event) override
    {
        updateKeyLine(event, false);
        for (int i = 0; i < 8; ++i)
        {
            bus.KeyMatrix[i] = _keyLine[i];
        }
    }

private:
    Bus &bus;
    z80 &cpu;

    std::array<uint8_t, 8> _keyLine;

    void updateKeyLine(QKeyEvent *event, bool pressed)
    {
        int key = event->key();
        bool isDown = pressed;

        auto modifyBit = [&](int line, int bit, bool clear)
        {
            if (clear)
                _keyLine[line] &= ~(1 << bit);
            else
                _keyLine[line] |= (1 << bit);
        };

        // Map keys to _keyLine array and bits
        switch (key)
        {
        case Qt::Key_Shift:
            modifyBit(0, 0, isDown);
            break;
        case Qt::Key_Z:
            modifyBit(0, 1, isDown);
            break;
        case Qt::Key_X:
            modifyBit(0, 2, isDown);
            break;
        case Qt::Key_C:
            modifyBit(0, 3, isDown);
            break;
        case Qt::Key_V:
            modifyBit(0, 4, isDown);
            break;
        case Qt::Key_A:
            modifyBit(1, 0, isDown);
            break;
        case Qt::Key_S:
            modifyBit(1, 1, isDown);
            break;
        case Qt::Key_D:
            modifyBit(1, 2, isDown);
            break;
        case Qt::Key_F:
            modifyBit(1, 3, isDown);
            break;
        case Qt::Key_G:
            modifyBit(1, 4, isDown);
            break;
        case Qt::Key_Q:
            modifyBit(2, 0, isDown);
            break;
        case Qt::Key_W:
            modifyBit(2, 1, isDown);
            break;
        case Qt::Key_E:
            modifyBit(2, 2, isDown);
            break;
        case Qt::Key_R:
            modifyBit(2, 3, isDown);
            break;
        case Qt::Key_T:
            modifyBit(2, 4, isDown);
            break;
        case Qt::Key_1:
            modifyBit(3, 0, isDown);
            std::cout<<"PC "<<std::hex<<cpu.PC<<std::endl;
            std::cout<<"F "<<std::hex<<(int)cpu.F<<std::endl;
            break;
        case Qt::Key_2:
            modifyBit(3, 1, isDown);
            std::cout<<"PC "<<std::hex<<cpu.PC<<std::endl;
            std::cout<<"F "<<std::hex<<(int)cpu.F<<std::endl;
            break;
        case Qt::Key_3:
            modifyBit(3, 2, isDown);
            break;
        case Qt::Key_4:
            modifyBit(3, 3, isDown);
            break;
        case Qt::Key_5:
            modifyBit(3, 4, isDown);
            break;
        case Qt::Key_0:
            modifyBit(4, 0, isDown);
            break;
        case Qt::Key_9:
            modifyBit(4, 1, isDown);
            break;
        case Qt::Key_8:
            modifyBit(4, 2, isDown);
            break;
        case Qt::Key_7:
            modifyBit(4, 3, isDown);
            break;
        case Qt::Key_6:
            modifyBit(4, 4, isDown);
            break;
        case Qt::Key_P:
            modifyBit(5, 0, isDown);
            break;
        case Qt::Key_O:
            modifyBit(5, 1, isDown);
            break;
        case Qt::Key_I:
            modifyBit(5, 2, isDown);
            break;
        case Qt::Key_U:
            modifyBit(5, 3, isDown);
            break;
        case Qt::Key_Y:
            modifyBit(5, 4, isDown);
            break;
        case Qt::Key_Return:
        case Qt::Key_Enter:
            modifyBit(6, 0, isDown);
            break;
        case Qt::Key_L:
            modifyBit(6, 1, isDown);
            std::cout<<"PC "<<std::hex<<cpu.PC<<std::endl;
            std::cout<<"F "<<std::hex<<(int)cpu.F<<std::endl;
            break;
        case Qt::Key_K:
            modifyBit(6, 2, isDown);
            break;
        case Qt::Key_J:
            modifyBit(6, 3, isDown);
            break;
        case Qt::Key_H:
            modifyBit(6, 4, isDown);
            break;
        case Qt::Key_Space:
            modifyBit(7, 0, isDown);
            break;
        case Qt::Key_Control:
            modifyBit(7, 1, isDown);
            break;
        case Qt::Key_M:
            modifyBit(7, 2, isDown);
            break;
        case Qt::Key_N:
            modifyBit(7, 3, isDown);
            break;
        case Qt::Key_B:
            modifyBit(7, 4, isDown);
            break;
        case Qt::Key_Backspace:
        case Qt::Key_Delete:
            modifyBit(0, 0, isDown);
            modifyBit(4, 0, isDown);
            break;
        case Qt::Key_Left:
            modifyBit(0, 0, isDown);
            modifyBit(3, 4, isDown);
            break;
        case Qt::Key_Right:
            modifyBit(0, 0, isDown);
            modifyBit(4, 2, isDown);
            break;
        case Qt::Key_Up:
            modifyBit(0, 0, isDown);
            modifyBit(4, 3, isDown);
            break;
        case Qt::Key_Down:
            modifyBit(0, 0, isDown);
            modifyBit(4, 4, isDown);
            break;
        default:
            break;
        }
    }
};

#include <QTimer>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    Memory memory(0x10000); // 64KB memory
    Bus bus(memory);
    z80 cpu;

    bus.loadROM("C:\\Users\\eelip\\Downloads\\48.rom");
    cpu.reset(&bus);

    ZXScreen screen(bus, cpu);
    screen.show();

    QTimer *screenUpdateTimer = new QTimer(&screen);
    QObject::connect(screenUpdateTimer, &QTimer::timeout, [&]()
                     {
    
    screen.updateScreenBuffer(); });
    screenUpdateTimer->start(20); // 50hz 16 for 60

    QTimer *cpuTimer = new QTimer(&screen);
    QObject::connect(cpuTimer, &QTimer::timeout, [&]()
                     {
       
        for (int i = 0; i < 1000; ++i) {
            cpu.run(bus.read(cpu.PC));
            bus.interrupt = false;
        } });
    cpuTimer->start(1); 

    return app.exec();
}

#include "zxSpectrum.moc"
