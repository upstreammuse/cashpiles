#include "itemrenderer.h"

ItemRenderer::ItemRenderer(QObject* parent) :
   ItemProcessor(parent)
{
}

void ItemRenderer::processItem(LedgerAccountBalance const& balance)
{
}

void ItemRenderer::processItem(LedgerAccountCommand const& account)
{
}

void ItemRenderer::processItem(LedgerBudget const& budget)
{
}

void ItemRenderer::processItem(LedgerComment const& comment)
{
}

void ItemRenderer::processItem(LedgerTransaction const& transaction_)
{
   emit transaction(transaction_);
}

void ItemRenderer::stop()
{
}
