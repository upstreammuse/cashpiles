#include "calendar.h"

#include <QGridLayout>
#include "calendarday.h"

// public

Calendar::Calendar(Timeline* timeline, QWidget* parent) :
   QGroupBox(parent)
{
   QGridLayout* layout(new QGridLayout(this));
   setLayout(layout);
   for (int row(0); row < 6; ++row)
   {
      for (int col(0); col < 7; ++col)
      {
         layout->addWidget(new CalendarDay(timeline, this), row, col);
         layout->itemAtPosition(row, col)->widget()->hide();
      }
   }
}

// public slots

void Calendar::setDate(QDate date)
{
   QGridLayout* layout(dynamic_cast<QGridLayout*>(this->layout()));
   for (int row(0); row < 6; ++row)
   {
      for (int col(0); col < 7; ++col)
      {
         layout->itemAtPosition(row, col)->widget()->hide();
      }
   }

   setTitle(date.toString("MMMM"));

   date.setDate(date.year(), date.month(), 1);
   for (int col(date.dayOfWeek() % 7), i(1), row(0); i <= date.daysInMonth();
        ++i)
   {
      date.setDate(date.year(), date.month(), i);
      CalendarDay* day(dynamic_cast<CalendarDay*>(
                          layout->itemAtPosition(row, col)->widget()));
      day->setDate(date);
      day->show();

      ++col;
      if (col == 7)
      {
         ++row;
         col = 0;
      }
   }
}
