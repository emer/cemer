#include "LIBLINEAR.h"

#include "ta_project.h"
#include "liblinear-1.8/linear.h"
#include <vector>

#define DEBUG false

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

void PrintProblem(struct problem *p) {

  cout << "p->l: " << p->l << "\n";
  cout << "p->n: " << p->n << "\n";
  cout << "p->bias: " << p->bias << "\n";
  taMisc::FlushConsole();

  cout << "p->y:";
  for (int i = 0; i < p->l; ++i)
    cout << p->y[i] << ";";

  taMisc::FlushConsole();

  cout << "p->x:";
  for (int i = 0; i < p->l; ++i)
    for (int j = 0; p->x[i][j].index != -1; ++j)
      cout << p->x[i][j].index << "," << p->x[i][j].value << ";";

  taMisc::FlushConsole();
}

void PrintParam(struct parameter *param) {
  cout << "param->solver_type: " << param->solver_type << "\n";
  cout << "param->eps: " << param->eps << "\n";
  cout << "param->C: " << param->C << "\n";
  cout << "param->nr_weight: " << param->nr_weight << "\n";
  taMisc::FlushConsole();
}

struct problem* CreateProblem(DataTable* data, String y_col) {

  struct problem *p = (struct problem *)malloc(sizeof(struct problem));
  if (!p) return false;
  p->x = (struct feature_node **)malloc(data->rows * sizeof(struct feature_node *));
  if (!p->x) {
    DestroyProblem(p);
    return false;

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
      return false;
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

bool ModelToDataTable(const struct model* m, DataTable* t) {
  // TODO: Check for m / t

  // model
  t->FindMakeCol("nr_class", taBase::VT_INT);
  t->FindMakeCol("nr_feature", taBase::VT_INT);
  t->FindMakeCol("bias", taBase::VT_DOUBLE);
  t->FindMakeColMatrix("label", taBase::VT_INT, 1, m->nr_class);
  t->FindMakeColMatrix("w", taBase::VT_DOUBLE, 1, m->nr_class*m->nr_feature);

  // param
  t->FindMakeCol("solver_type", taBase::VT_INT);
  t->FindMakeCol("eps", taBase::VT_DOUBLE);
  t->FindMakeCol("C", taBase::VT_DOUBLE);
  t->FindMakeCol("nr_weight", taBase::VT_INT);

  t->RemoveAllRows();
  t->AddRows();

  // model
  t->SetVal(m->nr_class, "nr_class", 0);
  t->SetVal(m->nr_feature, "nr_feature", 0);
  t->SetVal(m->bias, "bias", 0);
  
  for (int i=0; i < m->nr_class; ++i)
    t->SetMatrixVal(m->label[i], "label", 0, i);

  for (int i=0; i < m->nr_class*m->nr_feature; ++i)
    t->SetMatrixVal(m->w[i], "w", 0, i);

  // param
  t->SetVal(m->param.solver_type, "solver_type", 0);
  t->SetVal(m->param.eps, "eps", 0);
  t->SetVal(m->param.C, "C", 0);
  t->SetVal(m->param.nr_weight, "nr_weight", 0);
  
  return true;

}

struct model* DataTableToModel(DataTable* t) {
  // 9 cols / 1 row is a heuristic for a proper model table
  if (t->cols() != 9 || t->rows != 1) { 
    taMisc::Error("Model table either does not have 9 columns or does not have 1 row - invalid model table");
    return false;
  }
  // ALloc the model
  struct model *m = (struct model *)malloc(sizeof(struct model));
  if (!m) return false;
  
  // Alloc the kernel
  int n = t->GetVal("nr_class", 0).toInt()*t->GetVal("nr_feature", 0).toInt();
  m->w = (double *)malloc(n*sizeof(double));
  if (!m->w) { free(m); return false; }

  // Alloc the labels
  m->label = (int *)malloc(t->GetVal("nr_class", 0).toInt()*sizeof(int));
  if (!m->label) { free(m); return false; }  

  // model
  m->nr_class = t->GetValAsInt("nr_class", 0);
  m->nr_feature = t->GetValAsInt("nr_feature", 0);
  m->bias = t->GetValAsDouble("bias", 0);
  
  for (int i=0; i < m->nr_class; i++)
    m->label[i] = t->GetMatrixVal("label", 0, i).toInt();

  for (int i=0; i < m->nr_class*m->nr_feature; i++)
    m->w[i] = t->GetMatrixVal("w", 0, i).toDouble();

  // parameter
  m->param.solver_type = t->GetValAsInt("solver_type", 0);
  m->param.eps = t->GetValAsDouble("eps", 0);
  m->param.C = t->GetValAsDouble("C", 0);
  m->param.nr_weight = t->GetValAsInt("nr_weight", 0);

  return m;
}

void Cleanup(struct model *m = 0, struct problem *prob = 0, struct parameter *param = 0) {
  if (!prob == 0) DestroyProblem(prob);
  if (!m == 0) free_and_destroy_model(&m);
  if (!param == 0) destroy_param(param);
}

bool LIBLINEAR::Train(DataTable* train_data, DataTable* model_table, 
		      String y_col,  String solver) {

  struct problem *prob = CreateProblem(train_data, y_col);
  struct parameter *param = CreateParameter(solver);

  //#ifdef DEBUG
  //PrintProblem(prob);
  //PrintParam(param);
  //#endif

  if (check_parameter(prob, param) != NULL) {
    taMisc::Error("Parameter struct not setup correctly. Programmer error - please report.");
    Cleanup(0, prob, param);
    return false;
  }

  struct model *m = train(prob, param);

  if (!ModelToDataTable(m, model_table)) {
    taMisc::Error("Unable to convert model to DataTable. Programmer error - please report.");
    Cleanup(m, prob, 0);
    return false;
  } 
  
  
  Cleanup(m, prob, 0);

  return true;
}

bool LIBLINEAR::Predict(DataTable* test_data,
			String y_col,
			DataTable* model_data,
			DataTable* predicted_labels) {
  
  struct model *m = DataTableToModel(model_data);
  struct problem *p = CreateProblem(test_data, y_col);

  predicted_labels->Reset();
  predicted_labels->NewColInt("labels");
  predicted_labels->AddRows(test_data->rows);

  // Predict every vector and record the label
  for (int i=0; i < p->l; ++i)
    predicted_labels->SetVal(predict(m, p->x[i]), "labels", i);

  // Compute precision, recall, accuracy
  
  Cleanup(m, p, 0);
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
