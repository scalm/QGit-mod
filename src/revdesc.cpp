/*
    Author: Marco Costalba (C) 2005-2007

    Copyright: See COPYING file that comes with this distribution
*/
#include <QApplication>
#include <QMenu>
#include <QContextMenuEvent>
#include <QRegExp>
#include <QClipboard>
#include "domain.h"
#include "revdesc.h"

RevDesc::RevDesc(QWidget* p) : QTextBrowser(p), d(NULL) {
    fitted_height = 0;
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);

    connect(this, SIGNAL(anchorClicked(const QUrl&)),
            this, SLOT(on_anchorClicked(const QUrl&)));

    connect(this, SIGNAL(highlighted(const QUrl&)),
            this, SLOT(on_highlighted(const QUrl&)));

    connect(this, SIGNAL(textChanged()), this, SLOT(onTextChanged()));
}

void RevDesc::on_anchorClicked(const QUrl& link) {

    QRegExp re("[0-9a-f]{40}", Qt::CaseInsensitive);
    if (re.exactMatch(link.toString())) {

        setSource(QUrl()); // override default navigation behavior
        d->st.setSha(link.toString());
        UPDATE_DOMAIN(d);
    }
}

void RevDesc::on_highlighted(const QUrl& link) {

    highlightedLink = link.toString();
}

void RevDesc::on_linkCopy() {

    QClipboard* cb = QApplication::clipboard();
    cb->setText(highlightedLink);
}

void RevDesc::contextMenuEvent(QContextMenuEvent* e) {

    QMenu* menu = createStandardContextMenu();
    if (!highlightedLink.isEmpty()) {
        QAction* act = menu->addAction("Copy link SHA1");
        connect(act, SIGNAL(triggered()), this, SLOT(on_linkCopy()));
    }
    menu->exec(e->globalPos());
    delete menu;
}


QSize RevDesc::sizeHint() const {
  QSize sizehint = QTextBrowser::sizeHint();
  sizehint.setHeight(this->fitted_height);
  return sizehint;
}

void RevDesc::fitHeightToDocument() {
      this->document()->setTextWidth(this->viewport()->width());
      QSize document_size(this->document()->size().toSize());

      this->fitted_height = qRound( document_size.height() + document()->documentMargin() );
      this->updateGeometry();
}

void RevDesc::onTextChanged()
{
    fitHeightToDocument();
}
