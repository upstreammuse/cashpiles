#include <QApplication>
#include <QTimer>
#include "csvreader.h"
#include "mainwindow.h"
#include "ynabcsvmodel.h"

int main(int argc, char** argv)
{
   QApplication app(argc, argv);
   MainWindow mw;
   mw.show();

   CsvReader reader("register.csv");
   YnabCsvModel model;
   QObject::connect(&reader, SIGNAL(done()), &model, SLOT(showTrans()));
   QObject::connect(&reader, SIGNAL(record(QHash<QString,QString>)),
                    &model, SLOT(appendRecord(QHash<QString,QString>)));
   QObject::connect(&model, SIGNAL(transaction(Transaction)),
                    &mw, SLOT(showTransaction(Transaction)));

   QTimer::singleShot(0, &reader, SLOT(read()));
   return app.exec();
}
