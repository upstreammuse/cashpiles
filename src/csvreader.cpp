#include "csvreader.h"

#include <QFile>
#include <QHash>
#include <QMessageBox>

CsvReader::CsvReader(QObject* parent) :
   QObject(parent)
{
}

void CsvReader::readAll(QString const& filename)
{
   QFile f(filename);
   if (!f.open(QIODevice::ReadOnly))
   {
      QMessageBox::critical(
               0, "File not found",
               QString("Could not open the specified CSV file\n(%1)").arg(
                  filename));
      return;
   }

   int expectedFieldCount = 0;
   int fieldCount = 0;
   enum
   {
      EMPTY, NORMAL, NORMAL_ESCAPED, QUOTED, QUOTED_ESCAPED, CLOSED
   } fieldStatus = EMPTY;
   QString fieldValue;
   QStringList header;
   QByteArray line;
   QHash<QString, QString> record_;

   while (!(line = f.readLine()).isEmpty())
   {
      foreach (char const c, line)
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
               case NORMAL_ESCAPED:
                  fieldValue += c;
                  fieldStatus = NORMAL;
                  break;
               case QUOTED:
                  fieldValue += c;
                  break;
               case QUOTED_ESCAPED:
                  fieldValue += c;
                  fieldStatus = QUOTED;
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
               case NORMAL_ESCAPED:
                  fieldValue += c;
                  fieldStatus = NORMAL;
                  break;
               case QUOTED:
                  fieldStatus = CLOSED;
                  break;
               case QUOTED_ESCAPED:
                  fieldValue += c;
                  fieldStatus = QUOTED;
                  break;
               case CLOSED:
                  QMessageBox::critical(
                           0, "Error in file",
                           "There was an error in the CSV file.  An extra "
                           "quote was found when a comma was expected.");
                  return;
            }
         }
         else if (c == '\\')
         {
            switch (fieldStatus)
            {
               case EMPTY:
                  fieldStatus = NORMAL_ESCAPED;
                  break;
               case NORMAL:
                  fieldStatus = NORMAL_ESCAPED;
                  break;
               case NORMAL_ESCAPED:
                  fieldValue += c;
                  fieldStatus = NORMAL;
                  break;
               case QUOTED:
                  fieldStatus = QUOTED_ESCAPED;
                  break;
               case QUOTED_ESCAPED:
                  fieldValue += c;
                  fieldStatus = QUOTED;
                  break;
               case CLOSED:
                  QMessageBox::critical(
                           0, "Error in file",
                           "There was an error in the CSV file.  An extra "
                           "backslash was found when a comma was expected.");
                  return;
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
                     return;
                  }
                  fieldCount = 0;
                  break;
               case NORMAL_ESCAPED:
                  fieldValue += c;
                  fieldStatus = NORMAL;
                  break;
               case QUOTED:
                  fieldValue += c;
                  break;
               case QUOTED_ESCAPED:
                  fieldValue += c;
                  fieldStatus = QUOTED;
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
   emit finished();
}
