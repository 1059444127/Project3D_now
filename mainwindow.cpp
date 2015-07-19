#include "mainwindow.h"
#include "CaptureDialog.hpp"
MainWindow::MainWindow(QWidget *parent, Qt::WindowFlags flags)
    : QMainWindow(parent, flags)
{
    setupUi(this);
}

MainWindow::~MainWindow()
{

}

void MainWindow::on_capture_button_clicked()
{
    CaptureDialog dialog(this);
    dialog.setModal(true);
    dialog.exec();
}
