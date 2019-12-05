#!/bin/bash

source ~/bioinformatics/github/co1_metaanalysis/code/process_all_input_files.sh

classifier=${co1_path}code/tools/RDPTools/classifier.jar
classifierproperties=${co1_path}code/tools/CO1Classifierv3.2/mydata_trained/rRNAClassifier.properties
RDPresults_eval_R=${co1_path}code/RDPresults_eval.R
JAVA_EXEC=$( which java )


RDPclassifyCOI() { # This function will use RDPclassiffier to assign species taxon classes to sequences.
	#java -Xmx80g -jar ~/bioinformatics/github/co1_metaanalysis/code/tools/RDPTools/classifier.jar classify -c 0.8 -t ~/bioinformatics/github/co1_metaanalysis/code/tools/CO1Classifierv3.2/mydata_trained/rRNAClassifier.properties -o enafroCOI_all_clean_Diptera_taxa1 enafroCOI_all_clean_Diptera.fasta
	
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
			unset max_mem

			echo -e "\n\tProceeding with `basename -- ${i}`..."
			#Removing gaps & concatinating sequence lines
			remove_gaps $i
			input=${input_src}/${output_filename}_dgpd.fasta
			sed -i "s/ /_/g" $input
			
			# Setting the maximum memory to be used by java
			regexp='^[0-9]+[kKmMgG]$'

			until [[ "$max_mem" =~ $regexp ]]
                        do
				echo -e "\tThe maximum memory allocation to this java process. Specify the maximum size of the memory allocation pool in bytes. This value must be a multiple of 1024 and greater than 2 MB. Append the letter k or K to indicate kilobytes, m or M to indicate megabytes, g or G to indicate gigabytes. Recommended maximum memory is 1/4 of your RAM if working on PC or any size not exceeding max mem if on a cluster"
				read -p "Please enter maximum memory allocation to RDP classifier java process: " max_mem
                        done
			echo -e "\n\tClassifying sequences in `basename -- ${i}` using RDPclassifier trained on CO1 Eukaryote v3.2 training set from insects, mammals and some outliers...\n\tThis may take awhile. Kindly wait until it's done..."
			$JAVA_EXEC -Xmx${max_mem} -jar ${classifier} classify -c 0.8 -t ${classifierproperties} -o ${input_src}/${output_filename}_taxa ${input}
			echo -e "\n\tDONE. All classification results have been stored in ${input_src}/${output_filename}_taxa"
			rm ${input_src}/${output_filename}_dgpd.fasta
		else
			echo "input file error in `basename $i`: input file should be a .fasta file format"
			continue
		fi
	done
}

#===============================================================================================================================================================

RDPcoiresults2tsv() { # This function will take the raw RDPClassiffier results of COI sequences and transform it into a standard tab-delimited output format.
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
			echo -e"\nDONE the output file has been stored in ${input_src}/${output_filename}.tsv"
		else
			echo "input file error in `basename $i`: input file should be a raw RDPClassifier.tsv file format"
			continue
		fi
	done
}

#===============================================================================================================================================================

RDPresults_eval(){ # this function evaluates the results from the RDP classiffier and informs on the distribution of various clades prior to classification and the resultant distribution.
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

			echo -e "\nProceeding with `basename -- ${i}`"
			Rscript --vanilla ${RDPresults_eval_R} ${i} |& tee -a ${input_src}/${output_filename}_evaluation
			echo -e "\nThe ouput file from evaluation of `basename $i` has been stored in ${input_src}/${output_filename}_evaluation"
		else
			echo "input file error in `basename $i`: input file should be a raw RDPClassifier.tsv file format"
			continue
		fi
	done
		

}

#===============================================================================================================================================================

RDPcoiheaders() { # This function will take raw RDPClassiffier results of COI sequences or its resultant tab-delimited file (generated by RDPcoiresults2tsv function above) and generate typical FASTA format headers.
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
			unset seq_l
			unset RDPspecies_BsSc
			#Setting the minimum sequence length (seq_len) and minimum species assignment bootsrap score (Spesies_sc) 
			regexg='^(1|0\.[0-9]*)$'
			regexp='^[0-9]+$'
			until [[ "$seq_l" =~ $regexp ]]
			do
				read -p "Please enter the minimum sequence length accepted for a record: " seq_l
			done

			until [[ "$RDPspecies_BsSc" =~ $regexg ]]
			do 
				read -p "Please enter the minimum RDP Species Classification Bootstrap score. It should range from 0-1, the higher the better: " RDPspecies_BsSc
			done
			# Generating headers conditionally
			$AWK_EXEC -v seq_l=$Seq_l -v RDPspBsSc=$RDPspecies_BsSc 'BEGIN{
			FS="\t"; OFS="|" }NR == 1 { next }{
				if($16 < seq_l || $34 < RDPspBsSc) {next}
				else if ($16 >= seq_l && $34 >= RDPspBsSc && 
					($9 == $33 || ($9 == "NA" && $8 == $31) || 
					($9 == "NA" && $8 == "NA" && $5 == $29) || 
					($9 == "NA" && $8 == "NA" && $5 == "NA") ) ){
					print ">"$1,$2,$3,$27,"fam-"$5,"subfam-"$6,"tri-"$7, \
						"gs-"$8,"sp-"$9,"subsp-"$10,"country-"$11, \
						"exactsite-"$12,"lat_"$13,"lon_"$14,"elev-"$15,"l-"$16
				}
				else if ($16 >= seq_l && $34 >= RDPspBsSc && 
					$9 == "NA" && $8 == "NA" && $5 != "NA" && $5 != $29){
					print ">"$1,$2,$3,$27,"fam-"$5"-ASGMT-"$29,"subfam-"$6,"tri-"$7, \
						"gs-ASGMT-"$31,"sp-ASGMT-"$33,"subsp-"$10,"country-"$11, \
						"exactsite-"$12,"lat_"$13,"lon_"$14,"elev-"$15,"l-"$16
				}
				else if ($16 >= seq_l && $34 >= RDPspBsSc && 
					$9 == "NA" && $8 != "NA" && $8 != $31 && $5 == $29){
					print ">"$1,$2,$3,$27,"fam-"$29,"subfam-"$6,"tri-"$7, \
						"gs-"$8"-ASGMT-"$31,"sp-ASGMT-"$33,"subsp-"$10,"country-"$11, \
						"exactsite-"$12,"lat_"$13,"lon_"$14,"elev-"$15,"l-"$16
				}
				else if ($16 >= seq_l && $34 >= RDPspBsSc && 
					$9 == "NA" && $8 != "NA" && $8 != $31 && 
					$5 != "NA" && $5 != $29){
					print ">"$1,$2,$3,$27,"fam-"$5"-ASGMT-"$29,"subfam-"$6,"tri-"$7, \
						"gs-"$8"-ASGMT-"$31,"sp-ASGMT-"$33,"subsp-"$10,"country-"$11, \
						"exactsite-"$12,"lat_"$13,"lon_"$14,"elev-"$15,"l-"$16
				}
				else if ($16 >= seq_l && $34 >= RDPspBsSc && 
					$9 != "NA" && $9 != $33 && $8 == $31){
					print ">"$1,$2,$3,$27,"fam-"$29,"subfam-"$6,"tri-"$7, \
						"gs-"$31,"sp-"$9"-ASGMT-"$33,"subsp-"$10,"country-"$11, \
						"exactsite-"$12,"lat_"$13,"lon_"$14,"elev-"$15,"l-"$16
				}
				else if ($16 >= seq_l && $34 >= RDPspBsSc && 
					$9 != "NA" && $9 != $33 && $8 != "NA" && $8 != $31 &&
					$5 == $29){
					print ">"$1,$2,$3,$27,"fam-"$29,"subfam-"$6,"tri-"$7, \
						"gs-"$8"-ASGMT-"$31,"sp-"$9"-ASGMT-"$33,"subsp-"$10,"country-"$11, \
						"exactsite-"$12,"lat_"$13,"lon_"$14,"elev-"$15,"l-"$16
				}
				else if ($16 >= seq_l && $34 >= RDPspBsSc &&
					$9 != "NA" && $9 != $33 && $8 != "NA" && $8 != $31 &&
					$5 != "NA" && $5 != $29){
					print ">"$1,$2,$3,$27,"fam-"$5"-ASGMT-"$29,"subfam-"$6,"tri-"$7, \
						"gs-"$8"-ASGMT-"$31,"sp-"$9"-ASGMT-"$33,"subsp-"$10,"country-"$11, \
						"exactsite-"$12,"lat_"$13,"lon_"$14,"elev-"$15,"l-"$16
				}
			}' ${i} > ${input_src}/${output_filename}_headers
		else
			echo "input file error in `basename $i`: input file should be a raw RDPClassifier results file or RDPClassifier.tsv file format"
			continue
		fi
	done
}

#===============================================================================================================================================================
