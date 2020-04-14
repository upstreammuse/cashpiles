#pragma once

#include <memory>
#include "csvreader.h"
#include "currency.h"
#include "date.h"
#include "ledgeritem.h"

class Ledger;
class LedgerTransaction;

class YnabRegisterReader
{
public:
   YnabRegisterReader(std::string const& fileName, Ledger& ledger);
   void readAll();
   void setDateFormat(std::string const& dateFormat);

private:
   void processRecord(CsvReader::Record const& record);

private:
   std::string m_dateFormat = "MM/dd/yyyy";
   std::multimap<Date, std::shared_ptr<LedgerItem>> m_items;
   Ledger& m_ledger;
   CsvReader m_reader;
   std::shared_ptr<LedgerTransaction> m_transaction;
};
