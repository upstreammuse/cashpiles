#ifndef CSVREADER_H
#define CSVREADER_H

#include <QFile>

class CsvReader : public QObject
{
   Q_OBJECT

public:
   explicit CsvReader(QString const& filename, QObject* parent = nullptr);

public slots:
   void readAll();

signals:
   void finished();
   void record(QHash<QString, QString> const&, QString const&, int);

private:
   QFile m_file;
};

#endif
