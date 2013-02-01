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

#include "iDialogRegexp.h"
#include <iDialogRegexpPopulator>
#include <iTableView>
#include <CellRange>
#include <taiWidgetFieldRegexp>
#include <iLineEdit>

#include <taMisc>
#include <taiMisc>

#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QHeaderView>
#include <QScrollBar>
#include <QDialogButtonBox>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <QKeyEvent>


const QString iDialogRegexp::DOT_STAR(".*");

iDialogRegexp::iDialogRegexp(taiWidgetFieldRegexp* regexp_field, const String& field_name, iDialogRegexpPopulator *re_populator, const void *fieldOwner, bool read_only,
                             bool editor_mode)
  : inherited()
  , m_field(regexp_field)
  , m_populator(re_populator)
  , m_fieldOwner(fieldOwner)
  , m_read_only(read_only)
  , m_editor_mode(editor_mode)
  , m_apply_clicked(false)
  , m_proxy_model(0)
  , m_regexp_list(0)
  , m_regexp_line_edit(0)
  , m_regexp_combos()
  , m_table_model(NULL)
  , m_table_view(NULL)
  , btnAdd(0)
  , btnDel(0)
  , btnApply(0)
  , btnReset(0)
  , m_button_box(NULL)
{
  if (!m_populator) {
    // Shouldn't happen.
    return;
  }

  // Size the dialog.
  resize(taiM->dialogSize(taiMisc::dlgBig | taiMisc::dlgHor));
  setFont(taiM->dialogFont(taiM->ctrl_size));

  // Create the table model (should be done before its view is created).
  CreateTableModel();

  // Create layout
  QVBoxLayout *vbox = new QVBoxLayout(this);
  LayoutInstructions(vbox, field_name.toQString());
  LayoutRegexpList(vbox);
  QHBoxLayout *hbox_combos = LayoutEditBoxes(vbox);
  LayoutTableView(vbox, hbox_combos);
  LayoutButtons(vbox);

  // "Reset" the form with the text from the field.  Do this at the bottom,
  // since RegexpSelectionChanged() will be triggered.
  btnReset_clicked();
}

void iDialogRegexp::LayoutInstructions(QVBoxLayout *vbox, QString field_name)
{
  // Dialog title.
  QString title("Editing regular-expression field");
  if (!field_name.isEmpty()) {
    title.append(" \"").append(field_name).append("\"");
  }
  if (m_fieldOwner) {
    m_populator->adjustTitle(title, m_fieldOwner);
  }
  setWindowTitle(title);

  // Instructions.
  QLabel *instr = new QLabel;
  instr->setWordWrap(true);
  instr->setText(
    "Instructions: This dialog helps you construct a filter based on 'wildcard' or regular expressions that match a variable number of items in each field.  Each field in the filter expression is typically separated by a '/', and between those separators, there is a filter for each field.  Common regular expressions are '.*' which matches anything, '[a-z]' which matches any lowercase letter, '[a-z]*' which matches any number of lowercase letters, ^ = start of field, $ = end of field -- see http://en.wikipedia.org/wiki/Regular_expression#POSIX for details.  You can select items from fields using the drop-down menus, or type directly in the fields."
  );
  vbox->addWidget(instr);
}

void iDialogRegexp::LayoutRegexpList(QVBoxLayout *vbox)
{
  // Add/delete buttons to the left, list of regexps to the right.
  QHBoxLayout *list_hbox = new QHBoxLayout;
  list_hbox->setContentsMargins(0, 0, 0, 0);
  vbox->addLayout(list_hbox);

  // Arrange buttons vertically.
  QVBoxLayout *add_del_vbox = new QVBoxLayout;
  add_del_vbox->setContentsMargins(0, 0, 0, 0);
  list_hbox->addLayout(add_del_vbox);

  int button_width = 25;
  btnAdd = new QPushButton("+");
  btnAdd->setMaximumWidth(button_width);
  btnAdd->setEnabled(!isReadOnly());
  add_del_vbox->addWidget(btnAdd);

  btnDel = new QPushButton("-");
  btnDel->setMaximumWidth(button_width);
  btnDel->setEnabled(!isReadOnly());
  add_del_vbox->addWidget(btnDel);

  add_del_vbox->addStretch();

  // Connect add/del button clicks to our SLOTs.
  connect(btnAdd, SIGNAL(clicked()), this, SLOT(btnAdd_clicked()));
  connect(btnDel, SIGNAL(clicked()), this, SLOT(btnDel_clicked()));

  // Create a list widget to display the regexp parts.
  // The list will be populated later.
  m_regexp_list = new QListWidget;
  m_regexp_list->setEditTriggers(QAbstractItemView::NoEditTriggers);
  m_regexp_list->setSelectionMode(QAbstractItemView::ExtendedSelection);

  // We don't need a lot of room in the list widget, so size it based on
  // how much size the buttons require.
  m_regexp_list->setFixedHeight(add_del_vbox->minimumSize().height());

  list_hbox->addWidget(m_regexp_list);

  // Connect to the signal issued when the selection of regexp is changed.
  connect(m_regexp_list, SIGNAL(itemSelectionChanged()),
          this, SLOT(RegexpSelectionChanged()));
}

QHBoxLayout *iDialogRegexp::LayoutEditBoxes(QVBoxLayout *vbox)
{
  // Line-edit for the regexp.
  m_regexp_line_edit = new QLineEdit;
  vbox->addWidget(m_regexp_line_edit);

  // Connect to the signal issued when the regexp is edited
  // using the single line-edit box.
  connect(m_regexp_line_edit, SIGNAL(editingFinished()),
          this, SLOT(RegexpLineEdited()));

  // Layout for the combo-boxes.
  QHBoxLayout *hbox_combos = new QHBoxLayout;
  hbox_combos->setContentsMargins(0, 0, 0, 0);
  hbox_combos->setSpacing(1);
  vbox->addLayout(hbox_combos);

  // Create combo-boxes.
  for (int part = 0; part < m_num_parts; ++part) {
    // Create a combo-box.
    QComboBox *combo = new QComboBox;
    combo->setEditable(true);
    combo->setInsertPolicy(QComboBox::NoInsert);
    combo->setMinimumContentsLength(8);
    combo->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);

    // Add the combo-box to the widget,
    hbox_combos->addWidget(combo);
    // and also to a list for future reference.
    m_regexp_combos << combo;

    // Connect to the signal issued after the user chooses a combo-box item.
    connect(combo, SIGNAL(activated(int)),
            this, SLOT(RegexpPartChosen(int)));

    // Connect to the signal issued after the user manually enters text
    // in the combo-box.
    connect(combo->lineEdit(), SIGNAL(editingFinished()),
            this, SLOT(RegexpPartEdited()));
  }

  // Return the combos hbox so padding can be added in the next step.
  return hbox_combos;
}

void iDialogRegexp::LayoutTableView(
  QVBoxLayout *vbox, QHBoxLayout *hbox_combos)
{
  // Table view.  Set the model early so columns actions are valid.
  m_table_view = new iTableView;
  m_table_view->setModel(m_proxy_model);

  // Hide the full label column, it's only used for the m_proxy_model filter.
  m_table_view->setColumnHidden(LABEL_COL, true);

  // Allow the user to click column headers to sort the column.
  m_table_view->setSortingEnabled(true);

  // By default, sort by the index column.
  m_table_view->sortByColumn(INDEX_COL, Qt::AscendingOrder);

  if(m_editor_mode) {
    // leave this at default setting:
    // m_table_view->setEditTriggers(QAbstractItemView::AllEditTriggers);
    m_table_view->setSelectionBehavior(QAbstractItemView::SelectItems);
  }
  else {
    m_table_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table_view->setSelectionBehavior(QAbstractItemView::SelectRows);
  }

  // Disallow complex selections.
  m_table_view->setSelectionMode(QAbstractItemView::SingleSelection);

  // Keep the scrollbar visible at all times since the combo-boxes are
  // sized assuming it is present.  Hopefully this works on all platforms.
  m_table_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

  // Don't need to see the vertical headers since we have an index column.
  m_table_view->verticalHeader()->setVisible(false);

  // Add the table view with a stretch factor so it takes up any available
  // vertical space.
  vbox->addWidget(m_table_view, 1);

  // Customize the header row.
  QHeaderView *header = m_table_view->horizontalHeader();

  // Don't highlight the header cells when a selection is made (looks dumb).
  header->setHighlightSections(false);

  // Resize the index column to fit the contents.
#if (QT_VERSION >= 0x050000)
  header->setSectionResizeMode(INDEX_COL, QHeaderView::ResizeToContents);
#else
  header->setResizeMode(INDEX_COL, QHeaderView::ResizeToContents);
#endif

  // Resize other columns to take up all available space equally.
  int num_cols = m_num_parts + NUM_EXTRA_COLS;
  for (int col = NUM_EXTRA_COLS; col < num_cols; ++col) {
#if (QT_VERSION >= 0x050000)
    header->setSectionResizeMode(col, QHeaderView::Stretch);
#else
    header->setResizeMode(col, QHeaderView::Stretch);
#endif
  }

  // Pad the combos hbox to accomodate the index column and scrollbar.
  // This lines up the combo-boxes with their respective headers.
  hbox_combos->insertSpacing(0, header->sectionSize(INDEX_COL) + 2);
  int end_extra = 0;
  for(int part = 0; part < m_extra_cols; part++) {
    int col = NUM_EXTRA_COLS + m_num_parts + part;
    end_extra += header->sectionSize(col);
  }

  hbox_combos->addSpacing(end_extra + m_table_view->verticalScrollBar()->sizeHint().width() + 2);
}

void iDialogRegexp::LayoutButtons(QVBoxLayout *vbox)
{
  // Create a button box with OK, Cancel, Apply, Reset.
  // Save pointers to the Apply/Reset buttons so they can be
  // programmatically enabled/disabled.
  m_button_box = new QDialogButtonBox;
  vbox->addWidget(m_button_box);

  // Create either a cancel or close button,
  // depending on whether the dialog is read-only.
  QDialogButtonBox::StandardButton cancel =
    isReadOnly() ? QDialogButtonBox::Close : QDialogButtonBox::Cancel;
  m_button_box->addButton(QDialogButtonBox::Ok);
  m_button_box->addButton(cancel);
  btnApply = m_button_box->addButton(QDialogButtonBox::Apply);
  btnReset = m_button_box->addButton(QDialogButtonBox::Reset);

  // Connect the button-box buttons to our SLOTs.
  connect(m_button_box, SIGNAL(accepted()), this, SLOT(accept()));
  connect(m_button_box, SIGNAL(rejected()), this, SLOT(reject()));
  connect(btnApply, SIGNAL(clicked()), this, SLOT(btnApply_clicked()));
  connect(btnReset, SIGNAL(clicked()), this, SLOT(btnReset_clicked()));
}

void iDialogRegexp::CreateTableModel()
{
  // Get the list of labels to filter.
  m_populator->setSource(m_fieldOwner);
  QStringList headings = m_populator->getHeadings(m_editor_mode, m_extra_cols);
  QStringList labels = m_populator->getLabels();
  QString separator = m_populator->getSeparator();

  // Get the number of rows and columns.  Number of columns is based on
  // how many headings exist, plus any extra columns.
  int rows = labels.size();
  m_num_parts = headings.size() - m_extra_cols;
  int cols = m_num_parts + NUM_EXTRA_COLS + m_extra_cols;

  // Create table model
  m_table_model = new QStandardItemModel(rows, cols, this);

  // Determine how many characters in the string representation of
  // the number of rows, so we know how much padding to add.
  int field_width = QString::number(rows).size();

  for (int row = 0; row < rows; ++row) {
    // First column contains the label index
    // (base 10, padded with leading '0's).
    QStandardItem *item = new QStandardItem(
      QString("%1").arg(row, field_width, 10, QChar('0')));
    m_table_model->setItem(row, INDEX_COL, item);

    // Second column is hidden and contains the full label.
    // The regexp filter is applied to this column below.
    item = new QStandardItem(labels[row]);
    m_table_model->setItem(row, LABEL_COL, item);

    // Remaining columns contain the label parts.
    QStringList parts = labels[row].split(separator);
    for (int part = 0; part < parts.size(); ++part) {
      int col = part + NUM_EXTRA_COLS;
      if (col >= cols) break;
      item = new QStandardItem(parts[part]);
      item->setEditable(m_editor_mode);
      m_table_model->setItem(row, col, item);
    }
  }

  // Create a proxy model to filter the whole table.
  m_proxy_model = new QSortFilterProxyModel(this);
  m_proxy_model->setSourceModel(m_table_model);
  m_proxy_model->setFilterKeyColumn(LABEL_COL);

  // Set the header labels.
  m_proxy_model->setHeaderData(
    INDEX_COL, Qt::Horizontal, QString("Index"), Qt::DisplayRole);
  int part;
  for (part = 0; part < m_num_parts; ++part) {
    int col = part + NUM_EXTRA_COLS;
    m_proxy_model->setHeaderData(
      col, Qt::Horizontal, headings[part], Qt::DisplayRole);
  }
  for(; part < m_num_parts + m_extra_cols; part++) {
    int col = part + NUM_EXTRA_COLS;
    m_proxy_model->setHeaderData(
      col, Qt::Horizontal, headings[part], Qt::DisplayRole);
  }

  if(m_editor_mode) {
    connect(m_table_model, SIGNAL(itemChanged(QStandardItem*)),
            this, SLOT(TableItemChanged(QStandardItem*)) );
  }
}

void iDialogRegexp::keyPressEvent(QKeyEvent *e)
{
  // don't accept on enter
  bool ctrl_pressed = taiMisc::KeyEventCtrlPressed(e);
  bool is_enter = e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return;
  if (!ctrl_pressed && is_enter) {
    e->accept(); // just bail!
    return;
  }

  iDialog::keyPressEvent(e);
}

void iDialogRegexp::accept()
{
  if (!isReadOnly()) {
    btnApply_clicked();
  }

  inherited::accept();
}

void iDialogRegexp::TableItemChanged(QStandardItem* item) {
  int row = item->row();
  int col = item->column();
  if(col < NUM_EXTRA_COLS || col >= m_num_parts) return; // only handle labels

  // when anything changes, we need to re-compute the global label column for that row..
  QString separator = m_populator->getSeparator();
  QString nwlbl;
  for(int part=0; part < m_num_parts; part++) {
    QStandardItem* itm = m_table_model->item(row, part + NUM_EXTRA_COLS);
    nwlbl.append(itm->text());
    if(part < m_num_parts-1)
      nwlbl.append(separator);
  }
  QStandardItem* lbli = m_table_model->item(row, LABEL_COL);
  lbli->setText(nwlbl);
}

// Handle the "Add" button push.
void iDialogRegexp::btnAdd_clicked()
{
  // Enable the apply and reset buttons.
  setApplyEnabled(true);

  // Build a default regexp that will match anything between
  // the separators, such as:
  //   ".*\\..*\\..*\\..*\\..*"
  QStringList parts;
  for (int i = 0; i < m_regexp_combos.size(); ++i) {
    parts << DOT_STAR;
  }

  // Add the new regexp to the list and select it.
  m_regexp_list->addItem(JoinRegexp(parts));

  // This will trigger RegexpSelectionChanged().
  m_regexp_list->setCurrentRow(m_regexp_list->count() - 1,
    QItemSelectionModel::ClearAndSelect);
}

// Handle the "Delete" button push.
void iDialogRegexp::btnDel_clicked()
{
  // Enable the apply and reset buttons.
  setApplyEnabled(true);

  // Get the list of regexp(s) to delete.
  QList<QListWidgetItem *> selected_items = m_regexp_list->selectedItems();
  foreach (QListWidgetItem *item, selected_items) {
    // Deleting the items will remove them from the list.
    // It will also trigger RegexpSelectionChanged().
    delete item;
  }
}

void iDialogRegexp::btnApply_clicked()
{
  // Disable the apply and reset buttons.
  setApplyEnabled(false);

  // Combine all the regular expressions together.
  QList<QListWidgetItem *> all_items;
  for (int row = 0; row < m_regexp_list->count(); ++row) {
    all_items << m_regexp_list->item(row);
  }
  QString full_regexp = JoinRegexpAlternatives(all_items);

  if(m_field) {
    // Set the resulting regular expression in the field.  It will be
    // applied later, when the dialog is closed.
    m_field->rep()->setText(full_regexp);
    m_apply_clicked = true;
  }
  else {
    field_value = full_regexp;
  }

  if(m_editor_mode) {           // we now save changes back to guy..
    QStringList labels;
    int rows = m_table_model->rowCount();
    for(int row=0; row < rows; ++row) {
      QStandardItem* item = m_table_model->item(row, LABEL_COL);
      labels.append(item->text());
    }
    m_populator->setLabels(labels);
  }
}

void iDialogRegexp::btnReset_clicked()
{
  // Disable the apply and reset buttons.
  setApplyEnabled(false);

  // Get all the "OR" alternatives of the regexp (separated by |s)
  // and put them into the list.
  QString full_regexp;
  if(m_field) {
    full_regexp = m_field->rep()->text();
  }
  else {
    full_regexp = field_value.chars();
  }
  QStringList regexp_strings = SplitRegexpAlternatives(full_regexp);
  m_regexp_list->clear();
  m_regexp_list->addItems(regexp_strings);

  // Do this at the bottom, since RegexpSelectionChanged() will be triggered.
  if (m_regexp_list->count() == 0) {
    btnAdd_clicked();
  }
  else {
    m_regexp_list->setCurrentRow(0);
  }
}

// This function is called when a combo-box item has been selected
// to ensure the line-edit is kept in sync with the regexp-escaped
// text.
void iDialogRegexp::SelectCombo(QComboBox *combo, int index)
{
  // Ensure that the indicated item is actually selected.
  // This isn's needed when the user has explicitly chosen an item
  // from the combo-box (e.g., with the mouse), but is helpful when
  // the combo-box text is being set programmatically.
  combo->setCurrentIndex(index);

  // Set the combo-box's line-edit to the regexp-escaped version
  // of the selected string, which is stored in the UserRole
  // (hence, itemData is called vs. itemText).
  QVariant data = combo->itemData(index);
  QString escaped_string = data.toString();
  if (QLineEdit *line = combo->lineEdit()) {
    line->setText(escaped_string);
  }
}

// This function is called when the user chooses a combo-box item.
void iDialogRegexp::RegexpPartChosen(int index)
{
  // Get the combo-box that sent this signal.
  if (QComboBox *combo = dynamic_cast<QComboBox *>(sender())) {
    // Update its line-edit to reflect the escaped text.
    SelectCombo(combo, index);
  }

  // Choosing a combo-box item changes a part of the regexp,
  // so process that change.
  RegexpPartEdited();
}

// This function is called when a part of the regexp has been edited.
// This can occur by manual typing or by activating the combo-box.
void iDialogRegexp::RegexpPartEdited()
{
  QStringList regexp_parts;
  foreach (QComboBox *combo, m_regexp_combos) {
    // Get the string from the line-edit part of the combo-box.
    // Don't call itemText() here because that gives the DisplayRole
    // text, and we want the escaped text from the UserRole.
    // The SelectCombo() function ensures the combo-box's line-edit
    // is in sync with the UserRole text.
    regexp_parts << combo->lineEdit()->text();
  }

  // Join the parts together into the full regexp.
  QString new_text = JoinRegexp(regexp_parts);

  // Check if the glued-together parts differ from the existing text.
  if (new_text != m_regexp_line_edit->text()) {
    // Update the single line-edit box and propagate the updates
    // as if the user edited it manually.
    m_regexp_line_edit->setText(new_text);
    RegexpLineEdited();
  }
}

// This function is called when the user edits the text of the regexp
// using the single line-edit box.  It's also called when a part of the
// regexp has changed that results in a change to the whole regexp.
void iDialogRegexp::RegexpLineEdited()
{
  // Enable the apply and reset buttons.
  setApplyEnabled(true);

  // Get the selected item in the regexp list so its text can be updated.
  // There should be exactly one item selected, but check just in case.
  QList<QListWidgetItem *> selected_items = m_regexp_list->selectedItems();
  if (selected_items.isEmpty()) {
    // This should never happen.  No item was selected, but the update
    // should go somewhere, so make a new item, select it, and quit.
    m_regexp_list->addItem(m_regexp_line_edit->text());
    m_regexp_list->setCurrentRow(m_regexp_list->count() - 1);
    return;
  }

  // Set the line-edit text to the selected regular expression.
  selected_items[0]->setText(m_regexp_line_edit->text());

  // Call the function that handles when the user makes a new selection,
  // just to make sure everything is in sync.
  RegexpSelectionChanged();
}

// This function is called when the user selects a different regexp(s).
// Multiple regexps may be selected, but editing is only enabled if
// exactly one regexp is selected.  This function is also called to
// keep everything in sync after the current regexp has been edited.
void iDialogRegexp::RegexpSelectionChanged()
{
  // Get a list of currently selected regexp(s).
  QList<QListWidgetItem *> selected_items = m_regexp_list->selectedItems();

  // Only enable the delete button if there is something selected to delete.
  btnDel->setEnabled(!isReadOnly() && !selected_items.isEmpty());

  // If one thing selected, enable the edit boxes and populate them according to the selection.
  if (selected_items.size() == 1) {
    QString regexp = selected_items[0]->text();
    BuildCombos(regexp);
    EnableEditBoxes(regexp);
  }
  // Otherwise, disable them.
  else {
    m_regexp_line_edit->clear();
    m_regexp_line_edit->setEnabled(false);
    foreach (QComboBox *combo, m_regexp_combos) {
      combo->lineEdit()->setText("");
      combo->setEnabled(false);
    }
  }

  // Filter the table view based on the currently selected items.
  ApplyFilters(selected_items);
}

void iDialogRegexp::BuildCombos(QString regexp)
{
  // Loop over the combo boxes.
  for (int part = 0; part < m_regexp_combos.size(); ++part) {
    // Get filtered choices for this combo-box.
    QStringList choices = GetComboChoices(regexp, part);

    // Populate its values, starting with the wildcard.
    QComboBox *combo = m_regexp_combos[part];
    combo->clear();
    combo->addItem("<match any>", DOT_STAR);
    foreach (QString choice, choices) {
      // Set the choice as DisplayRole and the regular-expression-escaped
      // choice as UserRole.
      combo->addItem(choice, QRegExp::escape(choice));
    }
  }
}

QStringList iDialogRegexp::GetComboChoices(QString regexp, int part)
{
  // Split the full regexp into parts.
  QStringList regexp_parts = SplitRegexp(regexp);

  // Filter each combo-box's choices as if that combo was currently wild.
  if (part < regexp_parts.size()) {
    // Adjust the current regexp part and join the parts back together.
    regexp_parts[part] = DOT_STAR;
    regexp = JoinRegexp(regexp_parts);
  }

  // Make a QRegExp object based on the resulting string.
  const QRegExp temp_regexp(regexp);

  // Check each label for a match.  Build up a set of choices for this
  // combo box based on matching labels.
  QStringList labels = m_populator->getLabels();
  QString separator = m_populator->getSeparator();

  QSet<QString> part_choices;
  foreach (QString label, labels) {
    // If current label matches,
    if (label.contains(temp_regexp)) {
      // Break it into parts.
      QStringList parts = label.split(separator);
      if (part < parts.size()) {
        // Add the part as a choice.
        part_choices << parts[part];
      }
    }
  }

  // Sort the choices for this combo-box.
  QStringList strings = part_choices.toList();
  strings.sort();
  return strings;
}

void iDialogRegexp::EnableEditBoxes(QString regexp)
{
  // Set the single line-edit box to the full regexp and enable it.
  m_regexp_line_edit->setText(regexp);
  m_regexp_line_edit->setEnabled(!isReadOnly());

  // Split the full regexp into parts.
  QStringList regexp_parts = SplitRegexp(regexp);

  // Iterate through the combo-boxes and regexp parts.
  for (int part = 0; part < m_regexp_combos.size(); ++part) {
    // First, enable the combo-box.
    QComboBox *combo = m_regexp_combos[part];
    combo->setEnabled(!isReadOnly());

    // If there is a regexp part associated with this combo-box,
    // then set it.
    if (part < regexp_parts.size()) {
      // Check if the regexp part occurs in the UserRole (regexp-escaped)
      // string of any of the combo-box entries.  Or, if the regexp part
      // is empty, then make it match anything (which is at index 0).
      int idx = (regexp_parts[part] == "")
                ? 0 : combo->findData(regexp_parts[part]);

      // If a match was found, then select it.
      if (idx >= 0) {
        SelectCombo(combo, idx);
      }
      // Otherwise add a <custom> choice to the combo, and select it.
      else {
        // Put it just below the <match any>.
        combo->insertItem(1, "<custom>", regexp_parts[part]);
        SelectCombo(combo, 1);
      }
    }
    // No regexp part for this combo-box, so just set it to match anything.
    else {
      SelectCombo(combo, 0);
    }
  }
}

void iDialogRegexp::ApplyFilters(QList<QListWidgetItem *> selected_items)
{
  // Build the regexp and apply it to the proxy model.
  const QRegExp regexp(JoinRegexpAlternatives(selected_items));
  m_proxy_model->setFilterRegExp(regexp);
}

QStringList iDialogRegexp::SplitRegexpAlternatives(QString regexp)
{
  // Break apart the full regexp into its "OR" alternatives,
  // which are separated by |.
  if (regexp.startsWith('(') && regexp.endsWith(')')) {
    // Remove the parens.
    regexp.chop(1);
    regexp.remove(0,1);
  }
  return regexp.split("|", QString::SkipEmptyParts);
}

QString iDialogRegexp::JoinRegexpAlternatives(QList<QListWidgetItem *> items)
{
  // Combine all the selected regexp(s) together.  If more than one item
  // is selected, they will be combined with alternation (parens|and|bars).
  QString full_regexp_string;
  foreach (QListWidgetItem *item, items) {
    // Separate regexps by bars.
    if (!full_regexp_string.isEmpty()) {
      full_regexp_string.append("|");
    }
    full_regexp_string.append(item->text());
  }

  // Surround with parentheses if more than one alternative.
  if (items.size() > 1) {
    full_regexp_string.insert(0, "(");
    full_regexp_string.append(")");
  }

  return full_regexp_string;
}

QStringList iDialogRegexp::SplitRegexp(const QString &regexp)
{
  return regexp.split(GetEscapedSeparator());
}

QString iDialogRegexp::JoinRegexp(const QStringList &regexp_parts)
{
  // Join the parts together into the full regexp.
  return regexp_parts.join(GetEscapedSeparator());
}

QString iDialogRegexp::GetEscapedSeparator()
{
  QString separator = m_populator->getSeparator();
  return QRegExp::escape(separator);
}

void iDialogRegexp::setApplyEnabled(bool enabled)
{
  // Enable/disable the apply/reset buttons.
  btnApply->setEnabled(enabled);
  btnReset->setEnabled(enabled);
}

