#ifndef YNABCONSOLIDATOR_H
#define YNABCONSOLIDATOR_H

#include <QObject>

class LedgerItem;

class YnabConsolidator : public QObject
{
   Q_OBJECT

public:
   explicit YnabConsolidator(QObject* parent = nullptr);

public slots:
   void processItem(LedgerItem* item);
   void stopBudget();
   void stopRegister();

signals:
   void finished();
   void item(LedgerItem*);

private:
   bool m_stopBudget;
   bool m_stopRegister;
};

#endif
