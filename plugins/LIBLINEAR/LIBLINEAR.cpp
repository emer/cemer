#include "LIBLINEAR.h"

#include "ta_project.h"
#include "ta_math.h"
#include "ta_dataproc.h"
#include <errno.h>
#include <vector>
#include "liblinear-1.8/linear.h"

void LIBLINEAR::Initialize() {}
void LIBLINEAR::Destroy() {CutLinks();}
void LIBLINEAR::UpdateAfterEdit_impl() {inherited::UpdateAfterEdit_impl();}

#define Malloc(type,n) (type *)malloc((n)*sizeof(type))
static char *line = NULL;
static int max_line_len;
double bias = -1.0;

struct feature_node *x_space;
struct parameter param;
struct problem prob;
struct model* model_;

void exit_input_error(int line_num)
{
	fprintf(stderr,"Wrong input format at line %d\n", line_num);
	exit(1);
}

static char* readline(FILE *input)
{
	int len;
	
	if(fgets(line,max_line_len,input) == NULL)
		return NULL;

	while(strrchr(line,'\n') == NULL)
	{
		max_line_len *= 2;
		line = (char *) realloc(line,max_line_len);
		len = (int) strlen(line);
		if(fgets(line+len,max_line_len-len,input) == NULL)
			break;
	}
	return line;
}

void read_problem(const char *filename)
{
	int max_index, inst_max_index, i;
	long int elements, j;
	FILE *fp = fopen(filename,"r");
	char *endptr;
	char *idx, *val, *label;

	if(fp == NULL)
	{
		fprintf(stderr,"can't open input file %s\n",filename);
		exit(1);
	}

	prob.l = 0;
	elements = 0;
	max_line_len = 1024;
	line = Malloc(char,max_line_len);
	while(readline(fp)!=NULL)
	{
		char *p = strtok(line," \t"); // label

		// features
		while(1)
		{
			p = strtok(NULL," \t");
			if(p == NULL || *p == '\n') // check '\n' as ' ' may be after the last feature
				break;
			elements++;
		}
		elements++; // for bias term
		prob.l++;
	}
	rewind(fp);

	prob.bias=bias;

	prob.y = Malloc(int,prob.l);
	prob.x = Malloc(struct feature_node *,prob.l);
	x_space = Malloc(struct feature_node,elements+prob.l);

	max_index = 0;
	j=0;
	for(i=0;i<prob.l;i++)
	{
		inst_max_index = 0; // strtol gives 0 if wrong format
		readline(fp);
		prob.x[i] = &x_space[j];
		label = strtok(line," \t\n");
		if(label == NULL) // empty line
			exit_input_error(i+1);

		prob.y[i] = (int) strtol(label,&endptr,10);
		if(endptr == label || *endptr != '\0')
			exit_input_error(i+1);

		while(1)
		{
			idx = strtok(NULL,":");
			val = strtok(NULL," \t");

			if(val == NULL)
				break;

			errno = 0;
			x_space[j].index = (int) strtol(idx,&endptr,10);
			if(endptr == idx || errno != 0 || *endptr != '\0' || x_space[j].index <= inst_max_index)
				exit_input_error(i+1);
			else
				inst_max_index = x_space[j].index;

			errno = 0;
			x_space[j].value = strtod(val,&endptr);
			if(endptr == val || errno != 0 || (*endptr != '\0' && !isspace(*endptr)))
				exit_input_error(i+1);

			++j;
		}

		if(inst_max_index > max_index)
			max_index = inst_max_index;

		if(prob.bias >= 0)
			x_space[j++].value = prob.bias;

		x_space[j++].index = -1;
	}

	if(prob.bias >= 0)
	{
		prob.n=max_index+1;
		for(i=1;i<prob.l;i++)
			(prob.x[i]-2)->index = prob.n; 
		x_space[j-2].index = prob.n;
	}
	else
		prob.n=max_index;

	fclose(fp);
}


void PrintProblem() {

  cout << "===== PRINTING PROBLEM =====\n";
  cout << "prob.l: " << prob.l << "\n";
  cout << "prob.n: " << prob.n << "\n";
  cout << "prob.bias: " << prob.bias << "\n";
  taMisc::FlushConsole();

  cout << "prob.y:";
  for (int i = 0; i < prob.l; ++i)
    cout << "(" << i << ":" << prob.y[i] << ") ";
  cout << "\n\n";
  taMisc::FlushConsole();

  // We actually want to print the -1 final guy
  cout << "prob.x:\n";
  int max_j = 0;
  for (int i = 0; i < prob.l; ++i) {
    for (int j = 0; prob.x[i][j].index != -1; ++j) {
      max_j = j;
      cout << "(" << prob.x[i][j].index << ":" << prob.x[i][j].value << ") ";
    }
    cout << prob.x[i][max_j+1].index << "\n";
  }

  cout << "\n\n";
  taMisc::FlushConsole();
}

void PrintParam() {
  cout << "===== PRINTING PARAM =====\n";
  cout << "param.solver_type: " << param.solver_type << "\n";
  cout << "param.eps: " << param.eps << "\n";
  cout << "param.C: " << param.C << "\n";
  cout << "param.nr_weight: " << param.nr_weight << "\n";
  taMisc::FlushConsole();
}

bool PrintModel() {
  cout << "===== PRINTING MODEL =====\n";
  cout << "model_->nr_class: " << model_->nr_class << "\n";
  taMisc::FlushConsole();

  cout << "model_->nr_feature: " << model_->nr_feature << "\n";
  taMisc::FlushConsole();

  cout << "model_->bias: " << model_->bias << "\n";
  taMisc::FlushConsole();

  cout << "model_->w: " << "\n";
  for (int i = 0; i < model_->nr_feature*model_->nr_class; ++i)
    cout << "(" << i << ":" << model_->w[i] << ") ";

  cout << "\n\n";
  taMisc::FlushConsole();

  cout << "model_->label " << "\n";
  for (int i = 0; i < model_->nr_class; ++i)
    cout << "(" << i << ":" << model_->label[i] << ") ";
  cout << "\n\n";
  taMisc::FlushConsole();

  PrintParam();
  taMisc::FlushConsole();

  return true;
}

void DestroyParam() {
  destroy_param(&param);
}

void DestroyProblem() {
  free(prob.y);
  free(prob.x);
}

void DestroyModel() {
  free_and_destroy_model(&model_);
}

void DestroyMisc() {
  free(x_space);
  free(line);
}

void DestroyAll() {
  DestroyModel();
  DestroyParam();
  DestroyProblem();
  DestroyMisc();
}

void ConfigParam(String solver) {

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

  param.solver_type = st;
  param.eps = eps;
  param.C =  1; // cost of constraints violation - must be > 0, default = 1
  param.nr_weight = 0; // don't change penalty for any class
  param.weight_label = NULL;
  param.weight = NULL;

}

bool ModelToDataTable(DataTable* t) {
  // TODO: Check for m / t

  // model
  t->FindMakeCol("nr_class", taBase::VT_INT);
  t->FindMakeCol("nr_feature", taBase::VT_INT);
  t->FindMakeCol("bias", taBase::VT_DOUBLE);
  t->FindMakeColMatrix("label", taBase::VT_INT, 1, model_->nr_class);
  t->FindMakeColMatrix("w", taBase::VT_DOUBLE, 1, model_->nr_class*model_->nr_feature);

  // param
  t->FindMakeCol("solver_type", taBase::VT_INT);
  t->FindMakeCol("eps", taBase::VT_DOUBLE);
  t->FindMakeCol("C", taBase::VT_DOUBLE);
  t->FindMakeCol("nr_weight", taBase::VT_INT);

  t->RemoveAllRows();
  t->AddRows();

  // model
  t->SetVal(model_->nr_class, "nr_class", 0);
  t->SetVal(model_->nr_feature, "nr_feature", 0);
  t->SetVal(-1, "bias", 0);
  
  for (int i=0; i < model_->nr_class; ++i)
    t->SetMatrixVal(model_->label[i], "label", 0, i);

  for (int i=0; i < model_->nr_class*model_->nr_feature; ++i)
    t->SetMatrixVal(model_->w[i], "w", 0, i);

  // param
  t->SetVal(model_->param.solver_type, "solver_type", 0);
  t->SetVal(model_->param.eps, "eps", 0);
  t->SetVal(model_->param.C, "C", 0);
  t->SetVal(model_->param.nr_weight, "nr_weight", 0);
  
  return true;

}

void RunCrossValidation(int n_folds, DataTable *predicted_values, int n_rows) {

  int total_correct = 0;
  int *target = Malloc(int, prob.l);

  cross_validation(&prob, &param, n_folds, target);

  predicted_values->Reset();
  predicted_values->NewColDouble("accuracy");
  predicted_values->NewColInt("target");
  predicted_values->AddRows(n_rows);

  for (int i = 0; i < n_rows; ++i)
    predicted_values->SetVal(target[i], "target", i);

  for(int i = 0; i < prob.l; i++)
    if(target[i] == prob.y[i])
      ++total_correct;

  double accuracy = 100.0*total_correct/prob.l;

  predicted_values->SetVal(accuracy, "accuracy", 0);

  free(target);

}

bool ConvertDataTableToLIBSVM(DataTable* data, String fname, String y_col) {
  if (data->cols() <= 1) {taMisc::Warning("data must have at least two columns"); return false;}
  if (data->rows < 2) {taMisc::Warning("data must have at least two rows"); return false;}

  int y_col_idx = data->FindColNameIdx(y_col);

  ofstream libsvm_file;
  libsvm_file.open(fname, ios::trunc);
  
  for (int i = 0; i < data->rows; ++i) {

    libsvm_file << data->GetValAsInt(y_col_idx, i) << " ";

    for (int j = 0; j < data->cols(); ++j) {
      if (j == y_col_idx) continue;

      libsvm_file << j+1 << ":" << data->GetValAsDouble(j, i);
      if (j != data->cols() - 1)
	libsvm_file << " ";
    }

    libsvm_file << "\n";

    flush(libsvm_file);

  }

  libsvm_file.close();
  return true;

}

void ReadProblemClearData(String fname, String y_col, DataTable* data) {
  // In order to allow the problem to use up all available system
  // memory the pattern is to stream the data to file in libsvm
  // format, stream the DataTable to file, delete the table from
  // memory, and load the problem into memory from the libsvm
  // file. After training the problem is deleted and the table is
  // loaded back in. This happens unless fname has '.libsvm' in it,
  // in which case we don't have to mess with the table.

  if (fname.contains(".libsvm")) read_problem(fname);
  else {
    ConvertDataTableToLIBSVM(data, fname + ".libsvm", y_col);
    data->SaveAs(fname + ".dat.gz");
    data->Reset();
    read_problem(fname + ".libsvm");
  }

}

bool CheckParam(String fname, DataTable* data) {
  // Loads data if it fails

  if (check_parameter(&prob, &param) != NULL) {
    taMisc::Warning("Parameter struct not setup correctly. Programmer error - please report. Loading your data back in.");
    DestroyProblem();
    DestroyParam();
    if (!fname.contains(".libsvm")) data->Load(fname + ".dat.gz");
    return false;
  }
  return true;
}

void ReloadDataTable(String fname, DataTable* data) {
  if (!fname.contains(".libsvm")) data->Load(fname + ".dat.gz");
}

void fmeasure(DataTable* predicted_labels, DataTable* data, String y_col,
	      double& precision, double& recall, double& fmeasure,
	      double& tp, double& fp, double& tn, double& fn) {

  int max_idx = 0, cur_y = 0;
  double cur_val = 0.0;
  int n_classes = predicted_labels->GetValAsMatrix("probabilities", 0)->dim(0);

  // Loop over rows of probabilities                                                                                                                                                                                              
  for (int i = 0; i < predicted_labels->rows; ++i) {

    // Find the index of the winner
    taMath_double::vec_max((double_Matrix*)predicted_labels->GetValAsMatrix("probabilities", i), max_idx);

    // Get the right answer
    cur_y = data->GetVal(y_col, i).toInt();

    // Add up the kinds of errors                                                                                                                                                                                                 
    for (int j = 0; j < n_classes; ++j) {

      cur_val = predicted_labels->GetMatrixVal("probabilities", i, j).toDouble();

      if (j == max_idx && j == cur_y) tp += cur_val;       // true positive                                                                                                                                     
      if (j != max_idx && j == cur_y) fn += 1.0 - cur_val; // false negative                                                                                                                                    
      if (j == max_idx && j != cur_y) fp += cur_val;       // false positive                                                                                                                                    
      if (j != max_idx && j != cur_y) tn += 1.0 - cur_val; // true negative                                                                                                                                     

    }
  }

  precision = tp / (tp + fp);
  recall = tp / (tp + fn);
  fmeasure = 2.0*precision*recall/(precision+recall);

}


bool LIBLINEAR::Train(DataTable* train_data, DataTable* model_table, 
		      String y_col,  String fname_prefix, String model_fname, String solver) {

  ReadProblemClearData(fname_prefix, y_col, train_data);
  ConfigParam(solver);
  if (!CheckParam(fname_prefix, train_data)) return false;
  model_ = train(&prob, &param);
  save_model(model_fname, model_);
  DestroyAll();
  ReloadDataTable(fname_prefix, train_data);

  return true;
}



bool LIBLINEAR::CrossValidate(DataTable* all_data, DataTable* predicted_values, 
			      String y_col, int n_folds, String fname_prefix, String solver) {

  // TODO: Sanity checks on input. Wrap it in a method

  int n_rows = all_data->rows;
  
  ReadProblemClearData(fname_prefix, y_col, all_data);
  ConfigParam(solver);
  if (!CheckParam(fname_prefix, all_data)) return false;
  RunCrossValidation(n_folds, predicted_values, n_rows);
  DestroyAll();
  ReloadDataTable(fname_prefix, all_data);

  return true;
}

bool LIBLINEAR::Predict(DataTable* test_data,
			DataTable* predicted_labels,
			String y_col,
			String fname_prefix,
			String model_fname) {
  
  int n_rows = test_data->rows;

  ReadProblemClearData(fname_prefix, y_col, test_data);
  model_ = load_model(model_fname);

  predicted_labels->Reset();
  predicted_labels->NewColInt("labels");
  predicted_labels->NewColDouble("fmeasure");
  predicted_labels->NewColDouble("recall");
  predicted_labels->NewColDouble("precision");
  predicted_labels->NewColDouble("tp");
  predicted_labels->NewColDouble("fp");
  predicted_labels->NewColDouble("tn");
  predicted_labels->NewColDouble("fn");
  predicted_labels->NewColMatrix(taBase::VT_DOUBLE, "probabilities", 1, model_->nr_class);
  predicted_labels->AddRows(n_rows);

  double *prob_estimates = (double *)malloc(prob.n*sizeof(double));
  int label;

  for (int i = 0; i < prob.n; ++i)
    prob_estimates[i] = 0.0;

  // Predict every vector and record the predicted label and the probability estimates
  for (int i=0; i < prob.l; ++i) {

    label = predict_probability(model_, prob.x[i], prob_estimates);
    predicted_labels->SetVal(label, "labels", i);

    for (int j=0; j < model_->nr_class; ++j)
      predicted_labels->SetMatrixVal(prob_estimates[j], "probabilities", i, j);
  }

  DestroyProblem();
  DestroyModel();
  ReloadDataTable(fname_prefix, test_data);

  // Compute precision, recall, fmeasure

  double precision = 0.0, recall = 0.0, fm = 0.0, tp = 0, fp = 0, tn = 0, fn = 0;

  fmeasure(predicted_labels, test_data, y_col, precision, recall, fm, tp, fp, tn, fn);

  predicted_labels->SetVal(precision, "precision", 0);
  predicted_labels->SetVal(recall, "recall", 0);
  predicted_labels->SetVal(fm, "fmeasure", 0);
  predicted_labels->SetVal(tp, "tp", 0);
  predicted_labels->SetVal(fp, "fp", 0);
  predicted_labels->SetVal(tn, "tn", 0);
  predicted_labels->SetVal(fn, "fn", 0);

  free(prob_estimates);

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
