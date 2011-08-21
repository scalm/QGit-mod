#ifndef REFERENCETREEWIDGET_H
#define REFERENCETREEWIDGET_H

#include <QTreeWidget>
#include "git.h"
#include "domain.h"

class ReferenceTreeWidget : public QTreeWidget
{
    Q_OBJECT

public:
    enum ItemType
    {
        HeaderBranches = 257,
        HeaderRemotes  = 258,
        HeaderTags     = 259,
        LeafBranch     = 260,
        LeafRemote     = 261,
        LeafTag        = 262,
        HeaderRemote   = 263
    };

    ReferenceTreeWidget(QWidget *parent = 0);
    void setup(Domain *domain, Git *git);
    void update();

public slots:
    void showSearchedItems(QString searchedText = "");
    void changeReference(QTreeWidgetItem *item, int column);
    void contextMenu(const QPoint &pos);
    void checkout();
    void removeTag();

protected:
    void keyPressEvent(QKeyEvent *event);

private:
    Git *m_git;
    Domain *m_domain;

    QAction *collapseAllAction;
    QAction *expandAllAction;
    QAction *checkoutAction;
    QAction *removeTagAction;

    QIcon branchIcon;
    QIcon masterBranchIcon;
    QIcon tagIcon;

    void addNode(ItemType headerType, Reference::Type type);
    void addRemotesNodes();

    void selectReference(const QString &reference);

    void setAllItemsShown();
    void setShownItem(QTreeWidgetItem *item);

    QTreeWidgetItem *findReference(const QString &branch);
    QTreeWidgetItem *findReference(QTreeWidgetItem *parent, const QString &branch);

    bool isRegExpConformed(QString currentString, QString originalString);
    bool isItemShown(QTreeWidgetItem *item, QString currentString);
};

#endif // REFERENCETREEWIDGET_H
