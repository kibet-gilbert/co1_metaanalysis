#!/bin/bash

# this script retrives data from a list of provided countries


co1_path=~/bioinformatics/github/co1_metaanalysis/
afrodata_path=${co1_path}data/input/afro_data/

#countries=`cat $@`
#for i in `cat $@ | xargs -0 -L1` #countries
IFS=$'\n'
for i in `cat $@`
#while read i
do
	#wget --progress=dot --retry-connrefused -t inf -O ${afrodata_path}summary/"${i}".json -a ${afrodata_path}summary/wget_log http://www.boldsystems.org/index.php/API_Public/stats?geo="${i}"&taxon=arthropoda&format=json
	wget --show-progress --progress=bar:noscroll --retry-connrefused -t inf -O ${afrodata_path}bold_africa/"${i}".xml -a ${afrodata_path}wget_log http://www.boldsystems.org/index.php/API_Public/combined?geo="${i}"&taxon=arthropoda&format=tsv
done
