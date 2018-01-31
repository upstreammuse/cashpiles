#include "account.h"

Account::Account() :
   m_allClear(true),
   m_balance(0)
{
}

void Account::appendTransaction(Transaction const& transaction)
{
   m_allClear = m_allClear && transaction.cleared();
   m_balance += transaction.amount();
   verifyName(transaction.account());
   verifyDate(transaction.date());
   verifyBalance(transaction.balance(), transaction.hasBalance());
}

void Account::verifyBalance(int balance, bool hasBalance)
{
   if (hasBalance)
   {
      if (!m_allClear)
      {
         qWarning("uncleared transactions before a balance assertion");
      }
      if (balance != m_balance)
      {
         qWarning("balance assertion failed");
      }
   }
}

void Account::verifyDate(QDate const& date)
{
   if (m_latestDate.isNull())
   {
      m_latestDate = date;
   }
   if (date < m_latestDate)
   {
      qWarning("transaction dates are out of order");
   }
   m_latestDate = date;
}

void Account::verifyName(QString const& name)
{
   if (m_name.isNull())
   {
      m_name = name;
   }
   Q_ASSERT(m_name == name);
}
