#ifndef ACCOUNTBALANCERUI_H
#define ACCOUNTBALANCERUI_H

#include <QWidget>

class Currency;
class LedgerItem;
namespace Ui { class AccountBalancerUI; }

class AccountBalancerUI : public QWidget
{
   Q_OBJECT

public:
   explicit AccountBalancerUI(QWidget* parent = 0);
   ~AccountBalancerUI();

public slots:
   void message(QString const& msg);
   void message(LedgerItem const& item, QString const& msg);
   void setBalance(QString const& account, Currency const& balance);

private:
   Ui::AccountBalancerUI* ui;
};

#endif
