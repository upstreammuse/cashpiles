#include "accountbalancerui.h"
#include "ui_accountbalancerui.h"

#include "kernel/currency.h"
#include "kernel/ledgeritem.h"

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

void AccountBalancerUI::setBalance(QString const& account, bool onbudget,
                                   Currency const& balance)
{
   auto items = ui->treeWidget->findItems(account, Qt::MatchExactly, 0);
   if (items.empty())
   {
      auto category = ui->treeWidget->topLevelItem(onbudget ? 0 : 1);
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
   ui->treeWidget->expandAll();
   ui->treeWidget->resizeColumnToContents(0);
   ui->treeWidget->resizeColumnToContents(1);
}
