#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QSettings>
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
   // TODO use a real file name or registry
   QSettings* settings = new QSettings("~/cashpiles.ini",
                                       QSettings::IniFormat, this);

   // TODO build logic to ensure splits are equalized properly

   // TODO could have option to read budget expense/remaining and compare against transaction values for that month


   QMap<QString, QListWidgetItem*>::iterator it(
            m_accounts.find(transaction.account()));
   if (it == m_accounts.end())
   {
      QListWidgetItem* item = new QListWidgetItem("", ui->accountsList);
      item->setData(QListWidgetItem::UserType + 0, transaction.account());
      item->setData(QListWidgetItem::UserType + 1, 0);
      item->setText(item->data(QListWidgetItem::UserType + 0).toString() +
                    ":  " +
                    QString::number(
                       item->data(QListWidgetItem::UserType + 1).toInt()));
      it = m_accounts.insert(transaction.account(), item);
   }
   int amount = it.value()->data(QListWidgetItem::UserType + 1).toInt();
   amount += transaction.amount();
   it.value()->setData(QListWidgetItem::UserType + 1, amount);
   it.value()->setText(
            it.value()->data(QListWidgetItem::UserType + 0).toString() +
            ":  " +
            QString::number(
               it.value()->data(QListWidgetItem::UserType + 1).toInt()));

   foreach (TransactionSplit const& split, transaction.splits())
   {
      QMap<QString, QListWidgetItem*>::iterator it(
               m_budgets.find(split.category()));
      if (it == m_budgets.end())
      {
         QListWidgetItem* item = new QListWidgetItem("", ui->budgetsList);
         item->setData(QListWidgetItem::UserType + 0, split.category());
         item->setData(QListWidgetItem::UserType + 1, 0);
         item->setText(item->data(QListWidgetItem::UserType + 0).toString() +
                       ":  " +
                       QString::number(
                          item->data(QListWidgetItem::UserType + 1).toInt()));
         it = m_budgets.insert(split.category(), item);
      }
      int amount = it.value()->data(QListWidgetItem::UserType + 1).toInt();
      amount += split.amount();
      it.value()->setData(QListWidgetItem::UserType + 1, amount);
      it.value()->setText(
               it.value()->data(QListWidgetItem::UserType + 0).toString() +
               ":  " +
               QString::number(
                  it.value()->data(QListWidgetItem::UserType + 1).toInt()));
   }
}
