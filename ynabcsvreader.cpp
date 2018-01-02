#include "ynabcsvreader.h"

#include <QMessageBox>
#include <QFile>

YnabCsvReader::YnabCsvReader(const QString& filename, QObject* parent) :
   QObject(parent),
   m_filename(filename)
{
}

void YnabCsvReader::read()
{
   QFile f(m_filename);
   if (!f.open(QIODevice::ReadOnly))
   {
      QMessageBox::critical(
               0, "File not found",
               QString("Could not open the specified CSV file\n(%1)")
               .arg(m_filename));
   }

   size_t expectedFieldCount=0;
   size_t workingFieldCount = 0;
   enum {EMPTY, NORMAL, QUOTED, CLOSED} fieldStatus = EMPTY;
   QString fieldValue;
   QByteArray line;

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
                  emit field(fieldValue);
                  fieldValue.clear();
                  ++workingFieldCount;
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
                  // violating RFC, but can handle it
                  fieldValue += c;
                  break;
               case QUOTED:
                  fieldStatus = CLOSED;
                  break;
               case CLOSED:
                  QMessageBox::critical(0, "Error in file",
                                        "There was an error in the CSV file.  "
                                        "An extra quote was found when a comma "
                                        "was expected.");
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
                  emit field(fieldValue);
                  fieldValue.clear();
                  ++workingFieldCount;
                  fieldStatus = EMPTY;
                  if (expectedFieldCount == 0)
                  {
                     expectedFieldCount = workingFieldCount;
                     emit fieldCount(workingFieldCount);
                  }
                  else if (expectedFieldCount != workingFieldCount)
                  {
                     QMessageBox::critical(0, "Error in file",
                                           "There was an error in the CSV "
                                           "file.  The end of a line was found "
                                           "when more text was expected.");
                  }
                  workingFieldCount = 0;
                  break;
               case QUOTED:
                  fieldValue += c;
                  break;
            }
         }
         else if (c < ' ')
         {
            // eat this error silently
         }
         else
         {
            fieldValue += c;
         }
      }
   }

   f.close();
   emit done();
}
