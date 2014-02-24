/**
 * leabra.css (aka leabra.css.cpy)
 * Copyright (c) 2014 eCortex, Inc.
 * 
 * This file is part of the Emergent Test Framework.
 *
 * The Emergent Test Framework is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * The Emergent Test Framework is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the Emergent Test Framework.  If not, see http://www.gnu.org/licenses/.
 *
 * Emergent Test Framework
 * Version 0.8
 * Compatible with Emergent 6.x
 * 
 */

/*
 *  VerifyStandard - Ensure that all the components required for a standard test exist in a project
 */
void VerifyStandard(
        String sProject) {
    Program *program = .projects[sProject].programs.gp["LeabraAll_Std"]["LeabraBatch"];
    Network *network = .projects[sProject].networks["Network_0"];
    DataTable *data = .projects[sProject].data.gp["InputData"]["StdInputData"];
    DataTable *out = .projects[sProject].data.gp["OutputData"]["EpochOutputData"];
}

/*
 *  SetRandom - Set the randomizer to a NEW_SEED so that it has a different result each time
 */
void SetRandom(
	String sProject,
	String sProgramGroup,
	String sProgram,
	String sVariable) {
    .projects[sProject].programs.gp[sProgramGroup][sProgram].vars[sVariable].SetVar("NEW_SEED");
}

/*
 *  RunProgram - Run the specified Program with the given inputs
 */
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

/*
 *  OutputEpochsToTrain - In a standard test, use the Epoch output data to output the min/max/avg epochs
 */
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

/*
 *  RunStandard - Run a standard test all the way through - this is the "main" for a standard test
 */
void RunStandard(
        String sProject) {
    SetRandom(
        sProject,
        "LeabraAll_Std",
        "LeabraTrain",
        "rnd_init");
    RunProgram(
        sProject,
        "LeabraAll_Std",
        "LeabraBatch",
        "Network_0",
        "InputData",
        "StdInputData");
    OutputEpochsToTrain(
        sProject,
        sProject,
        "OutputData",
        "EpochOutputData");
}

