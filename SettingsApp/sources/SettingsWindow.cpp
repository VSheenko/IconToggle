#include "SettingsWindow.h"

SettingsWindow::SettingsWindow(QWidget *parent) : QMainWindow(parent) , ui(new Ui::MainWindow) {
    ui->setupUi(this);

    connect(ui->btn_apply, &QPushButton::clicked, this, &SettingsWindow::on_btn_apply_clicked);
    connect(ui->btn_reset, &QPushButton::clicked, this, &SettingsWindow::on_btn_reset_clicked);
}

SettingsWindow::~SettingsWindow() {
    delete ui;
}

void SettingsWindow::on_btn_reset_clicked() {

}

void SettingsWindow::on_btn_apply_clicked() {
//    ConfigWorker* config = ConfigWorker::Create("appsettings.bin");
//    config->Deserialization();

//    config->autoHideTime = ui->spinBox->value();
    if (ui->rb_double_click->isChecked()) {
//        config->keys[0] = 0;
//        config->keys[1] = 0;
//        config->keys[2] = 0;
    } else {
        // TODO: add key processing
    }
}

