#ifndef CALENDARDAY_H
#define CALENDARDAY_H

#include <QGroupBox>
#include "timeline.h"

class QDate;

class CalendarDay : public QGroupBox
{
   public:
      CalendarDay(Timeline* timeline, QWidget* parent = 0);

   public slots:
      void setDate(QDate date);

   private:
      void addScheduledTransaction(QString name);
      void mouseDoubleClickEvent(QMouseEvent* event);

      int nextRow_;
      Timeline* timeline_;
};

#endif
