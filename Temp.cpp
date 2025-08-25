#include "Temp.h"
#include <map>
#include <bits/stdc++.h>

int main(int argc, char **argv){
    if(argc < 1){
        cout << "Too few arguments; please include the date data directory to summarize" << endl;
		return 1;
	}
    
    // Get the Arguments 
    std::string folString = argv[1];

// Create txt file for Operation Mode 0; Intrigs
        std::ofstream outputFile0("OpMode0.txt");
        if (outputFile0.is_open()) {
            outputFile0 << "Operation Mode 0: Intrigs" << "\n";
           outputFile0.close();
        } else {
            std::cerr << "Unable to open file for writing." << std::endl;
        }

// Create txt file for Operation Mode 1; Normal
        std::ofstream outputFile1("OpMode1.txt");
        if (outputFile1.is_open()) {
            outputFile1 << "Operation Mode 1: Normal" << "\n";
            outputFile1.close();
        } else {
            std::cerr << "Unable to open file for writing." << std::endl;
        }

// Create txt file for Operation Mode 2; Extended Moon
        std::ofstream outputFile2("OpMode2.txt");
        if (outputFile2.is_open()) {
            outputFile2 << "Operaion Mode 2: Extended Moon" << "\n";
            outputFile2.close();
        } else {
            std::cerr << "Unable to open file for writing." << std::endl;
        }

// Create txt file for Operation Mode3; Door Closed
        std::ofstream outputFile3("OpMode3.txt");
        if (outputFile3.is_open()) {
            outputFile3 << "Operation Mode 3: Door Down" << "\n";
            outputFile3.close();
        } else {
            std::cerr << "Unable to open file for writing." << std::endl;
        }

// Create txt file for Data not being sorted into other files
        std::ofstream outputFile4("Other.txt");
        if (outputFile4.is_open()) {
            outputFile4 << "Other" << "\n";
            outputFile4.close();
        } else {
            std::cerr << "Unable to open file for writing." << std::endl;
        }


    // Load in all the files
    std::string FolderPath = Form("%s%s/",dataDir.c_str(),folString.c_str());
    std::cout << "directory: " << dataDir  <<  std::endl;

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
    }
    else if (fileNamesVec.size() == 1){
        std::cout << "attention needed" << std::endl;
    }
    else {

 

    // for(int f = 0; f<20; f++){
    for(int f = 0; f<static_cast<int>(fileNamesVec.size()); f++){
        
        // checks to make sure the  data  file is  readable
        std::string FilePath = Form("%s%s",FolderPath.c_str(),fileNamesVec[f].c_str());
        // std::cout << "file name " << fileNamesVec[f].c_str() << endl;
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
        if (nEntries==0) {
            continue; // acts like continue in parrellization
        }
	    
        TotalEvents = nEntries + nEntriesHLED;
        // std::cout << "Total Number of Events: " << nEntries << std::endl;
        for(int EventCounter =0; EventCounter < TotalEvents; EventCounter++){
        // std::cout << "Event Number" << EventCounter << std::endl;
        std::vector<float> Current;
        std::vector<float> BiasVoltage;
        std::string DataType;
        if (EventCounter > nEntries){
            // std::cout << "HLED" << std::endl;
            DataType = "HLED";
            treeHLED->GetEntry(EventCounter - nEntries); 
            Current = evHLED->Gethvc();
            BiasVoltage = evHLED->Gethv();
            // std::cout << DataType << endl;
        }
        else {
            tree->GetEntry(EventCounter);
            // std::cout << "test" << endl;
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
		// std::cout << "Current Rounded" << roundCurrent << std::endl;
        // std::cout << "BV0: " << BiasVoltage[0] << " BV1: " << BiasVoltage[1] << " BV2: " << BiasVoltage[2] << " BV3: " << BiasVoltage[3] << std::endl;
    // find avg BV and round to 10ths
		float sumV = std::accumulate(BiasVoltage.begin(), BiasVoltage.end(), 0.0);
		float BVAvg = sumV / BiasVoltage.size();
		// std::cout << "BV AVG: " <<  BVAvg << std::endl;
        float roundBVAvg = std::round(10 * BVAvg) / 10;
        // std::cout << "Avg BV rounded" << roundBVAvg << std::endl;
		// std::cout << "BV rounded" << std::round(BVAvg) << endl;


// program keeps getting killed
        if ((roundBVAvg == 42.0) && (roundCurrent <= 3.7)){
            std::ofstream outputFile0("OpMode0.txt", std::ios::app); 
            outputFile0 << "File Name: " << fileNamesVec[f].c_str() << ", Event Number: " << EventCounter << ", Data Type: " << DataType << ", Bias Voltage: " << BVAvg << "V, " << "Current: " << *maxCurrent << "mA \n";
            outputFile0.close();
        }
        else if ((roundBVAvg == 44.0) && (roundCurrent > 4.0)){
            std::ofstream outputFile1("OpMode1.txt", std::ios::app); 
            outputFile1 << "File Name: " << fileNamesVec[f].c_str() << ", Event Number: " << EventCounter << ", Data Type: " << DataType << ", Bias Voltage: " << BVAvg << "V, " << "Current: " << *maxCurrent << "mA \n";
            outputFile1.close();
        }    
         else if ((roundBVAvg == 41.5) && (roundCurrent >= 3.5)){
            std::ofstream outputFile2("OpMode2.txt", std::ios::app);
            outputFile2 << "File Name: " << fileNamesVec[f].c_str() << ", Event Number: " << EventCounter << ", Data Type: " << DataType << ", Bias Voltage: " << BVAvg << "V, " << "Current: " << *maxCurrent << "mA \n";
            outputFile2.close();
         }
        else if ((roundBVAvg == 44.0) && (roundCurrent <= 4.0)){
            std::ofstream outputFile3("OpMode3.txt", std::ios::app);
            outputFile3 << "File Name: " << fileNamesVec[f].c_str() << ", Event Number: " << EventCounter << ", Data Type: " << DataType << ", Bias Voltage: " << BVAvg << "V, " << "Current: " << *maxCurrent << "mA \n";
            outputFile3.close();
        }
        else {
            std::ofstream outputFile4("Other.txt", std::ios::app);
            outputFile4 << "File Name: " << fileNamesVec[f].c_str() << ", Event Number: " << EventCounter << ", Data Type: " << DataType << ", Bias Voltage: " << BVAvg << "V, " << "Current: " << *maxCurrent << "mA \n";
            outputFile4.close();
        }

    }

}
    }

}
