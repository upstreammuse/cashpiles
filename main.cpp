#include <QApplication>
#include "calendar.h"
#include "calendarday.h"
#include "mainwindow.h"
#include "scheduledeventdialog.h"
#include "timeline.h"

int main(int argc, char* argv[])
{
   QApplication app(argc, argv);

   Timeline timeline;
   timeline.insert(QDate::currentDate(), "Test Event");

   MainWindow mw(&timeline);
   mw.show();
   return app.exec();
}
