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

// originally obtained from: rich's blog at http://www.kdedevelopers.org/node/3823
// no license in evidence in the files, and no copyright notice anywhere..
// extensively modified by Emergent development team

#include "imediawiki.h"

iMediaWiki::iMediaWiki( QObject *parent )
  : QObject( parent )
{
  m_net_manager = NULL;
  m_maxItems = 10;
}

iMediaWiki::~iMediaWiki() {
}

void iMediaWiki::setNetworkAccessManager(QNetworkAccessManager* net_mgr) {
  m_net_manager = net_mgr;
  connect( m_net_manager, SIGNAL(finished(QNetworkReply*)), SLOT(finished(QNetworkReply *)) );
}

void iMediaWiki::search( const QString &searchTerm ) {
  QUrl url = m_apiUrl;
  url.addQueryItem( QString("action"), QString("query") );
  url.addQueryItem( QString("format"), QString("xml") );
  url.addQueryItem( QString("list"), QString("search") );
  url.addQueryItem( QString("srsearch"), searchTerm );
  url.addQueryItem( QString("srlimit"), QString::number(m_maxItems) );

  qDebug() << "Constructed search URL" << url;

  m_net_manager->get( QNetworkRequest(url) );
}

void iMediaWiki::finished( QNetworkReply *reply ) {
  if ( reply->error() != QNetworkReply::NoError ) {
    qDebug() << "Request failed, " << reply->errorString();
    emit finished(false);
    return;
  }

  qDebug() << "Request succeeded";
  bool ok = processSearchResult( reply );
  emit finished( ok );
}

bool iMediaWiki::processSearchResult( QIODevice *source ) {
  m_results.clear();

  QXmlStreamReader reader( source );
  while ( !reader.atEnd() ) {
    QXmlStreamReader::TokenType tokenType = reader.readNext();
    // qDebug() << "Token" << int(tokenType);
    if ( tokenType == QXmlStreamReader::StartElement ) {
      if ( reader.name() == QString("p") ) {
	QXmlStreamAttributes attrs = reader.attributes();
	//qDebug() << "Found page" << attrs.value( QString("title") );
	m_results << attrs.value( QString("title") ).toString();
      }
    }
    else if ( tokenType == QXmlStreamReader::Invalid )
      return false;
  }

  qDebug() << "Results" << m_results;
  return true;
}
