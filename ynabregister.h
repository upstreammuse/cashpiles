#ifndef YNABREGISTER_H
#define YNABREGISTER_H

#include <QDate>
#include <QObject>

class Transaction;

class YnabRegister : public QObject
{
   Q_OBJECT

public:
   explicit YnabRegister(QObject* parent = nullptr);

signals:
   void transaction(Transaction const&);

public slots:
   void appendRecord(QHash<QString, QString> const& record);
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
