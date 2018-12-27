#ifndef DATEVALIDATOR_H
#define DATEVALIDATOR_H

#include <QDate>
#include "itemprocessor.h"

class DateValidator : public ItemProcessor
{
public:
   DateValidator(QObject* parent);
   void processItem(LedgerAccountCommand const& account);
   void processItem(LedgerAllocation const& allocation);
   void processItem(LedgerBudget const& budget);
   void processItem(LedgerComment const& comment);
   void processItem(LedgerTransaction const& transaction);
   void stop();

private:
   void processDate(QDate const& date, QString const& fileName, int lineNum);

private:
   QDate m_latestDate;
};

#endif
