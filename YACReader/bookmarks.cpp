#include "bookmarks.h"
#include <QFile>
#include <QDataStream>
#include <QCoreApplication>
#include <QFileInfo>

#include <QMessageBox>

#define NUM_BOOKMARKS 250

Bookmarks::Bookmarks()
:lastPageIndex(0)
{
	list.load();
}
void Bookmarks::setLastPage(int index,const QPixmap & page)
{
	lastPageIndex = index;
	lastPage = page;
}
void Bookmarks::setBookmark(int index,const QPixmap & page)
{
	if(!bookmarks.contains(index))
	{
		bookmarks.insert(index,page);
		latestBookmarks.push_front(index);
		if(latestBookmarks.count()>3)
		{
			bookmarks.remove(latestBookmarks.back());
			latestBookmarks.pop_back();
		}
	}
	else //udate de pixmap;
	{
		bookmarks[index]=page;
	}
}

void Bookmarks::removeBookmark(int index)
{
    bookmarks.remove(index);
}

QList<int> Bookmarks::getBookmarkPages() const
{
    return bookmarks.keys();
}

QPixmap Bookmarks::getBookmarkPixmap(int page) const
{
    return bookmarks.value(page);
}

QPixmap Bookmarks::getLastPagePixmap() const
{
    return lastPage;
}

int Bookmarks::getLastPage() const
{
    return lastPageIndex;
}


bool Bookmarks::isBookmark(int page)
{
     return bookmarks.contains(page);
}

bool Bookmarks::imageLoaded(int page)
{
	return !bookmarks.value(page).isNull();
}

void Bookmarks::newComic(const QString & path)
{
	QFileInfo f(path);
	QString comicID = f.fileName().toLower()+QString::number(f.size());
	clear();
	BookmarksList::Bookmark b = list.get(comicID);
	comicPath=comicID;
	lastPageIndex = b.lastPage;
	latestBookmarks = b.bookmarks;
	for(int i=0;i<latestBookmarks.count();i++)
		bookmarks.insert(latestBookmarks.at(i),QPixmap());
	added = b.added;
	
}
void Bookmarks::clear()
{
	bookmarks.clear();
	latestBookmarks.clear();
    lastPageIndex=0;
    lastPage = QPixmap();
}

void Bookmarks::save()
{
	BookmarksList::Bookmark b;
	b.lastPage = lastPageIndex;
	b.bookmarks = getBookmarkPages();
	
	BookmarksList::Bookmark previousBookmarks;
	bool updated = ((previousBookmarks.lastPage != b.lastPage) || (previousBookmarks.bookmarks != b.bookmarks));

	if(b.added.isNull() || updated)
		b.added = QDateTime::currentDateTime();
	list.add(comicPath,b);
	list.save();
}
//-----------------------------------------------------------------------------
void BookmarksList::load()
{
	QFile f(QCoreApplication::applicationDirPath()+"/bookmarks.yacr");
	if(f.open(QIODevice::ReadOnly))
	{
		QDataStream dataS(&f);
		dataS >> list;
		f.close();
	}
}

void BookmarksList::save()
{
	QFile f(QCoreApplication::applicationDirPath()+"/bookmarks.yacr");
	f.open(QIODevice::WriteOnly);
	QDataStream dataS(&f);
	if(list.count()>NUM_BOOKMARKS)
		deleteOldest(list.count()-NUM_BOOKMARKS);
	dataS << list;
	f.close();
}


void BookmarksList::deleteOldest(int num)
{
	QString comic;
	QDateTime date(QDate(10000,1,1));//TODO MAX_DATE??
	for(QMap<QString,Bookmark>::const_iterator itr=list.begin();itr!=list.end();itr++)
	{
		if(itr->added<date)
		{
			comic=itr.key();
			date = itr->added;
		}
	}
	list.remove(comic);
}

void BookmarksList::add(const QString & comicID, const Bookmark & b)
{
	list.insert(comicID,b);
}

BookmarksList::Bookmark BookmarksList::get(const QString & comicID)
{
	//if(list.contains(comicID)
		return list.value(comicID);
}