#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "transaction.h"

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

void MainWindow::showTransaction(Transaction const& transaction)
{
   // TODO build logic to ensure splits are equalized properly

   // TODO could have option to read budget expense/remaining and compare against transaction values for that month

   QMap<QString, QListWidgetItem*>::iterator it(
            m_accounts.find(transaction.account()));
   if (it == m_accounts.end())
   {
      new QListWidgetItem(transaction.account(), ui->accountsList);
      QListWidgetItem* value = new QListWidgetItem("0", ui->accountsList);
      it = m_accounts.insert(transaction.account(), value);
   }
   int amount = it.value()->text().toInt() + transaction.amount();
   it.value()->setText(QString::number(amount));

   foreach (TransactionSplit const& split, transaction.splits())
   {
      QMap<QString, QListWidgetItem*>::iterator it(
               m_budgets.find(split.category()));
      if (it == m_budgets.end())
      {
         new QListWidgetItem(split.category(), ui->budgetsList);
         QListWidgetItem* value = new QListWidgetItem("0", ui->budgetsList);
         it = m_budgets.insert(split.category(), value);
      }
      int amount = it.value()->text().toInt() + split.amount();
      it.value()->setText(QString::number(amount));
   }
}
