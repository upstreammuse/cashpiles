#include "ipaccountbalancer.h"

#include <iostream>
#include <sstream>
#include "cashpiles.h"
#include "ledgeraccount.h"
#include "ledgeraccountbalance.h"
#include "ledgerbudget.h"
#include "ledgertransaction.h"
#include "ledgertransactionv2.h"
#include "reportaccount.h"
#include "reporter.h"
#include "texttable.h"

using std::make_shared;
using std::shared_ptr;
using std::string;
using std::stringstream;

IPAccountBalancer::IPAccountBalancer(Reporter& reporter) :
   m_reporter(reporter)
{
}

void IPAccountBalancer::finish()
{
   for (auto& report : m_reports)
   {
      if (report.second)
      {
         m_reporter.appendReport(report.second);
         report.second.reset();
      }
   }
}

Currency IPAccountBalancer::budgetable() const
{
   Currency total;
   for (auto account : m_accounts)
   {
      if (account.second.onBudget)
      {
         total += account.second.balance;
      }
   }
   return total;
}

void IPAccountBalancer::processItem(LedgerAccount const& accountCommand)
{
   auto accountName = accountCommand.name();
   auto& account = m_accounts[accountName];
   auto date = accountCommand.date();
   auto mode = accountCommand.mode();
   auto report = this->report(date, accountName);

   switch (mode)
   {
      case LedgerAccount::Mode::CLOSED:
         if (account.isClosed)
         {
            auto entry = make_shared<ReportAccountEntry>();
            entry->setDate(date);
            entry->setText(
                     "Attempted to close account that was already closed");
            report->appendEntry(entry);
         }
         else if (!account.balance.isZero())
         {
            auto entry = make_shared<ReportAccountEntry>();
            entry->setDate(date);
            entry->setText("Attempted to close account with non-zero balance");
            report->appendEntry(entry);
         }
         else if (account.hasPending)
         {
            auto entry = make_shared<ReportAccountEntry>();
            entry->setDate(date);
            entry->setText(
                     "Attempted to close account with pending transactions");
            report->appendEntry(entry);
         }
         else
         {
            auto entry = make_shared<ReportAccountEntry>();
            entry->setDate(date);
            entry->setText("Closed account");
            report->appendEntry(entry);
            m_reporter.appendReport(report);
            m_reports[accountName].reset();
            account.isClosed = true;
         }
         break;
      case LedgerAccount::Mode::OFF_BUDGET:
      case LedgerAccount::Mode::ON_BUDGET:
         if (!account.isClosed)
         {
            auto entry = make_shared<ReportAccountEntry>();
            entry->setDate(date);
            entry->setText("Attempted to open account that was already open");
            report->appendEntry(entry);
         }
         else
         {
            auto entry = make_shared<ReportAccountEntry>();
            entry->setDate(date);
            entry->setText("Opened account");
            report->appendEntry(entry);
            account.hasPending = false;
            account.isClosed = false;
            account.onBudget = (mode == LedgerAccount::Mode::ON_BUDGET);
         }
         break;
   }
}

void IPAccountBalancer::processItem(LedgerAccountBalance const& balance)
{
   auto accountName = balance.account();
   auto& account = m_accounts[accountName];
   auto amount = balance.amount();
   auto date = balance.date();
   auto report = this->report(date, accountName);

   if (amount != account.balance)
   {
      auto entry = make_shared<ReportAccountEntry>();
      entry->setDate(date);
      stringstream ss;
      ss << "Statement balance does not match calculated balance, difference "
         << (amount - account.balance).toString();
      entry->setText(ss.str());
      report->appendEntry(entry);
   }
   if (account.hasPending)
   {
      auto entry = make_shared<ReportAccountEntry>();
      entry->setDate(date);
      entry->setText("Statement includes pending transactions");
      report->appendEntry(entry);
      account.hasPending = false;
   }
   if (account.isClosed)
   {
      auto entry = make_shared<ReportAccountEntry>();
      entry->setDate(date);
      entry->setText("Statement balance entered for closed account");
      report->appendEntry(entry);
   }

   report->setBalanceEnd(date, amount);
   m_reporter.appendReport(report);
   m_reports[accountName].reset();
}

void IPAccountBalancer::processItem(LedgerTransaction const& transaction)
{
   auto accountName = transaction.account();
   auto& account = m_accounts[accountName];
   auto amount = transaction.amount();
   auto date = transaction.date();
   auto report = this->report(transaction.date(), accountName);

   ensureOpen(date, accountName, false);
   account.balance += amount;
   if (transaction.status() == LedgerTransaction::Status::PENDING)
   {
      account.hasPending = true;
   }

   auto entry = make_shared<ReportAccountEntry>();
   entry->setAmount(amount);
   entry->setCleared(
            transaction.status() == LedgerTransaction::Status::CLEARED);
   entry->setDate(date);
   entry->setText(transaction.payee());
   report->appendEntry(entry);
}

bool IPAccountBalancer::processItem(LedgerTransactionV2 const& transaction)
{
   m_workingDate = transaction.date();
   m_workingPayee = transaction.payee();
   m_workingStatus = transaction.status();
   return true;
}

void IPAccountBalancer::processItem(
      LedgerTransactionV2AccountEntry const& entry)
{
   auto accountName = entry.account();
   auto& account = m_accounts[accountName];
   auto amount = entry.amount().first;
   auto report = this->report(m_workingDate, accountName);

   ensureOpen(m_workingDate, accountName, true);
   account.balance += amount;
   if (m_workingStatus == LedgerTransactionV2::Status::PENDING)
   {
      account.hasPending = true;
   }

   auto entryR = make_shared<ReportAccountEntry>();
   entryR->setAmount(amount);
   entryR->setCleared(m_workingStatus == LedgerTransactionV2::Status::CLEARED);
   entryR->setDate(m_workingDate);
   entryR->setText(m_workingPayee);
   report->appendEntry(entryR);
}

void IPAccountBalancer::processItem(
      LedgerTransactionV2CategoryEntry const& entry)
{
   if (!entry.trackingAccount().second) return;
   auto accountName = entry.trackingAccount().first;
   auto& account = m_accounts[accountName];
   auto amount = entry.amount().first;
   auto report = this->report(m_workingDate, accountName);

   ensureOpen(m_workingDate, accountName, false);
   account.balance -= amount;
   if (m_workingStatus == LedgerTransactionV2::Status::PENDING)
   {
      account.hasPending = true;
   }

   auto entryR = make_shared<ReportAccountEntry>();
   entryR->setAmount(-amount);
   entryR->setCleared(m_workingStatus == LedgerTransactionV2::Status::CLEARED);
   entryR->setDate(m_workingDate);
   entryR->setText(m_workingPayee);
   report->appendEntry(entryR);
}

void IPAccountBalancer::processItem(LedgerTransactionV2OwnerEntry const& entry)
{
   if (!entry.trackingAccount().second) return;
   auto accountName = entry.trackingAccount().first;
   auto& account = m_accounts[accountName];
   auto amount = entry.amount().first;
   auto report = this->report(m_workingDate, accountName);

   ensureOpen(m_workingDate, accountName, false);
   account.balance -= amount;
   if (m_workingStatus == LedgerTransactionV2::Status::PENDING)
   {
      account.hasPending = true;
   }

   auto entryR = make_shared<ReportAccountEntry>();
   entryR->setAmount(-amount);
   entryR->setCleared(m_workingStatus == LedgerTransactionV2::Status::CLEARED);
   entryR->setDate(m_workingDate);
   entryR->setText(m_workingPayee);
   report->appendEntry(entryR);
}

void IPAccountBalancer::ensureOpen(Date const& date, string const& accountName,
                                   bool onBudget)
{
   auto& account = m_accounts[accountName];
   auto report = this->report(date, accountName);
   if (account.isClosed)
   {
      auto entry = make_shared<ReportAccountEntry>();
      entry->setDate(date);
      entry->setText("Automatically opening account");
      report->appendEntry(entry);
      account.isClosed = false;
      account.onBudget = onBudget;
      account.hasPending = false;
   }
}

shared_ptr<ReportAccount> IPAccountBalancer::report(
      Date const& date, string const& account)
{
   if (!m_reports[account])
   {
      m_reports[account] = make_shared<ReportAccount>();
      m_reports[account]->setAccount(account);
      m_reports[account]->setBalanceStart(date, m_accounts[account].balance);
   }
   return m_reports[account];
}
