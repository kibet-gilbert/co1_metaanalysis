#!/bin/bash
#This script has functions to process FASTA files.

AWK_EXEC=$( which gawk )
PYTHON_EXEC=$( which python )

realpath() { #Gets the absolute path to the a given file.
	${PYTHON_EXEC} -c "import os,sys; print(os.path.realpath(sys.argv[1]))" $1
}


#===============================================================================================================================================================
rename() { #Takes input file name and extracts the prefix of the name ("prefix"), The suffix (".suffix"), Absolute path to the file and the name of the Directory within which the file is found.
	input_filename=`basename -- ${i}`
	output_filename=${input_filename%.*}
	filename_ext=${input_filename##*.}
	src_dir_path=`dirname $(realpath ${i})`
	src_dir=${src_dir_path##*/}
	#echo -e "input is $i \ninput_filename is $input_filename \noutput_filename is $output_filename \nfilename_ext is $filename_ext \nsrc_dir_path is $src_dir_path \nsrc_dir is $src_dir"
}

#===============================================================================================================================================================

concatenate_fasta_seqs() { # This function converts a multiple line FASTA format sequence into a two line record of a header and a sequnce lines.

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

move_matched() { #This function is used within the filter_seqs function below. It moves and deletes the matching fasta records.
	for i in "$@"
	do
		matching_records=`grep $pattern_name $i | wc -l`
		echo -e "${matching_records} records match the pattern '$pattern_name'"
		if [ $matching_records -gt 0 ]
		then
			echo -e "\tRemoving any records with '$pattern_name' description in header from file..."
			$AWK_EXEC -v pat="$pattern_name" '/^>/{
			hdr=$0; next} {
			seq=$0 } {
			pattern=pat; gsub(/\|/,"\\\|", pattern)} hdr~pattern{
			print hdr; print seq }' $i >> ${input_src}/${output_filename}_${suffix}.${filename_ext}
			
			#Deleting matched records
			regexpno='^n|N|No|NO|no$'
			regexpyes='^y|Y|Yes|YES|yes$'
			if [[ $Choice =~ $regexpyes ]]
			then
				echo -e "The records that match $pattern_name have been deleted from `basename $i`"
				sed -i "/$pattern_name/,+1 d" $i
			elif [[ ${Choice} =~ $regexno ]]
			then
				echo -e "The records that match $pattern_name have been retained in `basename $i`"
			fi
		fi
	done
}

filter_seqs() { #this function copys a record that fits a provided pattern, e.g a taxon_name_description; the arguments provided, are the files to be searched for the patterns
	# LOGIC behind the code: To get the list of orders in description_taxon_names and their frequencies, from  which to select the undesired patterns (names), do:
	#grep ">" seqs.fasta | awk 'BEGIN {FS="|"; OFS="|" ; }; {print $2}' |sort | uniq -c > seqs_orders && less seqs_orders
	
	set -E
	trap ' return 1 ' ERR
	
	if [ $# -eq 0 ]
	then
		echo "Input error..."
		echo "Usage: ${FUNCNAME[0]} file1.*[file2.* file3.* ...]"
		return 1
	fi
	
	for i in "$@"
	do
		echo -e "\nProceeding with `basename -- $i`..."
		rename
		input_src=`dirname "$( realpath "${i}" )"`
		concatenate_fasta_seqs $i
		
		unset options
		
		echo -e "To extract sequences with specific words in the headers please select one of the options [1|2|3] to proceed or [4] to cancel"
		options[0]="Move records with word patterns specified in a file into one file"
		options[1]="Move records with string-patterns specified in a file into individual word-pattern-specific files"
		options[2]="Move records with specific single string into a file"
		options[3]="Exit"
		
		PS3='Select option [1|2|3] to delete, or [4] to exit: '
		select option in "${options[@]}"
		do
			unset pattern_name
			unset input_pattern_file
  			regexp='^[a-zA-Z0-9/_-\ \|]+$'
			regexp1='^n|y|N|Y|No|Yes|NO|YES|no|yes$'

			case $option in
				${options[0]})
					#echo "no error"
					until [[ -f ${input_pattern_file}  ]]
					do
						read -p "Please enter the path to the file with pattern names to be removed:: " input_pattern_file
						#cat ${input_pattern_file}
					done

					#deleting matches		
					echo -e "\nIf you wish to delete records that match the search patterns in $input_pattern_file from `basename $i` enter [YES] or [NO] if you wish to retain them"
					unset Choice
					read -p "Please enter [Yes] or [NO] to proceed:: " Choice
					until [[ "$Choice" =~ $regexp1 ]]
					do
						echo "INVALID choice $REPLY"
					done
					
					for line in `cat ${input_pattern_file}`
					do
						pattern_name=${line}
						suffix="matched"
						#echo ${pattern_name}
						move_matched "${i}"
					done
					echo -e "\n\tDONE. All deleted records have been stored in '${output_filename}_${suffix}.fasta'"
					break
					;;
				${options[1]})
					until [[ -f ${input_pattern_file} ]]
					do
						read -p "Please enter the path to the file with pattern names to be removed:: " input_pattern_file
					done

					echo -e "\nIf you wish to delete records that match the search patterns in $input_pattern_file from `basename $i` enter [YES] or [NO] if you wish to retain them"
					unset Choice
					read -p "Please enter [Yes] or [NO] to proceed:: " Choice
					until [[ "$Choice" =~ $regexp1 ]]
					do
						echo "INVALID choice $REPLY"
					done
					
					for line in `cat ${input_pattern_file}`
					do
						pattern_name=${line}
						suffix=${pattern_name}
						move_matched "${i}"
						echo -e "\n\tDONE. All deleted records have been stored in '${output_filename}_${suffix}.fasta'"
					done
					break
					;;
				${options[2]})
					until [[ "$pattern_name" =~ $regexp ]]
					do
						read -p "Please enter string pattern to be searched:: " pattern_name
					done

					echo -e "\nIf you wish to delete records that match the search pattern '$pattern_name' from `basename $i` enter [YES] or [NO] if you wish to retain them"
					unset Choice
					read -p "Please enter [Yes] or [NO] to proceed:: " Choice
					until [[ "$Choice" =~ $regexp1 ]]
					do
						echo "INVALID choice $REPLY"
					done

					suffix=`echo ${pattern_name} | sed 's/|/\./g'`
					move_matched "${i}"
					echo -e "\n\tDONE. All deleted records have been stored in '${output_filename}_${suffix}.fasta'"
					break
					;;
				${options[3]})
					echo -e "Exiting deletion of pattern-matched sequences..."
					break
					;;
				*)
					echo "INVALID choice: Select option [1|2|3] to delete, or [4] to exit: "
					;;
			esac
		done
	done
}
