#ifndef DATEVALIDATOR_H
#define DATEVALIDATOR_H

#include <QDate>
#include "model/itemprocessor.h"
#include "model/ledgeritem.h"

class DateValidator : public ItemProcessor
{
   Q_OBJECT

public:
   DateValidator(QObject* parent);
   void processItem(LedgerAccountCommand const& account);
   void processItem(LedgerBudget const& budget);
   void processItem(LedgerComment const& comment);
   void processItem(LedgerTransaction const& transaction);
   void stop();

signals:
   void message(LedgerItem const&, QString const&) const;

private:
   void processDate(QDate const& date, QString const& fileName, int lineNum);

private:
   QDate m_latestDate;
};

#endif
