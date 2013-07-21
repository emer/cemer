// Copyright, 1995-2013, Regents of the University of Colorado,
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

#include "taFiler.h"
#include <String_PArray>
#include <taMisc>
#include <tabMisc>
#include <taRootBase>

#include <gzstream>

#include <QFileInfo>
#include <QStringList>
#include <taiMisc>

using namespace std;

int taFiler::buf_size = 1016;
String taFiler::last_fname;
ContextFlag taFiler::no_save_last_fname;

taFiler* taFiler::New(const String& filetype_,
  const String& exts_, FilerFlags flags_)
{
  taFiler* rval = new taFiler(flags_);
  rval->m_dir = ".";
  rval->filetype = filetype_;
  rval->exts = exts_;

  return rval;
}

taFiler::taFiler(FilerFlags flags_)
  : taRefN()
  , filter()
  , filetype()
  , select_only(false)
  , istrm(0)
  , ostrm(0)
  , open_file(false)
  , file_selected(false)
  , m_dir()
  , m_fname()
  , m_tmp_fname()
  , exts()
  , flags(flags_)
  , fstrm(0)
  , compressed(false)
  , file_exists(false)
  , save_paths(false)
{
  if (HasFilerFlag(COMPRESS_REQ_DEF)) {
    if (taMisc::save_compress)
      SetFilerFlag(COMPRESS_REQ);
    else
      ClearFilerFlag(COMPRESS_REQ);
  }
}

taFiler::~taFiler() {
  Close();
  if (save_paths && taMisc::gui_active && tabMisc::root) {
    save_paths = false;
    tabMisc::root->Save();
  }
}

taFiler& taFiler::operator=(const taFiler& cp) {
  // can't copy an open one!
  Close();
  filter = cp.filter;
  exts = cp.exts;
  m_dir = cp.m_dir;
  m_fname = cp.m_fname;
  m_tmp_fname = cp.m_fname;
  flags = cp.flags;
  select_only = cp.select_only;
  file_selected = cp.file_selected;
  return *this;
}

void taFiler::Close() {
  compressed = false;
  file_exists = false; // exists now, but could be deleted etc.
  bool was_ostrm = false;
  if (fstrm) {
    fstrm->close();
    delete fstrm;
    fstrm = NULL;
    istrm = NULL;               // these are ptrs to fstrm
    if(ostrm) was_ostrm = true;
    ostrm = NULL;
  }
  if (istrm) {
    delete istrm;
    istrm = NULL;
  }
  if (ostrm) {
    was_ostrm = true;
    delete ostrm;
    ostrm = NULL;
  }
  open_file = false;
  file_selected = true;         // closing a file is as good as opening one
  if(was_ostrm && HasFilerFlag(TMP_SAVE_FILE) && HasFilerFlag(TMP_SAVE_FILE_USED)
     && m_tmp_fname.nonempty()) {
    String cur_fnm = FileName();
    String tmp_fnm = FileName_tmp();
    if(FileExists()) {
      QFile::remove(cur_fnm.chars());
    }
    if(FileExists()) {
      taMisc::Error("Unable to overwite existing file named", cur_fnm,
                    "for saving -- most likely you do not have appropriate permissions -- file is saved to temporary filename as:", tmp_fnm);
    }
    else {
      QFile::rename(tmp_fnm.chars(), cur_fnm.chars());
    }
  }
}

String taFiler::defExt() const {
  String rval;
  // only grab first in list as default extension
  if (exts.contains(","))
    rval = exts.before(",");
  else
    rval = exts;
  // Add a dot if necessary.
  return taFilerUtil::DottedExtension(rval);
}

String taFiler::FileName() const {
  if (m_dir.empty())
    return m_fname;
  else return m_dir + "/" + m_fname;
}

String taFiler::FileName_tmp() const {
  if (m_dir.empty())
    return m_tmp_fname;
  else return m_dir + "/" + m_tmp_fname;
}

String taFiler::FileNameForSaveAs() const {
  String fname = FileName();
  if (fname.endsWith(".proj")) {
    String temp_name = fname.before(".proj",-1);
    fname = temp_name + "_copy";
  }
  else {
    fname = fname + "_copy";
  }
  return fname;
}

bool taFiler::RenameFile(const String& new_fname, bool remove_existing) {
  String cur_fnm = FileName();
  if(remove_existing && QFile::exists(new_fname)) {
    if(!QFile::remove(new_fname.chars())) return false;
  }
  return QFile::rename(cur_fnm.chars(), new_fname.chars());
}

bool taFiler::FileExists() {
  return QFile::exists(FileName().chars());
}

void taFiler::GetTmpFname() {
  m_tmp_fname = "savetmp_" + m_fname;
}

const String taFiler::FilterText(bool incl_allfiles, QStringList* list) const {
  STRING_BUF(rval, 80);
  String_PArray sa_ft;
  String_PArray sa_ex;
  sa_ft.SetFromString(filetype, ",");
  sa_ex.SetFromString(exts, ",");
  // key on the exts, and just put a ? for type if we run out
  for (int i = 0; i < sa_ex.size; ++i) {
    using namespace taFilerUtil;
    String filetype = GetFiletype(i, sa_ft);
    String extension = GetDottedExtension(i, sa_ex);
    String filter = MakeFilter(filetype, extension, CompressEnabled());
    AddFilter(filter, rval, list);
  }

  if (incl_allfiles) {
    using namespace taFilerUtil;
    String filter = "All files (*)";
    AddFilter(filter, rval, list);
  }

  return rval;
}

void taFiler::FixFileName() {
  //ONLY call this on saves, not when user has already specified a name
  // General approach:
  // if m_fname is an actual existing file, we don't touch it
  if (file_exists) return;

  String base_fname = m_fname; // name w/o compress extension
  // if existing file has compression suffix, then activate compression
  if(m_fname.endsWith(taMisc::compress_sfx)) {
    compressed = true;
    base_fname = m_fname.before(taMisc::compress_sfx, -1);
  }

  // Get the actual extension (including dot), if any.
  String act_ext = base_fname.from('.', -1);
  if (act_ext.empty()) {
    // If no ext was supplied and we have a default one, apply it.
    act_ext = defExt(); // (might still be empty string after this call)
  }

  String nocompr_ext = act_ext;
  if (compressed) {
    act_ext += taMisc::compress_sfx;
  }

  if (m_fname.endsWith(act_ext)) {
    return; // done!
  }

  // ends with basic extension but not compressed one -- just add compresed
  if (compressed && m_fname.endsWith(nocompr_ext)) {
    m_fname += taMisc::compress_sfx;
    return;
  }

  // if user already has an extension, don't change it
  //note: because paths can have . we have to look at file only
  QFileInfo fi(m_fname);
  String f_only = fi.baseName();
  if (!f_only.contains('.') && nocompr_ext.nonempty()) {
    // otherwise add standard extension
    m_fname += nocompr_ext;
  }
  // if compressed, and don't have compr ext, add it
  if (compressed) {
    bool has_ext = m_fname.endsWith(taMisc::compress_sfx);
    if (!has_ext)
      m_fname += taMisc::compress_sfx;
  }
}
/*
void taFiler::GetDir() {
  QFileInfo fi(m_fname);
  m_dir = fi.absolutePath(); // note: doesn't deref symlinks, which is probably what we want
  if (!m_dir.empty() && (m_dir != ".")) {
    taMisc::load_paths.AddUnique(m_dir); // make sure its on the include path..
    taMisc::css_include_paths.AddUnique(m_dir); // make sure its on the include path..
  }
}*/

bool taFiler::IsCompressed() const {
  if (IsOpen()) return compressed;
  else return m_fname.endsWith(taMisc::compress_sfx);
}

istream* taFiler::open_read() {
  Close();
  // note: it is the filename that determines compress, not the flag
  QFileInfo qfi(FileName());
  if (m_fname.endsWith(taMisc::compress_sfx)) {
    compressed = true;
    if(qfi.isFile() && qfi.isReadable())
      istrm = new igzstream(FileName().chars(), ios::in);
  } else {
    fstrm = new fstream(FileName().chars(), ios::in);
    istrm = (istream*)fstrm;
  }
  // note: check "good" rather than "bad" because good is proactive, bad is only reactive
  if(!istrm || !istrm->good()) {
    taMisc::Error("File:",FileName(),"could not be opened for reading");
    Close();
    return NULL;
  }
  open_file = true;
  if (!no_save_last_fname)
    last_fname = FileName();
  return istrm;
}

bool taFiler::open_write_exist_check() {
  Close();
  QFileInfo qfi(FileName());
  if(qfi.isFile())
    return true;
  return false;
}

ostream* taFiler::open_write() {
  Close();
  bool hasfx = m_fname.endsWith(taMisc::compress_sfx);
  String use_fnm = FileName();
  if(FileExists() && !taMisc::FileWritable(use_fnm)) {
    int chs = taMisc::Choice("File to save (" + use_fnm + ") already exists and is not set as writable by you -- overwrite anyway?", "Overwrite", "Cancel");
    if(chs == 1) return NULL;
  }
  if(HasFilerFlag(TMP_SAVE_FILE) && FileExists()) {
    SetFilerFlag(TMP_SAVE_FILE_USED);
    GetTmpFname();
    use_fnm = FileName_tmp();
  }
  else {
    ClearFilerFlag(TMP_SAVE_FILE_USED);
    m_tmp_fname = "";           // clear this too for good measure
  }
  if (hasfx) {
    compressed = true;
    ostrm = new ogzstream(use_fnm, ios::out);
  } else {
    fstrm = new fstream(use_fnm, ios::out);
    ostrm = (ostream*)fstrm;
  }
  // note: check "good" rather than "bad" because good is proactive, bad is only reactive
  if (!ostrm->good()) {
    taMisc::Error("File",use_fnm,"could not be opened for writing -- check that you have permission to write to that location");
    Close();
    return NULL;
  }
  // success, so save/cache stuff
  open_file = true;
  if (!no_save_last_fname)
    last_fname = FileName();
  return ostrm;
}

ostream* taFiler::open_append() {
  Close();
  bool hasfx = (m_fname.endsWith(taMisc::compress_sfx));
  if (hasfx) {
    compressed = true;
    ostrm = new ogzstream(FileName().chars(), ios::out | ios::app);
  } else {
    fstrm = new fstream(FileName().chars(), ios::out | ios::app);
    ostrm = (ostream*)fstrm;
  }
  // note: check "good" rather than "bad" because good is proactive, bad is only reactive
  if (!ostrm->good()) {
    taMisc::Error("File",FileName(),"could not be opened for appending -- check that you have permission to write to that location");
    Close();
    return NULL;
  }
  open_file = true;
  if (!no_save_last_fname)
    last_fname = FileName();
  return ostrm;
}


istream* taFiler::Open() {
  file_selected = false;
  if (!GetFileName(foOpen)) return NULL;

  istream* rstrm = NULL;
  file_selected = true;
  if (open_read()) {
//      if (!select_only && (istrm->bad() || (istrm->peek() == EOF)))
//NOTE: not up to us to forbid opening an empty file!
    if (select_only)
      Close();
    else
      rstrm = istrm;
  }
  return rstrm;
}

ostream* taFiler::Save(bool tmp_fname_save) {
  if (m_fname.empty())
    return SaveAs(tmp_fname_save);
  SetFilerFlagState(TMP_SAVE_FILE, tmp_fname_save);
  if (!open_write()) {
    return SaveAs(tmp_fname_save);
  }
  return ostrm;
}

ostream* taFiler::SaveAs(bool tmp_fname_save, bool make_copy) {
  // do a first preliminary fix, which will, ex., add the default extension
  SetFilerFlagState(TMP_SAVE_FILE, tmp_fname_save);

  // tag the filename with _copy so original file is not overwritten
  if (make_copy) {
    String copyName = FileNameForSaveAs();
    SetFileName(copyName);
  }
  FixFileName();

  bool wasFileChosen = GetFileName(foSaveAs);

  ostream* rstrm = NULL;
  if (wasFileChosen) {
    file_selected = true;
    // we fix again, because of compression
    FixFileName();
    if (open_write()) {
      if (select_only)
        Close();
      else {
        rstrm = ostrm;
      }
    }
  }
  return rstrm;
}

ostream* taFiler::Append() {
  bool wasFileChosen = GetFileName(foAppend);

  ostream* rstrm = NULL;
  if (wasFileChosen) {
    file_selected = true;
    if (open_append()) {
      if (select_only)
        Close();
      else
        rstrm = ostrm;
    }
  }
  return rstrm;
}

void taFiler::SetFileName(const String& value) {
  QFileInfo fi(value);
  m_fname = fi.fileName();
  String tdir = fi.path();
  // if no path specified, assume current, else write it
  if (tdir.empty())
    m_dir = ".";
  else
    m_dir = tdir;
}

namespace taFilerUtil
{
  String DottedExtension(const String &extension)
  {
    // Ensure the extension (if any) begins with a dot.
    if (extension.nonempty() && extension[0] != '.')
    {
      return "." + extension;
    }
    return extension;
  }

  String UndottedExtension(const String &extension)
  {
    // Remove the leading dot from the extension (if there is one).
    if (extension.nonempty() && extension[0] == '.')
    {
      return extension.from(1);
    }
    return extension;
  }

  String GetFiletype(int idx, const String_PArray &filetypes)
  {
    String type = filetypes.SafeEl(idx);
    if (type.empty())
    {
      // No filetype at that index, so use the first filetype in the list.
      type = filetypes.SafeEl(0);
    }
    if (type.empty())
    {
      // Still empty, so use "? files".
      type = "?";
    }
    return type;
  }

  String GetDottedExtension(int idx, const String_PArray &extensions)
  {
    String ext = extensions.FastEl(idx);
    return taFilerUtil::DottedExtension(ext);
  }

  String MakeFilter(const String &filetype, const String &extension, bool is_compress_enabled)
  {
    // note: it's ok if extension is empty
    String filter = filetype + " files (*" + extension;
    if (is_compress_enabled)
    {
      filter.cat(" *").cat(extension).cat(taMisc::compress_sfx);
    }
    filter.cat(")");
    return filter;
  }

  void AddFilter(const String &new_filter, String &filters, QStringList *list)
  {
    if (filters.nonempty())
    {
      filters.cat(";;");
    }
    filters.cat(new_filter);
    if (list)
    {
      *list << new_filter;
    }
  }
}

#ifdef TA_NO_GUI
  //////////////////////////
  //   taFiler            //
  //////////////////////////

  // this is not called in the NO_GUI version, see ta_qtdialog.cpp
  bool taFiler::GetFileName(FileOperation filerOperation) {
    return false;
  }
#endif // TA_NO_GUI


///////////////////////////////////////////////////
//      Qt Code...

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
#if (QT_VERSION >= 0x050000)
  fd->setNameFilters(filter_list);
#else
  fd->setFilters(filter_list);
#endif

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
