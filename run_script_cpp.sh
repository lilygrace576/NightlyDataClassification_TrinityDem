#!/bin/bash
# run_script.sh

DATE=$1
echo "Running for $DATE"
apptainer exec --bind /storage/osg-otte1/shared/TrinityDemonstrator:/mnt /storage/osg-otte1/shared/TrinityDemonstrator/DataAnalysis/containers/rootandexact.sif /mnt/DataAnalysis/AncillaryData/file_database/NightlyClassificationScripts/NightlyClassification.cpp $DATE y
