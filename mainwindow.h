#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>

class Transaction;
class QListWidgetItem;

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
   void showTransaction(Transaction const& transaction);

private:
   Ui::MainWindow* ui;
   QMap<QString, QListWidgetItem*> m_accounts;
   QMap<QString, QListWidgetItem*> m_budgets;
};

#endif
