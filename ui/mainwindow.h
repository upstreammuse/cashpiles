#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class AccountBalancerUI;
class MessageUI;
namespace Ui { class MainWindow; }

class MainWindow : public QMainWindow
{
   Q_OBJECT

public:
   explicit MainWindow(QWidget* parent = 0);
   ~MainWindow();
   AccountBalancerUI* accountBalancerUI();
   MessageUI* messageUI();

private:
   Ui::MainWindow* ui;
};

#endif
