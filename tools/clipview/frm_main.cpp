#include "frm_main.h"

#include <qvariant.h>
#include <qclipboard.h>
#include <qapplication.h>
#include <qmime.h>
#include <QByteArray>
#include <qpushbutton.h>
#include <qcombobox.h>
#include <qtextedit.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

/*
 *  Constructs a frmMain as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
frmMain::frmMain( QWidget* parent)
    : QFrame( parent)
{
    setName( "frmMain" );

    btnFormats = new QPushButton( this );
    btnFormats->setGeometry( QRect( 11, 11, 90, 35 ) );

    cmbFormats = new QComboBox( FALSE, this );
    cmbFormats->setGeometry( QRect( 108, 15, 240, 27 ) );

    btnShow = new QPushButton( this );
    btnShow->setGeometry( QRect( 360, 10, 110, 35 ) );

    txtResults = new Q3TextEdit( this );
    txtResults->setGeometry( QRect( 10, 50, 551, 201 ) );
    txtResults->setTextFormat( Qt::LogText );

    lblDropZone = new QLabel( this );
    lblDropZone->setGeometry( QRect( 480, 10, 80, 35 ) );
    lblDropZone->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, lblDropZone->sizePolicy().hasHeightForWidth() ) );
    lblDropZone->setAcceptDrops( TRUE );
    lblDropZone->setFrameShape( QLabel::Box );
    lblDropZone->setFrameShadow( QLabel::Sunken );
    lblDropZone->setTextFormat( Qt::PlainText );
    languageChange();
    resize( QSize(567, 260).expandedTo(minimumSizeHint()) );
//    clearWState( WState_Polished );

    // signals and slots connections
    connect( btnFormats, SIGNAL( clicked(bool) ), this, SLOT( btnFormats_clicked() ) );
    connect( btnShow, SIGNAL( clicked(bool) ), this, SLOT( btnShow_clicked() ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
frmMain::~frmMain()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void frmMain::languageChange()
{
    setCaption( tr( "Clipboard Viewer" ) );
    btnFormats->setText( tr( "Formats" ) );
    btnShow->setText( tr( "Show" ) );
    lblDropZone->setText( tr( "Drop Zone" ) );
}

void frmMain::btnFormats_clicked()
{
    cmbFormats->clear();
    QClipboard* cb = QApplication::clipboard();
    const QMimeData* md = cb->mimeData(QClipboard::Clipboard);
    if (!md) return;
    QString fmt;
    QStringList fmts = md->formats();
    for (int i = 0; i < fmts.count(); ++i) {
        cmbFormats->insertItem(fmts[i]);
    }
/*obs    // special hidden data and command formats -- always provided when tacss used
    if (ms->provides("tacss/objectdesc")) {
      // add generic (non indexed) hidden formats
      cmbFormats->insertItem(QString("tacss/objectdata")); 
      QByteArray ba = ms->encodedData("tacss/objectdesc");
      QString str = QString(ba);
      // get the number of objects -- first before ";"
      int pos_colon = str.indexOf(";");
      str = str.left(pos_colon);
      int num = str.toInt();
      int i;
      for (i = 0; i < num; ++i) {
        cmbFormats->insertItem(QString("tacss/objectdata;index=%1").arg(i)); 
      }
    } */
//  delete ms;
}


void frmMain::btnShow_clicked()
{
    txtResults->clear();
    QClipboard* cb = QApplication::clipboard();
    QMimeSource* ms = cb->data(QClipboard::Clipboard);
    if (ms == NULL) return;
    QByteArray ba = ms->encodedData(cmbFormats->currentText());
    QString str = QString(ba);
    txtResults->setText(str);

}
