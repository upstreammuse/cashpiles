#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QLabel>
#include <QMainWindow>

#include "transaction.h"

namespace Ui {
   class MainWindow;
}

class MainWindow : public QMainWindow
{
   Q_OBJECT

public:
   explicit MainWindow(QWidget* parent = nullptr);
   ~MainWindow();

public slots:
   void showTransaction(const Transaction& transaction);

private:
   Ui::MainWindow* ui;
   QMap<QString, QLabel*> m_accounts;
   QMap<QString, QLabel*> m_budgets;
};

#endif
