#!/bin/bash
# this script will run the awk script on all its arguments

AWK_EXEC=$( which gawk )
PYTHON_EXEC=$( which python )
RSCRIPT_EXEC=$( which Rscript )
co1_path=~/bioinformatics/github/co1_metaanalysis/
AWK_SCRIPT=${CO1_path}code/buildfasta_jb.awk
xml_to_tsv=${co1_path}code/xml_to_tsv.py
data_cleanup=${co1_path}code/data_cleanup.R
afrodata_path=${co1_path}data/input/afro_data/

usage() { #checks if the positional arguments (input files) for execution of the script are defined
        if [ $# -eq 0 ]
        then
                echo "Input error..."
                echo "Usage: $0 file1.*[file2.* file3.* ...]"
                exit 1

        fi
}

rename() { #generates output file names with same input filename prefix. The suffix (".suffix") is set in individual functions.
        input_filename=`basename $i`
        output_filename=${input_filename%.*}
}


build_fasta() {

	usage $@
	
	echo "generating .fasta files from .tsv metadata files"
	
	for i in "$@"
	do
		if [ ! -f $i ]
		then
			echo "input error: file '$i' is non-existent!"
		elif [[ ( -f $i ) && ( `basename "$i"` =~ .*\.(tsv) ) ]]
		then
			rename
			echo -e "\nLet us proceed with file '${input_filename}'..."
			${AWK_EXEC} -f ${AWK_SCRIPT} "$i" > ${output_filename}.fasta
		else
			echo "input file error in `basename $i`: input file should be a .tsv file format"
			continue
		fi
	done
}

build_tsv() {
	
	TAB=$(printf '\t')
	
	usage $@

	echo "generating .tsv files from .xml downloads"

	for i in "$@"
	do
		if [ ! -f $i ]
		then
			echo "input error: file '$i' is non-existent!"
		elif [[ ( -f $i ) && ( `basename "$i"` =~ .*\.(xml) ) ]]
		then
			rename
			echo -e "\nLet us proceed with file '${input_filename}'..."
			sed 's/class/Class/g' "$i" | sed "s/$TAB/,/g" > ${afrodata_path}bold_africa/input.xml
			${PYTHON_EXEC} ${xml_to_tsv} ${afrodata_path}bold_africa/input.xml && mv output.tsv ${afrodata_path}bold_africa/${output_filename}.tsv
		else
			echo "input file error in `basename '$i'`: input file should be a .xml file format"
			continue
		fi
	done
}

clean_nsort_tsv() {

        usage $@

        echo "cleaningup and sorting .tsv files "

        for i in "$@"
        do
                if [ ! -f $i ]
                then
                        echo "input error: file '$i' is non-existent!"
                elif [[ ( -f $i ) && ( `basename "$i"` =~ .*\.(tsv) ) ]]
                then
                        rename
                        echo -e "\nLet us proceed with file '${input_filename}'..."
                        ${RSCRIPT_EXEC} --vanilla ${data_cleanup} $i
			awk '
			FNR==1 && NR!=1 { while (/^<header>/) getline; }
			1 {print}
			' ${afrodata_path}/clean_africa/COI_500to700_data.tsv >> ${afrodata_path}/clean_africa/afroCOI_500to700_data.tsv #file*.txt >all.txt
			awk '
                        FNR==1 && NR!=1 { while (/^<header>/) getline; }
                        1 {print}
                        ' ${afrodata_path}/clean_africa/COI_650to660_data.tsv >> ${afrodata_path}/clean_africa/afroCOI_650to660_data.tsv
			awk '
                        FNR==1 && NR!=1 { while (/^<header>/) getline; }
                        1 {print}
                        ' ${afrodata_path}/clean_africa/COI_all_data.tsv >> ${afrodata_path}/clean_africa/afroCOI_all_data.tsv
			awk '
                        FNR==1 && NR!=1 { while (/^<header>/) getline; }
                        1 {print}
                        ' ${afrodata_path}/clean_africa/COI_Over499_data.tsv >> ${afrodata_path}/clean_africa/afroCOI_Over499_data.tsv
			awk '
                        FNR==1 && NR!=1 { while (/^<header>/) getline; }
                        1 {print}
                        ' ${afrodata_path}/clean_africa/COI_Over700_data.tsv >> ${afrodata_path}/clean_africa/afroCOI_Over700_data.tsv
			awk '
                        FNR==1 && NR!=1 { while (/^<header>/) getline; }
                        1 {print}
                        ' ${afrodata_path}/clean_africa/COI_Under500_data.tsv >> ${afrodata_path}/clean_africa/afroCOI_Under500_data.tsv
                else
                        echo "input file error in `basename '$i'`: input file should be a .tsv file format"
                        continue
                fi
        done
}
