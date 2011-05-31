#ifndef LIBLINEAR_H
#define LIBLINEAR_H

#include "LIBLINEAR_def.h"
#include "ta_base.h" 
#include "ta_datatable.h"
#include "ta_matrix.h"

void DestroyProblem(struct problem *&p);
struct problem* CreateProblem(DataTable* data, String y_col);
struct parameter* CreateParameter(String solver);
bool CheckParameter(const problem *prob, const parameter *param);

class LIBLINEAR_API LIBLINEAR : public taNBase {
  
  INHERITED(taNBase) 
public:

  TA_SIMPLE_BASEFUNS(LIBLINEAR);

  bool Train(DataTable* data, String y_col, String solver = "MCSVM_CS"); // #BUTTON

protected:
  
  override void		UpdateAfterEdit_impl(); 
private:

  
  void	Initialize();
  void	Destroy();
};


class LIBLINEAR_API LIBLINEARPluginState : public taFBase {
  
  INHERITED(taFBase)
public:
  static LIBLINEARPluginState* instance(); 
  SIMPLE_LINKS(LIBLINEARPluginState)
  TA_BASEFUNS(LIBLINEARPluginState)
protected:
  override void		UpdateAfterEdit_impl();
private:
  SIMPLE_COPY(LIBLINEARPluginState)
  void	Initialize();
  void	Destroy();
};

#endif
