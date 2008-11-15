// Copyright, 1995-2007, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of The Emergent Toolkit
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

# include "ta_qt.h"

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
  // note: attempting to make these permanent, so changes don't get lost all the damn time
  static QFileDialog* fd = NULL;
  static taiFileDialogExtension* fde = NULL;

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
  int rval = 0;
  String caption;
  // get the path history in qt form
  // if there is a filename path, use that
  String eff_dir = m_dir;
  QStringList hist_paths;
  if (tabMisc::root) {
    tabMisc::root->recent_paths.ToQStringList(hist_paths);
    // if no path specified, start at most recent, and if none, we'll just use none
    if ((eff_dir.empty() || eff_dir == "."))
      eff_dir = tabMisc::root->recent_paths.SafeEl(0);
  }
  // if still none, use user's home
  if ((eff_dir.empty() || eff_dir == "."))
    eff_dir = taMisc::user_dir;
   // gack! only way to use semi-sep filters is in constructor...
  // note: actual caption set later

  QStringList filter_list;
  String filtext = filterText(true, &filter_list);

  if(!fd) {
    fd = new QFileDialog(NULL, "", eff_dir, filtext);

    int nw_wd = (int)((float)taiM->scrn_s.w * .5f);
    int nw_ht = (int)((float)taiM->scrn_s.h * .5f);

    fd->resize(nw_wd, nw_ht);

    fde = new taiFileDialogExtension();
    fd->setExtension(fde);
  }

  fd->setDirectory(eff_dir);
  fd->setFilters(filter_list);

  if(compressEnabled() && (compressReq() || isCompressed()))
    fd->setDefaultSuffix(ext + taMisc::compress_sfx);
  else
    fd->setDefaultSuffix(ext);

  switch (filerOperation) {
  case foOpen:
    fd->setAcceptMode(QFileDialog::AcceptOpen);
    if (flags & FILE_MUST_EXIST)
      fd->setFileMode(QFileDialog::AnyFile);
    else
      fd->setFileMode(QFileDialog::ExistingFile);
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
    fd->setFileMode(QFileDialog::AnyFile);
//OBS:    fd->style()->attribute("caption", "Select File to Save for Writing");
    caption = String("Save: ") + filter;
    
    } break;
  case foAppend:
    fd->setConfirmOverwrite((flags & CONFIRM_OVERWRITE));
    fd->setAcceptMode(QFileDialog::AcceptSave);
    fd->setFileMode(QFileDialog::AnyFile);
//OBS:    fd->style()->attribute("caption", "Select File to Append for Writing");
    caption = String("Append: ") + filter;
    break;
  }

  fd->setWindowTitle(caption);
  
  //  cerr << m_fname << endl;
  // todo: for some reason it is not using this arg if the file already exists!
  fd->selectFile(m_fname);
  // we always make and set the extension, but don't always show it
  fde->cbCompress->setEnabled(compressEnabled());
  fde->cbCompress->setChecked(compressEnabled() && (compressReq() || isCompressed()));
  //  fd->setOrientation(Qt::Vertical);
  fd->setViewMode(QFileDialog::Detail);
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
    // some weird bugs showing up in qt filer!
    tfname.gsub("..", ".");	// .. -> .
    tfname.gsub(ext + ext, ext); // .proj.proj -> .proj

    QFileInfo fi(tfname);
    // we always add the path here, to the sys paths -- if added again, it is a noop
    if (tabMisc::root) {
      tabMisc::root->AddRecentPath(fi.path(), true);
      save_paths = true;
    }
    // note: if we further fixup partial filenames, then compress could be true
    if (fd->fileMode() & QFileDialog::ExistingFile)
      file_exists = true;
    else {
      file_exists = fi.exists();
    }
    // compressed 'true' is absolutely based  on filename
    compressed = tfname.endsWith(taMisc::compress_sfx);
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
//   if (fd) {
//     delete fd;
//     fd = NULL;
//   }
  return result; 
}
