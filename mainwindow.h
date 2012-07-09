#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "timeline.h"

namespace Ui
{
   class MainWindow;
}

class MainWindow : public QMainWindow
{
      Q_OBJECT

   public:
      explicit MainWindow(Timeline* timeline, QWidget* parent = 0);
      ~MainWindow();

   signals:
      void date(QDate date);

   private slots:
      void readMonth();
      void setYear(int year);

   private:
      void setMonth(int month);

      QDate date_;
      Ui::MainWindow* ui;
};

#endif
