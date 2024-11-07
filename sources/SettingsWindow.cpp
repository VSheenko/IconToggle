#include "SettingsWindow.h"

SettingsWindow::SettingsWindow(QWidget *parent) : QMainWindow(parent) , ui(new Ui::MainWindow) {
    ui->setupUi(this);
}

SettingsWindow::~SettingsWindow() {
    delete ui;
}

