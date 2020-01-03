#include "texttable.h"

#include <QTextStream>

void TextTable::appendColumn(int index, QString const& item)
{
   if (index < m_columns.size())
   {
      ColumnData& data = m_columns[index];
      data.content.append(item);
      data.width = std::max(item.size(), m_columns[index].width);
   }
   else
   {
      setColumn(index, QStringList());
      appendColumn(index, item);
   }
}

void TextTable::clear()
{
   m_columns.clear();
}

void TextTable::print(QTextStream& out) const
{
   int height = 0;
   foreach (ColumnData const& data, m_columns)
   {
      height = std::max(data.content.size(), height);
   }

   for (int i = 0; i < height; ++i)
   {
      foreach (ColumnData const& data, m_columns)
      {
         if (i < data.content.size())
         {
            QString const& item = data.content[i];
            switch (data.align)
            {
               case Alignment::LeftAlign:
                  out << item.leftJustified(data.width);
                  break;
               case Alignment::RightAlign:
                  out << item.rightJustified(data.width);
                  break;
            }
         }
         else
         {
            out << QString("").leftJustified(data.width);
         }
      }
      out << endl;
   }
}

void TextTable::setColumn(int index, QStringList const& content,
                          Alignment align)
{
   ColumnData data;
   data.content = content;
   data.align = align;
   data.width = 0;
   foreach (QString const& s, content)
   {
      data.width = std::max(s.size(), data.width);
   }
   m_columns.insert(index, data);
}

void TextTable::setColumnAlignment(int index, Alignment align)
{
   if (index < m_columns.size())
   {
      m_columns[index].align = align;
   }
   else
   {
      setColumn(index, QStringList(), align);
   }
}
