#ifndef TEXTTABLE_H
#define TEXTTABLE_H

#include <QStringList>

class QTextStream;

class TextTable
{
public:
   enum class Alignment
   {
      LeftAlign,
      RightAlign
   };

public:
   void appendColumn(int index, QString const& item);
   void clear();
   void print(QTextStream& out) const;
   void setColumn(int index, QStringList const& content,
                  Alignment align = Alignment::LeftAlign);
   void setColumnAlignment(int index, Alignment align);

private:
   struct ColumnData
   {
      QStringList content;
      Alignment align;
      int width;
   };

private:
   QList<ColumnData> m_columns;
};

#endif
