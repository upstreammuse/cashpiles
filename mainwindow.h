#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QLocale>
#include <QMainWindow>
#include <QMap>
#include <QSettings>

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
   void showBudget(Transaction const& transaction);
   void showTransaction(Transaction const& transaction);

private:
   Ui::MainWindow* ui;
   QMap<QString, QListWidgetItem*> m_accounts;
   int m_budgetable;
   QMap<QString, QListWidgetItem*> m_budgets;
   QLocale m_locale;
   QSettings m_settings;
   int m_worth;
};

#endif
