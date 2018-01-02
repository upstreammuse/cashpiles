#include <QApplication>

#include "mainwindow.h"
#include "ynabcsvmodel.h"
#include "ynabcsvreader.h"

int main(int argc, char** argv)
{
   QApplication app(argc, argv);
   MainWindow mw;
   mw.show();

   YnabCsvReader reader("register.csv");
   YnabCsvModel model;
   QObject::connect(&reader, SIGNAL(done()), &model, SLOT(showTrans()));
   QObject::connect(&reader, SIGNAL(field(QString)),
                    &model, SLOT(appendField(QString)));
   QObject::connect(&reader, SIGNAL(fieldCount(size_t)),
                    &model, SLOT(setFieldCount(size_t)));
   QObject::connect(&model, SIGNAL(transaction(Transaction)),
                    &mw, SLOT(showTransaction(Transaction)));
   reader.read();

   return app.exec();
}
