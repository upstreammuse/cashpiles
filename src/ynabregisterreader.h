#pragma once

#include <memory>
#include "csvreader.h"
#include "currency.h"
#include "date.h"
#include "ledgeritem.h"

class Ledger;
class LedgerTransactionV2;

class YnabRegisterReader
{
public:
   YnabRegisterReader(std::string const& fileName, Ledger& ledger);
   void readAll();
   void setDateFormat(std::string const& dateFormat);

private:
   Currency parseCurrency(
         std::string const& currency, std::string const& filename,
         size_t linenum);
   Date parseDate(
         std::string const& date, std::string const& filename, size_t linenum);
   void processRecord(CsvReader::Record const& record);

private:
   std::string m_dateFormat = "MM/dd/yyyy";
   std::multimap<Date, std::shared_ptr<LedgerItem>> m_items;
   Ledger& m_ledger;
   CsvReader m_reader;
   std::shared_ptr<LedgerTransactionV2> m_transaction;
};
