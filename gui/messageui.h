#ifndef MESSAGEUI_H
#define MESSAGEUI_H

#include <QWidget>

class LedgerItem;
namespace Ui { class MessageUI; }

class MessageUI : public QWidget
{
   Q_OBJECT

public:
   explicit MessageUI(QWidget* parent = 0);
   ~MessageUI();

public slots:
   void appendMessage(QString const& msg);
   void appendMessage(LedgerItem const& item, QString const& msg);

private:
   Ui::MessageUI* ui;
};

#endif
