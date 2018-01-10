#ifndef YNABBUDGET_H
#define YNABBUDGET_H

#include <QDate>
#include <QObject>

class Transaction;

class YnabBudget : public QObject
{
   Q_OBJECT

public:
   explicit YnabBudget(QObject* parent = nullptr);

signals:
   // TODO this is a hack for now
   void transaction(Transaction const&);

public slots:
   void appendRecord(QHash<QString, QString> const& record);

private:
   QList<int> m_budgetColumn;
   QStringList m_categoryColumn;
   QList<QDate> m_dateColumn;
   QStringList m_groupColumn;
};

#endif
