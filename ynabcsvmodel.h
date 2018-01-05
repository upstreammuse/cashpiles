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
   void appendRecord(const QHash<QString, QString>& record);
   void showTrans();

private:
   QStringList m_accountColumn;
   QStringList m_categoryColumn;
   QList<QDate> m_dateColumn;
   QList<int> m_inflowColumn;
   QStringList m_memoColumn;
   QList<int> m_outflowColumn;
   QStringList m_payeeColumn;
};

#endif
