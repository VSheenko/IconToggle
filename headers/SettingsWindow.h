#ifndef ICONTOGGLE_SETTINGSWINDOW_H
#define ICONTOGGLE_SETTINGSWINDOW_H

#include <QMainWindow>
#include "../forms/ui_SettingsWindow.h"

class SettingsWindow : public QMainWindow {
Q_OBJECT

public:
    explicit SettingsWindow(QWidget *parent = nullptr);

    ~SettingsWindow();

private:
    Ui::MainWindow *ui;
};

#endif //ICONTOGGLE_SETTINGSWINDOW_H
