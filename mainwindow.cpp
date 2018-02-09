#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "transaction.h"

MainWindow::MainWindow(QWidget* parent) :
   QMainWindow(parent),
   ui(new Ui::MainWindow),
   m_budgetable(0),
   // TODO use a real file name or registry
   m_settings("~/cashpiles.ini", QSettings::IniFormat, this),
   m_worth(0)
{
   ui->setupUi(this);
}

MainWindow::~MainWindow()
{
   delete ui;
}

void MainWindow::on_actionImportYnab_triggered()
{
   qWarning("tried to import");
}

void MainWindow::showBudget(Transaction const& transaction)
{
   foreach (TransactionSplit const& split, transaction.splits())
   {
      QMap<QString, QListWidgetItem*>::iterator it(
               m_budgets.find(split.category()));
      if (it == m_budgets.end())
      {
         QListWidgetItem* item = new QListWidgetItem("", ui->budgetsList);
         item->setData(QListWidgetItem::UserType + 0, split.category());
         item->setData(QListWidgetItem::UserType + 1, 0);
         item->setText("");
         it = m_budgets.insert(split.category(), item);
      }
      int amount = it.value()->data(QListWidgetItem::UserType + 1).toInt();
      amount += split.amount();
      it.value()->setData(QListWidgetItem::UserType + 1, amount);
      it.value()->setText(
               it.value()->data(QListWidgetItem::UserType + 0).toString() +
               ": " +
               m_locale.toCurrencyString(
                  it.value()->data(QListWidgetItem::UserType + 1).toDouble() /
                  100));
   }
}

void MainWindow::showTransaction(Transaction const& transaction)
{
   QMap<QString, QListWidgetItem*>::iterator it(
            m_accounts.find(transaction.account()));
   if (it == m_accounts.end())
   {
      QListWidgetItem* item = new QListWidgetItem("", ui->accountsList);
      item->setData(QListWidgetItem::UserType + 0, transaction.account());
      item->setData(QListWidgetItem::UserType + 1, 0);
      item->setData(QListWidgetItem::UserType + 2, 0);
      item->setText("");
      it = m_accounts.insert(transaction.account(), item);
   }

   int amount = it.value()->data(QListWidgetItem::UserType + 1).toInt();
   amount += transaction.amount();
   it.value()->setData(QListWidgetItem::UserType + 1, amount);
   if (transaction.cleared())
   {
      amount = it.value()->data(QListWidgetItem::UserType + 2).toInt();
      amount += transaction.amount();
      it.value()->setData(QListWidgetItem::UserType + 2, amount);
   }

   it.value()->setText(
            it.value()->data(QListWidgetItem::UserType + 0).toString() +
            "\n\tWorking: " +
            m_locale.toCurrencyString(
               it.value()->data(QListWidgetItem::UserType + 1).toDouble() /
               100) +
            ", Cleared: " +
            m_locale.toCurrencyString(
               it.value()->data(QListWidgetItem::UserType + 2).toDouble() /
               100));

   m_worth += transaction.amount();
   ui->worth->setText(m_locale.toCurrencyString(double(m_worth) / 100));
   m_settings.beginGroup("Accounts");
   m_settings.beginGroup(transaction.account());
   m_settings.setValue("onBudget", m_settings.value("onBudget", true).toBool());
   if (m_settings.value("onBudget", true).toBool())
   {
      m_budgetable += transaction.amount();
   }
   ui->budgetable->setText(
            m_locale.toCurrencyString(double(m_budgetable) / 100));
   m_settings.endGroup();
   m_settings.endGroup();

   showBudget(transaction);
}
