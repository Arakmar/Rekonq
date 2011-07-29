/* ============================================================
*
* This file is a part of the rekonq project
*
* Copyright (C) 2011 by Vyacheslav Blinov <blinov dot vyacheslav at gmail dot com>
*
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License as
* published by the Free Software Foundation; either version 2 of
* the License or (at your option) version 3 or any later version
* accepted by the membership of KDE e.V. (or its successor approved
* by the membership of KDE e.V.), which shall act as a proxy
* defined in Section 14 of version 3 of the license.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
* ============================================================ */

//Self Includes
#include "tabpreviewpopup.h"

// Rekonq Includes
#include "webtab.h"
#include "tabbar.h"
#include "websnap.h"
#include "application.h"
#include "mainwindow.h"

//Qt Includes
#include <QLabel>
#include <QVBoxLayout>
#include <QPalette>
#include <QBitmap>
#include <QPoint>

// static
static const int borderRadius = 5;
static const int borderSpacing = 2;
static const double transparency = 0.90;


TabPreviewPopup::TabPreviewPopup(WebTab* tab, QWidget* parent)
    : KPassivePopup(parent),
      m_thumbnail(new QLabel(this)),
      m_url(new QLabel(this))
{
    m_thumbnail->setAlignment(Qt::AlignHCenter);
    m_url->setAlignment(Qt::AlignHCenter);

    QVBoxLayout *vb = new QVBoxLayout(this);
    vb->addWidget(m_thumbnail);
    vb->addWidget(m_url);
    this->setLayout(vb);

    layout()->setAlignment(Qt::AlignTop);
    layout()->setMargin(0);

    setFrameShape(QFrame::StyledPanel);
    setFrameShadow(QFrame::Raised);
    setWindowOpacity(transparency);

    setWebTab(tab);
}

TabPreviewPopup::~TabPreviewPopup()
{
    delete m_thumbnail;
    delete m_url;
}


void TabPreviewPopup::setWebTab(WebTab* tab)
{
    int w = (tab->parentWidget()->sizeHint().width() / TabBar::baseWidthDivisor);
    int h = w * rApp->mainWindow()->size().height() / rApp->mainWindow()->size().width();

    setThumbnail(WebSnap::renderTabPreview(*tab->page(), w, h));
    setUrl(tab->url().prettyUrl());

    setFixedSize(w, h + m_url->heightForWidth(w));
}


void TabPreviewPopup::setThumbnail(const QPixmap& pixmap)
{
    m_thumbnail->setPixmap(pixmap);
}

void TabPreviewPopup::setUrl(const QString& text)
{
    m_url->setText(text);
}

void TabPreviewPopup::setFixedSize(int w, int h)
{
    KPassivePopup::setFixedSize(w, h);
    m_url->setText(m_url->fontMetrics().elidedText(m_url->text(), Qt::ElideMiddle, this->width() - borderRadius));

    QPixmap pixmap(size());
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(pixmap.rect(), Qt::red);
    painter.setBrush(QBrush(Qt::black));
    painter.drawRoundRect(borderSpacing, borderSpacing, pixmap.width() - borderSpacing * 2, pixmap.height() - borderSpacing * 2, borderRadius, borderRadius);
    setMask(pixmap.createMaskFromColor(Qt::red));
}