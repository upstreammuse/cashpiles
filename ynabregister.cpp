#include "ynabregister.h"

#include "transaction.h"

YnabRegister::YnabRegister(QObject* parent) :
   QObject(parent)
{
}

void YnabRegister::appendRecord(QHash<QString, QString> const& record)
{
   for (QHash<QString, QString>::const_iterator it(record.begin());
        it != record.end(); ++it)
   {
      if (it.key() == "Account")
      {
         m_accountColumn.append(it.value());
      }
      else if (it.key() == "Category")
      {
         m_categoryColumn.append(it.value());
      }
      else if (it.key() == "Date")
      {
         m_dateColumn.append(QDate::fromString(it.value(), "MM/dd/yyyy"));
      }
      else if (it.key() == "Inflow")
      {
         QString fieldCopy(it.value());
         m_inflowColumn.append(
                  fieldCopy.replace('$', "").replace(',', "").replace('.', "")
                  .toInt());
      }
      else if (it.key() == "Memo")
      {
         m_memoColumn.append(it.value());
      }
      else if (it.key() == "Outflow")
      {
         QString fieldCopy(it.value());
         m_outflowColumn.append(
                  fieldCopy.replace('$', "").replace(',', "").replace('.', "")
                  .toInt());
      }
      else if (it.key() == "Payee")
      {
         m_payeeColumn.append(it.value());
      }
   }
   Q_ASSERT(m_accountColumn.size() == m_categoryColumn.size());
   Q_ASSERT(m_accountColumn.size() == m_dateColumn.size());
   Q_ASSERT(m_accountColumn.size() == m_inflowColumn.size());
   Q_ASSERT(m_accountColumn.size() == m_memoColumn.size());
   Q_ASSERT(m_accountColumn.size() == m_outflowColumn.size());
   Q_ASSERT(m_accountColumn.size() == m_payeeColumn.size());
}

void YnabRegister::showTrans()
{
   Transaction t;
   for (int i = 0; i < m_accountColumn.size(); ++i)
   {
      t.setDate(m_dateColumn[i]);
      t.setAccount(m_accountColumn[i]);

      QRegExp rx("Split \\(([0-9]*)/([0-9]*)\\) (.*)");
      bool inSplit = false;
      QString memo;
      if (rx.exactMatch(m_memoColumn[i]))
      {
         memo = rx.cap(3);
         size_t splitIndex = rx.cap(1).toInt();
         size_t splitCount = rx.cap(2).toInt();
         inSplit = splitIndex != splitCount;
      }
      else
      {
         memo = m_memoColumn[i];
      }
      t.appendSplit(TransactionSplit(
                       m_payeeColumn[i], m_categoryColumn[i], memo,
                       m_inflowColumn[i] - m_outflowColumn[i]));

      if (!inSplit)
      {
         emit transaction(t);
         t.clear();
      }
   }
}
