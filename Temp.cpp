#include "Temp.h"
#include <map>
#include <bits/stdc++.h>
#include <sstream>

int main(int argc, char **argv){
    if(argc < 1){
        cout << "Too few arguments; please include the date data directory to summarize" << endl;
		return 1;
	}
    
// Get the Arguments 
std::string folString = argv[1];

// Create txt file for Operation Mode 0; Intrigs
std::ofstream outputFile0("intrigs.txt");
if (outputFile0.is_open()) {
    outputFile0 << "Operation Mode 0: Intrigs" << "\n";
    outputFile0 << "File Name, " << "Test Events, " << "HLED Events, " << "Forced Events, " << "Avg Rounded Bias Voltage, " << "Avg Currents" << "\n"; 
    // outputFile0 << "File Name, " << "Event Number, " << "Data Type, " << "Bias Voltage (V), " << "Current (mA)" << "\n"; 
    // outputFile0.close();
} else {
    std::cerr << "Unable to open file for writing." << std::endl;
}

// Create txt file for Operation Mode 1; Normal
std::ofstream outputFile1("NormalOps.txt");
if (outputFile1.is_open()) {
    outputFile1 << "Operation Mode 1: Normal" << "\n";
    outputFile1 << "File Name, " << "Test Events, " << "HLED Events, " << "Forced Events, " << "Avg Rounded Bias Voltage, " << "Avg Currents" << "\n"; 
    // outputFile1 << "File Name, " << "Event Number, " << "Data Type, " << "Bias Voltage (V), " << "Current (mA)" << "\n"; 
    // outputFile1.close();
} else {
    std::cerr << "Unable to open file for writing." << std::endl;
}

// Create txt file for Operation Mode 2; Extended Moon
std::ofstream outputFile2("ExtMoonOps.txt");
if (outputFile2.is_open()) {
    outputFile2 << "Operaion Mode 2: Extended Moon" << "\n";
    outputFile2 << "File Name, " << "Test Events, " << "HLED Events, " << "Forced Events, " << "Avg Rounded Bias Voltage, " << "Avg Currents" << "\n"; 
    // outputFile2 << "File Name, " << "Event Number, " << "Data Type, " << "Bias Voltage (V), " << "Current (mA)" << "\n"; 
    // outputFile2.close();
} else {
    std::cerr << "Unable to open file for writing." << std::endl;
}

// Create txt file for Operation Mode3; Door Closed
std::ofstream outputFile3("DoorClosed.txt");
if (outputFile3.is_open()) {
    outputFile3 << "Operation Mode 3: Door Down" << "\n";
    outputFile3 << "File Name, " << "Test Events, " << "HLED Events, " << "Forced Events, " << "Avg Rounded Bias Voltage, " << "Avg Currents" << "\n"; 
    // outputFile3 << "File Name, " << "Event Number, " << "Data Type, " << "Bias Voltage (V), " << "Current (mA)" << "\n"; 
    // outputFile3.close();
} else {
    std::cerr << "Unable to open file for writing." << std::endl;
}

// Create txt file for Data not being sorted into other files
std::ofstream outputFile4("Other.txt");
if (outputFile4.is_open()) {
    outputFile4 << "Other" << "\n";
    outputFile4 << "File Name, " << "Test Events, " << "HLED Events, " << "Forced Events" << "\n"; 
    // outputFile4 << "File Name," << "Event Number," << "Data Type," << "Bias Voltage (V)," << "Current (mA)" << "\n"; 
    // outputFile4.close();
} else {
    std::cerr << "Unable to open file for writing." << std::endl;
}

// Create txt file for Data not being sorted into other files
std::ofstream outputFile5("AllFiles.txt");
if (outputFile5.is_open()) {
    outputFile5 << "File Name, " << "OpMode, " << "Test Events, " << "HLED Events, " << "Forced Events" << "\n"; 
} else {
    std::cerr << "Unable to open file for writing." << std::endl;
}

// Load in all the files
std::string FolderPath = Form("%s%s/",dataDir.c_str(),folString.c_str());

    // Createthe fileNames Vector based on the type of data found in the passes argument
    // Sim - Simulation data (Only Test Branch events)
    // muon - muon data (Forced Branch events)
    // bkg - Background data (Test Branch and HLED Branch events in seperate folder)
    // none - normal data

std::vector<std::string> fileNamesVec;
    
// check if any data collected
fileNamesVec=util->GetFilesInDirectory(FolderPath,".root");
if (fileNamesVec.size() == 0){
    std::cout << "no data collected" << std::endl;
    outputFile0.close();
    outputFile1.close();
    outputFile2.close();
    outputFile3.close();
    outputFile4.close();
    outputFile5.close();
} else if (fileNamesVec.size() == 1){
    std::cout << "attention needed" << std::endl;
    outputFile0.close();
    outputFile1.close();
    outputFile2.close();
    outputFile3.close();
    outputFile4.close();
    outputFile5.close()
} else {

    // if fileNamesVec has more than 1 file in it -> run through its data and add to respective files
    // commented below: just to run through 20 of the files
    // for(int f = 0; f<20; f++){
    for(int f = 0; f<static_cast<int>(fileNamesVec.size()); f++){
        // checks to make sure the  data  file is  readable
        std::string FilePath = Form("%s%s",FolderPath.c_str(),fileNamesVec[f].c_str());
        
        if (!util->isBranchPresentInFile(FilePath, "Test")) {
            continue; // Skip to the next branch if not present
        }

        int TotalEvents;
        int nEntries;

        // loads the events based on the type of data for each file
        LoadEvents(FilePath, "Test");
        LoadEventsHLED(FilePath, "HLED");
        ev = new IEvent();
        evHLED = new IEvent();
        sipmInfo = new ISiPM();
        SetBranches(ev);
        SetBranchesHLED(evHLED);
        tree->SetBranchAddress("SiPM", &sipmInfo);        
	    treeHLED->SetBranchAddress("SiPM", &sipmInfo);

        nEntries = tree->GetEntries();
        int nEntriesHLED = treeHLED->GetEntries();

        TotalEvents = nEntries + nEntriesHLED;

        std::vector<float> fileCurrent;
        std::vector<float> fileBV;
        std::string OpMode;

        // run through each event, get Bias Voltage and Current, add to respective files
        for(int EventCounter =0; EventCounter < TotalEvents; EventCounter++){
            // std::cout << "Event Number" << EventCounter << std::endl;
            std::vector<float> Current;
            std::vector<float> BiasVoltage;
            std::string DataType;
        // determine data type (TEST or HLED)
            if (EventCounter >= nEntries){
                DataType = "HLED";
                treeHLED->GetEntry(EventCounter - nEntries); 
                Current = evHLED->Gethvc();
                BiasVoltage = evHLED->Gethv();
                // std::cout << DataType << endl;
            // else if (EventCounter == 0){
            //     DataType = "Forced";
            // }
            } else {
                tree->GetEntry(EventCounter);
                DataType = "TEST";
                Current = ev->Gethvc();
                BiasVoltage = ev->Gethv();
                // std::cout << DataType << endl;
            }
       
            // std::cout << "Current0: " << Current[0] << " Current1: " << Current[1] << " Current2: " << Current[2] << " Current3: " << Current[3] << std::endl;
        // find max current and round to 10ths
		    auto maxCurrent = std::max_element(Current.begin(), Current.end());
            // std::cout << "Current Max " << *maxCurrent << endl;
            float roundCurrent = std::round(10 * *maxCurrent) / 10;
        // set sigfigs 
            curr << std::fixed << std::setprecision(1) << roundCurrent;
            std::cout << "sig fig current" << curr << std::endl;
        // add rounded max current for each event in file to current vector for file
            fileCurrent.push_back(roundCurrent);
		    // std::cout << "Current Rounded" << roundCurrent << std::endl;
        // find avg BV and round to 10ths
		    float sumV = std::accumulate(BiasVoltage.begin(), BiasVoltage.end(), 0.0);
		    float BVAvg = sumV / BiasVoltage.size();
            float roundBVAvg = std::round(10 * BVAvg) / 10;
        // add rounded average bias voltage for each event in file to bias voltage vector for file
            fileBV.push_back(roundBVAvg);
            // std::cout << "Avg BV rounded" << roundBVAvg << std::endl;

        }   // close for event in file

// not sure if in right place
    if (nEntries==0) {
        OpMode = "intrigs";
// if no Test entries -> file put into intrigs
        outputFile0 << std::fixed << std::setprecision(1);
        outputFile0 << fileNamesVec[f].c_str() << ", " << nEntries << ", " << nEntriesHLED << ", " << 0 << ", " << fileBV[0] << ", " << fileCurrent[0] << "\n";            
        outputFile5 << fileNamesVec[f].c_str() << ", " << OpMode << ", " << nEntries << ", " << nEntriesHLED << ", " << 0 << "\n";            
    }

// if elements of BV and Current vectors for each file are equal -> check which operation mode file belongs in and add to corresponding output file
    if((std::adjacent_find(fileBV.begin(), fileBV.end(), std::not_equal_to<>()) == fileBV.end()) && (std::adjacent_find(fileCurrent.begin(), fileCurrent.end(), std::not_equal_to<>()) == fileCurrent.end())){
        if ((fileBV[0] == 42.0) && (fileCurrent[0] <= 3.7)){
            OpMode = "intrigs";
            outputFile0 << std::fixed << std::setprecision(1);
            outputFile0 << fileNamesVec[f].c_str() << ", "  << nEntries << ", " << nEntriesHLED << ", " << 0 << ", " << fileBV[0] << ", " << fileCurrent[0] << "\n";
            outputFile5 << fileNamesVec[f].c_str() << ", " << OpMode << ", " << nEntries << ", " << nEntriesHLED << ", " << 0 << "\n";
        } else if ((fileBV[0] == 44.0) && (fileCurrent[0] > 4.0)){
            OpMode = "Normal Ops";
            outputFile1 << std::fixed << std::setprecision(1);
            outputFile1 << fileNamesVec[f].c_str() << ", "  << nEntries << ", " << nEntriesHLED << ", " << 0 << ", " << fileBV[0] << ", " << fileCurrent[0] << "\n";
            outputFile5 << fileNamesVec[f].c_str() << ", " << OpMode << ", " << nEntries << ", " << nEntriesHLED << ", " << 0 << "\n";
        } else if ((fileBV[0] == 41.5) && (fileCurrent[0] >= 3.5)){
            OpMode = "Ext. Moon Ops";
            outputFile2 << std::fixed << std::setprecision(1);
            outputFile2 << fileNamesVec[f].c_str() << ", "  << nEntries << ", " << nEntriesHLED << ", " << 0 << ", " << fileBV[0] << ", " << fileCurrent[0] << "\n";
            outputFile5 << fileNamesVec[f].c_str() << ", " << OpMode << ", " << nEntries << ", " << nEntriesHLED << ", " << 0 << "\n";
        } else if ((fileBV[0] == 44.0) && (fileCurrent[0] <= 4.0)){
            OpMode = "Door Closed Ops";
            outputFile3 << std::fixed << std::setprecision(1);
            outputFile3 << fileNamesVec[f].c_str() << ", "  << nEntries << ", " << nEntriesHLED << ", " << 0 << ", " << fileBV[0] << ", " << fileCurrent[0] << "\n";
            outputFile5 << fileNamesVec[f].c_str() << ", " << OpMode << ", " << nEntries << ", " << nEntriesHLED << ", " << 0 << "\n";
        }
    }
    // if elements of BV and Current vectors for each file are not equal -> add to other output file
    else {
        outputFile4 << fileNamesVec[f].c_str() << ", " << nEntries << ", " << nEntriesHLED << "\n";
    }        

        delete fO;
        delete ev;
        delete evHLED;
        delete sipmInfo;
        delete tree;
        delete treeHLED;

    }   // close for file in fileNamesVec

outputFile0.close();
outputFile1.close();
outputFile2.close();
outputFile3.close();
outputFile4.close();
outputFile5.close()

}   // close else (fileNamesVec.size() > 1)

}   // close main


    // sorting events of each file based on Bias Voltage and Current
        // if ((roundBVAvg == 42.0) && (roundCurrent <= 3.7)){
        //     // outputFile0 << fileNamesVec[f].c_str() << ", " << nEntries << ", " << nEntriesHLED << ", " << roundBVAvg << ", " << roundCurrent << "\n";            
        // } else if ((roundBVAvg == 44.0) && (roundCurrent > 4.0)){
        //     // outputFile1 << fileNamesVec[f].c_str() << ", " << nEntries << ", " << nEntriesHLED << ", " << roundBVAvg << ", " << roundCurrent << "\n";
        // } else if ((roundBVAvg == 41.5) && (roundCurrent >= 3.5)){
        //     // outputFile2 << fileNamesVec[f].c_str() << ", " << nEntries << ", " << nEntriesHLED << ", " << roundBVAvg << ", " << roundCurrent << "\n";
        // } else if ((roundBVAvg == 44.0) && (roundCurrent <= 4.0)){
        //     // outputFile3 << fileNamesVec[f].c_str() << ", " << nEntries << ", " << nEntriesHLED << ", " << roundBVAvg << ", " << roundCurrent << "\n";
        // } else {
        //     // outputFile4 << fileNamesVec[f].c_str() << ", " << nEntries << ", " << nEntriesHLED << ", " << roundBVAvg << ", " << roundCurrent << "\n";
        // }
