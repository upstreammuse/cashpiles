#include "ynabcsvmodel.h"

#include <QDebug>

#include "transaction.h"

YnabCsvModel::YnabCsvModel(QObject* parent) :
   QObject(parent),
   m_accountFieldIndex(-1),
   m_categoryFieldIndex(-1),
   m_dateFieldIndex(-1),
   m_fieldCount(0),
   m_inflowFieldIndex(-1),
   m_memoFieldIndex(-1),
   m_nextFieldIndex(0),
   m_outflowFieldIndex(-1),
   m_payeeFieldIndex(-1)
{
}

void YnabCsvModel::appendField(const QString& field)
{
   if (m_accountFieldIndex == size_t(-1) && field == "Account")
   {
      m_accountFieldIndex = m_nextFieldIndex;
   }
   else if (m_nextFieldIndex == m_accountFieldIndex)
   {
      m_accountColumn.append(field);
   }
   else if (m_categoryFieldIndex == size_t(-1) && field == "Category")
   {
      m_categoryFieldIndex = m_nextFieldIndex;
   }
   else if (m_nextFieldIndex == m_categoryFieldIndex)
   {
      m_categoryColumn.append(field);
   }
   else if (m_dateFieldIndex == size_t(-1) && field == "Date")
   {
      m_dateFieldIndex = m_nextFieldIndex;
   }
   else if (m_nextFieldIndex == m_dateFieldIndex)
   {
      m_dateColumn.append(QDate::fromString(field, "MM/dd/yyyy"));
   }
   else if (m_inflowFieldIndex == size_t(-1) && field == "Inflow")
   {
      m_inflowFieldIndex = m_nextFieldIndex;
   }
   else if (m_nextFieldIndex == m_inflowFieldIndex)
   {
      QString fieldCopy(field);
      m_inflowColumn.append(
               fieldCopy.replace('$', "").replace(',', "").replace('.', "")
               .toInt());
   }
   else if (m_memoFieldIndex == size_t(-1) && field == "Memo")
   {
      m_memoFieldIndex = m_nextFieldIndex;
   }
   else if (m_nextFieldIndex == m_memoFieldIndex)
   {
      m_memoColumn.append(field);
   }
   else if (m_outflowFieldIndex == size_t(-1) && field == "Outflow")
   {
      m_outflowFieldIndex = m_nextFieldIndex;
   }
   else if (m_nextFieldIndex == m_outflowFieldIndex)
   {
      QString fieldCopy(field);
      m_outflowColumn.append(
               fieldCopy.replace('$', "").replace(',', "").replace('.', "")
               .toInt());
   }
   else if (m_payeeFieldIndex == size_t(-1) && field == "Payee")
   {
      m_payeeFieldIndex = m_nextFieldIndex;
   }
   else if (m_nextFieldIndex == m_payeeFieldIndex)
   {
      m_payeeColumn.append(field);
   }

   ++m_nextFieldIndex;
   if (m_fieldCount != 0 && m_nextFieldIndex >= m_fieldCount)
   {
      m_nextFieldIndex = 0;
   }
}

void YnabCsvModel::setFieldCount(size_t count)
{
   if (m_fieldCount == 0)
   {
      m_fieldCount = count;
      if (m_nextFieldIndex >= m_fieldCount)
      {
         m_nextFieldIndex = 0;
      }
   }
   Q_ASSERT(m_fieldCount == count);
}

void YnabCsvModel::showTrans()
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
      t.appendSplit(m_payeeColumn[i], m_categoryColumn[i], memo,
                    m_inflowColumn[i] - m_outflowColumn[i]);

      if (!inSplit)
      {
         qDebug() << t;
         emit transaction(t);
         t.clear();
      }
   }
}
