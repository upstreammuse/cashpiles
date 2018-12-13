#ifndef YNABCONSOLIDATOR_H
#define YNABCONSOLIDATOR_H

#include <QDate>
#include <QMap>
#include <QObject>

class LedgerItem;

class YnabConsolidator : public QObject
{
   Q_OBJECT

public:
   explicit YnabConsolidator(QObject* parent = nullptr);

public slots:
   void processItem(QDate const& date, LedgerItem* item);
   void stopBudget();
   void stopRegister();

signals:
   void finished();
   void item(LedgerItem*);

private:
   QMap<QDate, LedgerItem*> m_items;
   bool m_stopBudget;
   bool m_stopRegister;
};

#endif
