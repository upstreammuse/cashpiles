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
         report.second->setAccount(report.first);
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

void IPAccountBalancer::processItem(LedgerAccount const& account_)
{
   auto name = account_.name();
   auto& account = m_accounts[name];
   auto mode = account_.mode();

   if (!m_reports[name])
   {
      m_reports[name] = make_shared<ReportAccount>();
      m_reports[name]->setBalanceStart(
               account_.date(), m_accounts[name].balance);
   }

   switch (mode)
   {
      case LedgerAccount::Mode::CLOSED:
         if (account.isClosed)
         {
            auto entry = make_shared<ReportAccountEntry>();
            entry->setDate(account_.date());
            entry->setText(
                     "Attempted to close account that was already closed");
            m_reports[name]->appendEntry(entry);
         }
         else if (!account.balance.isZero())
         {
            auto entry = make_shared<ReportAccountEntry>();
            entry->setDate(account_.date());
            entry->setText("Attempted to close account with non-zero balance");
            m_reports[name]->appendEntry(entry);
         }
         else
         {
            auto entry = make_shared<ReportAccountEntry>();
            entry->setDate(account_.date());
            entry->setText("Closed account");
            m_reports[name]->appendEntry(entry);
            m_reports[name]->setAccount(name);
            m_reporter.appendReport(m_reports[name]);
            m_reports[name].reset();
            account.isClosed = true;
         }
         break;
      case LedgerAccount::Mode::OFF_BUDGET:
      case LedgerAccount::Mode::ON_BUDGET:
         if (!account.isClosed)
         {
            auto entry = make_shared<ReportAccountEntry>();
            entry->setDate(account_.date());
            entry->setText("Attempted to open account that was already open");
            m_reports[name]->appendEntry(entry);
         }
         else
         {
            auto entry = make_shared<ReportAccountEntry>();
            entry->setDate(account_.date());
            entry->setText("Opened account");
            m_reports[name]->appendEntry(entry);
            account.hasPending = false;
            account.isClosed = false;
            account.onBudget = (mode == LedgerAccount::Mode::ON_BUDGET);
         }
         break;
   }
}

void IPAccountBalancer::processItem(LedgerAccountBalance const& balance)
{
   if (!m_reports[balance.account()])
   {
      m_reports[balance.account()] = make_shared<ReportAccount>();
      m_reports[balance.account()]->setBalanceStart(
               balance.date(), m_accounts[balance.account()].balance);
   }

   if (balance.amount() != m_accounts[balance.account()].balance)
   {
      auto entry = make_shared<ReportAccountEntry>();
      entry->setDate(balance.date());
      entry->setText("Statement balance does not match calculated balance");
      m_reports[balance.account()]->appendEntry(entry);
   }
   if (m_accounts[balance.account()].hasPending)
   {
      auto entry = make_shared<ReportAccountEntry>();
      entry->setDate(balance.date());
      entry->setText("Statement includes pending transactions");
      m_reports[balance.account()]->appendEntry(entry);
      m_accounts[balance.account()].hasPending = false;
   }
   if (m_accounts[balance.account()].isClosed)
   {
      auto entry = make_shared<ReportAccountEntry>();
      entry->setDate(balance.date());
      entry->setText("Statement balance entered for closed account");
      m_reports[balance.account()]->appendEntry(entry);
   }

   m_reports[balance.account()]->setAccount(balance.account());
   m_reports[balance.account()]->setBalanceEnd(
            balance.date(), balance.amount());
   m_reporter.appendReport(m_reports[balance.account()]);
   m_reports[balance.account()].reset();
}

void IPAccountBalancer::processItem(LedgerTransaction const& transaction)
{
   m_accounts[transaction.account()].balance += transaction.amount();
   if (transaction.status() == LedgerTransaction::Status::PENDING)
   {
      m_accounts[transaction.account()].hasPending = true;
   }

   if (!m_reports[transaction.account()])
   {
      m_reports[transaction.account()] = make_shared<ReportAccount>();
      m_reports[transaction.account()]->setBalanceStart(
               transaction.date(), m_accounts[transaction.account()].balance);
   }

   auto entry = make_shared<ReportAccountEntry>();
   entry->setCleared(
            transaction.status() == LedgerTransaction::Status::CLEARED);
   entry->setDate(transaction.date());
   entry->setText(transaction.payee());
   m_reports[transaction.account()]->appendEntry(entry);
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
   auto& account = m_accounts[entry.account()];
   auto amount = entry.amount().first;

   account.balance += amount;
   if (m_workingStatus == LedgerTransactionV2::Status::PENDING)
   {
      account.hasPending = true;
   }

   if (!m_reports[entry.account()])
   {
      m_reports[entry.account()] = make_shared<ReportAccount>();
      m_reports[entry.account()]->setBalanceStart(
               m_workingDate, m_accounts[entry.account()].balance);
   }

   auto entry_ = make_shared<ReportAccountEntry>();
   entry_->setAmount(amount);
   entry_->setCleared(m_workingStatus == LedgerTransactionV2::Status::CLEARED);
   entry_->setDate(m_workingDate);
   entry_->setText(m_workingPayee);
   m_reports[entry.account()]->appendEntry(entry_);
}

void IPAccountBalancer::processItem(
      LedgerTransactionV2CategoryEntry const& entry)
{
   if (!entry.trackingAccount().second) return;
   auto& account = m_accounts[entry.trackingAccount().first];
   auto amount = entry.amount().first;

   account.balance -= amount;
   if (m_workingStatus == LedgerTransactionV2::Status::PENDING)
   {
      account.hasPending = true;
   }

   if (!m_reports[entry.trackingAccount().first])
   {
      m_reports[entry.trackingAccount().first] = make_shared<ReportAccount>();
      m_reports[entry.trackingAccount().first]->setBalanceStart(
               m_workingDate,
               m_accounts[entry.trackingAccount().first].balance);
   }

   auto entry_ = make_shared<ReportAccountEntry>();
   entry_->setAmount(-amount);
   entry_->setCleared(m_workingStatus == LedgerTransactionV2::Status::CLEARED);
   entry_->setDate(m_workingDate);
   entry_->setText(m_workingPayee);
   m_reports[entry.trackingAccount().first]->appendEntry(entry_);
}

void IPAccountBalancer::processItem(LedgerTransactionV2OwnerEntry const& entry)
{
   if (!entry.trackingAccount().second) return;
   auto& account = m_accounts[entry.trackingAccount().first];
   auto amount = entry.amount().first;

   account.balance -= amount;
   if (m_workingStatus == LedgerTransactionV2::Status::PENDING)
   {
      account.hasPending = true;
   }

   // TODO this needs to be refactored as a member method
   if (!m_reports[entry.trackingAccount().first])
   {
      m_reports[entry.trackingAccount().first] = make_shared<ReportAccount>();
      m_reports[entry.trackingAccount().first]->setBalanceStart(
               m_workingDate,
               m_accounts[entry.trackingAccount().first].balance);
   }

   auto entry_ = make_shared<ReportAccountEntry>();
   entry_->setAmount(-amount);
   entry_->setCleared(m_workingStatus == LedgerTransactionV2::Status::CLEARED);
   entry_->setDate(m_workingDate);
   entry_->setText(m_workingPayee);
   m_reports[entry.trackingAccount().first]->appendEntry(entry_);
}
