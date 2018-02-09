#include "transactionsplit.h"

TransactionSplit::TransactionSplit(QString const& payee,
                                   QString const& category, QString const& memo,
                                   int amount, bool isTransfer) :
   m_amount(amount),
   m_category(category),
   m_isTransfer(isTransfer),
   m_memo(memo),
   m_payee(payee)
{
}

int TransactionSplit::amount() const
{
   return m_amount;
}

QString TransactionSplit::category() const
{
   return m_category;
}

bool TransactionSplit::isTransfer() const
{
   return m_isTransfer;
}

QString TransactionSplit::memo() const
{
   return m_memo;
}

QString TransactionSplit::payee() const
{
   return m_payee;
}
