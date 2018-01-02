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

void MainWindow::showTransaction(const Transaction& transaction)
{
   // TODO build logic to ensure splits are equalized properly

   QMap<QString, QLabel*>::iterator it(m_accounts.find(transaction.account()));
   if (it == m_accounts.end())
   {
      QLabel* namelabel = new QLabel(transaction.account(), ui->accountsBox);
      ui->accountsBox->layout()->addWidget(namelabel);
      namelabel->show();
      QLabel* valuelabel = new QLabel("0", ui->accountsBox);
      ui->accountsBox->layout()->addWidget(valuelabel);
      valuelabel->show();
      it = m_accounts.insert(transaction.account(), valuelabel);
   }
   int amount = it.value()->text().toInt() + transaction.amount();
   it.value()->setText(QString::number(amount));

   foreach (TransactionSplit split, transaction.splits())
   {
      QMap<QString, QLabel*>::iterator it(m_budgets.find(split.category));
      if (it == m_budgets.end())
      {
         QLabel* namelabel = new QLabel(split.category, ui->budgetsBox);
         ui->budgetsBox->layout()->addWidget(namelabel);
         namelabel->show();
         QLabel* valuelabel = new QLabel("0", ui->budgetsBox);
         ui->budgetsBox->layout()->addWidget(valuelabel);
         valuelabel->show();
         it = m_budgets.insert(split.category, valuelabel);
      }
      int amount = it.value()->text().toInt() + split.amount;
      it.value()->setText(QString::number(amount));
   }
}
