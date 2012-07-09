#include "scheduledeventdialog.h"
#include "ui_scheduledeventdialog.h"

ScheduledEventDialog::ScheduledEventDialog(QWidget *parent) :
   QDialog(parent),
   ui(new Ui::ScheduledEventDialog)
{
   ui->setupUi(this);
}

ScheduledEventDialog::~ScheduledEventDialog()
{
   delete ui;
}
