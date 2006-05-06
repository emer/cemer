/* Notes

   Dimension accessor routines
   
   Most functions have three variations:
     Xxx(int d0, int d1=0, int d2=0, int d3=0)
     XxxN(const MatrixGeom& indices)
     Xxx_Flat(int idx)
   The dimensionality of the matrix always controls access -- extra supplied
   dimension indices are ignored. Matrixes can have more than 4 dimensions
   (up to TA_MATRIX_DIMS_MAX) but 4 is usually sufficient for most purposes.
   If you need to access more than 4, you will have to use a MatrixGeom object
   to hold your indices.
   The "_Flat" variation enables efficient access of the elements as a flat
   1-d array; idx can have any value up to count-1.

*/

class TA_API IMatrix: public virtual ITypedObject { // #VIRT_BASE #NO_INSTANCE #NO_TOKENS multi-dimensional data array interface
public:
  virtual int		count() const = 0; // number of elements, for flat access
  virtual int		dims() const = 0; // number of dimensions, >= 1
  virtual int		dim(int dim) const = 0; // size of given dimension
  virtual int 		frames() const = 0;	// number of frames currently in use (value of highest dimension) 
  virtual int 		frameSize() const = 0;	// number of elements in each frame (product of inner dimensions) 
  virtual bool		isFixedData() const = 0; // true if using fixed (externally managed) data storage
  
  virtual TypeDef*	GetDataTypeDef() const = 0; // type of data, ex TA_int, TA_float, etc.
  
  virtual const String	SafeElAsStr(int d0, int d1=0, int d2=0, int d3=0) const = 0; 
    // (safely) returns the element as a string
  virtual const String	SafeElAsStrN(const MatrixGeom& indices) const = 0; 
    // (safely) returns the element as a string
  virtual const String	SafeElAsStr_Flat(int idx) const = 0; 
    // (safely) returns the element as a string, interpreting the matrix as a flat array
  virtual void		SetFmStr(const String& str, int d0, int d1=0, int d2=0, int d3=0) = 0;
    // (safely) sets the element as a string
  virtual void		SetFmStrN(const String& str, const MatrixGeom& indices) = 0;
    // (safely) sets the element as a string
  virtual void		SetFmStr_Flat(const String& str, int idx) = 0;
    // (safely) sets the element as a string, interpreting the matrix as a flat array
    
  virtual const Variant	SafeElAsVar(int d0, int d1=0, int d2=0, int d3=0) const = 0; 
    // (safely) returns the element as a variant
  virtual const Variant	SafeElAsVarN(const MatrixGeom& indices) const = 0; 
    // (safely) returns the element as a variant
  virtual const Variant	SafeElAsVar_Flat(int idx) const = 0; 
    // (safely) returns the element as a variant
  virtual void		SetFmVar(const Variant& var, int d0, int d1=0, int d2=0, int d3=0) = 0;
    // (safely) sets the element as a string
  virtual void		SetFmVarN(const Variant& var, const MatrixGeom& indices) = 0;
    // (safely) sets the element as a string
  virtual void		SetFmVar_Flat(const Variant& var, int idx) = 0;
    // (safely) sets the element as a string
    
  virtual bool		StrValIsValid(const String& str, String* err_msg = NULL) const
    {return true;}
    // validates a proposed string-version of a value, ex. float_Matrix can verify valid floating rep of string 
     

  virtual void		AddFrames(int n); // add n new blank frames
  virtual void		AllocFrames(int n); // make sure space exists for n frames
  virtual void		EnforceFrames(int n); // set size to n frames, blanking new elements if added
  virtual void		RemoveFrame(int n); // remove the given frame, copying data backwards if needed
  virtual void		Reset() {EnforceFrames(0);}
  
  virtual bool		InRange(int d0, int d1=0, int d2=0, int d3=0) const;  // 'true' if indices in range
  virtual bool		InRangeN(const MatrixGeom& indices) const;  // 'true' if >= indices-d and indices in range
  
  virtual void		SetGeom(int num, int d0, int d1=0, int d2=0, int d3=0) = 0;
    // set geom 
  virtual void		SetGeomN(const MatrixGeom& geom_) = 0;
   // #MENU #MENU_ON_Matrix set geom for any sized array
  
//nn  virtual ~IMatrix() {}

protected:
  virtual int		SafeElIndex(int d0, int d1=0, int d2=0, int d3=0) const = 0; 
  virtual int		SafeElIndexN(const MatrixGeom& indices) const = 0; 
};