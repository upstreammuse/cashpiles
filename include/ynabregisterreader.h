#ifndef YNABREGISTERREADER_H
#define YNABREGISTERREADER_H

#include <QDate>
#include <QObject>
#include "ledgertransaction.h"

class YnabRegisterReader : public QObject
{
   Q_OBJECT

public:
   enum ClearedStatus
   {
      UNCLEARED,
      CLEARED,
      RECONCILED
   };

public:
   explicit YnabRegisterReader(QObject* parent = nullptr);

public slots:
   void processRecord(QHash<QString, QString> const& record,
                      QString const& filename, int lineNum);
   void stop();

signals:
   void finished();
   void item(LedgerItem*);

private:
   QStringList m_accountColumn;
   QStringList m_categoryColumn;
   QList<ClearedStatus> m_clearedColumn;
   QList<QDate> m_dateColumn;
   QStringList m_filenameColumn;
   QList<int> m_inflowColumn;
   QList<int> m_lineNumColumn;
   QStringList m_memoColumn;
   QList<int> m_outflowColumn;
   QStringList m_payeeColumn;
};

#endif
