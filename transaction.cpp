#include "transaction.h"

#include <QTextStream>

QString Transaction::account() const
{
   return m_account;
}

int Transaction::amount() const
{
   int sum = 0;
   foreach (int a, m_amounts)
   {
      sum += a;
   }
   return sum;
}

void Transaction::appendSplit(const QString& payee, const QString& category,
                              const QString& memo, int amount)
{
   m_payees.append(payee);
   m_categories.append(category);
   m_memos.append(memo);
   m_amounts.append(amount);
}

void Transaction::clear()
{
   m_account.clear();
   m_amounts.clear();
   m_categories.clear();
   m_date = QDate();
   m_memos.clear();
   m_payees.clear();
}

void Transaction::setAccount(const QString& account)
{
   m_account = account;
}

void Transaction::setDate(const QDate& date)
{
   m_date = date;
}

QList<TransactionSplit> Transaction::splits() const
{
   QList<TransactionSplit> splits;
   for (int i = 0; i < m_payees.size(); ++i)
   {
      TransactionSplit split;
      split.amount = m_amounts[i];
      split.category = m_categories[i];
      split.memo = m_memos[i];
      split.payee = m_payees[i];
      splits.append(split);
   }
   return splits;
}

Transaction::operator QString() const
{
   QString str;
   QTextStream ts(&str);
   ts << "Account:" << m_account << ", Date:" << m_date.toString() << '\n';
   for (int i = 0; i < m_payees.size(); ++i)
   {
      ts << "    Payee:" << m_payees[i] << ", Category:" << m_categories[i]
            << ", Memo:" << m_memos[i] << ", Amount:" << m_amounts[i] << '\n';
   }
   return str;
}
