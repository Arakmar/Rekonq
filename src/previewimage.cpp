/* ============================================================
*
* This file is a part of the rekonq project
*
* Copyright (C) 2009 by Andrea Diamantini <adjam7 at gmail dot com>
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

#include "previewimage.h"
#include "previewimage.moc"

#include <QFile>

#include <KUrl>
#include <KStandardDirs>
#include <KDebug>

PreviewImage::PreviewImage(const QString &url, const QString &pos)
    : QLabel()
    , ws(0)
{   
    QString path = KStandardDirs::locateLocal("cache", QString("thumbs/rek") + pos + ".png", true);
    
    if(QFile::exists(path))
    {
        kDebug() << "exists! Loading it...";
        m_pixmap.load(path);
        setPixmap( m_pixmap );
    }
    else
    {
        QString path = KStandardDirs::locate("appdata", "pics/loading.mng");
        setPixmap( QPixmap(path) );
    
        ws = new WebSnap( url, pos );
        connect(ws, SIGNAL(finished()), this, SLOT(setSiteImage()));
    }
}


PreviewImage::~PreviewImage()
{
    kDebug() << "bye bye..";
}

void PreviewImage::setSiteImage()
{
    kDebug() << "Done. works?";
    setPixmap( ws->previewImage() );
}