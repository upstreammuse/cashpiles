#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QCheckBox>
#include "kernel/currency.h"
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
   int rowIndex = ui->transactions->rowCount();
   ui->transactions->setRowCount(rowIndex + 1);

   auto item = new QTableWidgetItem(QTableWidgetItem::UserType);
   item->setData(Qt::DisplayRole, transaction.date());
   item->setTextAlignment(Qt::AlignLeft | Qt::AlignTop);
   ui->transactions->setItem(rowIndex, 0, item);

   item = new QTableWidgetItem(QTableWidgetItem::UserType);
   item->setData(Qt::DisplayRole, transaction.cleared() ? "C" : "");
   item->setForeground(QBrush(QColor(0, 100, 0)));
   item->setTextAlignment(Qt::AlignLeft | Qt::AlignTop);
   ui->transactions->setItem(rowIndex, 1, item);

   item = new QTableWidgetItem(transaction.account());
   item->setTextAlignment(Qt::AlignLeft | Qt::AlignTop);
   ui->transactions->setItem(rowIndex, 2, item);

   if (transaction.entries().size() == 1)
   {
      LedgerTransactionEntry entry = transaction.entries()[0];

      item = new QTableWidgetItem(entry.payee());
      item->setTextAlignment(Qt::AlignLeft | Qt::AlignTop);
      ui->transactions->setItem(rowIndex, 3, item);

      item = new QTableWidgetItem(entry.hasCategory() ? entry.category() : "");
      item->setTextAlignment(Qt::AlignLeft | Qt::AlignTop);
      ui->transactions->setItem(rowIndex, 4, item);

      item = new QTableWidgetItem(entry.amount().toString());
      item->setFont(QFont("Courier"));
      item->setTextAlignment(Qt::AlignRight | Qt::AlignTop);
      ui->transactions->setItem(rowIndex, 5, item);
   }
   else
   {
      item = new QTableWidgetItem("--- Split ---");
      item->setTextAlignment(Qt::AlignLeft | Qt::AlignTop);
      ui->transactions->setItem(rowIndex, 3, item);

      item = new QTableWidgetItem("");
      item->setTextAlignment(Qt::AlignLeft | Qt::AlignTop);
      ui->transactions->setItem(rowIndex, 4, item);

      item = new QTableWidgetItem(transaction.amount().toString());
      item->setFont(QFont("Courier"));
      item->setTextAlignment(Qt::AlignRight | Qt::AlignTop);
      ui->transactions->setItem(rowIndex, 5, item);

      foreach (LedgerTransactionEntry const& entry, transaction.entries())
      {
         item = ui->transactions->item(rowIndex, 3);
         item->setText(item->text() + "\n" + entry.payee());

         item = ui->transactions->item(rowIndex, 4);
         item->setText(item->text() + "\n");
         if (entry.hasCategory())
         {
            item->setText(item->text() + entry.category());
         }

         item = ui->transactions->item(rowIndex, 5);
         item->setText(item->text() + "\n" + entry.amount().toString());
      }
   }
}

void MainWindow::beautify()
{
   ui->accounts->expandAll();
   ui->accounts->resizeColumnToContents(0);
   ui->accounts->resizeColumnToContents(1);

   ui->messages->setSortingEnabled(true);
   ui->messages->sortByColumn(1, Qt::AscendingOrder);
   ui->messages->resizeColumnsToContents();

   ui->transactions->sortByColumn(0, Qt::AscendingOrder);
   ui->transactions->resizeRowsToContents();
   ui->transactions->resizeColumnsToContents();
}

void MainWindow::setAccountBalance(QString const& account, bool onbudget,
                                   Currency const& balance)
{
   auto items = ui->accounts->findItems(account, Qt::MatchExactly, 0);
   if (items.empty())
   {
      auto category = ui->accounts->topLevelItem(onbudget ? 0 : 1);
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
