#include "mainwindow.h"

#include <QDate>
#include <QLayout>
#include "calendar.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(Timeline* timeline, QWidget* parent) :
   QMainWindow(parent),
   date_(QDate::currentDate()),
   ui(new Ui::MainWindow)
{
   ui->setupUi(this);
   ui->radioButton->setText(QDate::longMonthName(1, QDate::StandaloneFormat));
   ui->radioButton_2->setText(QDate::longMonthName(2, QDate::StandaloneFormat));
   ui->radioButton_3->setText(QDate::longMonthName(3, QDate::StandaloneFormat));
   ui->radioButton_4->setText(QDate::longMonthName(4, QDate::StandaloneFormat));
   ui->radioButton_5->setText(QDate::longMonthName(5, QDate::StandaloneFormat));
   ui->radioButton_6->setText(QDate::longMonthName(6, QDate::StandaloneFormat));
   ui->radioButton_7->setText(QDate::longMonthName(7, QDate::StandaloneFormat));
   ui->radioButton_8->setText(QDate::longMonthName(8, QDate::StandaloneFormat));
   ui->radioButton_9->setText(QDate::longMonthName(9, QDate::StandaloneFormat));
   ui->radioButton_10->setText(
            QDate::longMonthName(10, QDate::StandaloneFormat));
   ui->radioButton_11->setText(
            QDate::longMonthName(11, QDate::StandaloneFormat));
   ui->radioButton_12->setText(
            QDate::longMonthName(12, QDate::StandaloneFormat));

   QVBoxLayout* layout = dynamic_cast<QVBoxLayout*>(ui->tab->layout());

   Calendar* calendar(new Calendar(timeline, this));
   layout->insertWidget(0, calendar);

   connect(ui->radioButton, SIGNAL(clicked()), this, SLOT(readMonth()));
   connect(ui->radioButton_2, SIGNAL(clicked()), this, SLOT(readMonth()));
   connect(ui->radioButton_3, SIGNAL(clicked()), this, SLOT(readMonth()));
   connect(ui->radioButton_4, SIGNAL(clicked()), this, SLOT(readMonth()));
   connect(ui->radioButton_5, SIGNAL(clicked()), this, SLOT(readMonth()));
   connect(ui->radioButton_6, SIGNAL(clicked()), this, SLOT(readMonth()));
   connect(ui->radioButton_7, SIGNAL(clicked()), this, SLOT(readMonth()));
   connect(ui->radioButton_8, SIGNAL(clicked()), this, SLOT(readMonth()));
   connect(ui->radioButton_9, SIGNAL(clicked()), this, SLOT(readMonth()));
   connect(ui->radioButton_10, SIGNAL(clicked()), this, SLOT(readMonth()));
   connect(ui->radioButton_11, SIGNAL(clicked()), this, SLOT(readMonth()));
   connect(ui->radioButton_12, SIGNAL(clicked()), this, SLOT(readMonth()));
   connect(ui->spinBox, SIGNAL(valueChanged(int)), this, SLOT(setYear(int)));

   connect(this, SIGNAL(date(QDate)), calendar, SLOT(setDate(QDate)));

   emit date(date_);
}

MainWindow::~MainWindow()
{
   delete ui;
}

// private slots

void MainWindow::readMonth()
{
   if (ui->radioButton->isChecked())
   {
      setMonth(1);
   }
   else if (ui->radioButton_2->isChecked())
   {
      setMonth(2);
   }
   else if (ui->radioButton_3->isChecked())
   {
      setMonth(3);
   }
   else if (ui->radioButton_4->isChecked())
   {
      setMonth(4);
   }
   else if (ui->radioButton_5->isChecked())
   {
      setMonth(5);
   }
   else if (ui->radioButton_6->isChecked())
   {
      setMonth(6);
   }
   else if (ui->radioButton_7->isChecked())
   {
      setMonth(7);
   }
   else if (ui->radioButton_8->isChecked())
   {
      setMonth(8);
   }
   else if (ui->radioButton_9->isChecked())
   {
      setMonth(9);
   }
   else if (ui->radioButton_10->isChecked())
   {
      setMonth(10);
   }
   else if (ui->radioButton_11->isChecked())
   {
      setMonth(11);
   }
   else if (ui->radioButton_12->isChecked())
   {
      setMonth(12);
   }
}

void MainWindow::setYear(int year)
{
   date_.setDate(year, date_.month(), 1);
   emit date(date_);
}

// private

void MainWindow::setMonth(int month)
{
   date_.setDate(date_.year(), month, 1);
   emit date(date_);
}
