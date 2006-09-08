

class ViewDataBase: public taOBase {
public:
  void*		data; // for schema, this is the class (TypeDef) for which it is referring; for data, it is data object, or NULL if self (makes it easier to distinguish default/self vs. client items)
  String	key; // #READ_ONLY #SHOW
  String	desc;
  
  virtual bool IsSimple() const {return false;} // only overridden for Simple guy
};

class ViewData_PList: // ptrlist, for typedefs

class ViewData_List; // tabse list, for tab

// simple guy -- useful for all the standard scalar types, also used for "GetValue" apis

class ViewData_Variant: public ViewDataBase {
public:
  Variant	value;
  
  bool IsSimple() const {return true;}
};



// examples of higher-level guys:

class ViewData_Label: public ViewDataBase {
public:
  coord3d_guy	coord;
  String	value;
};





//CHANGES TO TypeDef etc.


TypeDef:
  ViewItem_PList view_schema; // on demand-created list
  
  void AddSchemaItem
  
  void AddSchemaItem(ViewItem*, TypeDef* client = NULL) ; // adds the item, 
  const SchemaItem* FindSchemaItem(key, TypeDef* client = NULL); // finds the item for the key, NULL if none
  
functionality to add:
* adding a parent needs to copy links to all parent schema


// Changes to taBase


// Destructor changes:
// * need to iterate view data, find 
// virtual support:
ViewItem_List* GetViewItemList(bool force_create); // generic
 // def: {return StatGetViewItemList(this, fc);}
// Generic class support:
  class level list of ViewItem_Lists, hashed on object address
//  generic GetViewItemList delegates to:
ViewItem_List* StatGetViewItemList(taBase* instance, bool force_create); // generic
  // ex.: {static VI_L* list = NULL; if (force_create && !list) make list; 
    // if (!list) return NULL; rval = list->Find(instance); if no, create and add, then return}
  

// typical

// Schema:

// Instance:
const Variant GetViewValue(key, data_item = NULL) const; // simple api for getting scalar data from simple vd
// looks for instance, if none, looks for schema, if yes, return default value, else return nil

void SetViewValue(const Variant&, key, data_item = NULL) const; // simple api for setting scalar data to simple vd
// looks for instance, if so, sets value, if not, creates instance w/ value and adds

const SchemaItem* FindViewItem(key, data_item = NULL); // finds the item for the key, NULL if none
void  AddViewItem(SchemaItem*, key, data_item = NULL); // adds the item, if di, also sets link there
void LinkViewItem(ViewItem*); // for linking by a viewer/owner

VIEW_DATA() macro
  smartptr for list
  accessor func for list
  
  