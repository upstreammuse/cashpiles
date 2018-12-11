#ifndef YNABBUDGETREADER_H
#define YNABBUDGETREADER_H

#include <QDate>
#include <QObject>
#include "ledgerbudgetallocation.h"

class YnabBudgetReader : public QObject
{
   Q_OBJECT

public:
   explicit YnabBudgetReader(QObject* parent = nullptr);

public slots:
   void processRecord(QHash<QString, QString> const& record,
                      QString const& filename, int lineNum);

signals:
   void item(LedgerItem*);
};

#endif
