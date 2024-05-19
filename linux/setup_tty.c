#include <termios.h>

int setup_tty(int tty_fd) {
    struct termios serial;
    memset(&serial, 0, sizeof(struct termios));

    serial.c_cflag =
        CS8 | CREAD | CLOCAL; // 8-bites keretméret, vétel engedélyezése, modem
                              // control tiltása
    serial.c_cc[VMIN] = 1; // karakterenkénti olvasás engedélyezése
    serial.c_cc[VTIME] = 5; // nem-kanonikus olvasás időlimitje tizedmásodpercben
    cfsetospeed(&serial, B115200); // adó sebességének beállítása
    cfsetispeed(&serial, B115200); // vevő sebességének beállítása

    // beállítások alkalmazása
    tcsetattr(tty_fd, TCSANOW, &serial);
}

