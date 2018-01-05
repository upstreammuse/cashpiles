#include "csvreader.h"

#include <QFile>
#include <QHash>
#include <QMessageBox>

CsvReader::CsvReader(const QString& filename, QObject* parent) :
   QObject(parent),
   m_filename(filename)
{
}

void CsvReader::read()
{
   QFile f(m_filename);
   if (!f.open(QIODevice::ReadOnly))
   {
      QMessageBox::critical(
               0, "File not found",
               QString("Could not open the specified CSV file\n(%1)").arg(
                  m_filename));
   }

   int expectedFieldCount = 0;
   int fieldCount = 0;
   enum {EMPTY, NORMAL, QUOTED, CLOSED} fieldStatus = EMPTY;
   QString fieldValue;
   QStringList header;
   QByteArray line;
   QHash<QString, QString> record_;

   while (!(line = f.readLine()).isEmpty())
   {
      foreach (const char c, line)
      {
         if (c == ',')
         {
            switch (fieldStatus)
            {
               case EMPTY:
               case NORMAL:
               case CLOSED:
                  if (expectedFieldCount == 0)
                  {
                     header.append(fieldValue);
                  }
                  else
                  {
                     record_[header[fieldCount]] = fieldValue;
                  }
                  fieldValue.clear();
                  ++fieldCount;
                  fieldStatus = EMPTY;
                  break;
               case QUOTED:
                  fieldValue += c;
                  break;
            }
         }
         else if (c == '"')
         {
            switch (fieldStatus)
            {
               case EMPTY:
                  fieldStatus = QUOTED;
                  break;
               case NORMAL:
                  fieldValue += c;
                  break;
               case QUOTED:
                  fieldStatus = CLOSED;
                  break;
               case CLOSED:
                  QMessageBox::critical(
                           0, "Error in file",
                           "There was an error in the CSV file.  An extra "
                           "quote was found when a comma was expected.");
                  break;
            }
         }
         else if (c == '\n')
         {
            switch (fieldStatus)
            {
               case EMPTY:
               case NORMAL:
               case CLOSED:
                  if (expectedFieldCount == 0)
                  {
                     header.append(fieldValue);
                  }
                  else
                  {
                     record_[header[fieldCount]] = fieldValue;
                  }
                  fieldValue.clear();
                  ++fieldCount;
                  fieldStatus = EMPTY;
                  if (expectedFieldCount == 0)
                  {
                     expectedFieldCount = fieldCount;
                  }
                  else if (expectedFieldCount == fieldCount)
                  {
                     emit record(record_);
                     record_.clear();
                  }
                  else if (expectedFieldCount != fieldCount)
                  {
                     QMessageBox::critical(
                              0, "Error in file",
                              "There was an error in the CSV file.  The end of "
                              "a line was found when more text was expected.");
                  }
                  fieldCount = 0;
                  break;
               case QUOTED:
                  fieldValue += c;
                  break;
            }
         }
         else if (c >= ' ')
         {
            fieldValue += c;
         }
      }
   }

   f.close();
   emit done();
}
