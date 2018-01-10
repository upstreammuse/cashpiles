#ifndef CSVREADER_H
#define CSVREADER_H

#include <QObject>

class CsvReader : public QObject
{
   Q_OBJECT

public:
   explicit CsvReader(QString const& filename, QObject* parent = nullptr);

public slots:
   void read();

signals:
   void done();
   void record(QHash<QString, QString> const&);

private:
   QString m_filename;
};

#endif
