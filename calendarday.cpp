#include "calendarday.h"

#include <QMessageBox>
#include <QMouseEvent>
#include <QPushButton>
#include <QGridLayout>
#include <QLabel>

// public

CalendarDay::CalendarDay(Timeline* timeline, QWidget* parent) :
   QGroupBox(parent),
   timeline_(timeline)
{
   setLayout(new QGridLayout(this));
}

// public slots

void CalendarDay::setDate(QDate date)
{
   setTitle(date.toString("d"));
   QGridLayout* layout(dynamic_cast<QGridLayout*>(this->layout()));

   for (int row(0); row < layout->rowCount(); ++row)
   {
      for (int col(0); col < layout->columnCount(); ++col)
      {
         QLayoutItem* item = layout->itemAtPosition(row, col);
         layout->removeItem(item);
         delete item;
      }
   }
   nextRow_ = 0;

   if (timeline_->find(date) != timeline_->end())
   {
      addScheduledTransaction(timeline_->find(date).value());
   }
}

// private

void CalendarDay::addScheduledTransaction(QString name)
{
   QGridLayout* layout(dynamic_cast<QGridLayout*>(this->layout()));
   layout->addWidget(new QLabel(name, this), nextRow_, 0);
   layout->addWidget(new QPushButton("Pay", this), nextRow_, 1);
   layout->addWidget(new QPushButton("Skip", this), nextRow_, 2);
   ++nextRow_;
}

void CalendarDay::mouseDoubleClickEvent(QMouseEvent* event)
{
   QMessageBox msg;
   msg.setText("New event requested?");
   msg.exec();
   event->accept();
}
