#ifndef YNABCSVMODEL_H
#define YNABCSVMODEL_H

#include <QDate>
#include <QObject>

#include "transaction.h"

class YnabCsvModel : public QObject
{
   Q_OBJECT

public:
   explicit YnabCsvModel(QObject* parent = nullptr);

signals:
   void transaction(const Transaction&);

public slots:
   void appendField(const QString& field);
   void setFieldCount(size_t count);
   void showTrans();

private:
   QStringList m_accountColumn;
   size_t m_accountFieldIndex;
   QStringList m_categoryColumn;
   size_t m_categoryFieldIndex;
   QList<QDate> m_dateColumn;
   size_t m_dateFieldIndex;
   size_t m_fieldCount;
   QList<int> m_inflowColumn;
   size_t m_inflowFieldIndex;
   QStringList m_memoColumn;
   size_t m_memoFieldIndex;
   size_t m_nextFieldIndex;
   QList<int> m_outflowColumn;
   size_t m_outflowFieldIndex;
   QStringList m_payeeColumn;
   size_t m_payeeFieldIndex;
};

#endif
