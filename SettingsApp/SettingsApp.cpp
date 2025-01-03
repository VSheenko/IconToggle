#include <QApplication>
#include <QPushButton>
#include "SettingsWindow.h"

int main(int argc, char* argv[]) {
    QApplication a(argc, argv);
    SettingsWindow win;
    win.show();
    return a.exec();
}
