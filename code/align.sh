#!/bin/bash

#"""This script is a module with alignment instructions as fuctions, methods and properties."""

co1_path=~/bioinformatics/github/co1_metaanalysis/
muscle_dest=${co1_path}data/output/alignment/muscle_output/
mafft_dest=${co1_path}data/output/alignment/mafft_output/
makemergetable=${co1_path}code/makemergetable.rb
tcoffee_dest=${co1_path}data/output/alignment/t_coffee_output/
pasta_dest=${co1_path}data/output/alignment/pasta_output/
run_pasta=${co1_path}code/tools/pasta_code/pasta/run_pasta.py
sate_dest=${co1_path}data/output/alignment/sate_output/
MPIRUN=$( which mpirun )
mpionly=${co1_path}code/mpionly.noscheduler
PYTHON_EXEC=$( which python )

usage() { #checks if the positional arguments (input files) for execution of the script are defined
	if [ $# -eq 0 ]
	then
		echo "Input error..."
		echo "Usage: ${FUNCNAME[1]} file1.fasta[file2.fasta file3.fasta ...]"
		return 1
		
	fi
}

realpath() { #
	${PYTHON_EXEC} -c "import os,sys; print(os.path.realpath(sys.argv[1]))" $1
}

rename() { #generates output file names with same input filename prefix. The suffix (".suffix") is set in individual functions that perform different tasks.
	input_filename=$(basename -- "${i}")
	output_filename=${input_filename%.*}
	filename_ext=${input_filename##*.}
	src_dir_path=`dirname $(realpath ${i})`
	src_dir=${src_dir_path##*/}
}

#========================================================================================
#MUSCLE
#========================================================================================
###muscle

muscle_large() { #muscle aligment of large datasets, long execution times is an issue
	#-maxiters 2: Iterations beyond 2 attempt refinement, which often results in a small improvement, at most
	usage $@
	echo "muscle starting alinment..."
	
	for i in $@
	do
		if [ ! -f $i ]
		then
			echo "input error: file $i is non-existent!"
		elif [[ ( -f $i ) && ( `basename $i` =~ .*\.(afa|fasta|fa)$ ) ]]
		then
			rename
			echo -e "\nproceeding with file `basename $i`..."
			muscle -in $i -fastaout ${muscle_dest}./aligned/${output_filename}.afa -clwout ${muscle_dest}./aligned/${output_filename}.aln -maxiters 2
		else
			echo "input file error in `basename $i`: input file should be a .fasta file format"
			continue
		fi
	done
}


#========================================================================================
muscle_refine() {
	#attempt to improve an existing alignment. Input is an existing MSA
	usage $@
	echo "starting refinment of existing MSAs..."

	for i in $@
	do
		if [ ! -f $i ]
		then
			echo "input error: file $i is non-existent!"
		elif [[ ( -f $i ) && ( `basename $i` =~ .*\.(afa|fasta|fa|aln)$ ) ]]
		then
			rename
			echo -e "\nproceeding with file `basename $i`..."
			muscle -in $i -fastaout ${src_dir_path}/${output_filename}_rfnd.afa -clwout ${src_dir_path}/${output_filename}_rfnd.aln -refine
		else
			echo "input file error in `basename $i`: input file should be a .afa file format"
			continue
		fi
	done
}


#========================================================================================
muscle_p2p() {
	#takes two existing MSAs ("profiles") and aligns them to each other, keeping the columns in each MSA intact.The final alignment is made by inserting columns of gaps into the MSAs as needed. The alignments of sequences in each input MSAs are thus fully preserved in the output alignment.
	usage $@
	unset in1_outname #&& echo "good: output filename var in1_outname is empty"
	unset in2_outname #&& echo "good: output filename var in2_outname is empty"


	for i in $@
        do
		if [ $# -ne 2 ]
		then
			echo "input file error: only two input files are allowed!"
			break
		elif [[ ( -f $i ) && ( `basename $i` =~ .*\.(afa|fasta|fa|aln) ) ]]
		then
			if [ -z $in1_outname ]
			then 
				rename $i
				in1_outname=$output_filename
			elif [ -n $in1_outname ]
			then
				rename $i
				in2_outname=$output_filename
			fi
			#echo -e "in1_outname= ${in1_outname}\nin2_outname=${in2_outname}"
			continue
                elif [ ! -f $i ]
                then
                        echo "input error: file $i is non-existent!"
			break
		else
			echo "input file error in `basename $i`: input file should be a .afa file format"
			break
		fi
        done

	if [[ ( -n $in1_outname ) && ( -n $in2_outname  )  ]]
	then
		echo -e "\nproceeding with file `basename $1` and `basename $2`..."
		muscle -profile -in1 $1 -in2 $2 -fastaout ${src_dir_path}/${in1_outname}_${in2_outname}.afa #-clwout ${src_dir_path}/${in1_outname}_${in2_outname}.aln
		if [ $? -eq 0 ]
		then
			echo -e "\n\tDONE. All trimmed records have been stored in ${src_dir_path}/${in1_outname}_${in2_outname}.[afa|aln]\n"
		fi
	else
		echo " A error with output_filenames: in1_outname and in2_outname "
	fi
}



#========================================================================================
#MAFFT

#========================================================================================

###mafft :highly similar ∼50,000 – ∼100,000 sequences × ∼5,000 sites incl. gaps (2016/Jul)

##G-INS-1 option is applicable to large data, when huge RAM and a large number of CPU cores are available (at most 26.0 GB)
#By a new flag, --large, the G-INS-1 option has become applicable to large data without using huge RAM.This option uses files, instead of RAM, to store temporary data. The default location of temporary files is $HOME/maffttmp/ (linux, mac and cygwin) or %TMP% (windows) and can be changed by setting the MAFFT_TMPDIR environmental variable.

#syntax:	mafft --large --globalpair --thread n in > out

mafft_GlINS1() {
	usage $@

        for i in $@
        do
                if [ ! -f $i ]
                then
                        echo "input error: file $i is non-existent!"
                elif [[ ( -f $i ) && ( `basename $i` =~ .*\.(afa|fasta|fa)$ ) ]]
                then
                        rename
                        echo -e "\nmafft G-large-INS-1 MSA: proceeding with file `basename $i`..."
			printf "Choose from the following output formats: \n"
			select output_formats in fasta_output_format clustal_output_format none_exit
			do
				case $output_formats in
					fasta_output_format)
						echo -e "\nGenerating .fasta output\n"
						mafft --large --globalpair --thread -1 --reorder $i > ${mafft_dest}aligned/${output_filename}.fasta
						break
						;;
					clustal_output_format)
						echo -e "\nGenerating a clustal format output\n"
						mafft --large --globalpair --thread -1 --reorder --clustalout $1 > ${mafft_dest}aligned/${output_filename}.aln
						break
						;;
					none_exit)
						break
						;;
					*) echo "error: Invalid selection!"
				esac
			done
                else
                        echo "input file error in `basename $i`: input file should be a .fasta file format"
                        continue
                fi
        done
}


#=======================================================================================
#MPI version of high-accuracy progressive options, [GLE]-large-INS-1; Two environmental variables, MAFFT_N_THREADS_PER_PROCESS and MAFFT_MPIRUN, have to be set:
#The number of threads to run in a process: Set "1" unless using a MPI/Pthreads hybrid mode.
#	export MAFFT_N_THREADS_PER_PROCESS="1"
#Location of mpirun/mpiexec and options: mpirun or mpiexec must be from the same library as mpicc that was used in compiling
#	export MAFFT_MPIRUN="/somewhere/bin/mpirun -n 160 -npernode 16 -bind-to none ..." (for OpenMPI)
#OR	export MAFFT_MPIRUN="/somewhere/bin/mpirun -n 160 -perhost  16 -binding none ..." (for MPICH)

#mpi command: Add "--mpi --large" to the normal command of G-INS-1, L-INS-1 or E-INS-1
#	mafft --mpi --large --localpair --thread 16 input

#mafft L-INS-I command:
#mafft --localpair --maxiterate 1000 input_file > output_file


mafft_local() {
        usage $@

        for i in $@
        do
                if [ ! -f $i ]
                then
                        echo "input error: file $i is non-existent!"
                elif [[ ( -f $i ) && ( `basename $i` =~ .*\.(afa|fasta|fa)$ ) ]]
                then
			unset num_cpus
			regexp='^[0-9][1-9]*$'
			until [[ "$num_cpus" =~ $regexp ]]
			do
				read -p "Please enter the number of physical cores to be used: " num_cpus
			done

			rename
                        echo -e "\nmafft L-large-INS-1 MSA: proceeding with file `basename $i`..."
                        printf "Choose from the following output formats: \n"
                        select output_formats in fasta_output_format clustal_output_format none_exit
                        do
                                case $output_formats in
                                        fasta_output_format)
                                                echo -e "\nGenerating .fasta output\n"
						bash $mpionly
                                                mafft --mpi --large --localpair --thread -${num_cpus} --reorder $i > ${mafft_dest}aligned/${output_filename}.fasta
						break
                                                ;;
                                        clustal_output_format)
                                                echo -e "\nGenerating a clustal format output\n"
						bash $mpionly
                                                mafft --mpi --large --localpair --thread -${num_cpus} --reorder --clustalout $1 > ${mafft_dest}aligned/${output_filename}_l.aln
						break
                                                ;;
                                        none_exit)
                                                break
                                                ;;
                                        *) echo "error: Invalid selection!"
                                esac
                        done
                else
                        echo "input file error in `basename $i`: input file should be a .fasta file format"
                        continue
                fi
        done
}




#========================================================================================
##Merge multiple sub-MSAs into a single MSA

#syntax:	cat subMSA1 subMSA2 > input
#		ruby makemergetable.rb subMSA1 subMSA2 > subMSAtable
#		mafft --merge subMSAtable input > output

inputfiletest() { #test files if they exist, if they are the right format and generate the output file names.
	echo -e "\n unsetting output_filenames: in1_outname, in2_outnames, ...,in5_outnames"
	unset $in1_outname
	unset $in2_outname
	unset $in3_outname
	unset $in4_outname
	unset $in5_outname

        for i in $@
        do
                if [ $# -ge 2 ]
                then
                        echo "input file error: only two or more input files are allowed!"
                        break
                elif [[ ( -f $i ) && ( `basename $i` == *.fasta$) ]]
                then
                        if [ -z $in1_outname ]
                        then
                                rename $i
                                in1_outname=$output_filename
			elif [ -z $in2_outname ]
			then
                                rename $i
                                in2_outname=$output_filename
			elif [ -z $in3_outname ]
			then
				rename $i
				in3_outname=$output_filename
			elif [ -z $in4_outname ]
			then
				rename $i
				in4_outname=$output_filename
			elif [ -z $in5_outname ]
			then
				rename $i
				in5_outname=$output_filename
			else 
				echo -e "\n input file errors: you have provided more than 5 input files. At most 5 input files can be renamed by this function\n"
				echo "================================================="
				return 0
                        fi
                elif [ ! -f $i ]
                then
                        echo "input error: file $i is non-existent!"
                        return 0
                else
                        echo "input file error in `basename $i`: input file should be a .fasta file format"
                        return 0 
                fi
        done
}

#=====================================================================================
outputfilename() { #generates the name of the output (results) file when being merged
        case $# in
		2)
			outname=${in1_outname}_${in2_outname}
			echo -e "\nmerging two MSA files: `basename $1` and `basename $2` \n"
			;;
		3)
			outname=${in1_outname}_${in2_outname}_${in3_outname}
			echo -e "\nmerging three MSA files: `basename $1`, `basename $2` and `basename $3`\n"
			;;
		4)
			outname=${in1_outname}_${in2_outname}_${in3_outname}_${in4_outname}
			echo -e "\nmerging four MSA files: `basename $1`, `basename $2`, `basename $3` and `basename $4`\n"
			;;
		5)
			outname=${in1_outname}_${in2_outname}_${in3_outname}_${in4_outname}_${in5_outname}
			echo -e "\nmerging five MSA files: `basename $1`, `basename $2`, `basename $3`, `basename $4` and `basename $5`\n"
			;;
		*)
			echo -e "\n ERROR: you have more than 5 input files, only five can be accommodated..."
			exit 1
			;;
	esac

}


#================================================================================
##Merge multiple sub-MSAs into a single MSA
#Each sub-MSA is forecd to form a monophyletic cluster in version 7.043 and higher (2013/May/26).

#syntax:        cat subMSA1 subMSA2 > input
#               ruby makemergetable.rb subMSA1 subMSA2 > subMSAtable
#               mafft --merge subMSAtable input > output

mafft_merge() {
	echo -e "\nwarning: Each sub-MSA is forced to form a monophyletic cluster in version 7.043 and higher (2013/May/26)."
	printf "Enter [Yes] to continue or [No] to exit: "
	read choice
	case $choice in
		[yY][eE][sS] | [yY] )
			usage $@ #testing the arguments
			RUBY_EXEC=$( which ruby )
			
			inputfiletest $@ #assesing the validity of the input files
			
			outputfilename $@ # generating output file name
			
			cat $@ > ${mafft_dest}merged/input.fasta
			${RUBY_EXEC} ${makemergetable} $@ > ${mafft_dest}merged/subMSAtable
			mafft --merge ${mafft_dest}merged/subMSAtable ${mafft_dest}merged/input.fasta > ${mafft_dest}merged/${outname}.fasta
			;;
		[nN][oO] | [nN] )
			echo "exiting the operation"
			;;
		*)
			echo "Invalid input: please enter [Yes] or [No]"
			;;
	esac

}



#========================================================================================
## --add: Adding unaligned full-length sequence(s) into an existing alignment

#new_sequences and existing_alignment files are single multi-FASTA format file. Gaps in existing_alignment are preserved, but the alignment length may be changed in the default setting. If the --keeplength option is given, then the alignment length is unchanged.  Insertions at the new sequences are deleted. --reorder to rearrange sequence order.

#syntax:	% mafft --add new_sequences --reorder existing_alignment > output

mafft_add() {
	echo -e "\n \$1 should be the new_sequences: unaligned full-length sequence(s) to be added into the existing alignment (\$2) "
	printf "\nEnter [Yes] to continue or [No] to exit: "
	read choice
	case $choice in
		[yY][eE][sS] | [yY] )
			usage $@
			
			inputfiletest $@
			
			outputfilename $@
			
			mafft --add $1 --reorder $2 > ${mafft_dest}addseq/${outname}.fasta
			;;
		[nN][oO] | [nN] )
			echo "exiting the --add sequences operation"
			;;
		*)
			echo "Invalid input: please enter [Yes] or [No]"
			;;
	esac
}


#===============================================================================
## --addfragments: Adding unaligned fragmentary sequence(s) into an existing alignment

#syntax:	Accurate option
#syntax:	% mafft --addfragments fragments --reorder --thread -1 existing_alignment > output
#		Fast option (accurate enough for highly similar sequences):
#		% mafft --addfragments fragments --reorder --6merpair --thread -1 existing_alignment > output

mafft_addfragmets() {
	echo -e "\n \$1 is fragments is a single multi-FASTA format file and \$2 existing_alignment is a single multi-FASTA format file "
	printf "\nEnter [Yes] to continue or [No] to exit:  "
	read choice
	case $choice in
		[yY][eE][sS] | [yY] )
			usage $@

			inputfiletest $@

			outputfilename $@

			mafft --addfragments $1 --reorder --thread -1 $2 > ${mafft_dest}addfragments/${outname}.fasta
			;;
		[nN][oO] | [nN] )
			echo "exiting the --addfragments operation"
			;;
		*)
			echo "Invalid input: please enter [Yes] or [No]"
			;;
	esac
}


#=====================================================================================
#T_COFFEE
#=====================================================================================

### t_coffee: the regressive mode of T-Coffee is meant to align very large datasets with a high accuracy. It starts by aligning the most distantly related sequences first. It uses this guide tree to extract the N most diverse sequences. In this first intermediate MSA, each sequence is either a leaf or the representative of a subtree. The algorithm is re-aplied recursively onto every representative sequence until all sequences have been incorporated in an internediate MSA of max size N. The final MSA is then obtained by merging all the intermediate MSAs into the final MSA.

#Fast and accurate: the regressive alignment is used to align the sequences in FASTA format. The tree is estimated using the mbed method of Clustal Omega (-reg_tree=mbed), the size of the groups is 100 (-reg_nseq=100) and the method used to align the groups is Clustal Omega:

#syntax:	$ t_coffee -reg -seq proteases_large.fasta -reg_nseq 100 -reg_tree mbed -reg_method clustalo_msa -outfile proteases_large.aln -outtree proteases_large.mbed

#-seq		:provide sequences. must be in FASTA format
#-reg_tree	:defines the method to be used to estimste the tree
#-outtree	:defines the name of newly computed out tree. mbed method of Clustal Omega is used.
#-outfile**	:defines the name of output file of the MSA
#-reg_nseq	:sets the max size of the subsequence alignments; the groups is 100
#-reg_thread	:sets max threads to be used
#-reg_method**	:defines the method to be used to estimate MSA: Clustal Omega
#-multi_core    :Specifies that T-Coffee should be multithreaded or not; by default all relevant steps are parallelized; DEFAULT: templates_jobs_relax_msa_evaluate OR templates_jobs_relax_msa_evaluate (when flag set)
#-n_core        :Number of cores to be used by machine [default=0 => all those defined in the environement]

tcoffee_large() {
	usage $@
	echo "t-coffee starting alinment..."
	
	for i in $@
	do
		if [ ! -f $i ]
		then
			echo "input error: file $i is non-existent!"
		elif [[ ( -f $i ) && ( `basename $i` =~ .*\.(afa|fasta|fa)$ ) ]]
		then
			unset num_cpus
			regexp='^[0-9][1-9]*$'
			until [[ "$num_cpus" =~ $regexp ]]
			do
				read -p "Please enter the number of physical cores to be used: " num_cpus
			done

			rename
			echo -e "\nproceeding with file `basename $i`..."
			t_coffee -reg -multi_core -n_core=${num_cpus} -seq $i -reg_nseq 100 -reg_tree mbed -reg_method clustalo_msa -outfile ${tcoffee_dest}aligned/${output_filename}.fasta -newtree ${tcoffee_dest}trees/${output_filename}.mbed
		else
			echo "input file error in `basename $i`: input file should be a .fasta file format"
			continue
			
		fi
	done

}



#=====================================================================================
##Doing MSA of DNA sequences protein Translates.

#Working with coding DNA: it is advisable to first align the sequences at the protein level and later thread back the DNA onto your aligned proteins.

#Translating DNA sequences into protein sequences: often safer and more accurate to align them as proteins (as protein sequences are more conserved than their corresponding DNA sequence)

#Syntax:        $ t_coffee -other_pg seq_reformat -in proteases_small_dna.fasta -action \ +translate -output fasta_seq

#Back-translation with the bona fide DNA sequences:

#Syntax:        t_coffee -other_pg seq_reformat -in proteases_small_dna.fasta -in2 \ proteases_small.aln -action +thread_dna_on_prot_aln -output clustalw





#=====================================================================================

##Evaluating Your Alignment: Most of T_coffee evalution methods are designed for protein sequences (notably structure based methods), however, T-Coffee via sequence-based-methods (TCS and CORE index) offers some possibilities to evaluate also DNA alignments

#The CORE index is the basis of T-Coffee is an estimation of the consistency between your alignment and the computed library( by default a list of pairs of residues that align in possible global and 10 best local pairwise alignments). The higher the consistency, the better the alignment.
#Computing the CORE index of any alignment: To evaluate any existing alignment with the CORE index, provide that alignment with the -infile flag and specify that you want to evaluate it

#syntax:	$ t_coffee -infile=proteases_small_g10.aln -output=html -score

COREindex() { #Evaluating an existing alignment with the CORE index
	usage $@
	echo "t_coffee starting MSA alignment evaluation using CORE index... "
	
	for i in $@
	do
		if [ ! -f $i ]
		then
			echo "input error: file $i is non-existent!"
		elif [[ ( -f $i ) && ( `basename $i` =~ .*\.(afa|fasta|fa|aln)$ ) ]]
		then
			rename
			outfile_dest
			echo -e "\nproceeding with `basename $i` alignment file evaluatio..."
			t_coffee -infile=$i -multi_core -n_core=32 -output=html -score -outfile ${output_dest}scores/coreindex/${output_filename}.html
		else
			echo "input file error in `basename $i`: input file should be a *.aln file format"
			continue
		fi
	done
}


#=====================================================================================

outfile_dest() { #Redirecting the output results based on input results source path
	input_src=`dirname "$( realpath "${i}" )"`
	unset output_dest
	case $input_src in
		${muscle_dest}aligned)
			output_dest=$muscle_dest
			;;
		${mafft_dest}aligned)
			output_dest=$mafft_dest
			;;
		${tcoffee_dest}aligned)
			output_dest=$tcoffee_dest
			;;
		${pasta_dest}aligned)
			output_dest=$pasta_dest
			;;
		${sate_dest}aligned)
			output_dest=$sate_dest
			;;
		*)
			echo -e "input file is from unrecognized source directory `dirname "$( realpath "${i}" )"` "
			return 1
			;;
	esac
}


#=====================================================================================

# However, to evaluate an alignment, the use of Transitive Consistency Score (TCS) procedure is recommended. TCS is an alignment evaluation score that makes it possible to identify the most correct positions in an MSA. It has been shown that these positions are the most likely to be structuraly correct and also the most informative when estimating phylogenetic trees.
#Evaluating an existing MSA with Transitive Consistency Score (TCS): most informative when used to identify low-scoring portions within an MSA. *.score_ascii file displays the score of the MSA, the sequences and the residues. *.score_html file displays a colored version score of the MSA, the sequences and the residues

#syntax:	$ t_coffee -infile sample_seq1.aln -evaluate -output=score_ascii,aln,score_html

TCSeval() { #Evaluating an existing alignment with the TCS
        usage $@
        echo "t_coffee starting MSA alignment evaluation using TCS... "

        for i in $@
        do
                if [ ! -f $i ]
                then
                        echo "input error: file $i is non-existent!"
                elif [[ ( -f $i ) && ( `basename $i` =~ .*\.(afa|fasta|fa|aln)$ ) ]]
                then
                        rename
			outfile_dest
			if [ -z "$output_dest" ]
			then
				echo -e "\noutput destination folder not set: input can only be sourced from: \n$muscle_dest; \n$mafft_dest; \n$tcoffee_dest; \n$pasta_dest; and \n$sate_dest"
				continue
			else
				echo -e "\nproceeding with `basename $i` alignment file evaluatio..."
				select TCS_library_estimation_format in proba_pair fast_mafft_kalign_muscle_combo none_exit
				do
					case $TCS_library_estimation_format in
						proba_pair)
							echo -e "\nTCS evaluation using default aligner proba_pair"
							t_coffee -multi_core -n_core=32 -infile $i -evaluate -method proba_pair -output=score_ascii,html -outfile ${output_dest}scores/tcs/${output_filename}_score
							break
							;;
						fast_mafft_kalign_muscle_combo)
							echo -e "\nTCS evaluation using a series of fast multiple aligners; mafft_msa,kalign_msa,muscle_msa. \nThis option is not accurate and can not be relied on in filtering sequences"
							t_coffee -multi_core -n_core=32 -infile $i -evaluate -method mafft_msa,kalign_msa,muscle_msa -output=score_ascii,html -outfile ${output_dest}scores/tcs/${output_filename}_fastscore
							break
							;;
						none_exit)
							break
							;;
						*)
							echo "error: Invalid selection!"
					esac
				done
			fi
                else
                        echo "input file error in `basename $i`: input file should be a .fasta file format"
                        continue
                fi
        done
}


#=====================================================================================
#Filtering unreliable MSA positions: columns
#TCS allows you to filter out from your alignment regions that appears unreliable according to the consistency score; the filtering can be made at the residue level or the column level:
#	 t_coffee -infile sample_seq1.aln -evaluate -output tcs_residue_filter3,tcs_column_filter3,tcs_residue_lower4

#sample_seq1.tcs_residue_filter3 :All residues with a TCS score lower than 3 are filtered out
#sample_seq1.tcs_column_filter3 :All columns with a TCS score lower than 3 are filtered out
#sample_seq1.tcs_residue_lower4 :All residues with a TCS score lower than 3 are in lower case

#t_coffee -infile sample_seq1.aln -evaluate -output tcs_residue_filter1,tcs_column_filter1

#=====================================================================================

##Estimating the diversity in your alignment:

# The "-other_pg" flag: call a collection of tools that perform other operations: reformatting, evaluating results, comparing methods. After the flag -other_pg, the common T-Coffee flags are not recognized. "-seq_reformat" flag: calls one of several tools to reformat/trim/clean/select your input data but also your output results, from a very powerful reformatting utility named seq_reformat

# "-output" option of "seq_reformat", will output all the pairwise identities, as well as the average level of identity between each sequence and the others:
#               "-output sim_idscore" realign your sequences pairwise so it can accept unaligned or aligned sequences alike. "-output sim" computes the identity using the sequences as they are in your input file so it is only suited for MSAs

#Syntax:        $ t_coffee -other_pg seq_reformat -in sample_seq1.aln -output sim

#               "-in" and "in2" flags: define the input file(s)
#               "-output" flag: defines output format*



#=====================================================================================
#PASTA
#=====================================================================================
#
#Usage:		$run_pasta.py [options] <settings_file1> <settings_file2> ...
#syntax:	$run_pasta.py -i <input_fasta_file> -j <job_name> --temporaries <TEMP_DIR> -o <output_dir>

pasta_aln() { #MSA alignment using pasta
	usage $@
	echo "PASTA starting alinment..."
	
	PYTHON3_EXEC=$( which python3 )
	runpasta=${co1_path}code/tools/pasta_code/pasta/run_pasta.py

	for i in $@
	do
		if [ ! -f $i ]
		then
			echo "input error: file $i is non-existent!"
		elif [[ ( -f $i ) && ( `basename $i` =~ .*\.(afa|fasta|fa|aln)$ ) ]]
		then
			echo -e "\tProceeding with `basename $i`" 
			echo -e "\tPlease select the mafft alignment method;\n\tlocal[mafft_linsi] or global[mafft_ginsi]:"
			select type_of_alignment in mafft_linsi mafft_ginsi mafft_linsi_with_starting_tree mafft_ginsi_with_starting_tree none_exit
			do
				echo -e "\n\tSet the number of threads used, otherwise PASTA will use all available cpus. ( icipe only: use 4 for pc and 32 for hpc)"
				unset num_cpus
				regexp='^[0-9][1-9]*$'
				until [[ "$num_cpus" =~ $regexp ]]
				do
					read -p "Please enter the number of cpus to be used: " num_cpus
				done

				rename ${i}
				jobs_dest=${pasta_dest}\jobs/${src_dir}
				aligned_dest=${pasta_dest}aligned/${src_dir}
				temporaries_dest=${pasta_dest}temporaries
				dest=("${jobs_dest}" "${aligned_dest}" "${temporaries_dest}")
				for dest1 in ${dest[@]}
				do
					until [[ -d ${dest1} ]]
					do
						echo "creating output directory '${dest1}'..."
						mkdir ${dest1}
					done
				done

				case $type_of_alignment in
					mafft_linsi)
						echo -e "\nDoing local alignment of `basename $i`..."
						#${PYTHON3_EXEC} ${runpasta} --aligner=mafft -i $i -j ${output_filename} --temporaries=${pasta_dest}temporaries/ -o ${pasta_dest}\jobs/
						${PYTHON3_EXEC} ${runpasta} --num-cpus=${num_cpus} --aligner=mafft -i $i -j ${output_filename} --temporaries=${temporaries_dest} -o ${jobs_dest}
						cp ${jobs_dest}/*.${output_filename}.aln ${aligned_dest}/ && mv ${aligned_dest}/{*.${output_filename}.aln,${output_filename}.aln}
						cp ${jobs_dest}/${output_filename}.tre ${aligned_dest}/${output_filename}.tre
						break
						;;
					mafft_ginsi)
		       				echo -e "\nDoing global alignment of `basename $i`..."
						#${PYTHON3_EXEC} ${runpasta} --aligner=ginsi -i $i -j ${output_filename} --temporaries=${pasta_dest}temporaries/ -o ${pasta_dest}\jobs/
						${PYTHON3_EXEC} ${runpasta} --num-cpus=${num_cpus} --aligner=ginsi -i $i -j ${output_filename} --temporaries=${temporaries_dest} -o ${jobs_dest}
						cp ${jobs_dest}/*.${output_filename}.aln ${aligned_dest}/ && mv ${aligned_dest}/{*.${output_filename}.aln,${output_filename}.aln}
						cp ${jobs_dest}/${output_filename}.tre ${aligned_dest}/${output_filename}.tre
						break
						;;
					mafft_linsi_with_starting_tree)
						unset start_tree
                                                echo -e "\nDoing local alignment of `basename $i` using a starting tree..."
						until [[ ( -f "$start_tree" ) && ( `basename -- "$start_tree"` =~ .*\.(tre) ) ]]
						do
							echo -e "\n\tFor the starting tree provide the full path to the file, the filename included."
							read -p "Please enter the file to be used as the starting tree: " start_tree
						done
                                                #${PYTHON3_EXEC} ${runpasta} --aligner=mafft -i $i -t $start_tree -j ${output_filename} --temporaries=${pasta_dest}temporaries/ -o ${pasta_dest}\jobs/
						${PYTHON3_EXEC} ${runpasta} --num-cpus=${num_cpus} --aligner=mafft -i $i -t $start_tree -j ${output_filename} --temporaries=${temporaries_dest} -o ${jobs_dest}
                                                cp ${jobs_dest}/*.${output_filename}.aln ${aligned_dest}/ && mv ${aligned_dest}/{*.${output_filename}.aln,${output_filename}.aln}
                                                cp ${jobs_dest}/${output_filename}.tre ${aligned_dest}/${output_filename}.tre
						break
						;;
					mafft_ginsi_with_starting_tree)
						unset start_tree
                                                echo -e "\nDoing global alignment of `basename $i` using a starting tree..."
						until [[ ( -f "$start_tree" ) && ( `basename -- "$start_tree"` =~ .*\.(tre) ) ]]
                                                do
							echo -e "\n\tFor the starting tree provide the full path to the file, the filename included."
                                                        read -p "Please enter the file to be used as the starting tree: " start_tree
                                                done
                                                #${PYTHON3_EXEC} ${runpasta} --aligner=ginsi -i $i -j ${output_filename} --temporaries=${pasta_dest}temporaries/ -o ${pasta_dest}\jobs/
						${PYTHON3_EXEC} ${runpasta} --num-cpus=${num_cpus} --aligner=ginsi -i $i -j ${output_filename} --temporaries=${temporaries_dest} -o ${jobs_dest}/
                                                cp ${jobs_dest}/*.${output_filename}.aln ${aligned_dest}/ && mv ${aligned_dest}/{*.${output_filename}.aln,${output_filename}.aln}
                                                cp ${jobs_dest}/${output_filename}.tre ${aligned_dest}/${output_filename}.tre
						break
						;;
					none_exit)
						break
						;;
					*)
						echo "error: Invalid selection!"
				esac
			done
                else
                        echo "input file error in `basename $i`: input file should be a .fasta file format"
                        continue
                fi
        done
}



#removing gappy columns with at most n residues.
#python3 ../../../../code/tools/pasta_code/pasta/run_seqtools.py -infile jobs/eafroCOI_all_data2_temp_iteration_2_seq_unmasked_alignment -informat COMPACT3 -outfile eafroCOI_all_iter2.fasta -outformat FASTA -masksites 20 -rename jobs/eafroCOI_all_data2_temp_name_translation.txt


#=====================================================================================
upp_align() { #UPP stands for Ultra-large alignments using Phylogeny-aware Profiles. A modification of SEPP, SATé-Enabled Phylogenetic Placement, for performing alignments of ultra-large and fragmentary datasets.
	#Usage: $ python <bin>/run_upp.py -s <unaligned_sequences>
	#To run UPP with a pre-computed backbone alignment and tree, run
	#	$ python <bin>/run_upp.py -s input.fas -a <alignment_file> -t <tree_file>
	#To run the parallelized version of UPP, run
	#	$ python <bin>/run_upp.py -s input.fas -x <cpus>
	usage $@
	echo "UPP starting alinment..."

	PYTHON3_EXEC=$( which python3 )
	run_upp=${co1_path}code/tools/sepp/run_upp.py
	
	for i in $@
	do
		if [ ! -f $i ]
		then
			echo "input error: file $i is non-existent!"
		elif [[ ( -f $i ) && ( `basename $i` =~ .*\.(afa|fasta|fa|aln)$ ) ]]
		then
			echo -e "\tProceeding with `basename $i`"
			echo -e "\tPlease select the type of alignment method;\n\tUsing unaligned sequences only[using_sequences_only] or using a backbone[using_precomputed_backbone]:"
			select type_of_alignment in using_sequences_only using_precomputed_backbone none_exit
			do
				echo -e "\n\tSet the number of threads used, otherwise UPP will use all available cpus. ( icipe only: use 4 for pc and 32 for hpc)"
				unset num_cpus
				regexp='^[0-9][1-9]*$'
                                until [[ "$num_cpus" =~ $regexp ]]
                                do
                                        read -p "Please enter the number of cpus to be used: " num_cpus
                                done

				rename ${i}
				jobs_upp_dest=${pasta_dest}jobs_upp/${src_dir}
				aligned_dest=${pasta_dest}aligned/${src_dir}
				temporaries_dest=${pasta_dest}temporaries/sepp
				dest=("${jobs_upp_dest}" "${aligned_dest}" "${temporaries_dest}")
				for dest1 in ${dest[@]}
				do
					until [[ -d ${dest1} ]]
					do
						echo "creating output directory '${dest1}'..."
						mkdir ${dest1}
					done
				done

				case $type_of_alignment in
					using_sequences_only)
						echo -e "\nDoing Multiple Sequence Alignment of `basename $i` based on the fragmentary sequences alone"
						${PYTHON3_EXEC} ${run_upp} -s $i -o ${output_filename} --tempdir ${temporaries_dest} -d ${jobs_upp_dest} #-x ${num_cpus}
						cp ${jobs_upp_dest}/*.${output_filename}_alignment.fasta ${aligned_dest}
						break
						;;
					using_precomputed_backbone)
						rename
						unset backbone
						unset start_tree
						echo -e "\nDoing Multiple Sequence Alignment of `basename $i` using a backbone alignment and a starting tree..."

						until [[ ( -f "$start_tree" ) && ( `basename -- "$start_tree"` =~ .*\.(tre)$ ) ]]
						do
							echo -e "\n\tFor the starting tree provide the full path to the file, the filename included."
							read -p "Please enter the file to be used as the starting tree: " start_tree
						done

						until [[ ( -f "$backbone" ) && ( `basename -- "$backbone"` =~ .*\.(aln|fasta|fa|afa)$ ) ]]
						do
							echo -e "\n\tFor the backbone alignment provide the full path to the file, the filename included."
							read -p "Please enter the file to be used as the backbone alignment: " backbone
						done

						${PYTHON3_EXEC} ${run_upp} -s $i -a ${backbone} -t ${start_tree} -o ${output_filename} --tempdir ${temporaries_dest} -d ${jobs_upp_dest} #-x ${num_cpus}
						cp ${jobs_upp_dest}/*.${output_filename}_alignment.fasta ${aligned_dest}/
                                                break
                                                ;;
                                        none_exit)
                                                break
                                                ;;
                                        *)
                                                echo "error: Invalid selection!"
                                esac
                        done
                else
                        echo "input file error in `basename $i`: input file should be a .fasta file format"
                        continue
                fi
        done

}
