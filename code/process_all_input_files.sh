#!/bin/bash
# this script will run the awk script on all its arguments

AWK_EXEC=$( which gawk )
PYTHON_EXEC=$( which python )
RSCRIPT_EXEC=$( which Rscript )
co1_path=~/bioinformatics/github/co1_metaanalysis/
AWK_SCRIPT=${co1_path}code/buildfasta_jb.awk
xml_to_tsv=${co1_path}code/xml_to_tsv.py
data_cleanup=${co1_path}code/data_cleanup.R
inputdata_path=${co1_path}data/input/input_data/

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


build_fasta() { #This function generates .fasta files from .tsv files using an awk script

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


bolddata_retrival() { # This fuction retrives data belonging to a list of country names given. Input can be a file containing names of select countries or idividual country names
	IFS=$'\n'
	
	for i in `cat $@`
	do
		wget --show-progress --progress=bar:noscroll --retry-connrefused -t inf -O ${inputdata_path}bold_africa/"${i}".xml -a ${inputdata_path}wget_log http://www.boldsystems.org/index.php/API_Public/combined?geo="${i}"&taxon=arthropoda&format=tsv
	done
}


build_tsv() { #This function generates .tsv files from .xml files using python script and Beautifulsoup4 and pandas package
	
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
			sed 's/class/Class/g' "$i" | sed "s/$TAB/,/g" > ${inputdata_path}bold_africa/input.xml
			${PYTHON_EXEC} ${xml_to_tsv} ${inputdata_path}bold_africa/input.xml && mv output.tsv ${inputdata_path}bold_africa/${output_filename}.tsv
		else
			echo "input file error in `basename '$i'`: input file should be a .xml file format"
			continue
		fi
	done
}


append_tsvfile() { # this function tests if the .tsv file has content and if it does it appends it to a cummulative file of all input records. This function is applied in the function below: clean_sort_tsv()

	if [ `grep -v "X..bioinformatics" ${input} | wc -l` -ge 1 ]
	then
		awk 'FNR==1 { while (/^X..bioinformatics/) getline; }   1 {print}' ${input} >> ${output}
	else
		echo -e "\n `basename $input` from `basename $i` has no content besides the header!!!"
	fi
}


clean_sort_tsv() { #This function cleans the .tsv files, sort the records into differnt files based on the sequence length and finally appends this files to a cummulative files of diffent input files

        usage $@

        echo "cleaningup and sorting .tsv files "

	output_files_africa=("${inputdata_path}clean_africa/afroCOI_500to700_data.tsv" "${inputdata_path}clean_africa/afroCOI_650to660_data.tsv" "${inputdata_path}clean_africa/afroCOI_all_data.tsv" "${inputdata_path}clean_africa/afroCOI_Over499_data.tsv" "${inputdata_path}clean_africa/afroCOI_Over700_data.tsv" "${inputdata_path}clean_africa/afroCOI_Under500_data.tsv")

	output_files_eafrica=("${inputdata_path}clean_eafrica/eafroCOI_500to700_data.tsv" "${inputdata_path}clean_eafrica/eafroCOI_650to660_data.tsv" "${inputdata_path}clean_eafrica/eafroCOI_all_data.tsv" "${inputdata_path}clean_eafrica/eafroCOI_Over499_data.tsv" "${inputdata_path}clean_eafrica/eafroCOI_Over700_data.tsv" "${inputdata_path}clean_eafrica/eafroCOI_Under500_data.tsv")


	for i in ${output_files_africa[@]}
	do
		grep "processid" $1 > $i && echo -e "\nInput file $i is set"
	done

	for i in ${output_files_eafrica[@]}
        do
                grep "processid" $1 > $i && echo -e "\nInput file $i is set"
        done
	

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

			case $output_filename in
				Algeria|Madagascar|Angola|Malawi|Benin|Mali|Botswana|Mauritania|Burkina_Faso|Mauritius|Morocco|Cameroon|Mozambique|Cape_Verde|Namibia|Central_African_Republic|Nigeria|Chad|Niger|Comoros|Republic_of_the_Congo|Cote_d_Ivoire|Reunion|Democratic_republic_of_the_Congo|Djibouti|Sao_Tome_and_Principe|Egypt|Senegal|Equatorial_Guinea|Seychelles|Eritrea|Sierra_Leone|Somalia|Gabon|South_Africa|Gambia|Ghana|Sudan|Guinea-Bissau|Swaziland|Guinea|Togo|Tunisia|Lesotho|Liberia|Zambia|Libya|Zimbabwe)
					input=${inputdata_path}clean_africa/COI_500to700_data.tsv
					output=${output_files_africa[0]}
					append_tsvfile

					input=${inputdata_path}clean_africa/COI_650to660_data.tsv
					output=${output_files_africa[1]}
					append_tsvfile

					input=${inputdata_path}clean_africa/COI_all_data.tsv 
					output=${output_files_africa[2]}
					append_tsvfile

					input=${inputdata_path}clean_africa/COI_Over499_data.tsv
					output=${output_files_africa[3]}
					append_tsvfile
					
					input=${inputdata_path}clean_africa/COI_Over700_data.tsv
					output=${output_files_africa[4]}
					append_tsvfile
					
					input=${inputdata_path}clean_africa/COI_Under500_data.tsv
					output=${output_files_africa[5]}
					append_tsvfile
					;;
				*)
					input=${inputdata_path}clean_africa/COI_500to700_data.tsv
					output=${output_files_eafrica[0]}
					append_tsvfile
					
					input=${inputdata_path}clean_africa/COI_650to660_data.tsv
					output=${output_files_eafrica[1]}
					append_tsvfile
					
					input=${inputdata_path}clean_africa/COI_all_data.tsv
					output=${output_files_eafrica[2]}
					append_tsvfile
					
					input=${inputdata_path}clean_africa/COI_Over499_data.tsv
					output=${output_files_eafrica[3]}
					append_tsvfile
					
					input=${inputdata_path}clean_africa/COI_Over700_data.tsv
					output=${output_files_eafrica[4]}
					append_tsvfile
					
					input=${inputdata_path}clean_africa/COI_Under500_data.tsv
					output=${output_files_eafrica[5]}
					append_tsvfile
					
					;;
			esac
		 else
                        echo "input file error in `basename $i`: input file should be a .tsv file format"
                        continue
                fi
        done
}
