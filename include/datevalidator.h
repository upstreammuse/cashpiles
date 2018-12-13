#ifndef DATEVALIDATOR_H
#define DATEVALIDATOR_H

#include <QDate>
#include "itemprocessor.h"

class DateValidator : public ItemProcessor
{
public:
   DateValidator(QObject* parent);
   void processItem(LedgerAccountCommand const& account);
   void processItem(LedgerBudgetAllocation const& allocation);
   void processItem(LedgerComment const& comment);
   void processItem(LedgerTransaction const& transaction);

private:
   void processDate(QDate const& date, QString const& fileName, int lineNum);

private:
   QDate m_latestDate;
};

#endif
