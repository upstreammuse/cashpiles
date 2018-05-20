#ifndef NATIVEMODEL_H
#define NATIVEMODEL_H

#include <QDate>
#include <QObject>
#include "ledgeraccountcommand.h"
#include "ledgerbudgetallocation.h"
#include "ledgercomment.h"
#include "ledgertransaction.h"
#include "nativeaccountcommand.h"
#include "nativebudgetcommand.h"
#include "nativecomment.h"
#include "nativetransaction.h"

class NativeModel : public QObject
{
   Q_OBJECT

signals:
   void accountChanged(LedgerAccountCommand const& command);
   void budgetAllocated(LedgerBudgetAllocation const& allocation);
   void commented(LedgerComment const& comment);
   void transacted(LedgerTransaction const& transaction);

public slots:
   void parseAccount(NativeAccountCommand const& accountCommand);
   void parseBudget(NativeBudgetCommand const& budgetCommand);
   void parseComment(NativeComment const& comment);
   void parseTransaction(NativeTransaction const& transaction);

private:
   int parseCurrency(QString curr, int line);
   QDate parseDate(QString const& date, int line);

private:
   int m_decimalDigits = -1;
};

#endif
