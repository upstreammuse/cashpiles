#ifndef DATEVALIDATOR_H
#define DATEVALIDATOR_H

#include <QDate>
#include "itemprocessor.h"

class DateValidator : public ItemProcessor
{
public:
   void processItem(LedgerAccount const& account);
   void processItem(LedgerBudget const& budget);
   void processItem(LedgerReserve const& reserve);
   void processItem(LedgerTransaction const& transaction);

private:
   void processDate(QDate const& date, QString const& fileName, uint lineNum);

private:
   QDate m_latestDate;
};

#endif
