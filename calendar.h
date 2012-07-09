#ifndef CALENDAR_H
#define CALENDAR_H

#include <QGroupBox>
#include "timeline.h"

class QDate;

class Calendar : public QGroupBox
{
      Q_OBJECT

   public:
      Calendar(Timeline* timeline, QWidget* parent = 0);

   public slots:
      void setDate(QDate date);
};

#endif
