#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "kernel/currency.h"
#include "kernel/ledgeritem.h"

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
   ui->messages->setSortingEnabled(false);
   ui->messages->setRowCount(ui->messages->rowCount() + 1);
   auto item_ = new QTableWidgetItem(item.fileName());
   ui->messages->setItem(ui->messages->rowCount() - 1, 0, item_);
   item_ = new QTableWidgetItem(QTableWidgetItem::UserType);
   item_->setData(Qt::DisplayRole, item.lineNum());
   ui->messages->setItem(ui->messages->rowCount() - 1, 1, item_);
   item_ = new QTableWidgetItem(msg);
   ui->messages->setItem(ui->messages->rowCount() - 1, 2, item_);
   ui->messages->setSortingEnabled(true);
   ui->messages->sortByColumn(1, Qt::AscendingOrder);
   ui->messages->resizeColumnsToContents();
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
   ui->accounts->expandAll();
   ui->accounts->resizeColumnToContents(0);
   ui->accounts->resizeColumnToContents(1);
}
