#pragma once

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
   QString m_dateFormat = "MM/dd/yyyy";
   QMap<QDate, QSharedPointer<LedgerItem>> m_items;
   Ledger& m_ledger;
   CsvReader* m_reader;
   QSharedPointer<LedgerTransaction> m_transaction;
};
