#!/bin/bash
# run_script.sh

DATE=$1
P=$2
echo "Running for $DATE"
echo "Using path: $P"
apptainer exec --bind /storage/osg-otte1/shared/TrinityDemonstrator:/mnt /storage/osg-otte1/shared/TrinityDemonstrator/DataAnalysis/containers/rootandexact.sif /mnt/DataAnalysis/AncillaryData/file_database/NightlyClassificationScripts/ClassifyData $DATE $P
