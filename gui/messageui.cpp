#include "messageui.h"
#include "ui_messageui.h"

#include "kernel/ledgeritem.h"

MessageUI::MessageUI(QWidget* parent) :
   QWidget(parent),
   ui(new Ui::MessageUI)
{
   ui->setupUi(this);
}

MessageUI::~MessageUI()
{
   delete ui;
}

void MessageUI::appendMessage(QString const& msg)
{
   ui->messages->setRowCount(ui->messages->rowCount() + 1);
   auto item = new QTableWidgetItem(msg);
   ui->messages->setItem(ui->messages->rowCount() - 1, 2, item);
}

void MessageUI::appendMessage(LedgerItem const& item, QString const& msg)
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
