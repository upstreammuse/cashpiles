#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QCheckBox>
#include "kernel/currency.h"
#include "kernel/ledgeraccountbalance.h"
#include "kernel/ledgeritem.h"
#include "kernel/ledgertransaction.h"

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

void MainWindow::appendBalance(LedgerAccountBalance const& balance)
{
   auto item = new QTreeWidgetItem(ui->balances);
   item->setText(0, balance.account());
   item->setData(1, Qt::DisplayRole, balance.date());
   item->setText(2, balance.amount().toString());
   item->setFont(2, QFont("Courier"));
   item->setTextAlignment(2, Qt::AlignRight | Qt::AlignVCenter);
}

void MainWindow::appendMessage(QString const& msg)
{
   ui->messages->setRowCount(ui->messages->rowCount() + 1);
   auto item = new QTableWidgetItem(msg);
   ui->messages->setItem(ui->messages->rowCount() - 1, 2, item);
}

void MainWindow::appendMessage(LedgerItem const& item, QString const& msg)
{
   ui->messages->setRowCount(ui->messages->rowCount() + 1);
   auto item_ = new QTableWidgetItem(item.fileName());
   ui->messages->setItem(ui->messages->rowCount() - 1, 0, item_);
   item_ = new QTableWidgetItem(QTableWidgetItem::UserType);
   item_->setData(Qt::DisplayRole, item.lineNum());
   ui->messages->setItem(ui->messages->rowCount() - 1, 1, item_);
   item_ = new QTableWidgetItem(msg);
   ui->messages->setItem(ui->messages->rowCount() - 1, 2, item_);
}

void MainWindow::appendTransaction(LedgerTransaction const& transaction)
{
   auto item = new QTreeWidgetItem(ui->transactions);
   item->setData(0, Qt::DisplayRole, transaction.date());
   item->setText(1, transaction.cleared() ? "C" : "");
   item->setForeground(1, QBrush(QColor(0, 100, 0)));
   item->setTextAlignment(1, Qt::AlignCenter | Qt::AlignVCenter);
   item->setText(2, transaction.account());

   if (transaction.entries().size() == 1)
   {
      LedgerTransactionEntry entry = transaction.entries()[0];
      item->setText(3, entry.payee());
      item->setText(4, entry.hasCategory() ? entry.category() : "");
      item->setText(5, entry.amount().toString());
      item->setFont(5, QFont("Courier"));
      item->setTextAlignment(5, Qt::AlignRight | Qt::AlignVCenter);
   }
   else
   {
      item->setText(3, "--- Split ---");
      item->setText(4, "");
      item->setText(5, transaction.amount().toString());
      item->setFont(5, QFont("Courier"));
      item->setTextAlignment(5, Qt::AlignRight | Qt::AlignVCenter);

      foreach (LedgerTransactionEntry const& entry, transaction.entries())
      {
         auto child = new QTreeWidgetItem(item);
         child->setText(3, entry.payee());
         child->setText(4, entry.hasCategory() ? entry.category() : "");
         child->setText(5, entry.amount().toString());
         child->setFont(5, QFont("Courier"));
         child->setTextAlignment(5, Qt::AlignRight | Qt::AlignVCenter);
      }
   }
}

void MainWindow::beautify()
{
   ui->accounts->expandAll();
   ui->accounts->resizeColumnToContents(0);
   ui->accounts->resizeColumnToContents(1);

   ui->balances->hideColumn(0);
   ui->balances->sortByColumn(1, Qt::AscendingOrder);
   ui->balances->resizeColumnToContents(1);
   ui->balances->resizeColumnToContents(2);

   ui->messages->setSortingEnabled(true);
   ui->messages->sortByColumn(1, Qt::AscendingOrder);
   ui->messages->resizeColumnsToContents();

   ui->transactions->expandAll();
   ui->transactions->sortByColumn(0, Qt::AscendingOrder);
   for (int i = 0; i < ui->transactions->columnCount(); ++i)
   {
      ui->transactions->resizeColumnToContents(i);
   }
}

void MainWindow::setAccountBalance(QString const& account, bool onbudget,
                                   Currency const& balance)
{
   auto items = ui->accounts->findItems(account, Qt::MatchExactly, 0);
   if (items.empty())
   {
      auto category = ui->accounts->topLevelItem(onbudget ? 1 : 2);
      Q_ASSERT(category);
      QStringList texts = {account, balance.toString()};
      QTreeWidgetItem* item = new QTreeWidgetItem(category, texts);
      item->setFont(1, QFont("Courier"));
      item->setTextAlignment(1, Qt::AlignRight | Qt::AlignVCenter);
      category->sortChildren(0, Qt::AscendingOrder);
   }
   else
   {
      Q_ASSERT(items.size() == 1);
      Q_ASSERT(items[0]);
      items[0]->setText(2, balance.toString());
   }
}

void MainWindow::on_accounts_itemSelectionChanged()
{
   auto items = ui->accounts->selectedItems();
   Q_ASSERT(items.size() == 1);

   if (items[0] == ui->accounts->topLevelItem(0))
   {
      ui->balances->setHidden(true);
      for (int i = 0; i < ui->transactions->topLevelItemCount(); ++i)
      {
         QTreeWidgetItem* transItem = ui->transactions->topLevelItem(i);
         transItem->setHidden(false);
      }
   }
   else if (items[0] == ui->accounts->topLevelItem(1)||
            items[0] == ui->accounts->topLevelItem(2))
   {
      ui->balances->setHidden(true);
      QSet<QString> accounts;
      for (int i = 0; i < items[0]->childCount(); ++i)
      {
         accounts.insert(items[0]->child(i)->text(0));
      }
      for (int i = 0; i < ui->transactions->topLevelItemCount(); ++i)
      {
         QTreeWidgetItem* transItem = ui->transactions->topLevelItem(i);
         transItem->setHidden(!accounts.contains(transItem->text(2)));
      }
   }
   else
   {
      ui->balances->setHidden(false);
      for (int i = 0; i < ui->balances->topLevelItemCount(); ++i)
      {
         QTreeWidgetItem* item = ui->balances->topLevelItem(i);
         item->setHidden(item->text(0) != items[0]->text(0));
      }
      for (int i = 0; i < ui->transactions->topLevelItemCount(); ++i)
      {
         QTreeWidgetItem* transItem = ui->transactions->topLevelItem(i);
         transItem->setHidden(transItem->text(2) != items[0]->text(0));
      }
   }
}
