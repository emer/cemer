#include "LIBLINEAR.h"

#include "ta_project.h"
#include "liblinear-1.8/linear.h"
#include <vector>

void LIBLINEAR::Initialize() {}
void LIBLINEAR::Destroy() {CutLinks();}
void LIBLINEAR::UpdateAfterEdit_impl() {inherited::UpdateAfterEdit_impl();}

void DestroyProblem(struct problem *&p) {
  if (p) {
    if (p->x) {
      for (int i = 0; i < p->l; ++i) {
        free(p->x[i]);
        p->x[i] = 0;
      }
      free(p->x);
      p->x = 0;
    }
    free(p);
    p = 0;
  }
}

struct problem* CreateProblem(DataTable* data, String y_col) {

  struct problem *p = (struct problem *)malloc(sizeof(struct problem));
  if (!p) return 0;
  p->x = (struct feature_node **)malloc(data->rows * sizeof(struct feature_node *));
  if (!p->x) {
    DestroyProblem(p);
    return 0;

  }

  int y_col_idx = data->FindColNameIdx(y_col);

  p->y = (int*)data->data[y_col_idx]->AR()->data();
  p->l = 0; // Will be set on the fly as rows are allocated.
  p->n = data->cols();
  p->bias = -1;

  const double epsilon = .00000000001;
  std::vector<struct feature_node> fn_vec;
  for (int i = 0; i < data->rows; ++i) {

    // Collect non-zero row elements in a temporary vector.
    fn_vec.clear();
    for (int j = 0; j < data->cols(); ++j) {

      if (j == y_col_idx) continue; // Ignore the column used as labels.

      // Only put non-zero values into the sparse matrix.
      double d = data->GetValAsDouble(j, i);
      if (fabs(d) > epsilon) {
        struct feature_node fn = { j, d };
        fn_vec.push_back(fn);

      }
    }

    // Alloc the array of feature_nodes.
    p->x[i] = (struct feature_node *)malloc((fn_vec.size() + 1) * sizeof(struct feature_node));
    if (!p->x[i]) {
      DestroyProblem(p);
      return 0;
    }

    // Copy the temp vector values into the malloc'd array.
    copy(fn_vec.begin(), fn_vec.end(), p->x[i]);

    // Add the -1 sentinel value at the end of the array.
    p->x[i][fn_vec.size()].index = -1;

    // Keep track of how many rows have been created.
    ++p->l;

  }

  return p;
}

struct parameter* CreateParameter(String solver) {

  struct parameter *param = (struct parameter *)malloc(sizeof(struct parameter));

  // Determine user-defined solver type
  int st = 0;
  double eps = 0.0;

  if      (solver == "L2R_LR")              { st = L2R_LR;              eps = .01; }
  else if (solver == "L2R_L2LOSS_SVC_DUAL") { st = L2R_L2LOSS_SVC_DUAL; eps = .1;  }
  else if (solver == "L2R_L2LOSS_SVC")      { st = L2R_L2LOSS_SVC;      eps = .01; }
  else if (solver == "L2R_L1LOSS_SVC_DUAL") { st = L2R_L1LOSS_SVC_DUAL; eps = .1;  }
  else if (solver == "MCSVM_CS")            { st = MCSVM_CS;            eps = .1;  }
  else if (solver == "L1R_L2LOSS_SVC")      { st = L1R_L2LOSS_SVC;      eps = .01; }
  else if (solver == "L1R_LR")              { st = L1R_LR;              eps = .01; }
  else if (solver == "L2R_LR_DUAL")         { st = L2R_LR_DUAL;         eps = .1;  }
  else return false;

  param->solver_type = st;
  param->eps = eps;
  param->C =  1; // cost of constraints violation - must be > 0, default = 1
  param->nr_weight = 0; // don't change penalty for any class

  return param;

}

bool LIBLINEAR::Train(DataTable* data, String y_col, String solver) {

  struct problem *prob = CreateProblem(data, y_col);
  struct parameter *param = CreateParameter(solver);

  if (check_parameter(prob, param) != NULL) {
    taMisc::Error("paramter struct not setup correctly. programmer error - please report.");
    DestroyProblem(prob); // Use their impl?
    //destroy_param(param);
    return false;
  }

  struct model *m = train(prob, param);

  DestroyProblem(prob); // Use their impl?
  //free_model_content(model);
  //free_and_destroy_model(*model);
  //destroy_param(param);
  return true;
}

LIBLINEARPluginState* LIBLINEARPluginState::instance() {
  if (tabMisc::root == NULL) return NULL;
  LIBLINEARPluginState* rval = (LIBLINEARPluginState*)
    tabMisc::root->plugin_state.FindType(StatTypeDef(0));
  return rval;
}

void LIBLINEARPluginState::Initialize() {SetBaseFlag(NAME_READONLY);}
void LIBLINEARPluginState::Destroy() {CutLinks(); }
void LIBLINEARPluginState::UpdateAfterEdit_impl() {inherited::UpdateAfterEdit_impl();}
