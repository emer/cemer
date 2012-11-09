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

#include "ta_qt.h"

#include <QApplication>
#include <QBoxLayout>
#include <QCheckBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QFrame>
#include <QLayout>
// #include <QScopedPointer> // not avail it seems on some relatively recent platforms
#include <memory> // std::auto_ptr
#include <QWidget>
#include <QUrl>

//////////////////////////////////
//  taiFileDialogExtension      //
//////////////////////////////////

class taiFileDialogExtension : public QWidget {
INHERITED(QWidget)
public:
  QVBoxLayout* lay1; // outer
  QHBoxLayout* lay2; // for check boxes
  QCheckBox*   cbCompress;
  taiFileDialogExtension(QWidget* parent = NULL);

private:
  void init();
};

taiFileDialogExtension::taiFileDialogExtension(QWidget* parent)
  : inherited(parent)
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
//  taFiler                     //
//////////////////////////////////

bool taFiler::GetFileName(FileOperation filerOperation) {
  String tfname;
  if (!taMisc::gui_active) {
    if (m_fname.empty()) {
      cout << "Please provide file name: ";
      cin >> tfname;
      SetFileName(tfname);
    }
    return true;
  }

  // get the path history in qt form
  // if there is a filename path, use that
  String eff_dir = m_dir;
  QStringList hist_paths;
  if (tabMisc::root) {
    //    tabMisc::root->recent_paths.ToQStringList(hist_paths);
    // note: filer seems to use reverse ordering: top in menu is last on this list:
    String_Array& rpth = tabMisc::root->recent_paths;
    for (int i = rpth.size; i-- > 0; ) {
      hist_paths.append(rpth.FastEl(i).toQString());
    }
    // if no path specified, start at most recent, and if none, we'll just use none
    if (eff_dir.empty() || eff_dir == ".") {
      //      eff_dir = rpth.SafeEl(0);
      eff_dir = taMisc::GetCurrentPath(); // use current!
    }
  }

  // if still none, use user's home
  if (eff_dir.empty() || eff_dir == ".") {
    eff_dir = taMisc::user_dir;
  }
   // gack! only way to use semi-sep filters is in constructor...
  // note: actual caption set later

  QStringList filter_list;
  String filtext = FilterText(true, &filter_list);

  // note: no static FileDialog!!! causes many issues:
  // * compression extension thing not always working
  // * filters can only be added in ctor
  // * some use-cases cause no Save dialog if prev is Open
  // DO NOT MAKE STATIC!!!!
  // Use QScopedPointer to manage memory within this scope (function).
  //  QScopedPointer<QFileDialog> fd(new QFileDialog(NULL, "", eff_dir, filtext));
  std::auto_ptr<QFileDialog> fd(new QFileDialog(NULL, "", eff_dir, filtext));

  int nw_wd = (int)((float)taiM->scrn_s.w * .5f);
  int nw_ht = (int)((float)taiM->scrn_s.h * .5f);
  fd->resize(nw_wd, nw_ht);

  taiFileDialogExtension* fde = new taiFileDialogExtension();
  fd->setExtension(fde);

#ifdef TA_OS_MAC
  // native dialog does not support compression stuff
  fd->setOptions(QFileDialog::DontUseNativeDialog);
#endif

  QList<QUrl> urls;
  taRootBase* root = tabMisc::root;
  for (int i=0; i<root->sidebar_paths.size; i++) {
    String sbp = root->sidebar_paths[i];
    if (sbp.empty()) continue;
    urls << QUrl::fromLocalFile(sbp);
  }
  fd->setSidebarUrls(urls);

  fd->setDirectory(eff_dir);
  fd->setFilters(filter_list);

  bool is_default_filename_compressed =
    CompressEnabled() && (CompressReq() || IsCompressed());
  // Qt expects a suffix without a leading dot.
  String suffix = taFilerUtil::UndottedExtension(
    is_default_filename_compressed ? defExt() + taMisc::compress_sfx : defExt());
  fd->setDefaultSuffix(suffix);

  String caption;
  switch (filerOperation) {
    case foOpen:
      fd->setAcceptMode(QFileDialog::AcceptOpen);
      if (HasFilerFlag(FILE_MUST_EXIST))
      {
        fd->setFileMode(QFileDialog::ExistingFile);
      }
      else
      {
        // TBD: Is this ability actually useful?  Will it even work?
        taMisc::Warning("File dialog will allow opening a non-existant file");
        fd->setFileMode(QFileDialog::AnyFile);
      }
      caption = String("Open: ") + filter;
      break;

    case foSave:
      // we already have filename!
      return true;

    case foSaveAs:
      fd->showExtension(true);
      fd->setConfirmOverwrite(HasFilerFlag(CONFIRM_OVERWRITE));
      fd->setAcceptMode(QFileDialog::AcceptSave);
      fd->setFileMode(QFileDialog::AnyFile);
      caption = String("Save: ") + filter;
      break;

    case foAppend:
      fd->setConfirmOverwrite(HasFilerFlag(CONFIRM_OVERWRITE));
      fd->setAcceptMode(QFileDialog::AcceptSave);
      fd->setFileMode(QFileDialog::AnyFile);
      caption = String("Append: ") + filter;
      break;
  }

  fd->setWindowTitle(caption);

  // todo: for some reason it is not using this arg if the file already exists!
  fd->selectFile(m_fname);
  // we always make and set the extension, but don't always show it
  fde->cbCompress->setEnabled(CompressEnabled());
  fde->cbCompress->setChecked(is_default_filename_compressed);
  //  fd->setOrientation(Qt::Vertical);
  fd->setViewMode(QFileDialog::Detail);
  fd->setHistory(hist_paths);

  QApplication::setOverrideCursor(QCursor(Qt::ArrowCursor)); // in case busy, recording, etc
  int rval = fd->exec();
  QApplication::restoreOverrideCursor();

  if (rval != QDialog::Accepted) {
    return false;
  }

  // first persist the sidebars
  urls = fd->sidebarUrls();
  for (int i=0; i<urls.count(); i++) {
    String sbp = urls[i].toLocalFile();
    root->sidebar_paths.AddUnique(sbp); // save it!
  }

  //note: Qt4 requires us to get the file indirectly, from the list
  QStringList sfs(fd->selectedFiles());
  if (sfs.isEmpty()) {
    // shouldn't happen!
    taMisc::Warning("No files returned");
    return false;
  }

  tfname = sfs[0];
  QFileInfo fi(tfname);
  // we always add the path here, to the sys paths -- if added again, it is a noop
  if (tabMisc::root) {
    tabMisc::root->AddRecentPath(fi.path(), true);
    save_paths = true;
  }

  // note: if we further fixup partial filenames, then compress could be true
  file_exists = fd->fileMode() == QFileDialog::ExistingFile ||
                fd->fileMode() == QFileDialog::ExistingFiles ||
                fi.exists();

  // compressed 'true' is absolutely based on filename,
  compressed = tfname.endsWith(taMisc::compress_sfx);

  // but if file doesn't exist, we could fix it up, so we set based on checkbox
  if (!file_exists && !compressed) {
    compressed = fde->cbCompress->isChecked();
  }

  SetFileName(tfname);
  return true;
}
