// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/CSS
//
//   This library is free software; you can redistribute it and/or
//   modify it under the terms of the GNU Lesser General Public
//   License as published by the Free Software Foundation; either
//   version 2.1 of the License, or (at your option) any later version.
//   
//   This library is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//   Lesser General Public License for more details.

// ta_filer_qt.cpp -- qt-gui only routines in filer


#include "ta_filer.h"

#include "ta_base.h"
#include "ta_project.h"

#include <QApplication>
#include <QBoxLayout>
#include <QCheckBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QFrame>
#include <QLayout>
#include <QWidget>

//////////////////////////////////
//  taiFileDialogExtension	//
//////////////////////////////////

class taiFileDialogExtension: public QWidget {
INHERITED(QWidget)
public:
  QVBoxLayout* lay1; // outer
  QHBoxLayout* lay2; // for check boxes 
  QCheckBox* 	cbCompress;
  taiFileDialogExtension(QWidget* parent = NULL);
  
private:
  void	init();

};

taiFileDialogExtension::taiFileDialogExtension(QWidget* parent)
:inherited(parent)
{
  init();
}

void taiFileDialogExtension::init() {
  lay1 = new QVBoxLayout(this); // def margin=2
  QFrame* sep = new QFrame(this);
  sep->setFrameStyle(QFrame::HLine | QFrame::Sunken);
  lay1->addWidget(sep);
  
  lay2 = new QHBoxLayout(); // def margin=2
  lay1->addLayout(lay2);
  cbCompress = new QCheckBox("compress", this);
  lay2->addWidget(cbCompress);
  lay2->addStretch();
}


//////////////////////////////////
//  taFiler			//
//////////////////////////////////

bool taFiler::GetFileName(FileOperation filerOperation) {
  String tfname;
  if(!taMisc::gui_active) {
    if (m_fname.empty()) {
      cout << "Please provide file name: ";
      cin >> tfname;
      setFileName(tfname);
    }
    return true;
  }
  bool result = false;
  QStringList hist_paths;
  String last_dir;
  if (tabMisc::root) {
    tabMisc::root->recent_paths.ToQStringList(hist_paths);
    last_dir = tabMisc::root->recent_paths.SafeEl(0);
  }
  // if doing a SaveAs, or otherwise no dir, then use most recent
  if((m_dir.empty() || m_dir == "."))
    m_dir = last_dir; // if still empty, we'll just use whatever, prob working dir
  // gack! only way to use semi-sep filters is in constructor...
  // note: actual caption set later
  QFileDialog* fd = new QFileDialog(NULL, "", m_dir, filterText());

  taiFileDialogExtension* fde = new taiFileDialogExtension();

  String caption;
  switch (filerOperation) {
  case foOpen:
    fd->setAcceptMode(QFileDialog::AcceptOpen);
    if (flags & FILE_MUST_EXIST)
      fd->setMode(QFileDialog::AnyFile);
    else
      fd->setMode(QFileDialog::ExistingFile);
//OBS:    fd->style()->attribute("caption", "Select File to Open for Reading");
    caption = String("Open: ") + filter;
    break;
  case foSave:
    // we already have filename!
    result = true;
    goto exit;
  case foSaveAs: {
    fd->showExtension(true);
    fd->setConfirmOverwrite((flags & CONFIRM_OVERWRITE));
    fd->setAcceptMode(QFileDialog::AcceptSave);
    fd->setMode(QFileDialog::AnyFile);
//OBS:    fd->style()->attribute("caption", "Select File to Save for Writing");
    caption = String("Save: ") + filter;
    
    } break;
  case foAppend:
    fd->setConfirmOverwrite((flags & CONFIRM_OVERWRITE));
    fd->setAcceptMode(QFileDialog::AcceptSave);
    fd->setMode(QFileDialog::AnyFile);
//OBS:    fd->style()->attribute("caption", "Select File to Append for Writing");
    caption = String("Append: ") + filter;
    break;
  }

  fd->setCaption(caption);
  
  //  cerr << m_fname << endl;
  // todo: for some reason it is not using this arg if the file already exists!
  fd->selectFile(m_fname);
  // we always make and set the extension, but don't always show it
  fde->cbCompress->setEnabled(compressEnabled());
  fde->cbCompress->setChecked(compressEnabled() && compressReq());
  fd->setExtension(fde);
  fd->setOrientation(Qt::Vertical);
  int rval;

  fd->setHistory(hist_paths);

  QApplication::setOverrideCursor(QCursor(Qt::ArrowCursor)); // in case busy, recording, etc
  rval = fd->exec();
  QApplication::restoreOverrideCursor();
  if (rval == QDialog::Accepted) {
    //note: Qt4 requires us to get the file indirectly, from the list
    QStringList sfs(fd->selectedFiles());
    if (sfs.isEmpty()) goto exit; // shouldn't happen!
    { // block necessitated by exit
    tfname = sfs[0];
    QFileInfo fi(tfname);
    // we always add the path here, to the sys paths -- if added again, it is a noop
    if (tabMisc::root)
      tabMisc::root->AddRecentPath(fi.path());
    // note: if we further fixup partial filenames, then compress could be true
    if (fd->fileMode() & QFileDialog::ExistingFile)
      file_exists = true;
    else {
      file_exists = fi.exists();
    }
    // compressed 'true' is absolutely based  on filename
    compressed = m_fname.endsWith(taMisc::compress_sfx);
    // but if file doesn't exist, we could fix it up, so we set based on checkbox
    if (!file_exists && !compressed) {
      compressed = fde->cbCompress->isChecked();
    } 
    setFileName(tfname);
    //obs m_dir = fd->directory().absolutePath();
    result = true;
    }
  }

exit:
  if (fd) {
    delete fd;
    fd = NULL;
  }
  return result; 
}
