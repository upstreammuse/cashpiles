#include <QApplication>
#include <QTimer>
#include "csvreader.h"
#include "mainwindow.h"
#include "ynabregister.h"

int main(int argc, char** argv)
{
   QApplication app(argc, argv);
   MainWindow mw;
   mw.show();

   CsvReader reader("register.csv");
   YnabRegister reg;
   QObject::connect(&reader, SIGNAL(done()), &reg, SLOT(showTrans()));
   QObject::connect(&reader, SIGNAL(record(QHash<QString,QString>)),
                    &reg, SLOT(appendRecord(QHash<QString,QString>)));
   QObject::connect(&reg, SIGNAL(transaction(Transaction)),
                    &mw, SLOT(showTransaction(Transaction)));

   QTimer::singleShot(0, &reader, SLOT(read()));
   return app.exec();
}
