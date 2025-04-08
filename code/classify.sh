#!/bin/bash

#set -E
#trap ' echo Error $? occured on $LINENO ' ERR

source ~/bioinformatics/github/co1_metaanalysis/code/process_all_input_files.sh

classifier=${co1_path}code/tools/RDPTools/classifier.jar
classifierproperties3=${co1_path}code/tools/CO1Classifierv3.2/mydata_trained/rRNAClassifier.properties
classifierproperties4=${co1_path}code/tools/CO1Classifierv4/mydata_trained/rRNAClassifier.properties
RDPresults_eval_R=${co1_path}code/RDPresults_eval.R
JAVA_EXEC=$( which java )


RDPclassifyCOI() { # This function will use RDPclassiffier to assign species taxon classes to sequences.
	#java -Xmx80g -jar ~/bioinformatics/github/co1_metaanalysis/code/tools/RDPTools/classifier.jar classify -c 0.8 -t ~/bioinformatics/github/co1_metaanalysis/code/tools/CO1Classifierv3.2/mydata_trained/rRNAClassifier.properties -o enafroCOI_all_clean_Diptera_taxa1 enafroCOI_all_clean_Diptera.fasta

	trap ' echo Error $? occured on $LINENO && return 1' ERR
	if [ $# -lt 2 ]
	then
		echo "Input error..."
		echo "Usage: ${FUNCNAME[0]} -i 'file1.fasta* [file2.fasta* file3.fasta* ...]' [-l <INTEGER>] [-m <INTEGERg|G|m|M>]"
		return 1
	fi

	unset maxlen
	unset max_mem

	local OPTIND=1
	while getopts 'i:l:m:' key
	do
		case "${key}" in
			i)
				files=$OPTARG
				for inputfile in $files
				do
					if [ ! -f $inputfile ]
					then
						echo -e "\tInput file error in `basename -- $inputfile`: provided file does not exist"
						echo -e "\tUsage: ${FUNCNAME[0]} -i 'file1.fasta* [file2.fasta* file3.fasta* ...]' [-l <INTEGER>] [-m <INTEGERg|G|m|M>]"
						return 1
					elif [[ ( -f $inputfile ) && ( `basename -- $inputfile` =~ .*\.(aln|afa|fasta|fa|fst)$ ) ]]
					then
						continue
					fi
				done
				;;
			l)
				if [[ ( ! $OPTARG =~ ^[1-9][0-9]*$ ) || ( $OPTARG -lt 500 ) ]]
				then
					echo -e "\tinput error: the value $OPTARG should be an integer greater than 500"
				elif [[ -z ${OPTARG+x} ]]
				then
					maxlen=3000
					echo -e "\tMaximum sequence length sequence length is set to 3000bp, anything longer will not be classified"
				elif [[ $OPTARG =~ ^[1-9][0-9]*$ ]]
				then
					maxlen=$OPTARG
				fi
				;;
			m)
				if [[ ! $OPTARG =~ ^[[:digit:]]*(g|G|m|M)$ ]]
				then
					echo -e "\tinput error: file $OPTARG is non-existent!"
					echo "Usage: ${FUNCNAME[0]} -i <file.fasta> [-l <INTEGER>] [-m <INTEGERg|G|m|M>]"
					return 1
				else
					max_mem=$OPTARG
					echo -e "\tMaximum memory (RAM) allocation to RDP classifier process is set at $max_mem"
				fi
				;;
			?)
				echo "Input error..."
				echo 'Usage: ${FUNCNAME[0]} -i "file1.fasta* [file2.fasta* file3.fasta* ...]" [-l <INTEGER>] [-m <INTEGERg|G|m|M>]'
				return 1
				;;
		esac
        done


	for i in "$files"
	do
		if [ -z $i ]
		then
			echo -e "\tinput error: input file is missing!: use -i <inputfile.fasta>"
		elif [[ ( -f $i ) && ( `basename -- "$i"` =~ .*\.(aln|afa|fasta|fa|fst)$ ) ]]
		then
			regexp='^[0-9]+$'
			input_src=`dirname "$( realpath "${i}" )"`
			rename $i

			echo -e "\tProceeding with `basename -- ${i}`..."
			#Removing gaps & concatinating sequence lines
			remove_gaps $i
			input=${input_src}/${output_filename}_dgpd.fasta
			sed -i "s/ /_/g" $input
			#Removing long sequences.
			until [[ "$maxlen" =~ $regexp ]]
			do
				echo -e "\tRDPclassiffier struggles to classify very long sequences and runs out of memory. To avoid this, we recommend you exclude records with more than a specific maximum limit of sequence length."
				read -p "Please enter the maximum sequence length (e.g 3000):: " maxlen
			done

			echo -e "\tMoving records with over ${maxlen} sequence length to ${input_src}/${output_filename}_unclassified.${filename_ext}"
			$AWK_EXEC -v max=$maxlen -v output_filename=$output_filename -v filename_ext=$filename_ext -v input_src=$input_src '/^>/{
			hdr=$0; next} {
			seq=$0 } match(seq,/^.*$/) {
			if (RLENGTH<=max) {
				print hdr"\n" seq > input_src"/"output_filename"_clean."filename_ext}
			else
				print hdr"\n" seq > input_src"/"output_filename"_unclassified."filename_ext
			}' ${input} && mv ${input_src}/${output_filename}_clean.${filename_ext} ${input_src}/${output_filename}_dgpd.fasta

			# Setting the maximum memory to be used by java
			regexp='^[0-9]+[kKmMgG]$'

			until [[ "$max_mem" =~ $regexp ]]
                        do
				echo -e "\tThe maximum memory allocation to this java process::\n\tSpecify the maximum size of the memory allocation pool in bytes. This value must be a multiple of 1024 and greater than 2 MB. Append the letter k or K to indicate kilobytes, m or M to indicate megabytes, g or G to indicate gigabytes. Recommended maximum memory is 1/4 of your RAM if working on PC or any size not exceeding max mem if on a cluster"
				read -p "Please enter maximum memory allocation to RDP classifier java process: " max_mem
                        done
			echo -e "\tClassifying sequences in `basename -- ${i}` using RDPclassifier trained on CO1 Eukaryote v4 training set from insects, mammals and some outliers...\n\tThis may take awhile. Kindly wait until it's done..."
			$JAVA_EXEC -Xmx${max_mem} -jar ${classifier} classify -c 0.8 -t ${classifierproperties4} -o ${input_src}/${output_filename}_taxa ${input}
			sed -i 's/\t-\t/\t\t/g' ${input_src}/${output_filename}_taxa
			taxa_output=${input_src}/${output_filename}_taxa
			if [ $? -eq 0 ]
			then
				echo -e "\n\tDONE. All classification results have been stored in ${input_src}/${output_filename}_taxa"
			fi
			rm ${input}
		else
			echo "input file error in `basename -- $i`: input file should be a .fasta file format"
			continue
		fi
	done
}

#===============================================================================================================================================================

RDPcoiresults2tsv() { # This function will take the raw RDPClassiffier results of COI sequences and transform it into a standard tab-delimited output format.
	trap ' echo Error $? occured on $LINENO && return 1' ERR
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
		elif [[ ( -f $i ) && ( `basename -- "$i"` =~ .*_taxa$ ) ]]
		then
			echo -e "\nConverting $i to TSV format..."
			input_src=`dirname "$( realpath "${i}" )"`
			cp ${i} ${input_src}/${i}.edit
			rename ${input_src}/${i}.edit

			sed -i 's/|l-/\|/g; s/|fam-/\|/g; s/|subfam-/\|/g; s/|tri-/\|/g; s/|gs-/\|/g; s/|sp-/\|/g; s/|subsp-/\|/g; s/|country-/\|/g; s/|exactsite-/\|/g; s/|lat_/\|/g; s/|lon_/\|/g; s/|elev-/\|/g; s/|/\t/g; s/\t\t/\t/g' ${input_src}/${i}.edit
			$AWK_EXEC 'BEGIN {FS="\t";OFS="\t";}; BEGINFILE{
			print "Process_ID","Phylum","Class","Order", \
				"Family","Subfamily","Tribe","Genus", \
				"Species","Subspecies","Country","Exactsite", \
				"Latitude","Longitude","Elevation","seq_len", \
				"As_CellularOrganism","CellularOrganism_sc", \
				"As_superkingdom","Superkingdom_sc", \
				"As_kingdom","Kingdom_sc","As_phylum", \
				"Phylum_sc","As_class","Class_sc", \
				"As_order","Order_sc","As_family","Family_sc", \
				"As_genus","Genus_sc","As_species","Species_sc"}
			{print $1,$2,$3,$4,$5,$6,$7,$8,$9,$10,$11,$12,$13, \
				$14,$15,$16,$17,$19,$20,$22,$23,$25,$26,$28, \
				$29,$31,$32,$34,$35,$37,$38,$40,$41,$43}' ${input_src}/${i}.edit > ${input_src}/${output_filename}.tsv

			rm ${input_src}/${i}.edit
			if [ $? -eq 0 ]
			then
				echo -e "\nDONE the output file has been stored in ${input_src}/${output_filename}.tsv"
			fi
		else
			echo "input file error in `basename -- $i`: input file should be a raw RDPClassifier.tsv file format"
			continue
		fi
	done
}

#===============================================================================================================================================================

RDPresults_eval(){ # this function evaluates the results from the RDP classiffier and informs on the distribution of various clades prior to classification and the resultant distribution.
	trap ' echo Error $? occured on $LINENO && return 1 ' ERR
	if [ $# -eq 0 ]
	then
		echo "Input error..."
		echo "Usage: ${FUNCNAME[0]} file1.tsv|_taxa[file2.* file3.* ...]"
		return 1
	fi
	
	for i in "$@"
	do
		if [ ! -f $i ]
		then
			echo "input error: file '$i' is non-existent!"
		elif [[ ( -f $i ) && ( `basename -- "$i"` =~ .*(.tsv|_taxa)$ ) ]]
		then
			input_src=`dirname "$( realpath "${i}" )"`
			if [[ `basename -- "$i"` =~ .*_taxa$ ]]
			then
				RDPcoiresults2tsv ${i}
				i=${input_src}/${output_filename}.tsv
			else
				i=${i}
			fi

			rename ${i}

			echo -e "\nProceeding with evaluating `basename -- ${i}`\n\n***************************************************************************************************************************************************************"
			Rscript --vanilla ${RDPresults_eval_R} ${i} |& tee -a ${input_src}/${output_filename}_evaluation
			echo -e "***************************************************************************************************************************************************************\n\n"
			if [ $? -eq 0 ]
			then
				echo -e "\nThe ouput file from evaluation of `basename -- $i` has been stored in ${input_src}/${output_filename}_evaluation"
			fi
		else
			echo "input file error in `basename -- $i`: input file should be a raw RDPClassifier.tsv file format"
			continue
		fi
	done
		

}

#===============================================================================================================================================================
RDPcoiheaders() { # This function will take raw RDPClassiffier results of COI sequences or its resultant tab-delimited file (generated by RDPcoiresults2tsv function above) and generate typical FASTA format headers.
	
	trap ' echo Error $? occured on $LINENO && return 1 ' ERR
	if [ $# -lt 2 ]
	then
		echo "Input error..."
		echo "Usage: ${FUNCNAME[0]} -i 'file1.tsv|_taxa[file2.* file3.* ...]' [-l <INTEGER>] [-s <percentage decimals>]"
		return 1
	fi

	unset seq_l
	unset RDPspecies_BsSc

	local OPTIND=1
	while getopts 'i:l:s:' key
	do
		case "${key}" in
			i)
				files=$OPTARG
				for inputfile in $files
				do
					if [ ! -f $inputfile ]
					then
						echo -e "\tInput file error in `basename -- $inputfile`: provided file does not exist"
						echo -e "\tUsage: ${FUNCNAME[0]} -i 'file1.tsv|_taxa[file2.* file3.* ...]' [-l <INTEGER>] [-s <percentage decimals>]"
						return 1
					elif [[ ( -f $inputfile ) && ( `basename -- $inputfile` =~ .*(tsv|taxa)$ ) ]]
					then
						continue
					fi
				done
				;;
			l)
				if [ ! $OPTARG =~ ^[0-9]+$ ]
				then
					echo -e "\tinput error: the Sequence length value $OPTARG should be an integer"
				elif [[ -z ${OPTARG+x} ]]
				then
					until [ $seq_l =~ ^[0-9]+$ ]
					do
						read -p "Please enter the minimum sequence length accepted for a record: " seq_l
					done
				elif [ $OPTARG =~ ^[1-9][0-9]*$ ]
				then
					seq_l=$OPTARG
				fi
				;;
			s)
				if [ $OPTARG =~ ^1|0\.[0-9]+$ ]
				then
					RDPspecies_BsSc=$OPTARG
					echo -e "\tMinimum RDPClassifier species classification bootstrap score is set at $RDPspecies_BsSc"
				elif [[ -z ${OPTARG+x} ]]
				then
					until [ $RDPspecies_BsSc =~ ^1|0\.[0-9]+$ ]
					do
						read -p "Please enter the minimum RDP Species Classification Bootstrap score. It should range from 0-1, the higher the better: " RDPspecies_BsSc
					done
				else
					echo -e "\tinput error: The RDPClassifier bootstrap score ranges form 0.0 - 1.0, $OPTARG is not set properly!"
					echo "Usage: ${FUNCNAME[0]} -i 'file1.tsv|_taxa[file2.* file3.* ...]' [-l <INTEGER>] [-s <percentage decimals>]"
					return 1
				fi
				;;
			?)
				echo "Input error..."
				echo 'Usage: ${FUNCNAME[0]} -i 'file1.tsv|_taxa[file2.* file3.* ...]' [-l <INTEGER>] [-s <percentage decimals>]'
				return 1
				;;
		esac
        done
	for i in "$files"
	do
		if [[ ( -f $i ) && ( `basename -- "$i"` =~ .*(.tsv|_taxa)$ ) ]]
		then
			input_src=`dirname "$( realpath "${i}" )"`
			if [[ `basename -- "$i"` =~ .*_taxa$ ]]
			then
				RDPcoiresults2tsv ${i}
				i=${input_src}/${output_filename}.tsv
			else
				i=${i}
			fi
			
			rename ${i}
			# Generating headers conditionally
			$AWK_EXEC -v seq_l=$Seq_l -v RDPspBsSc=$RDPspecies_BsSc 'BEGIN{
			FS="\t"; OFS="|" }NR == 1 { next }{
				if($16 < seq_l || $34 < RDPspBsSc) {next}
				else if ($16 >= seq_l && $34 >= RDPspBsSc)
					if ($2 == "NA") {phylum = "NA-ASGMT-"$23}
					else if ($2 == $23) {phylum = $2}
					else if ($2 != $23) {phylum = $2"-ASGMT-"$23}
					else {phylum = "NA"}
					if ($3 == "NA") {class = "NA-ASGMT-"$25}
                                        else if ($3 == $25) {class = $3}
                                        else if ($3 != $25) {class = $3"-ASGMT-"$25}
                                        else {class = "NA"}
					if ($4 == "NA") {order = "NA-ASGMT-"$27}
                                        else if ($4 == $27) {order = $4}
                                        else if ($4 != $27) {order = $4"-ASGMT-"$27}
                                        else {order = "NA"}
					if ($5 == "NA") {family = "NA-ASGMT-"$29}
                                        else if ($5 == $29) {family = $5}
                                        else if ($5 != $29) {family = $5"-ASGMT-"$29}
                                        else {family = "NA"}
					if ($8 == "NA") {genus = "NA-ASGMT-"$31}
                                        else if ($8 == $31) {genus = $8}
                                        else if ($8 != $31) {genus = $8"-ASGMT-"$31}
                                        else {genus = "NA"}
					if ($9 == "NA") {species = "NA-ASGMT-"$33}
                                        else if ($9 == $33) {species = $9}
                                        else if ($9 != $33) {species = $9"-ASGMT-"$33}
                                        else {species = "NA"}
					print ">"$1,phylum,class,order,"fam-"family,"subfam-"$6,"tri-"$7, \
						"gs-"genus,"sp-"species,"subsp-"$10,"country-"$11, \
						"exactsite-"$12,"lat_"$13,"lon_"$14,"elev-"$15,"l-"$16
			}' ${i} > ${input_src}/${output_filename}_l-${seq_l}-Sc-${RDPspecies_BsSc}_headers

			if [ $? -eq 0 ]
			then
				echo -e "\tDONE generating headers. The output file has been stored in `basename -- ${input_src}/${output_filename}_l-${seq_l}-Sc-${RDPspecies_BsSc}_headers`"
			fi
		else
			echo "input file error in `basename -- $i`: input file should be a raw RDPClassifier results file or RDPClassifier.tsv file format"
			continue
		fi
	done
}

