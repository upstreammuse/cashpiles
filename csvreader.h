#ifndef CSVREADER_H
#define CSVREADER_H

#include <QObject>

class CsvReader : public QObject
{
   Q_OBJECT

public:
   explicit CsvReader(const QString& filename, QObject* parent = nullptr);

public slots:
   void read();

signals:
   void done();
   void record(const QHash<QString, QString>&);

private:
   QString m_filename;
};

#endif
