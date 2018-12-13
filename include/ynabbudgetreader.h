#ifndef YNABBUDGETREADER_H
#define YNABBUDGETREADER_H

#include <QObject>

class LedgerItem;

class YnabBudgetReader : public QObject
{
   Q_OBJECT

public:
   explicit YnabBudgetReader(QObject* parent = nullptr);

public slots:
   void processRecord(QHash<QString, QString> const& record,
                      QString const& fileName, int lineNum);

signals:
   void item(QDate const&, LedgerItem*);
};

#endif
