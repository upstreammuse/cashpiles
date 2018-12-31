#include "accountbalancerui.h"
#include "ui_accountbalancerui.h"

#include "currency.h"
#include "ledgeritem.h"

AccountBalancerUI::AccountBalancerUI(QWidget* parent) :
   QWidget(parent),
   ui(new Ui::AccountBalancerUI)
{
   ui->setupUi(this);
}

AccountBalancerUI::~AccountBalancerUI()
{
   delete ui;
}

void AccountBalancerUI::message(QString const& msg)
{
   ui->plainTextEdit->appendPlainText(msg);
}

void AccountBalancerUI::message(LedgerItem const& item, QString const& msg)
{
   ui->plainTextEdit->appendPlainText(QString("%1, file '%2', line %3")
                                      .arg(msg)
                                      .arg(item.fileName())
                                      .arg(item.lineNum()));
}

void AccountBalancerUI::setBalance(QString const& account,
                                   Currency const& balance)
{
   ui->tableWidget->setSortingEnabled(false);
   auto items = ui->tableWidget->findItems(account, Qt::MatchExactly);
   if (items.isEmpty())
   {
      ui->tableWidget->setRowCount(ui->tableWidget->rowCount() + 1);
      QTableWidgetItem* item = new QTableWidgetItem(account);
      ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, 0, item);
      item = new QTableWidgetItem(balance.toString());
      ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, 1, item);
   }
   else
   {
      ui->tableWidget->setItem(items[0]->row(), 1,
            new QTableWidgetItem(balance.toString()));
   }
   ui->tableWidget->setSortingEnabled(true);
   ui->tableWidget->sortByColumn(0, Qt::AscendingOrder);
}
