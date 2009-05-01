// Copyright, 1995-2007, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of The Emergent Toolkit
//
//   Emergent is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   Emergent is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.

#ifndef IMEDIAWIKI_H
#define IMEDIAWIKI_H

// originally obtained from: rich's blog at http://www.kdedevelopers.org/node/3823
// no license in evidence in the files, and no copyright notice anywhere..

// NOTE: this file is NOT being actively persued and instead interface is at:
// ta/ta_mediawiki.h/.cpp

#include <QObject>
#include <QStringList>
#include <QUrl>

class QNetworkReply;
class QNetworkAccessManager;

class iMediaWiki : public QObject {
  Q_OBJECT
public:
  iMediaWiki( QObject *parent );
  virtual ~iMediaWiki();

  QStringList results() const { return m_results; }

  int maxItems() const { return m_maxItems; }
  void setMaxItems( int limit ) { m_maxItems = limit; }

  QUrl apiUrl() const { return apiUrl; }
  void setApiUrl( const QUrl &url ) { m_apiUrl = url; }

  QNetworkAccessManager* networkAccessManager() { return m_net_manager; }
  void setNetworkAccessManager(QNetworkAccessManager* net_mgr) { m_net_manager = net_mgr; }

signals:
  void finished( bool success );

public slots:
  void search( const QString &searchTerm );
  // sends an action=query message with srsearch=searchTerm -- results are in results() when finished signal is raised

protected slots:
  void finished( QNetworkReply *reply );

protected:
  QStringList 	m_results;
  QUrl 		m_apiUrl;
  QNetworkAccessManager* m_net_manager;
  int 		m_maxItems;

  bool processSearchResult( QIODevice *source );
};

#endif // IMEDIAWIKI_H
