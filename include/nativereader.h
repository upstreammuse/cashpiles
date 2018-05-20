#ifndef NATIVEREADER_H
#define NATIVEREADER_H

#include <QObject>
#include "nativeaccountcommand.h"
#include "nativebudgetcommand.h"
#include "nativecomment.h"
#include "nativetransaction.h"

class QIODevice;

class NativeReader : public QObject
{
   Q_OBJECT

public:
   explicit NativeReader(QObject* parent = nullptr);

public slots:
   void readAll(QString const& filename);

signals:
   void account(NativeAccountCommand const& accountCommand);
   void budget(NativeBudgetCommand const& budgetCommand);
   void comment(NativeComment const& comment);
   void finished();
   void transaction(NativeTransaction const& transaction);

private:
   int nextItemNum();
   void processAccount(QRegularExpressionMatch const& match);
   void processBudget(QRegularExpressionMatch& match);
   void processComment(QRegularExpressionMatch const& match);
   void processCompactTransaction(QRegularExpressionMatch const& match);
   void processCompactTransactionOff(QRegularExpressionMatch const& match);
   void processLine(QString const& line);
   void processTransaction(QRegularExpressionMatch& match);
   QString readLine();
   void unReadLine(QString const& line);

private:
   QIODevice* m_file = nullptr;
   int m_itemNum = 0;
   int m_lineNum = 0;
   QStringList m_lines;
};

#endif
