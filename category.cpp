#include "category.h"

Category::Category() :
   m_balance(0)
{
}

void Category::appendSplit(TransactionSplit const& split)
{
   // TODO better to have an inheritance tree of transaction split types that
   // can handle transfers and spending with the correct methods for each, etc.
   if (split.category() == "")
   {
      return;
   }

   m_balance += split.amount();
   verifyName(split.category());
   verifyBalance();
}

void Category::verifyBalance()
{
   if (m_balance < 0)
   {
      qWarning("Balance went negative for category %s", m_name.toStdString().c_str());
   }
}

void Category::verifyName(QString const& name)
{
   if (m_name.isNull())
   {
      m_name = name;
   }
   Q_ASSERT(m_name == name);
}
