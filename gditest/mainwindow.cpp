#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    graphWidget = new GraphicsWidget(this);
    ui->vLayoutGraphic->addWidget(graphWidget);
}

MainWindow::~MainWindow()
{
    delete ui;
}
