#ifndef DataVarWrite_h
#define DataVarWrite_h 1

// parent includes:
#include <DataVarSimple>

// member includes:

// declare all other types mentioned but not required to include:


taTypeDef_Of(DataVarWrite);

class TA_API DataVarWrite : public DataVarSimple {
  // A program element for writing to a data table -- scalar var/col and matrix var/col supported, and enum to/from Matrix localist code also supported (see DataVarWriteMatrix for accessing individual matrix cells)
INHERITED(DataVarSimple)
public:
  String	GetDisplayName() const override;
  String  GetToolbarName() const override { return "write to data table"; }

  PROGEL_SIMPLE_BASEFUNS(DataVarWrite);
protected:

private:
  void	Initialize();
  void	Destroy()	{ CutLinks(); }
};

#endif // DataVarWrite_h
