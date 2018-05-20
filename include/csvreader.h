#ifndef CSVREADER_H
#define CSVREADER_H

#include <QObject>

class CsvReader : public QObject
{
   Q_OBJECT

public:
   explicit CsvReader(QObject* parent = nullptr);

public slots:
   void readAll(QString const& filename);

signals:
   void finished();
   void record(QHash<QString, QString> const&);
};

#endif
