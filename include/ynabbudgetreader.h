#ifndef YNABBUDGETREADER_H
#define YNABBUDGETREADER_H

#include <QObject>

class LedgerBudgetAllocation;
class LedgerItem;

class YnabBudgetReader : public QObject
{
   Q_OBJECT

public:
   explicit YnabBudgetReader(QObject* parent = nullptr);

public slots:
   void processRecord(QHash<QString, QString> const& record,
                      QString const& fileName, int lineNum);
   void stop();

signals:
   void finished();
   void item(QDate const&, LedgerItem*);

private:
   LedgerBudgetAllocation* m_allocation = nullptr;
};

#endif
