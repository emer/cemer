
void VerifyStandard(
        String sProject) {
    Program *program = .projects[sProject].programs.gp["LeabraAll_Std"]["LeabraBatch"];
    Network *network = .projects[sProject].networks["Network_0"];
    DataTable *data = .projects[sProject].data.gp["InputData"]["StdInputData"];
    DataTable *out = .projects[sProject].data.gp["OutputData"]["EpochOutputData"];
}

void SetRandom(
	String sProject,
	String sProgramGroup,
	String sProgram,
	String sVariable) {
    .projects[sProject].programs.gp[sProgramGroup][sProgram].vars[sVariable].SetVar("NEW_SEED");
}

void RunProgram(
	String sProject,
	String sProgramGroup,
	String sProgram,
	String sNetwork,
	String sInputDataGroup,
	String sInputDataTable) {
    Program *program = .projects[sProject].programs.gp[sProgramGroup][sProgram];
    Network *network = .projects[sProject].networks[sNetwork];
    DataTable *data = .projects[sProject].data.gp[sInputDataGroup][sInputDataTable];
    program->SetVar("network", network);
    program->SetVar("input_data", data);
    program->Run();
}

void OutputEpochsToTrain(
	String sTestName,
	String sProject,
	String sOutputDataGroup,
	String sOutputDataTable) {
    DataTable *out = .projects[sProject].data.gp[sOutputDataGroup][sOutputDataTable];
    out->Sort("batch", false);
    int batches = 1 + out->GetVal("batch",0);
    int sum = 0;
    int max = 0;
    int min = 99999999999;
    for (int batch = 0; batch < batches; batch++) {
        out->ShowAllRows();
        out->Filter("batch", Relation::EQUAL, batch);
        out->Sort("epoch", false);
        int epochs = out->GetVal("epoch", 0);
        sum += epochs;
        if (epochs > max)
            max = epochs;
        if (epochs < min)
            min = epochs;
    } 
    out->ShowAllRows();
    float avg = (float) sum / (float) batches;
    cout << "{ \"name\": \"" << sTestName << "\", \"min\": " << min << ", \"max\": " << max << ", \"avg\": " << avg << " }" << endl;
}

void OutputANotB(
	String sTestName,
	String sProject,
	String sOutputDataGroup,
	String sOutputDataTable) {
    DataTable *out = .projects[sProject].data.gp[sOutputDataGroup][sOutputDataTable];
    out->Filter("trial_name", Relation::CONTAINS, "choice");
    int rows = out->ItemCount();
    float sum = 0;
    float max = 0;
    float min = 99999999999;
    for (int row = 0; row < rows; row++) {
        float result = out->GetMatrixFlatVal("Reach_act", row, 0);
        sum += result;
        if (result > max)
            max = result;
        if (result < min)
            min = result;
    } 
    out->ShowAllRows();
    float avg = sum / (float) rows;
    cout << "{ \"name\": \"" << sTestName << "\", \"min\": " << min << ", \"max\": " << max << ", \"avg\": " << avg << " }" << endl;
}
