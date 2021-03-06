#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class AccountBalancerUI;
class Currency;
class LedgerAccountBalance;
class LedgerItem;
class LedgerTransaction;
class MessageUI;
class QTreeWidgetItem;
namespace Ui { class MainWindow; }

class MainWindow : public QMainWindow
{
   Q_OBJECT

public:
   explicit MainWindow(QWidget* parent = 0);
   ~MainWindow();

public slots:
   void appendBalance(LedgerAccountBalance const& balance);
   void appendMessage(QString const& msg);
   void appendMessage(LedgerItem const& item, QString const& msg);
   void appendTransaction(LedgerTransaction const& transaction);
   void beautify();
   void setAccountBalance(QString const& account, bool onbudget,
                          Currency const& balance);

private slots:
   void on_accounts_itemSelectionChanged();

private:
   Ui::MainWindow* ui;
};

#endif
