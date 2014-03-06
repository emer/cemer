#ifndef DataVarRead_h
#define DataVarRead_h 1

// parent includes:
#include <DataVarSimple>

// member includes:

// declare all other types mentioned but not required to include:


taTypeDef_Of(DataVarRead);

class TA_API DataVarRead : public DataVarSimple {
  // A program element for reading from a data table -- scalar var/col and matrix var/col supported, and enum to/from Matrix localist code also supported (see DataVarReadMatrix for accessing individual matrix cells)
INHERITED(DataVarSimple)
public:
  String	GetDisplayName() const override;
  String  GetToolbarName() const override { return "read from \n data table"; }

  bool          CanCvtFmCode(const String& code, ProgEl* scope_el) const override;
  bool          CvtFmCode(const String& code) override;

  PROGEL_SIMPLE_BASEFUNS(DataVarRead);
protected:

private:
  void	Initialize();
  void	Destroy()	{ CutLinks(); }
};

#endif // DataVarRead_h
