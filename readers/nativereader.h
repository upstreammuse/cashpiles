#ifndef NATIVEREADER_H
#define NATIVEREADER_H

#include <QObject>
#include "kernel/interval.h"
#include "kernel/ledgeraccountcommand.h"

class QIODevice;

class NativeReader : public QObject
{
   Q_OBJECT

public:
   explicit NativeReader(QString const& fileName, QObject* parent = nullptr);

public slots:
   void readAll();

signals:
   void item(LedgerItem*);
   void finished();
   void message(QString const&) const;
   void message(LedgerItem const&, QString const&) const;

private:
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
   QDate parseDate(QString const& date);
   Interval parseInterval(QString interval);
   LedgerAccountCommand::Mode parseMode(QString const& command);

private:
   QIODevice* m_file = nullptr;
   QString m_fileName;
   int m_lineNum = 0;
   QStringList m_lines;
};

#endif
