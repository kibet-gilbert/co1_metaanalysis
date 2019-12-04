#!/bin/bash
# this script performs may functions including: run awk script to generate .fasta format files, delete repeated seqs, trim seqs ...

AWK_EXEC=$( which gawk )
PYTHON_EXEC=$( which python )
RSCRIPT_EXEC=$( which Rscript )
co1_path=~/bioinformatics/github/co1_metaanalysis/
AWK_SCRIPT=${co1_path}code/buildfasta_jb.awk
xml_to_tsv=${co1_path}code/xml_to_tsv.py
data_cleanup=${co1_path}code/data_cleanup.R
inputdata_path=${co1_path}data/input/

usage() { #checks if the positional arguments (input files) for execution of the script are defined
        if [ $# -eq 0 ]
        then
                echo "Input error..."
                echo "Usage: ${FUNCNAME[1]} file1.*[file2.* file3.* ...]"
                exit 1

        fi
}

realpath() { #
	${PYTHON_EXEC} -c "import os,sys; print(os.path.realpath(sys.argv[1]))" $1
}


rename() { #generates output file names with same input filename prefix. The suffix (".suffix") is set in individual functions that perform different tasks.
        input_filename=`basename -- $i`
        output_filename=${input_filename%.*}
	filename_ext=${input_filename##*.}
	src_dir_path=`dirname $(realpath ${i})`
	src_dir=${src_dir_path##*/}
	#echo -e "input is $i \ninput_filename is $input_filename \noutput_filename is $output_filename \nfilename_ext is $filename_ext \nsrc_dir_path is $src_dir_path \nsrc_dir is $src_dir"
}


build_fasta() { #This function generates .fasta files from .tsv files using an awk script

	usage $@
	
	echo "generating .fasta files from .tsv metadata files"

	for i in "$@"
	do
		if [ ! -f $i ]
		then
			echo "input error: file '$i' is non-existent!"
		elif [[ ( -f $i ) && ( `basename -- "$i"` =~ .*\.(tsv)$ ) ]]
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
	if [[ ( $# -eq 0 ) || ! ( `echo $1` =~ -.*$ ) ]]
	then
		echo "Input error..."
		echo "function usage: ${FUNCNAME[0]} [-a] [-c <name of country>] [-f <a file with list of countries>]"
		return 1
	fi

	local OPTIND=1
	countries=()

	while getopts 'ac:f:' key
	do
		case "${key}" in
			f)
				if [ ! -f $OPTARG ]
				then
					echo "input error: file $OPTARG is non-existent!"
				elif [[ ( -f $OPTARG ) && ( `basename $OPTARG` =~ ^countries$ ) ]]
				then
					countries+=("$(while IFS="\n" read -r line || [[ "$line" ]]; do geography+=("`echo $line | sed 's/ /%20/g'`"); done < $OPTARG)")
				else
					echo "input file error in `basename $OPTARG`: input file should be named 'countries'"
				fi
				;;
			c)
				countries+=(`echo $OPTARG | sed 's/ /%20/g'`)
				;;
			a)
				countries=("all")
				;;
			?)
				echo "Input error..."
				echo "function usage: ${FUNCNAME[0]} [-a] [-c <name of country>] [-f <a file with list of countries>]"
				return 1
				;;
		esac
	done

	echo -e "\n\tDownloading data of countries named in ${countries[@]} from www.boldsystems.org"
	unset taxon_nam
	regexp='^[a-zA-Z0-9/_-\ ]+$'
	
	until [[ "$taxon_nam" =~ $regexp ]]
	do
		read -p "Please enter taxon name to be searched, ensure the spelling is right otherise you get everything downloaded:: " taxon_nam
	done

	taxon_name=`echo $taxon_nam | sed 's/ /%20/g'`
	
	wgetoutput_dir=${inputdata_path}bold_africa/${taxon_name}
	until [[ -d ${wgetoutput_dir} ]]
	do
		echo "Creating output directory '${wgetoutput_dir}'"
		mkdir ${wgetoutput_dir}
	done
	
	IFS=$'\n'
	if [[ ! ( `echo ${countries[0]}` =~ "all" ) ]]
	then
		for i in ${countries[@]}
		do
			wget --show-progress --progress=bar:noscroll --retry-connrefused -t inf -O ${wgetoutput_dir}/"${i}".xml -a ${wgetoutput_dir}/${taxon_nam}_wget_log "http://www.boldsystems.org/index.php/API_Public/combined?geo=${i}&taxon=${taxon_name}&format=xml"
		done
	elif [[ ( `echo ${countries[0]}` =~ "all" ) ]]
	then
		wget --show-progress --progress=bar:noscroll --retry-connrefused -t inf -O ${wgetoutput_dir}/"${taxon_nam}".xml -a ${wgetoutput_dir}/${taxon_nam}_wget_log "http://www.boldsystems.org/index.php/API_Public/combined?taxon=${taxon_name}&format=xml"
	fi
}


build_tsv() { #This function generates .tsv files from .xml files using python script and Beautifulsoup4 and pandas package

	usage $@
	
	TAB=$(printf '\t')
	
	echo -e "\n\tgenerating .tsv files from .xml downloads"

	for i in "$@"
	do
		if [ ! -f $i ]
		then
			echo -e "\n\t!!!input error: file '$i' is non-existent!"
		elif [[ ( -f $i ) && ( `basename -- "$i"` =~ .*\.(xml)$ ) ]]
		then
			input_src=`dirname "$( realpath "${i}" )"`
			rename 
			echo -e "\tLet us proceed with file '${input_filename}'..."
			sed 's/class/Class/g' "$i" | sed "s/$TAB/,/g" > ${inputdata_path}bold_africa/input.xml
			${PYTHON_EXEC} ${xml_to_tsv} ${inputdata_path}bold_africa/input.xml && mv output.tsv ${input_src}/${output_filename}.tsv
			echo -e "\n\tDONE. The output file has been stored in ${input_src}/${output_filename}.tsv"
		else
			echo -e "\n\tinput file error in `basename -- '$i'`: input file should be a .xml file format"
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
                elif [[ ( -f $i ) && ( `basename -- "$i"` =~ .*\.(tsv)$ ) ]]
                then
                        rename
			input_src=`dirname "$( realpath "${i}" )"`
			outfilename=`echo "${input_src}/${output_filename}"`
                        echo -e "\nLet us proceed with file '${input_filename}'..."
                        ${RSCRIPT_EXEC} --vanilla ${data_cleanup} $i

			case $output_filename in
				Algeria|Madagascar|Angola|Malawi|Benin|Mali|Botswana|Mauritania|Burkina_Faso|Mauritius|Morocco|Cameroon|Mozambique|Cape_Verde|Namibia|Central_African_Republic|Nigeria|Chad|Niger|Comoros|Republic_of_the_Congo|Cote_d_Ivoire|Reunion|Democratic_republic_of_the_Congo|Djibouti|Sao_Tome_and_Principe|Egypt|Senegal|Equatorial_Guinea|Seychelles|Eritrea|Sierra_Leone|Somalia|Gabon|South_Africa|Gambia|Ghana|Sudan|Guinea-Bissau|Swaziland|Guinea|Togo|Tunisia|Lesotho|Liberia|Zambia|Libya|Zimbabwe)
					input=${outfilename}_500to700_data.tsv
					output=${output_files_africa[0]}
					append_tsvfile

					input=${outfilename}_650to660_data.tsv
					output=${output_files_africa[1]}
					append_tsvfile

					input=${outfilename}_all_data.tsv 
					output=${output_files_africa[2]}
					append_tsvfile

					input=${outfilename}_Over499_data.tsv
					output=${output_files_africa[3]}
					append_tsvfile
					
					input=${outfilename}_Over700_data.tsv
					output=${output_files_africa[4]}
					append_tsvfile
					
					input=${outfilename}_Under500_data.tsv
					output=${output_files_africa[5]}
					append_tsvfile
					;;
				Kenya|Tanzania|Uganda|Rwanda|Burundi|South_Sudan|Ethiopia)
					input=${outfilename}_500to700_data.tsv
					output=${output_files_eafrica[0]}
					append_tsvfile
					
					input=${outfilename}_650to660_data.tsv
					output=${output_files_eafrica[1]}
					append_tsvfile
					
					input=${outfilename}_all_data.tsv
					output=${output_files_eafrica[2]}
					append_tsvfile
					
					input=${outfilename}_Over499_data.tsv
					output=${output_files_eafrica[3]}
					append_tsvfile
					
					input=${outfilename}_Over700_data.tsv
					output=${output_files_eafrica[4]}
					append_tsvfile
					
					input=${outfilename}_Under500_data.tsv
					output=${output_files_eafrica[5]}
					append_tsvfile
					
					;;
				*)
					echo -e "The file $output_filename \b.tsv is not in the list of African countries or is not in the right format."
					;;
			esac
		 else
                        echo "input file error in `basename -- $i`: input file should be a .tsv file format"
                        continue
                fi
        done
}

#=====================================================================================
subset_seqs(){ #This function takes a faster sequence file and split it into multiple files simply based on nucleotide sequence length
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
		elif [[ ( -f $i ) && ( `basename -- "$i"` =~ .*\.(aln|afa|fasta|fa|fst)$ ) ]]
		then
			input_src=`dirname "$( realpath "${i}" )"`
			rename $i

			echo -e "\n\tProceeding with `basename -- ${i}`...\nThe file will be split into four files based on nucleotide sequence length: len under 500; len over 700; len of 650to660; len of 500to700 - 650to660."
			#Concatenating the fasta nucleotide sequences
			concatenate_fasta_seqs $i
			#Removing gaps
			remove_gaps $i
			i=${input_src}/${output_filename}_dgpd.fasta
			#Introducing a field "l-xxx" that has the length of the sequence in the header
			echo -e "\tWould you like to introduce a field 'l-xxx* in the header that denotes the length of the sequence?'"
			read -p "Please enter [Yes] or [No] to proceed: " choice
			case $choice in
				YES|Yes|yes|Y|y)
					echo -e "Adding a seq. length field, ';l-xxx*' in the header to denote the sequence length..."
					$AWK_EXEC '/^>/{
					hdr=$0; next} {
					seq=$0 } match(seq,/^.*$/) { 
					LEN=RLENGTH }{
					print hdr"|l-"LEN; print seq }' $i > ${input_src}/${output_filename}_dgpd_len.fasta
					mv ${input_src}/${output_filename}_dgpd_len.fasta ${input_src}/${output_filename}_dgpd.fasta
					echo -e "\tDONE adding seq. length to the header."
					;;
				No|NO|no|N|n)
					echo -e "\tSequence length field will not be added to the header..."
					;;
				*)
					echo "ERROR!!! Invalid selection"
					;;
			esac

			i=${input_src}/${output_filename}_dgpd.fasta
			echo -e "\tProceeding with subsetting the file..."

			$AWK_EXEC -v output_filename=$output_filename -v filename_ext=$filename_ext -v input_src=$input_src 'BEGIN{FS="|";}; /^>/{
			hdr=$0; next} {
				seq=$0 } match(seq,/^.*$/) { len=RLENGTH }; {
				if(len<500){
					print hdr"\n" seq > input_src"/"output_filename"_Under500."filename_ext
				}
				else if (len>700){
					print hdr"\n" seq > input_src"/"output_filename"_Over700."filename_ext
				}
				else if (len>=650 && len<=660){
					print hdr"\n" seq > input_src"/"output_filename"_650to660."filename_ext
				}
				else {
					print hdr"\n" seq > input_src"/"output_filename"_500to700_data-650to660."filename_ext
				}
			}' $i
			
			echo -e "\n\tCongratulations. OPERATION DONE!\n\tRecords with less than 500 nucleotides have been saved in '${input_src}/${output_filename}_Under500.${filename_ext}'\n\tRecords with over 700 nucleotides have been saved in '${input_src}/${output_filename}_Over700.${filename_ext}'\n\tRecords with 650 to 660 nucleotides have been saved in '${input_src}/${output_filename}_650to660.${filename_ext}'\n\tAnd records with 500 to 700 nucleotides excluding those with 650 to 660 nucleotides have been saved in '${input_src}/${output_filename}_500to700_data-650to660.${filename_ext}'"
		else
			echo "input file error in `basename $i`: input file should be a .fasta file format"
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

replacing_headers() { #This function takes an input file of edited_fasta_format_headers and searches through a fasta_format_sequence file and substitute it's headers if their uniq IDs match
 	if [ $# -eq 0 ]
	then
		echo "Input error..."
      		echo "Usage: ${FUNCNAME[0]} seq.fasta [seq2.fasta seq3.fasta ...]"
      		return
        fi
	
	unset headers
	until [[ ( -f "$headers" ) && ( `basename -- "$headers"` =~ .*_(headers|fasta|fa|afa|aln)$ ) ]]
	do
		echo -e "\nFor the headers_[aln|fasta|fa|afa] input, provide the full path to the file, the filename included."
		read -p "Please enter the file to be used as the FASTA headers source: " headers
		#$.*/[\]'^
		sed -i "s/\r$//g; s/ /_/g; s/\&/_n_/g; s/\//+/g; s/'//g; s/\[//g; s/\]//g" $headers
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
			sed -i "s/\r$//g; s/ /_/g; s/\&/_n_/g; s/\//+/g; s/'//g; s/\[//g; s/\]//g" $i

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
			echo -e "\t...`echo -e "$duplicate_headers" | wc -l` records are repeated in $( basename -- ${i} ),\n\t...would you like to proceed and delete all repeats?"
			read -p "Please enter [Yes] or [No] to proceed: " choice
		else
			choice="No"
		fi
		case $choice in
			YES|Yes|yes|Y|y)
				echo -e "\t...Will proceed with deleting duplicates in $( basename -- ${i} )"
				concatenate_fasta_seqs $i
				$AWK_EXEC -F'[>|]' 'FNR==1{delete seen} FNR%2{f=seen[$2]++} !f' $i > ${input_src}/${output_filename}_cleaned && mv ${input_src}/${output_filename}_cleaned $( realpath "${i}" )
				echo -e "\t...Duplicate records deleted"
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
move_unwanted() { #	
	for i in "$@"
	do
		echo -e "\nProceeding with `basename -- $i`..."
		matching_records=`grep $pattern_name $i | wc -l`
		echo -e "${matching_records} records match the pattern '$pattern_name'"
		rename
		input_src=`dirname "$( realpath "${i}" )"`
                                        
		if [ $matching_records -gt 0 ]
		then
			concatenate_fasta_seqs $i
			echo -e "\tRemoving any records with '$pattern_name' description in header from file..."
			$AWK_EXEC -v pattern="$pattern_name" 'BEGIN { RS="\n>"; ORS="\n"; FS="\n"; OFS="\n" }; $1 ~ pattern {print ">"$0;}' $i >> ${input_src}/${output_filename}_${unwanted}.fasta
			sed -i "/$pattern_name/,+1 d" $i
			echo -e "\n\tDONE. All deleted records have been stored in '${output_filename}_${unwanted}.fasta'"
		fi
	done
}

delete_unwanted() { #this function copys a record that fits a provided pattern, e.g a non-insect class taxon_name_description; the arguments provided, are the files to be searched for the patterns
	# LOGIC behind the code: To get the list of orders in description_taxon_names and their frequencies, from  which to select the undesired patterns (names), do: 
	#grep ">" seqs.fasta | awk 'BEGIN {FS="|"; OFS="|" ; }; {print $2}' |sort | uniq -c > seqs_orders && less seqs_orders

	if [ $# -eq 0 ]
        then
                echo "Input error..."
                echo "Usage: ${FUNCNAME[0]} file1.*[file2.* file3.* ...]"
                return 1

        fi

	echo -e "To delete sequences with specific words in the headers please select one of the options [1|2|3] to proceed or [4] to cancel"
	options[0]="Move records with word patterns specified in a file into one file"
	options[1]="Move records with word patterns specified in a file into individual word specific file"
	options[2]="Move records with specific single word into word specific file"
	options[3]="Exit"
	PS3='Select option [1|2|3] to delete, or [4] to exit: '	
	select option in "${options[@]}"
	do
		unset pattern_name
		unset input_pattern_file
	
		matching_records=0

		regexp='^[a-zA-Z0-9/_-\ ]+$'

		case $option in
			${options[0]})
				#echo "no error"
				until [[ -f ${input_pattern_file}  ]]
				do
					read -p "Please enter the path to the file with pattern names to be removed:: " input_pattern_file
					#cat ${input_pattern_file}
				done

				for line in `cat ${input_pattern_file}`
				do
					pattern_name=${line}
					unwanted="unwanted"
					#echo ${pattern_name}
					move_unwanted "$@"
				done
				;;
			${options[1]})
				until [[ -f ${input_pattern_file} ]]
				do
					read -p "Please enter the path to the file with pattern names to be removed:: " input_pattern_file
				done

				for line in `cat ${input_pattern_file}`
				do
					pattern_name=${line}
					unwanted=${pattern_name}
					move_unwanted "$@"
				done
				;;
			${options[2]})
				until [[ "$pattern_name" =~ $regexp ]]
				do
					read -p "Please enter string pattern to be searched:: " pattern_name
				done
				unwanted=${pattern_name}
				move_unwanted "$@"
				;;
			${options[3]})
				echo -e "Exiting deletion of unwanted sequences..."
				break
				;;
			*)
				echo "INVALID choice $REPLY"
				;;
		esac
	done
}

#===============================================================================================================================================================

trimming_seqaln() { #This function trims aligned sequences in a file on both ends and retains a desired region based on input field (sequence position) values.

	if [ $# -eq 0 ]
        then
                echo "Input error..."
                echo "Usage: ${FUNCNAME[0]} file1.*[file2.* file3.* ...]\nInput files must be well aligned files"
                return 1
        fi

	for i in "$@"
	do
		if [ ! -f $i ]
                then
                        echo "input error: file '$i' is non-existent!"
                elif [[ ( -f $i ) && ( `basename -- "$i"` =~ .*\.(aln|afa|fasta|fa|fst)$ ) ]]
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

delete_shortseqs_gaps() { #This function identifies and removes sequences that have specified number of gaps at the ends. It stores the cleaned sequences.

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
                elif [[ ( -f $i ) && ( `basename -- "$i"` =~ .*\.(aln|afa|fasta|fa)$ ) ]]
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

regexp='^[0-9]+$'
set_start_Ngaps() { # This functions sets the value of the variable "start_Ngaps"
	until [[ "$start_Ngaps" =~ $regexp ]]
	do
		read -p "Please enter the muximum number of '-'s or Ns allowed at start position: " start_Ngaps
	done
}
set_end_Ngaps() { #This function sets the value of the variable "end_Ngaps"
	until [[ "$end_Ngaps" =~ $regexp ]]
	do
		read -p "Please enter the maximum number of '-'s or Ns allowed at the end position: " end_Ngaps
	done
}
set_mid_Ns() { #This function sets the value of the variable "mid_Ns"
	 until [[ "$mid_Ns" =~ $regexp ]]
	 do
		 read -p "Please enter the maximum length of N-string allowed within the sequence: " mid_Ns
	 done
}

delete_shortseqs() { #This function identifies and removes sequences that have specified number of gaps "-" and Ns at the ends or Ns strings within. It stores the cleaned sequences.
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
		elif [[ ( -f $i ) && ( `basename -- "$i"` =~ .*\.(aln|afa|fasta|fa)$ ) ]]
		then
			input_src=`dirname "$( realpath "${i}" )"`
			rename $i
			concatenate_fasta_seqs $i
			
			echo -e "\tYou are going to remove sequences that have more than a specific number of undefined nucleotides, 'N', or gaps, '-', at the beginning and end or specific maximum length of N character-string within the sequence."
			echo -e "\tWhich type of characters are to consider?"
			PS3='Select a choice from the following options, Enter [1] or [2] or [3] or [4]: '
			options=("Gaps at the ends only" "Ns and gaps at the ends only" "Gaps & Ns in the entire sequence only" "Quit")
			select opt in "${options[@]}"
			do
				unset start_Ngaps
				unset end_Ngaps
				unset mid_Ns

				case $opt in
					"Gaps at the ends only")
						echo -e "\tProceed and enter the accepted maximum number of '-'s at the start and end positions of the sequences.\n\tIntegers only accepted!!!"
						set_start_Ngaps

						set_end_Ngaps

						$AWK_EXEC -v start_g=$start_Ngaps -v end_g=$end_Ngaps ' /^>/ { hdr=$0; next }; match($0,/^-*/) && RLENGTH<=start_g && match($0,/-*$/) && RLENGTH<=end_g { print hdr; print }' $i > ${input_src}/${output_filename}_sg${start_Ngaps}-e${end_Ngaps}.aln
						
						echo -e "\n\tDONE. All cleaned records have been stored in ${input_src}/${output_filename}_sg${start_Ngaps}-e${end_Ngaps}.aln\n"
						break
						;;
					"Ns and gaps at the ends only")
						echo -e "\tProceed and enter the accepted maximum number of Ns or '-'s at the start and end positions of the sequences.\n\tIntegers only accepted!!!"
						set_start_Ngaps

						set_end_Ngaps

						$AWK_EXEC -v start_N=$start_Ngaps -v end_N=$end_Ngaps ' /^>/ { hdr=$0; next }; match($0,/^[-Nn]*/) && RLENGTH<=start_N && match($0,/[-Nn]*$/) && RLENGTH<=end_N { print hdr; print }' $i > ${input_src}/${output_filename}_s${start_Ngaps}-e${end_Ngaps}.aln
						echo -e "\n\tDONE. All cleaned records have been stored in $input > ${input_src}/${output_filename}_s${start_Ngaps}-e${end_Ngaps}.aln\n"
						break
						;;
					"Gaps & Ns in the entire sequence only")
						echo -e "\tProceed and enter the accepted maximum number of Ns or '-'s at the start, within and end positions of the sequences.\n\tIntegers only accepted!!!"
						set_start_Ngaps

						set_mid_Ns
						
						set_end_Ngaps
						
						echo -e "\tRemoving sequences in `basename -- ${i}` that have more than ${start_Ngaps} Ns or '-'s in start position, ${mid_Ns} length N string within  and ${end_Ngaps} in end position"
						#substr(s, i [, n])
						#awk -v start_N=10 -v mid_N=11 -v end_N=10 '/^>/{hdr=$0; next} { seq=$0 } match(seq,/^[-Nn]*/) && RLENGTH > start_N { next } { seq=substr(seq,RSTART+RLENGTH) } match(seq,/[-Nn]*$/) && RLENGTH > end_N { next } { seq=substr(seq,1,RSTART-1) } { while (match(seq,/[-Nn]+/)) { if(RLENGTH>mid_N) next seq=substr(seq,RSTART+RLENGTH) } } { print hdr; print $0 }' file
						$AWK_EXEC -v start_N=$start_Ngaps -v mid_N=$mid_Ns -v end_N=$end_Ngaps '
						/^>/{hdr=$0; next}
						{ seq=$0 }
						match(seq,/^[-Nn]*/) && RLENGTH > start_N { next }
						{ seq=substr(seq,RSTART+RLENGTH) }
						match(seq,/[-Nn]*$/) && RLENGTH > end_N { next }
						{ seq=substr(seq,1,RSTART-1) }
						{ while (match(seq,/[-Nn]+/)) {
							if(RLENGTH>mid_N) next
							seq=substr(seq,RSTART+RLENGTH)
							}
						}
						{ print hdr; print $0 }' $i > ${input_src}/${output_filename}_s${start_Ngaps}-${mid_Ns}-e${end_Ngaps}.aln
						echo -e "\n\tDONE. All cleaned records have been stored in $input > ${input_src}/${output_filename}_s${start_Ngaps}-${mid_Ns}-e${end_Ngaps}.aln\n"
						break
						;;
					"Quit")
						break
						;;
					*) echo "INVALID choice $REPLY"
						;;
				esac
			done
		else
			echo "input file error in `basename $i`: input file should be a .aln file format"
			continue
		fi
	done
}
#===============================================================================================================================================================

remove_gaps() { # Removing gaps, "-" in a sequence.
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
   		elif [[ ( -f $i ) && ( `basename -- "$i"` =~ .*\.(aln|afa|fasta|fa)$ ) ]]
		then
			input_src=`dirname "$( realpath "${i}" )"`
			rename $i
			concatenate_fasta_seqs $i

			echo -e "\tRemoving gaps, '-', from `basename -- ${i}`"
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
		elif [[ ( -f $i ) && ( `basename -- "$i"` =~ .*\.(aln|afa|fasta|fa|fst)$ ) ]]
		then
			echo -e "\t...Concatinating sequence lines for each record in `basename -- ${i}`..."
			input_src=`dirname "$( realpath "${i}" )"`
			$AWK_EXEC '/^>/ {if (FNR==1) print $0; else print "\n" $0; }; !/^>/ {gsub("\n","",$0); printf $0}' $i > ${input_src}/outfile.afa && mv ${input_src}/outfile.afa ${i}
		else
			echo "input file error in `basename $i`: input file should be a .aln file format"
			continue
		fi
	done
}


#===============================================================================================================================================================

retrive_originalseqs() { # Retriving a subset of unaligned sequences from original parent file. The input is an FASTA file that has been modified (trimmed or Ns or gaps removed).
	if [ $# -eq 0 ]
	then
		echo "Input error..."
		echo "Usage: ${FUNCNAME[0]} seqfile1.fasta [seqfile2.fasta seqfile3.fasta seqfile4.fasta]"
		return 1
	fi
	
	for i in "$@"
	do
		if [ ! -f $i ]
		then
			echo "input error: file '$i' is non-existent!"
		elif [[ ( -f $i ) && ( `basename -- "$i"` =~ .*\.(aln|afa|fasta|fa) ) ]]
		then
			inputfile=${i}
			unset parentfile
			until [[ ( -f $parentfile ) && ( `basename -- "$parentfile"` =~ .*\.(aln|afa|fasta|fa)$ ) ]]
			do
				read -p "Please enter the path to the parent file with the orginal sequences:: " parentfile
			done

			rename ${inputfile}
			outputfilename=${output_filename}
			concatenate_fasta_seqs ${inputfile} ${parentfile}
			# lets open and save both files using an editor i.e vim to ensure all lines have an "newline" ending.
			vim ${inputfile} <<- EOF
			^[
			:wq!
			EOF
			vim ${parentfile} <<- EOF
			^[
			:wq!
			EOF
			input_src=`dirname "$( realpath "${inputfile}" )"`
			echo -e "\t...Copying ${inputfile} to ./retrinput.fasta"
			cat ${inputfile} > ${input_src}/retrinput.fasta
			echo -e "\t...Appending ${parentfile} to ./retrinput.fasta"
			cat ${parentfile} >> ${input_src}/retrinput.fasta
			delete_repeats ${input_src}/retrinput.fasta <<- EOF
			y
			EOF
			x=$( cat ${inputfile} | wc -l )
			echo -e "\t...Extracting records from ./retrinput.fasta not found in ${inputfile} to ./retrinput1.fasta"
			$AWK_EXEC -v x=$x '{if ( FNR<=x ) {next} else {print $0} }' ${input_src}/retrinput.fasta > ${input_src}/retrinput1.fasta
			echo -e "\t...Copying ./retrinput1.fasta to ./retrinput2.fasta"
			cat ${input_src}/retrinput1.fasta > ${input_src}/retrinput2.fasta
			echo -e "\t...Appending ${parentfile} to ./retrinput2.fasta"
			cat ${parentfile} >> ${input_src}/retrinput2.fasta 
			delete_repeats ${input_src}/retrinput2.fasta <<- EOF
			y
			EOF
			y=$( cat ${input_src}/retrinput1.fasta | wc -l )
			echo -e "\t...Extracting records from ./retrinput2.fasta not found in ./retrinput1.fasta to .${outputfilename}_original.fasta"
			$AWK_EXEC -v y=$y '{if ( FNR<=y ) {next} else {print $0} }' ${input_src}/retrinput2.fasta > ${input_src}/${outputfilename}_original.fasta
			rm ${input_src}/retrinput2.fasta ${input_src}/retrinput1.fasta ${input_src}/retrinput.fasta 
			echo -e "The retrived original records are saved in ${input_src}/${outputfilename}_original.fasta"
		else
			echo "input file error in `basename $i`: input file should be a .aln file format"
			continue
		fi
	done
}


