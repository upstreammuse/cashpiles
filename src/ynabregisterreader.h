#pragma once

#include <memory>
#include <QDate>
#include <QMap>
#include <QSharedPointer>
#include "csvreader.h"
#include "currency.h"
#include "ledgeritem.h"

class Ledger;
class LedgerTransaction;

class YnabRegisterReader : public QObject
{
   Q_OBJECT

public:
   YnabRegisterReader(QString const& fileName, Ledger& ledger,
                      QObject* parent = nullptr);
   void readAll();
   void setDateFormat(QString const& dateFormat);

private:
   void processRecord(CsvReader::Record const& record);

private:
   std::string m_dateFormat = "MM/dd/yyyy";
   QMap<QDate, std::shared_ptr<LedgerItem>> m_items;
   Ledger& m_ledger;
   CsvReader m_reader;
   std::shared_ptr<LedgerTransaction> m_transaction;
};
