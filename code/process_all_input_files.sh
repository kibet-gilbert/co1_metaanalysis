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

rename() { #generates output file names with same input filename prefix. The suffix (".suffix") is set in individual functions that perform different tasks.
        input_filename=`basename -- $i`
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
		elif [[ ( -f $i ) && ( `basename -- "$i"` =~ .*\.(tsv) ) ]]
		then
			rename
			echo -e "\nLet us proceed with file '${input_filename}'..."
			${AWK_EXEC} -f ${AWK_SCRIPT} "$i" > ${output_filename}.fasta
		else
			echo "input file error in `basename -- $i`: input file should be a .tsv file format"
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
		elif [[ ( -f $i ) && ( `basename -- "$i"` =~ .*\.(xml) ) ]]
		then
			rename 
			echo -e "\nLet us proceed with file '${input_filename}'..."
			sed 's/class/Class/g' "$i" | sed "s/$TAB/,/g" > ${inputdata_path}bold_africa/input.xml
			${PYTHON_EXEC} ${xml_to_tsv} ${inputdata_path}bold_africa/input.xml && mv output.tsv ${inputdata_path}bold_africa/${output_filename}.tsv
		else
			echo "input file error in `basename -- '$i'`: input file should be a .xml file format"
			continue
		fi
	done
}

#=======================================================================================
append_tsvfile() { # this function tests if the .tsv file has content and if it does it appends it to a cummulative file of all input records. This function is applied in the function below: clean_sort_tsv()

	if [ `grep -v "X..bioinformatics" ${input} | wc -l` -ge 1 ]
	then
		awk 'FNR==1 { while (/^X..bioinformatics/) getline; }   1 {print}' ${input} >> ${output}
	else
		echo -e "\n `basename -- $input` from `basename -- $i` has no content besides the header!!!"
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
                elif [[ ( -f $i ) && ( `basename -- "$i"` =~ .*\.(tsv) ) ]]
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
                        echo "input file error in `basename -- $i`: input file should be a .tsv file format"
                        continue
                fi
        done
}


#======================================================================================
#genereting headers from csv files with inputs...

#One of the input files came as excel .xml spreadsheet, which was easily converted to a text.csv file by the excel-compatible GUI LibreOffice Calc in ubuntu. To build fasta-headers from the text.csv file the following code was used:
#awk 'BEGIN { FS=","; OFS="|" } ; { for(i=1; i<=NF; i++) if($i ~ /^ *$/) $i = "NA" }; { print  ">" $1, $4, "gs-"$5, "sp-"$6, "subsp-"$7, "country-"$8,"exactsite-"$9, "lat_"$10, "lon_"$11, "elev-"$12 }' text.csv > headers && less headers

#identifying duplicates in two fasta files based on unique id...
#grep ">" seq1.fasta | awk 'BEGIN { FS="|"}; {print $1}' > file1
#grep ">" seq2.fasta | awk 'BEGIN { FS="|"}; {print $1}' > file2
#comm -12 file1 file2 > repeats && less repeats

replacing_headers() { #This function takes an input file of edited_fasta_format_headers and searches through a fasta_format_sequence file and substitute their headers if their uniq IDs match
 	if [ $# -lt 2 ]
	then
		echo "Input error..."
      		echo "Usage: $0 headers_file seq.fasta"
      		return
        fi
	
	echo -e "\n\tStarting operation....\n\tPlease wait, this may take a while...."
	for line in `cat $1`
	do
		#x=$( head -10 idrck_headers | tail -1 | awk 'BEGIN { FS="|"; }{print $1;}') && echo $x
		x=`echo "$line" | ${AWK_EXEC} 'BEGIN { RS="\n"; FS="|"; }{ x = $1; print x; }'`
		y=`echo "$line" | ${AWK_EXEC} 'BEGIN { RS="\n"; FS="|"; }{ y = $0; print y; }'`
		#echo -e "\n $x \n $y"
		
		z=`grep "$x" $2`
		#echo "$z"
		for one_z in `echo -e "${z}"`
		do
			if [ $one_z == $y ]
			then
				return 0
			else
				#echo ${one_z}
				sed -i "s/${one_z}/${y}/g" $2
				#sed -i "s/^.*\b${x}\b.*$/${y}/g" $2
			fi
		done
		echo -e "\n\tCongratulations...Operation done."
	done
}


delete_repeats() { #This function takes a fasta_format_sequences file and deletes repeats of sequences based on identical headers.
	if [ $# -eq 0 ]
        then
                echo "Input error..."
                echo "Usage: $0 file1.*[file2.* file3.* ...]"
                return 1
        fi

        for i in "$@"
        do
		rename
		input_src=`dirname "$( realpath "${i}" )"`
		unset duplicate_headers
		duplicate_headers=`grep ">" $i | $AWK_EXEC 'BEGIN { FS="|"; }; {print $1; }' | sort | uniq -d`
		if [ ! -z "$duplicate_headers" ]
		then
			echo -e "\t`echo -e "$duplicate_headers" | wc -l` records are repeated in $i,\n\twould you like to proceed and delete any repeats?"
			read -p "Please enter [Yes] or [No] to proceed: " choice
		else
			choice="No"
		fi
		case $choice in
			YES|Yes|yes|Y|y)
				$AWK_EXEC -F'[>|]' 'FNR==1{delete seen} FNR%2{f=seen[$2]++} !f' $i > ${input_src}/${output_filename}_cleaned && mv ${input_src}/${output_filename}_cleaned $( realpath "${i}" )
				echo -e "\tDuplicate records deleted\n"
				;;
			No|NO|no|N|n)
				if [ ! -z "$duplicate_headers" ]
				then
					echo -e "\tWould you like to save a list of the dublicates?"
					read -p "Please enter [Yes] or [No] to proceed: " option
					case $option in
						YES|Yes|yes|Y|y)
							echo -e "\tCancelling....\nThe list of repeated sequences is in file called '_duplicates'\n"
							echo -e "$duplicate_headers" > ${input_src}/${output_filename}_duplicates
							;;
						No|NO|no|N|n)
							echo -e "\tCancelling...\n"
							;;
						*)
							echo "ERROR!!! Invalid selection"
							;;
					esac
				else
					echo -e "\tNo duplicate records in $i\n"
				fi
				;;
			*)
				echo "ERROR!!! Invalid selection"
				;;
		esac
        done

}


delete_unwanted() { #this function copys a record that fits a provided pattern, i.e a name that may be from a non-insect class based on the taxon_name_description; the arguments provided, are the files to be searched for the patterns
	# To get the list of orders in description_taxon_names and their frequencies, from  which to select the undesired patterns (names), do: 
	#grep ">" seqs.fasta | awk 'BEGIN {FS="|"; OFS="|" ; }; {print $2}' |sort | uniq -c > seqs_orders && less seqs_orders

	if [ $# -eq 0 ]
        then
                echo "Input error..."
                echo "Usage: $0 file1.*[file2.* file3.* ...]"
                return 1

        fi

	unset $pattern_name
	read -p "Please enter string pattern to be searched:: " pattern_name
	echo -e "\n\tDeleting all records with description '$pattern_name'..."

	for i in "$@"
	do
		echo -e "\n\tProceeding with `basename -- $i`..."
		rename
		input_src=`dirname "$( realpath "${i}" )"`
		#awk -v name="$input_r" 'BEGIN {RS="\n>"; ORF="\n>"}; $0 ~ name {print ">"$0}' test_all.fasta | less
		$AWK_EXEC -v pattern="$pattern_name" 'BEGIN { RS="\n>"; ORS="\n"; FS="\n"; OFS="\n" }; $0 ~ pattern {print ">"$0;}' $i >> ${input_src}/${output_filename}_undesired.fasta
		sed -i "/$pattern_name/,+1 d" $i
	done
	echo -e "\n\tDONE. All deleted records have been stored in '${output_filename}_undesired.fasta'"
}


trimming_seqaln() { #This function trims aligned sequences in a file on both ends and retains a desired region based on input field values.

	if [ $# -eq 0 ]
        then
                echo "Input error..."
                echo "Usage: $0 file1.*[file2.* file3.* ...]"
                return 1
        fi

	for i in "$@"
	do
		if [ ! -f $i ]
                then
                        echo "input error: file '$i' is non-existent!"
                elif [[ ( -f $i ) && ( `basename -- "$i"` =~ .*\.(aln|afa|fasta|fa) ) ]]
                then
			input_src=`dirname "$( realpath "${i}" )"`
			rename $i
			echo -e "\tThis function will trim your sequences at specific positions and output the desired columns. Proceed and enter the desired start and end positions of the blocks to extract.\n\tTrimming `basename -- $i`..."
			read -p "Please enter the start position: " start_pos
			read -p "Please enter the end position: " end_pos
			echo -e "\tYou are trimming `basename -- ${i}` at position ${start_pos} to ${end_pos}"
			$AWK_EXEC -v start_p=$start_pos -v end_p=$end_pos \
				'BEGIN{FS=""; OFS=""; }; /^>/ { print "\n" $0 }; !/^>/ { for(v=start_p; v<=end_p; v++) { printf "%s", $v; if (v <= end_p) { printf "%s", OFS; } else { printf "\n"; } }}' $input > ${input_src}/${output_filename}_trmmd.aln
			#awk 'BEGIN {FS=""; OFS=""; }; /^>/ {print "\n" $0 }; !/^>/ { for(i=1087; i<=2574; i++) { printf "%s", $i; if (i <= 2574) { printf "%s", OFS; } else { printf "\n"; } }}' input.aln | less
			echo -e "\n\tDONE. All trimmed records have been stored in $input > ${input_src}/${output_filename}_trmmd.aln\n"
		else
			echo "input file error in `basename $i`: input file should be a .fasta file format"
                        continue
		fi
	done
}

delete_shortseqs() { #This function identifies and removes sequences that have specified number of gaps at the ends. It stores the removed sequences.

	if [ $# -eq 0 ]
        then
                echo "Input error..."
                echo "Usage: $0 file1.*[file2.* file3.* ...]"
                return 1
        fi

        for i in "$@"
        do
                if [ ! -f $i ]
                then
                        echo "input error: file '$i' is non-existent!"
                elif [[ ( -f $i ) && ( `basename -- "$i"` =~ .*\.(aln|afa|fasta|fa) ) ]]
                then
                        input_src=`dirname "$( realpath "${i}" )"`
                        rename $i
                        echo -e "\tThis function will remove sequences that havea specified number of gaps at the start or end of the sequence. Proceed and enter the desired number of gaps at the start and end positions of the sequences.\n\tRemoving seqeunces in `basename -- $i`..."
                        read -p "Please enter the number of gaps at start position: " start_gaps
                        read -p "Please enter the number of gaps at the end position: " end_gaps
                        echo -e "\tRemoving sequences in `basename -- ${i}` that have ${start_gaps} gaps in start position and ${end_gaps} in end position"
                        $AWK_EXEC -v start_g=$start_gaps -v end_g=$end_gaps \
                                'BEGIN{RS="\n>"; FS="\n"; ORS="\n>"; OFS="\n"; }; '#'BEGIN{FS=""; OFS=""; }; /^>/ { print "\n" $0 }; !/^>/ { for(v=start_p; v<=end_p; v++) { printf "%s", $v; if (v <= end_p) { printf "%s", OFS; } else { printf "\n"; } }}' $input > ${input_src}/${output_filename}_trmmd.aln
                        echo -e "\n\tDONE. All removed records have been stored in $input > ${input_src}/${output_filename}_short.aln\n"
                else
                        echo "input file error in `basename $i`: input file should be a .fasta file format"
                        continue
                fi
        done
}
