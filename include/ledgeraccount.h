#ifndef LEDGERACCOUNT_H
#define LEDGERACCOUNT_H

#include <QMap>
#include <QString>

class LedgerAccount
{
public:
   void addBalance(int amount);
   int balance() const;

   bool budget() const;
   void setBudget(bool budget);

   QString name() const;
   void setName(QString const& name);

   void addTransferFrom(QString const& payee, int amount);
   void addTransferTo(QString const& payee, int amount);

private:
   int m_balance = 0;
   bool m_budget = false;
   QString m_name;
   QMap<QString, int> m_transfersFrom;
   QMap<QString, int> m_transfersTo;
};

#endif
