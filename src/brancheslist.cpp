#include "brancheslist.h"
#include "mainimpl.h"
BranchesList::BranchesList(QWidget *parent) :
    QListWidget(parent)
{
    QObject::connect(this, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(itemDoubleClicked(QListWidgetItem*)));
}

void BranchesList::setup(Domain *domain, Git *git)
{
    g = git;
    d = domain;
}

void BranchesList::update() {
    this->clear();

    addList("Branches", Git::BRANCH);
    addList("Remotes", Git::RMT_BRANCH);
    addList("Tags", Git::TAG);
}

void BranchesList::addList(QString header, Git::RefType type)
{
    QStringList list = g->getAllRefNames(type, !Git::optOnlyLoaded);
    QListWidgetItem* li;
    if (list.isEmpty()) {
        return;
    }

    if (this->count() > 0) {
        li = new QListWidgetItem();
        this->addItem(li);
    }

    li = new QListWidgetItem();
    li->setText(header);
    QFont font = li->font();
    font.setBold(true);
    li->setFont(font);

    QColor white(255, 255, 255);
    QColor black(128, 128, 128);
    QLinearGradient localGradient(0, 0, 0, 20);
    localGradient.setColorAt(0, white);
    localGradient.setColorAt(1, black);
    li->setBackground(localGradient);
    this->addItem(li);

    list.sort();
    FOREACH_SL (it, list) {
        li = new BranchListItem(QString(*it));
        this->addItem(li);
    }
}

void BranchesList::itemDoubleClicked(QListWidgetItem *item) {
    if (!item->data(Qt::UserRole).isNull()) {
        BranchListItem* branchLI = (BranchListItem*)item;
        d->m()->changeBranch(branchLI->getBranchName());
    }
}


BranchListItem::BranchListItem(QString branchName) : QListWidgetItem(branchName) {
    this->branchName = branchName;
    this->setData(Qt::UserRole, branchName);
}

