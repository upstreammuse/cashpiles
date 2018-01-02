#ifndef YNABCSVREADER_H
#define YNABCSVREADER_H

#include <QObject>

class YnabCsvReader : public QObject
{
   Q_OBJECT

public:
   explicit YnabCsvReader(const QString& filename, QObject* parent = nullptr);
   void read();

signals:
   void done();
   void field(const QString&);
   void fieldCount(size_t);

private:
   QString m_filename;
};

#endif
