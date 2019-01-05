#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class AccountBalancerUI;
class Currency;
class LedgerItem;
class LedgerTransaction;
class MessageUI;
namespace Ui { class MainWindow; }

class MainWindow : public QMainWindow
{
   Q_OBJECT

public:
   explicit MainWindow(QWidget* parent = 0);
   ~MainWindow();

public slots:
   void appendMessage(QString const& msg);
   void appendMessage(LedgerItem const& item, QString const& msg);
   void appendTransaction(LedgerTransaction const& transaction);
   void beautify();
   void setAccountBalance(QString const& account, bool onbudget,
                          Currency const& balance);

private:
   Ui::MainWindow* ui;
};

#endif
