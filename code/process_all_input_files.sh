#!/bin/bash
# this script will run the awk script on all its arguments

AWK_EXEC=$( which gawk )
PYTHON_EXEC=$( which python )
RSCRIPT_EXEC=$( which Rscript )
co1_path=~/bioinformatics/github/co1_metaanalysis/
AWK_SCRIPT=${co1_path}code/buildfasta_jb.awk
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

append_record() {
	awk '
	FNR==1 { while (/^X..bioinformatics/) getline; }
	1 {print}
	'
}

file_check() {
	if [ `grep -v "X..bioinformatics" $1 | wc -l` -ge 1 ]
	then
		echo -e "\n `basename $1` is a good file"
	else
		echo -e "\n `basename $1` has no content besides the header!!!"
		continue
	fi

}

clean_sort_tsv() {

        usage $@

        echo "cleaningup and sorting .tsv files "

	output_files_africa=("${afrodata_path}clean_africa/afroCOI_500to700_data.tsv" "${afrodata_path}clean_africa/afroCOI_650to660_data.tsv" "${afrodata_path}clean_africa/afroCOI_all_data.tsv" "${afrodata_path}clean_africa/afroCOI_Over499_data.tsv" "${afrodata_path}clean_africa/afroCOI_Over700_data.tsv" "${afrodata_path}clean_africa/afroCOI_Under500_data.tsv")

	output_files_eafrica=("${afrodata_path}clean_eafrica/afroCOI_500to700_data.tsv" "${afrodata_path}clean_eafrica/afroCOI_650to660_data.tsv" "${afrodata_path}clean_eafrica/afroCOI_all_data.tsv" "${afrodata_path}clean_eafrica/afroCOI_Over499_data.tsv" "${afrodata_path}clean_eafrica/afroCOI_Over700_data.tsv" "${afrodata_path}clean_eafrica/afroCOI_Under500_data.tsv")


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
					input=${afrodata_path}clean_africa/COI_500to700_data.tsv
					if [ `grep -v "X..bioinformatics" ${input} | wc -l` -ge 1 ]
					then
						awk 'FNR==1 { while (/^X..bioinformatics/) getline; }   1 {print}' ${input} >> ${output_files_africa[0]}
					else
						echo -e "\n `basename $input` from `basename $i` has no content besides the header!!!"
					fi

					#awk 'FNR==1 { while (/^X..bioinformatics/) getline; }   1 {print}' ${afrodata_path}/clean_africa/COI_500to700_data.tsv >> ${output_files_africa[0]}
					input=${afrodata_path}clean_africa/COI_650to660_data.tsv
                                        if [ `grep -v "X..bioinformatics" ${input} | wc -l` -ge 1 ]
                                        then
                                                awk 'FNR==1 { while (/^X..bioinformatics/) getline; }   1 {print}' ${input} >> ${output_files_africa[1]}
                                        else
                                                echo -e "\n `basename $input` from `basename $i` has no content besides the header!!!"
                                        fi

					#awk 'FNR==1 { while (/^X..bioinformatics/) getline; }   1 {print}' ${afrodata_path}/clean_africa/COI_650to660_data.tsv >> ${output_files_africa[1]}
					input=${afrodata_path}clean_africa/COI_all_data.tsv 
                                        if [ `grep -v "X..bioinformatics" ${input} | wc -l` -ge 1 ]
                                        then
                                                awk 'FNR==1 { while (/^X..bioinformatics/) getline; }   1 {print}' ${input} >> ${output_files_africa[2]}
                                        else
                                                echo -e "\n `basename $input` from `basename $i` has no content besides the header!!!"
                                        fi

					#awk 'FNR==1 { while (/^X..bioinformatics/) getline; }	1 {print}' ${afrodata_path}/clean_africa/COI_all_data.tsv >> ${output_files_africa[2]}
					input=${afrodata_path}clean_africa/COI_Over499_data.tsv
                                        if [ `grep -v "X..bioinformatics" ${input} | wc -l` -ge 1 ]
                                        then
                                                awk 'FNR==1 { while (/^X..bioinformatics/) getline; }   1 {print}' ${input} >> ${output_files_africa[3]}
                                        else
                                                echo -e "\n `basename $input` from `basename $i` has no content besides the header!!!"
                                        fi
					#awk 'FNR==1 { while (/^X..bioinformatics/) getline; }	1 {print}' ${afrodata_path}/clean_africa/COI_Over499_data.tsv >> ${output_files_africa[3]}
					input=${afrodata_path}clean_africa/COI_Over700_data.tsv
                                        if [ `grep -v "X..bioinformatics" ${input} | wc -l` -ge 1 ]
                                        then
                                                awk 'FNR==1 { while (/^X..bioinformatics/) getline; }   1 {print}' ${input} >> ${output_files_africa[4]}
                                        else
                                                echo -e "\n `basename $input` from `basename $i` has no content besides the header!!!"
                                        fi
					#awk 'FNR==1 { while (/^X..bioinformatics/) getline; }	1 {print}' ${afrodata_path}/clean_africa/COI_Over700_data.tsv >> ${output_files_africa[4]}
					input=${afrodata_path}clean_africa/COI_Under500_data.tsv
                                        if [ `grep -v "X..bioinformatics" ${input} | wc -l` -ge 1 ]
                                        then
                                                awk 'FNR==1 { while (/^X..bioinformatics/) getline; }   1 {print}' ${input} >> ${output_files_africa[5]}
                                        else
                                                echo -e "\n `basename $input` from `basename $i` has no content besides the header!!!"
                                        fi
					#awk 'FNR==1 { while (/^X..bioinformatics/) getline; }	1 {print}' ${afrodata_path}/clean_africa/COI_Under500_data.tsv >> ${output_files_africa[5]}
					;;
				*)
					input=${afrodata_path}clean_africa/COI_500to700_data.tsv
                                        if [ `grep -v "X..bioinformatics" ${input} | wc -l` -ge 1 ]
                                        then
                                                awk 'FNR==1 { while (/^X..bioinformatics/) getline; }   1 {print}' ${input} >> ${output_files_eafrica[0]}
                                        else
                                                echo -e "\n `basename $input` from `basename $i` has no content besides the header!!!"
                                        fi
					#awk 'FNR==1 { while (/^X..bioinformatics/) getline; }   1 {print}' ${afrodata_path}/clean_africa/COI_500to700_data.tsv >> ${output_files_eafrica[0]}
					input=${afrodata_path}clean_africa/COI_650to660_data.tsv
                                        if [ `grep -v "X..bioinformatics" ${input} | wc -l` -ge 1 ]
                                        then
                                                awk 'FNR==1 { while (/^X..bioinformatics/) getline; }   1 {print}' ${input} >> ${output_files_eafrica[1]}
                                        else
                                                echo -e "\n `basename $input` from `basename $i` has no content besides the header!!!"
                                        fi
					#awk 'FNR==1 { while (/^X..bioinformatics/) getline; }   1 {print}' ${afrodata_path}/clean_africa/COI_650to660_data.tsv >> ${output_files_eafrica[1]}
					input=${afrodata_path}clean_africa/COI_all_data.tsv
                                        if [ `grep -v "X..bioinformatics" ${input} | wc -l` -ge 1 ]
                                        then
                                                awk 'FNR==1 { while (/^X..bioinformatics/) getline; }   1 {print}' ${input} >> ${output_files_eafrica[2]}
                                        else
                                                echo -e "\n `basename $input` from `basename $i` has no content besides the header!!!"
                                        fi
					#awk 'FNR==1 { while (/^X..bioinformatics/) getline; }   1 {print}' ${afrodata_path}/clean_africa/COI_all_data.tsv >> ${output_files_eafrica[2]}
					input=${afrodata_path}clean_africa/COI_Over499_data.tsv
                                        if [ `grep -v "X..bioinformatics" ${input} | wc -l` -ge 1 ]
                                        then
                                                awk 'FNR==1 { while (/^X..bioinformatics/) getline; }   1 {print}' ${input} >> ${output_files_eafrica[3]}
                                        else
                                                echo -e "\n `basename $input` from `basename $i` has no content besides the header!!!"
                                        fi
					#awk 'FNR==1 { while (/^X..bioinformatics/) getline; }   1 {print}' ${afrodata_path}/clean_africa/COI_Over499_data.tsv >> ${output_files_eafrica[3]}
					input=${afrodata_path}clean_africa/COI_Over700_data.tsv
                                        if [ `grep -v "X..bioinformatics" ${input} | wc -l` -ge 1 ]
                                        then
                                                awk 'FNR==1 { while (/^X..bioinformatics/) getline; }   1 {print}' ${input} >> ${output_files_eafrica[4]}
                                        else
                                                echo -e "\n `basename $input` from `basename $i` has no content besides the header!!!"
                                        fi
					#awk 'FNR==1 { while (/^X..bioinformatics/) getline; }   1 {print}' ${afrodata_path}/clean_africa/COI_Over700_data.tsv >> ${output_files_eafrica[4]}
					input=${afrodata_path}clean_africa/COI_Under500_data.tsv
                                        if [ `grep -v "X..bioinformatics" ${input} | wc -l` -ge 1 ]
                                        then
                                                awk 'FNR==1 { while (/^X..bioinformatics/) getline; }   1 {print}' ${input} >> ${output_files_eafrica[5]}
                                        else
                                                echo -e "\n `basename $input` from `basename $i` has no content besides the header!!!"
                                        fi
					#awk 'FNR==1 { while (/^X..bioinformatics/) getline; }   1 {print}' ${afrodata_path}/clean_africa/COI_Under500_data.tsv >> ${output_files_eafrica[5]}
					;;
			esac
		 else
                        echo "input file error in `basename $i`: input file should be a .tsv file format"
                        continue
                fi
        done
}
