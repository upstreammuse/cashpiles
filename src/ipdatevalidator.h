#ifndef IPDATEVALIDATOR_H
#define IPDATEVALIDATOR_H

#include "date.h"
#include "itemprocessor.h"

class Ledger;

class IPDateValidator : public ItemProcessor
{
public:
   IPDateValidator(Ledger& ledger);
   void processItem(LedgerAccount const& account);
   bool processItem(LedgerBudget const& budget);
   void processItem(LedgerReserve const& reserve);
   void processItem(LedgerTransaction const& transaction);

private:
   void processDate(Date const& date, int id);

private:
   Date m_latestDate;
   Ledger& m_ledger;
};

#endif
