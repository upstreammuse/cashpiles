#include "datesequencer.h"

// public

DateSequencer::DateSequencer(QDate startDate, QDate endDate,
                             Frequency frequency, int increment) :
   endDate_(endDate),
   frequency_(frequency),
   increment_(increment),
   startDate_(startDate)
{
   recalculate();
}

void DateSequencer::setEndDate(QDate date)
{
   endDate_ = date;
   recalculate();
}

void DateSequencer::setFrequency(int frequency)
{
   frequency_ = Frequency(frequency);
   recalculate();
}

void DateSequencer::setIncrement(int increment)
{
   increment_ = increment;
   recalculate();
}

void DateSequencer::setStartDate(QDate date)
{
   startDate_ = date;
   recalculate();
}

// private

QDate DateSequencer::increment(QDate date)
{
   bool lastDayOfMonth(date.day() == date.daysInMonth());
   switch (frequency_)
   {
      case NONE:
         break;
      case DAILY:
         date = date.addDays(increment_);
         break;
      case WEEKDAYS:
         for (date = date.addDays(1); date.dayOfWeek() > 5;
              date = date.addDays(1)) ;
         break;
      case WEEKENDS:
         for (date = date.addDays(1); date.dayOfWeek() < 6;
              date = date.addDays(1)) ;
         break;
      case WEEKLY:
         date = date.addDays(increment_ * 7);
         break;
      case MONTHLY:
         date = date.addMonths(increment_);
         if (lastDayOfMonth)
         {
            date = date.addDays(date.daysInMonth() - date.day());
         }
         break;
      case ANNUALLY:
         date = date.addYears(increment_);
         break;
   }

   return date;
}

void DateSequencer::recalculate()
{
   dates_.clear();
   if (frequency_ == NONE)
   {
      dates_ += startDate_;
   }
   else
   {
      for (QDate date(startDate_); date <= endDate_; date = increment(date))
      {
         dates_ += date;
      }
   }
}
