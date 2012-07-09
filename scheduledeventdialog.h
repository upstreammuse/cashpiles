#ifndef SCHEDULEDEVENTDIALOG_H
#define SCHEDULEDEVENTDIALOG_H

#include <QDialog>

namespace Ui {
   class ScheduledEventDialog;
}

class ScheduledEventDialog : public QDialog
{
      Q_OBJECT

   public:
      explicit ScheduledEventDialog(QWidget *parent = 0);
      ~ScheduledEventDialog();

   private:
      Ui::ScheduledEventDialog *ui;
};

#endif
