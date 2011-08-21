#include <QDebug>
#include <QKeyEvent>
#include "referencetreewidget.h"
#include "mainimpl.h"

ReferenceTreeWidget::ReferenceTreeWidget(QWidget *parent) : QTreeWidget(parent),
    branchIcon(QString::fromUtf8(":/icons/resources/branch.png")),
    masterBranchIcon(QString::fromUtf8(":/icons/resources/branch_master.png")),
    tagIcon(QString::fromUtf8(":/icons/resources/tag.png"))
{
    setContextMenuPolicy(Qt::CustomContextMenu);

    QObject::connect(this, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),
                     this, SLOT(changeReference(QTreeWidgetItem*, int)));

    QObject::connect(this, SIGNAL(customContextMenuRequested(QPoint)),
                     this, SLOT(contextMenu(QPoint)));

    collapseAllAction = new QAction(tr("Collapse all"), this);
    QObject::connect(collapseAllAction, SIGNAL(triggered()),
                     this, SLOT(collapseAll()));

    expandAllAction = new QAction(tr("Expand all"), this);
    QObject::connect(expandAllAction, SIGNAL(triggered()),
                     this, SLOT(expandAll()));

    checkoutAction = new QAction(tr("Checkout"), this);
    QObject::connect(checkoutAction, SIGNAL(triggered()),
                     this, SLOT(checkout()));

    removeTagAction = new QAction(tr("Remove"), this);
    QObject::connect(removeTagAction, SIGNAL(triggered()),
                     this, SLOT(removeTag()));

    this->setRootIsDecorated(false);
    this->setIndentation(10);

    QPalette p = this->palette();
    p.setColor(QPalette::Base, p.color(QPalette::Window));
    this->setPalette(p);
}

void ReferenceTreeWidget::setup(Domain *domain, Git *git)
{
    m_domain = domain;
    m_git = git;
}

void ReferenceTreeWidget::update()
{
    clear();

    addNode(HeaderBranches, Reference::BRANCH);
    addNode(HeaderRemotes, Reference::REMOTE_BRANCH);
    addNode(HeaderTags, Reference::TAG);

    expandAll();
}

void ReferenceTreeWidget::addNode(ItemType headerType, Reference::Type type)
{
    QTreeWidgetItem *headerNode = new QTreeWidgetItem(this, headerType);

    switch (headerType) {
    case (HeaderBranches):
        headerNode->setData(0, Qt::DisplayRole, "[Branches]");
        headerNode->setData(0, Qt::UserRole, "Branches");
        break;
    case (HeaderRemotes):
        headerNode->setData(0, Qt::DisplayRole, "[Remotes]");
        headerNode->setData(0, Qt::UserRole, "Remotes");
    case (HeaderTags):
        headerNode->setData(0, Qt::DisplayRole, "[Tags]");
        headerNode->setData(0, Qt::UserRole, "Tags");
        break;
    default:
        break;
    }

    QFont font = headerNode->font(0);
    font.setBold(true);
    headerNode->setFont(0, font);

    addTopLevelItem(headerNode);

    QStringList referencesTypeList = m_git->getAllRefNames(type, !Git::optOnlyLoaded);
    referencesTypeList.sort();

    QTreeWidgetItem *tempItemList;

    QString lastRemoteName;
    QString remoteName;
    QTreeWidgetItem* parentNode = headerNode;
    QString text;
    int i;

    QString branchName;

    FOREACH_SL (it, referencesTypeList) {
        bool isCurrent = (m_git->currentBranch().compare(*it) == 0);

        switch (headerType) {
        case (HeaderBranches):
            tempItemList = new QTreeWidgetItem(headerNode, LeafBranch);
            tempItemList->setData(0, Qt::DisplayRole, QString(*it));

            if (isCurrent) {
                QFont font = tempItemList->font(0);
                font.setBold(true);
                tempItemList->setFont(0, font);
                tempItemList->setForeground(0, Qt::red);
            }

            tempItemList->setIcon(0, branchIcon);

            if (*it == "master") {
                tempItemList->setIcon(0, masterBranchIcon);
            }

            break;
        case (HeaderRemotes):
            branchName = QString(*it);
            i = branchName.indexOf("/");
            if (i > 0) {
                remoteName = branchName.left(i);
                text = branchName.mid(i + 1);
                if (remoteName.compare(lastRemoteName) != 0) {
                    parentNode = new QTreeWidgetItem(headerNode, HeaderRemote);
                    parentNode->setData(0, Qt::DisplayRole, remoteName);
                    parentNode->setData(0, Qt::UserRole, remoteName);
                    lastRemoteName = remoteName;
                }
            } else {
                parentNode = headerNode;
                text = branchName;
                lastRemoteName = "";
            }

            tempItemList = new QTreeWidgetItem(parentNode, LeafRemote);
            tempItemList->setData(0, Qt::DisplayRole, text);
            tempItemList->setData(0, Qt::UserRole, branchName);
            tempItemList->setIcon(0, QIcon(QString::fromUtf8(":/icons/resources/branch.png")));

            break;
        case (HeaderTags):
            tempItemList = new QTreeWidgetItem(headerNode, LeafTag);
            tempItemList->setData(0, Qt::DisplayRole, QString(*it));
            tempItemList->setIcon(0, tagIcon);
            break;
        default:
            break;
        }

        tempItemList->setData(0, Qt::UserRole, QString(*it));
    }
}

void ReferenceTreeWidget::setAllItemsShown()
{
    for (int i = 0; i < topLevelItemCount(); i++) {
        setShownItem(topLevelItem(i));
    }
}

void ReferenceTreeWidget::setShownItem(QTreeWidgetItem *item)
{
    item->setHidden(false);

    if (item->childCount() > 0) {
        for (int i = 0; i < item->childCount(); i++) {
            setShownItem(item->child(i));
        }
    }
}

void ReferenceTreeWidget::changeReference(QTreeWidgetItem *item, int column)
{
    if ((item->type() != LeafBranch)
            && (item->type() != LeafRemote)
            && (item->type() != LeafTag)) {
        return;
    }

    bool* stateTree = new bool[topLevelItemCount()];
    const QString& branch = item->data(0, Qt::UserRole).toString();

    for (int i = 0; i < topLevelItemCount(); i++) {
        stateTree[i] = this->topLevelItem(i)->isExpanded();
    }

    // rebuild tree
    m_domain->m()->changeBranch(branch);

    // set back statement
    for (int i = 0; i < topLevelItemCount(); i++) {
        this->topLevelItem(i)->setExpanded(stateTree[i]);
    }

    delete[] stateTree;
    clearSelection();
    selectReference(branch);
}

void ReferenceTreeWidget::selectReference(const QString &reference)
{
    QTreeWidgetItem* item = findReference(reference);

    if (item)
        setCurrentItem(item);
}

QTreeWidgetItem* ReferenceTreeWidget::findReference(const QString &reference)
{
    for (int i = 0; i < topLevelItemCount(); i++) {
        QTreeWidgetItem* item = findReference(topLevelItem(i), reference);

        if (item)
            return item;
    }

    return NULL;
}

QTreeWidgetItem* ReferenceTreeWidget::findReference(QTreeWidgetItem *parent,
                                                    const QString & reference)
{
    if (parent->type() == LeafBranch
            || parent->type() == LeafRemote
            || parent->type() == LeafTag) {
        if (parent->data(0, Qt::UserRole).toString().compare(reference) == 0) {
            return parent;
        }
    }

    for (int j = 0; j < parent->childCount(); j++) {
        QTreeWidgetItem* foundItem = findReference(parent->child(j), reference);
        if (foundItem)
            return foundItem;
    }

    return NULL;
}

void ReferenceTreeWidget::contextMenu(const QPoint & pos)
{
    QPoint globalPos = viewport()->mapToGlobal(pos);
    globalPos += QPoint(10, 10);

    QMenu branchesTreeContextMenu(tr("Context menu"), this);
    QTreeWidgetItem* item = selectedItems().first();

    switch (item->type()) {
    case HeaderBranches:
        ;
    case HeaderRemotes:
        ;
    case HeaderTags:
        branchesTreeContextMenu.addAction(collapseAllAction);
        branchesTreeContextMenu.addAction(expandAllAction);
        break;
    case LeafBranch:
        checkoutAction->setData(item->data(0, Qt::DisplayRole));
        branchesTreeContextMenu.addAction(checkoutAction);
        break;
    case LeafRemote:
        break;
    case LeafTag:
        checkoutAction->setData(item->data(0, Qt::DisplayRole));
        branchesTreeContextMenu.addAction(checkoutAction);
        branchesTreeContextMenu.addAction(removeTagAction);
        break;
    }

    if (!branchesTreeContextMenu.isEmpty()) {
        branchesTreeContextMenu.exec(globalPos);
    }
}

void ReferenceTreeWidget::checkout()
{
    QString reference = (checkoutAction->data()).toString();
    m_domain->m()->checkout(reference);
    update();
}

void ReferenceTreeWidget::removeTag()
{
}

void ReferenceTreeWidget::keyPressEvent(QKeyEvent *event)
{
    if ((event->key() >= Qt::Key_A) && (event->key() <= Qt::Key_Z)) {
        m_domain->m()->searchBranchLineEdit->show();
        m_domain->m()->searchBranchLineEdit->setFocus();
        m_domain->m()->searchBranchLineEdit->setText(event->text());
    }
}

void ReferenceTreeWidget::showSearchedItems(QString searchedText)
{
    bool f = false;
    setAllItemsShown();
    if (searchedText.contains("/")) {
        QString firstPart = searchedText.left(searchedText.indexOf("/"));
        QString lastPart = searchedText.mid(searchedText.indexOf("/") + 1);
        // top level item named Remotes for tree is 1 index

        // hide all, exept Remotes
        for (int i = 0; i < topLevelItemCount(); i++) {
            if (i != 1)
                f = isItemShown(topLevelItem(i), searchedText);
        }
        // hide header in "header/lower"
        f = isItemShown(topLevelItem(1), firstPart);
        // find "lower"
        for (int i = 0; i < topLevelItem(1)->childCount(); i++) {
            if (topLevelItem(1)->child(i)->text(0) == firstPart) {
                f = isItemShown(topLevelItem(1)->child(i), lastPart);
            }
        }
    } else {
        if (!(searchedText.simplified().isEmpty())) {
            for (int i = 0; i < topLevelItemCount(); i++) {
                f = isItemShown(topLevelItem(i), searchedText);
            }
        }
    }
}

bool ReferenceTreeWidget::isItemShown(QTreeWidgetItem *item, QString currentString)
{
    if (isRegExpConformed(currentString, item->text(0))) {
        item->setHidden(false);
        return true;
    } else {
        if (item->childCount() > 0) {
            bool flag = false;
            for (int i = 0; i < item->childCount(); i++) {
                if (isItemShown(item->child(i), currentString)) {
                    if (flag == false) {
                        flag = true;
                    }
                }
            }
            item->setHidden(!flag);
            return flag;
        } else {
            item->setHidden(true);
            return false;
        }
    }
}

bool ReferenceTreeWidget::isRegExpConformed(QString currentString, QString originalString)
{
    if (originalString.indexOf(currentString, 0, Qt::CaseInsensitive) == 0)
        return true;
    else
        return false;
}

