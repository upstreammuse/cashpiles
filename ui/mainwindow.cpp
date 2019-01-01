#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent) :
   QMainWindow(parent),
   ui(new Ui::MainWindow)
{
   ui->setupUi(this);
}

MainWindow::~MainWindow()
{
   delete ui;
}

AccountBalancerUI* MainWindow::accountBalancerUI()
{
   return ui->accountBalancerUI;
}

MessageUI* MainWindow::messageUI()
{
   return ui->messageUI;
}
