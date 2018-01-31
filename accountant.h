#ifndef ACCOUNTANT_H
#define ACCOUNTANT_H

#include <QObject>
#include "account.h"
#include "category.h"
#include "transaction.h"

class Accountant : public QObject
{
   Q_OBJECT

public:
   explicit Accountant(QObject* parent = nullptr);

public slots:
   void appendTransaction(Transaction const& transaction);

private:
   QMap<QString, Account> m_accounts;
   QMap<QString, Category> m_categories;
};

#endif
