#ifndef DATESEQUENCER_H
#define DATESEQUENCER_H

#include <QDate>

class DateSequencer
{
   public:
      enum Frequency
      {
         NONE,
         DAILY,
         WEEKDAYS,
         WEEKENDS,
         WEEKLY,
         MONTHLY,
         ANNUALLY
      };
      DateSequencer(QDate startDate, QDate endDate, Frequency frequency,
                    int increment);
      void setEndDate(QDate date);
      void setFrequency(int frequency);
      void setIncrement(int increment);
      void setStartDate(QDate date);

   protected:
      const QList<QDate>& dates() const { return dates_; }

   private:
      QDate increment(QDate date);
      void recalculate();

      QList<QDate> dates_;
      QDate endDate_;
      Frequency frequency_;
      int increment_;
      QDate startDate_;
};

#endif
