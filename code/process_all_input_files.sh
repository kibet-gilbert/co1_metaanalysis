#!/bin/bash
# this script performs may functions including: run awk script to generate .fasta format files, delete repeated seqs, trim seqs ...

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
                echo "Usage: ${FUNCNAME[1]} file1.*[file2.* file3.* ...]"
                return 1

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
			input_src=`dirname "$( realpath "${i}" )"`
			rename
			echo -e "\nLet us proceed with file '${input_filename}'..."
			${AWK_EXEC} -f ${AWK_SCRIPT} "$i" > ${input_src}/${output_filename}.fasta
		else
			echo "input file error in `basename -- $i`: input file should be a .tsv file format"
			continue
		fi
	done
}


bolddata_retrival() { # This fuction retrives data belonging to a list of country names given. Input can be a file containing names of select countries or idividual country names

	usage $@
	echo -e "\n\tDownloading data of countries named in $@ from www.boldsystems.org"

	IFS=$'\n'
	
	for i in `cat $@`
	do
		wget --show-progress --progress=bar:noscroll --retry-connrefused -t inf -O ${inputdata_path}bold_africa/"${i}".xml -a ${inputdata_path}wget_log http://www.boldsystems.org/index.php/API_Public/combined?geo="${i}"&taxon=arthropoda&format=tsv
	done
}


build_tsv() { #This function generates .tsv files from .xml files using python script and Beautifulsoup4 and pandas package

	usage $@
	
	TAB=$(printf '\t')
	
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
 	if [ $# -eq 0 ]
	then
		echo "Input error..."
      		echo "Usage: ${FUNCNAME[0]} seq.fasta [seq2.fasta seq3.fasta ...]"
      		return
        fi
	
	unset headers
	until [[ ( -f "$headers" ) && ( `basename -- "$headers"` =~ .*_(fasta|fa|afa) ) ]]
	do
		echo -e "\nFor the headers_[aln|fasta|fa|afa] input provide the full path to the file, the filename included."
		read -p "Please enter the file to be used as the FASTA headers source: " headers
		sed -i 's/\r$//g; s/ /_/g; s/\&/_n_/g; s/\//\\&/g' $headers
	done
	
	echo -e "\n\tStarting operation....\n\tPlease wait, this may take a while...."
	for i in "$@"
	do
		unset records
		number_of_replacements=0
		records=$( grep ">" $i | wc -l )
		unset x
		unset y
		unset z
		echo -e "\nProceeding with `basename -- $i`..."
		for line in `cat ${headers}`
		do
			#x=$( head -10 idrck_headers | tail -1 | awk 'BEGIN { FS="|"; }{print $1;}') && echo $x
			x=`echo "$line" | ${AWK_EXEC} 'BEGIN { RS="\n"; FS="|"; }{ x = $1; print x; }'`
			y=`echo "$line" | ${AWK_EXEC} 'BEGIN { RS="\n"; FS="|"; }{ y = $0; print y; }'`
			#echo -e "\n $x \n $y"

			#Characters to replace from the headers as they will affect the performance of sed: carriage Returns (), white spaces ( ), back slashes (/), and ampersand '&' characters; they greately hamper the next step of header substitution.
			sed -i 's/\r$//g; s/ /_/g; s/\&/_n_/g; s/\//+/g' $i

			z=`grep "$x" $i`
			#echo "$z"
			for one_z in `echo -e "${z}"`
			do
				if [ $one_z == $y ]
				then
					echo -e "Change for ${x} already in place..."
					continue
				else
					echo -e "Substituting header for ${x}..."
					sed -i "s/${one_z}/${y}/g" $i
					#sed -i "s/^.*\b${x}\b.*$/${y}/g" $i
				fi
				number_of_replacements=$( expr $number_of_replacements + 1 )
			done
		done
		echo -e "\nDONE. $number_of_replacements replacements done in `basename -- $i` out of $records records it has"
	done
	echo -e "\n\tCongratulations...Operation done."
}

#===============================================================================================================================================================

delete_repeats() { #This function takes a fasta_format_sequences file and deletes repeats of sequences based on identical headers.
	#in multiple files at once: awk -F'[|]' 'FNR%2{f=seen[$1]++} !f' *
	#in each file: awk -F'[|]' 'FNR==1{delete seen} FNR%2{f=seen[$1]++} !f' *
	if [ $# -eq 0 ]
        then
                echo "Input error..."
                echo "Usage: ${FUNCNAME[0]} file1.*[file2.* file3.* ...]"
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
			echo -e "\t`echo -e "$duplicate_headers" | wc -l` records are repeated in $i,\n\twould you like to proceed and delete all repeats?"
			read -p "Please enter [Yes] or [No] to proceed: " choice
		else
			choice="No"
		fi
		case $choice in
			YES|Yes|yes|Y|y)
				concatenate_fasta_seqs $i
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

#===============================================================================================================================================================

delete_unwanted() { #this function copys a record that fits a provided pattern, i.e a name that may be from a non-insect class based on the taxon_name_description; the arguments provided, are the files to be searched for the patterns
	# To get the list of orders in description_taxon_names and their frequencies, from  which to select the undesired patterns (names), do: 
	#grep ">" seqs.fasta | awk 'BEGIN {FS="|"; OFS="|" ; }; {print $2}' |sort | uniq -c > seqs_orders && less seqs_orders

	if [ $# -eq 0 ]
        then
                echo "Input error..."
                echo "Usage: ${FUNCNAME[0]} file1.*[file2.* file3.* ...]"
                return 1

        fi

	echo -e "To delete sequences with specific words in the headers please choose [Yes] to proceed or [No] to cancel"
	PS3='Select option YES to delete, [1] or NO to exit, [2]: '	
	select option in YES NO
	do
		unset pattern_name
		matching_records=0

		regexp='^[a-zA-Z0-9/_-\ ]+$'

		case $option in
			YES)
				until [[ "$pattern_name" =~ $regexp ]]
				do
					read -p "Please enter string pattern to be searched:: " pattern_name
				done
				
				echo -e "\n\tDeleting all records with description '$pattern_name'..."
				matching_records=`grep $pattern_name $i | wc -l`
				echo -e "\t${matching_records} records match the pattern: $pattern_name"
				
				for i in "$@"
				do
					echo -e "\nProceeding with `basename -- $i`..."
					matching_records=`grep $pattern_name $i | wc -l`
					echo -e "${matching_records} records match the pattern: $pattern_name"
					rename
					input_src=`dirname "$( realpath "${i}" )"`
					
					#awk -v name="$input_r" 'BEGIN {RS="\n>"; ORF="\n>"}; $0 ~ name {print ">"$0}' test_all.fasta | less
					
					concatenate_fasta_seqs $i
					echo -e "\tRemoving any records with '$pattern_name' from file..."
					$AWK_EXEC -v pattern="$pattern_name" 'BEGIN { RS="\n>"; ORS="\n"; FS="\n"; OFS="\n" }; $1 ~ pattern {print ">"$0;}' $i >> ${input_src}/${output_filename}_${pattern_name}.fasta
					sed -i "/$pattern_name/,+1 d" $i
				done
				echo -e "\n\tDONE. All deleted records have been stored in '${output_filename}_${pattern_name}.fasta'"
				;;
			NO)
				echo -e "Exiting deletion of unwanted sequences..."
				break
		esac
	done
}

#===============================================================================================================================================================

trimming_seqaln() { #This function trims aligned sequences in a file on both ends and retains a desired region based on input field (sequence position) values.

	if [ $# -eq 0 ]
        then
                echo "Input error..."
                echo "Usage: ${FUNCNAME[0]} file1.*[file2.* file3.* ...]"
                return 1
        fi

	for i in "$@"
	do
		if [ ! -f $i ]
                then
                        echo "input error: file '$i' is non-existent!"
                elif [[ ( -f $i ) && ( `basename -- "$i"` =~ .*\.(aln|afa|fasta|fa|fst) ) ]]
                then
			input_src=`dirname "$( realpath "${i}" )"`
			rename $i
			echo -e "\tThis function will trim your sequences at specific positions and output the desired columns. Proceed and enter the desired start and end positions of the blocks to extract.\n\tTrimming `basename -- $i`..."
			unset start_pos
			unset end_pos
			regexp='^[0-9]+$'
			until [[ "$start_pos" =~ $regexp ]]
			do
				read -p "Please enter the start position: " start_pos
			done

			until [[ "$end_pos" =~ $regexp ]]
			do
				read -p "Please enter the end position: " end_pos
			done

			echo -e "\tYou are trimming `basename -- ${i}` at position ${start_pos} to ${end_pos}"

			concatenate_fasta_seqs $i
			$AWK_EXEC -v start_p=$start_pos -v end_p=$end_pos \
				'BEGIN{FS=""; OFS=""; }; /^>/ {if (FNR==1) {print $0; } else { print "\n" $0 }}; !/^>/ { for(v=start_p; v<=end_p; v++) { printf "%s", $v; if (v <= end_p) { printf "%s", OFS; } }}' $i > ${input_src}/${output_filename}_trmmd${start_pos}-${end_pos}.aln
			#awk 'BEGIN {FS=""; OFS=""; }; /^>/ {print "\n" $0 }; !/^>/ { for(v=1087; v<=2574; v++) { printf "%s", $v; if (v <= 2574) { printf "%s", OFS; } else { printf "\n"; } }}' input.aln | less
			echo -e "\n\tDONE. All trimmed records have been stored in ${input_src}/${output_filename}_trmmd${start_pos}-${end_pos}.aln\n"
		else
			echo "input file error in `basename $i`: input file should be a .fasta file format"
                        continue
		fi
	done
}


#===============================================================================================================================================================

delete_shortseqs() { #This function identifies and removes sequences that have specified number of gaps at the ends. It stores the cleaned sequences.

	if [ $# -eq 0 ]
        then
                echo "Input error..."
                echo "Usage: ${FUNCNAME[0]} file1.*[file2.* file3.* ...]"
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
			concatenate_fasta_seqs $i

			echo -e "\tThis function will remove sequences that have a specified number of gaps, '-', at the start or end of the sequence. Proceed and enter the accepted maximum number of gaps at the start and end positions of the sequences.\n\tIntegers only accepted!!!"
			unset start_gaps
			unset end_gaps
			regexp='^[0-9]+$'
                        until [[ "$start_gaps" =~ $regexp ]]
                        do
				read -p "Please enter the muximum number of gaps allowed at start position: " start_gaps
			done

                        until [[ "$end_gaps" =~ $regexp ]]
                        do
				read -p "Please enter the maximum number of gaps allowed at the end position: " end_gaps
			done

                        echo -e "\tRemoving sequences in `basename -- ${i}` that have more than ${start_gaps} gaps in start position and ${end_gaps} in end position"

			$AWK_EXEC -v start_g=$start_gaps -v end_g=$end_gaps ' /^>/ { hdr=$0; next }; match($0,/^-*/) && RLENGTH<=start_g && match($0,/-*$/) && RLENGTH<=end_g { print hdr; print }' $i > ${input_src}/${output_filename}_sn${start_gaps}-eg${end_gaps}.aln
			#awk -v start_g=$start_gaps -v end_g=$end_gaps ' /^>/ { hdr=$0; next }; (x="^-{"start_g+1",}")(y="-{"end_g+1",}$") !match($0, x) && !match($0, y)  {print x y hdr; print }' input_trmmd.aln | wc -l #Does not remove all as needed
			echo -e "\n\tDONE. All cleaned records have been stored in $input > ${input_src}/${output_filename}_sg${start_gaps}-eg${end_gaps}.aln\n"
                else
                        echo "input file error in `basename $i`: input file should be a .aln file format"
                        continue
                fi
        done
}

#===============================================================================================================================================================


delete_shortseqs_N() { #This function identifies and removes sequences that have specified number of gaps "-" and Ns at the ends or Ns strings within. It stores the cleaned sequences.
	if [ $# -eq 0 ]
	then
		echo "Input error..."
		echo "Usage: ${FUNCNAME[0]} file1.*[file2.* file3.* ...]"
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
			concatenate_fasta_seqs $i
			
			echo -e "\tThis function will remove sequences that have a specified number of undefined nucleotides, 'N', or gaps, '-', at the start or end and specific maximum length of N-string within the sequence. Proceed and enter the accepted maximum number of Ns or '-'s at the start, within and end positions of the sequences.\n\tIntegers only accepted!!!"
			unset start_Ns
			unset end_Ns
			unset mid_Ns
			regexp='^[0-9]+$'
			until [[ "$start_Ns" =~ $regexp ]]
			do
				read -p "Please enter the muximum number of Ns or '-'s allowed at start position: " start_Ns
			done
			
			until [[ "$mid_Ns" =~ $regexp ]]
			do
				read -p "Please enter the maximum length of N-string allowed within the sequence:" mid_Ns
			done

			until [[ "$end_Ns" =~ $regexp ]]
			do
				read -p "Please enter the maximum number of Ns or '-'s allowed at the end position: " end_Ns
			done
			
			echo -e "\tRemoving sequences in `basename -- ${i}` that have more than ${start_Ns} Ns or '-'s in start position, ${mid_Ns} length N string within  and ${end_Ns} in end position"

			#$AWK_EXEC -v start_N=$start_Ns -v mid_N=$mid_Ns -v end_N=$end_Ns ' /^>/ { hdr=$0; next }; match($0,/^[-Nn]*/) && RLENGTH<=start_N && match($0,/[Nn]*/) && RLENGTH<=mid_N && match($0,/[-Nn]*$/) && RLENGTH<=end_N { print hdr; print }' $i > ${input_src}/${output_filename}_sN${start_Ns}-eN${end_Ns}.aln			
			$AWK_EXEC -v start_N=$start_Ns -v end_N=$end_Ns ' /^>/ { hdr=$0; next }; match($0,/^[-Nn]*/) && RLENGTH<=start_N && match($0,/[-Nn]*$/) && RLENGTH<=end_N { print hdr; print }' $i > ${input_src}/${output_filename}_sN${start_Ns}-eN${end_Ns}.aln
			echo -e "\n\tDONE. All cleaned records have been stored in $input > ${input_src}/${output_filename}_sN${start_Ns}-eN${end_Ns}.aln\n"
		else
			echo "input file error in `basename $i`: input file should be a .aln file format"
			continue
		fi
	done
}
#===============================================================================================================================================================

remove_gaps() { # Removing gaps, "-" in a sequnce.
	# Works only for singular line header and sequence lines, i.e does not concatenate separate lines of the same sequence.
	#awk 'BEGIN{ RS="\n";ORS="\n" }/^>/{print}; !/^>/{ gsub("-","",$0); print $0 }' enafroCOI_500to700_data-650to660_st22n1006-en1474n3479.aln | less -S
	if [ $# -eq 0 ]
	then
		echo "Input error..."
		echo "Usage: ${FUNCNAME[0]} file1.*[file2.* file3.* ...]"
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
			echo -e "\n\tRemoving gaps, '-', from `basename -- ${i}`"
			concatenate_fasta_seqs $i
			$AWK_EXEC 'BEGIN{ RS="\n";ORS="\n" }/^>/{print}; !/^>/{ gsub("-","",$0); print $0 }' $i > ${input_src}/${output_filename}_dgpd.fasta
		else
			echo "input file error in `basename $i`: input file should be a .aln file format"
			continue
		fi
	done

}


#===============================================================================================================================================================

concatenate_fasta_seqs() { # This function converts a multiple line FASTA format sequence into a two line record of a header and a sequnce lines.
	#awk '/^>/ {if (FNR==1) print $0; else print "\n" $0; }; !/^>/ {gsub("\n","",$0); printf $0}' renafroCOI_500to700_data-650to660_st22n1006-en1474n3479_head25000_tail125001.afa | less -S

	if [ $# -eq 0 ]
	then
		echo "Input error..."
		echo "Usage: ${FUNCNAME[0]} seqfile1.fasta [seqfile2.fasta seqfile3.fasta ...]"
		return 1
	fi

	for i in "$@"
	do
		if [ ! -f $i ]
		then
			echo "input error: file '$i' is non-existent!"
		elif [[ ( -f $i ) && ( `basename -- "$i"` =~ .*\.(aln|afa|fasta|fa|fst) ) ]]
		then
			echo -e "\t concatinating sequence lines for each record in `basename -- ${i}`..."
			input_src=`dirname "$( realpath "${i}" )"`
			$AWK_EXEC '/^>/ {if (FNR==1) print $0; else print "\n" $0; }; !/^>/ {gsub("\n","",$0); printf $0}' $i > ${input_src}/outfile.afa && mv ${input_src}/outfile.afa ${i}
		else
			echo "input file error in `basename $i`: input file should be a .aln file format"
			continue
		fi
	done
}



retrive_original_seqs() { # Retriving a subset of unaligned sequences from original parent file
	if [ $# -ne 2 ]
	then
		echo "Input error..."
		echo "Usage: ${FUNCNAME[0]} seqfile.fasta parentfile.fasta"
		return 1
	fi

	cat $1 > ./in_put.fasta
	cat $2 >> ./in_put.fasta
	delete_repeats ./in_put.fasta
	x=`wc -l $1`
	$AWK_EXEC -v x=$x '{if ( NRF<=x ) {next} else {print $0} }' ./in_put.fasta > in_put1.fasta
	cat ./in_put1.fasta > ./in_put.fasta
	cat $2 >> ./in_put.fasta
	delete_repeats ./in_put.fasta
	y=`wc -l ./in_put1.fasta`
	$AWK_EXEC -v y=$y '{if ( NRF<=y ) {next} else {print $0} }' ./input1.fasta > out_put.fasta 
	mv out_put.fasta $1
}


