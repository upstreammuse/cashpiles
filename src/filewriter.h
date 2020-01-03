#ifndef FILEWRITER_H
#define FILEWRITER_H

#include <QObject>
#include "itemprocessor.h"

class QIODevice;

class FileWriter : public QObject, public ItemProcessor
{
public:
   explicit FileWriter(QString const& fileName, QObject* parent = nullptr);
   void finish();
   void processItem(LedgerAccount const& account);
   void processItem(LedgerBudget const& budget);
   void processItem(LedgerBudgetGoalEntry const& entry);
   void processItem(LedgerBudgetIncomeEntry const& entry);
   void processItem(LedgerBudgetReserveAmountEntry const& entry);
   void processItem(LedgerBudgetReservePercentEntry const& entry);
   void processItem(LedgerBudgetRoutineEntry const& entry);
   void processItem(LedgerComment const& comment);
   void processItem(LedgerReserve const& reserve);
   void processItem(LedgerTransaction const& transaction);
   void setDateFormat(QString const& dateFormat);
   void start();
   bool success();

private:
   void breakBetween();
   void failed(QString const& message);

private:
   QString m_dateFormat = "yyyy/MM/dd";
   QIODevice* m_file = nullptr;
   QString m_fileName;
   bool m_firstItem = true;
   bool m_success = false;
};

#endif
