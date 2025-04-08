#!/bin/bash

# For set up please do as follows
# $ conda activate <environment_name>
# $ conda install -c bioconda mafft bmge iqtree
# If you wish to work with Mafft from a HPC and exploit its higher_performance version using MPI version install it from source code from this link >> https://mafft.cbrc.jp/alignment/software/mpi.html
# And set the path below to its installation
run_mafft=/path/to/mafft-7.471-without-extensions/bin/mafft

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
	input_filename=$(basename -- "$@")
	output_filename=${input_filename%.*}
	filename_ext=${input_filename##*.}
	src_dir_path=`dirname $(realpath $@)`
	src_dir=${src_dir_path##*/}
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

			rename $i
                        echo -e "\nmafft L-large-INS-1 MSA: proceeding with file `basename $i`..."
                        printf "Choose from the following installations of MAFFT: \n"
                        select mafft_install in local_environment source_code none_exit
                        do
                                case $mafft_install in
                                        local_environment)
                                                echo -e "\nGenerating .fasta output\n"
                                                mafft --localpair --maxiterate 1000 --thread -${num_cpus} --reorder $i > ${src_dir_path}/${output_filename}.afa
						aligned_file=${src_dir_path}/${output_filename}.afa
						break
                                                ;;
                                        source_code)
                                                echo -e "\nGenerating a clustal format output\n"
						bash $mpionly
						${run_mafft} --mpi --large --localpair --thread -${num_cpus} --reorder --clustalout $1 > ${src_dir_path}/${output_filename}_l.aln
						aligned_file=${src_dir_path}/${output_filename}_l.aln
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


#=====================================================================================
#Handling Gappy sites and entropy using BMGe: can be installed from source code or in a conda environment:
#source code Syntax:$	java -Xmx2048m -jar BMGE-1.12/BMGE.jar -i enafroCOI_all_clean_sN10-eN10.aln -t DNA -g 0.95 -b 1 -h 0:1 -of without_gappy.fasta : the actual run
#conda env syntax:$ bmge -i enafroCOI_all_clean_sN10-eN10.aln -t DNA -g 0.95 -b 1 -h 0:1 -of without_gappy.fasta : the actual run

#	$ java -jar BMGE-1.12/BMGE.jar -? :For help
#	-i : Defines the input file: FASTA or PHYLIP format
#	-t [AA|DNA|CODON]: Defines input sequence coding
#	-o [-op,-of,-on,-oh]: Defines the output file format: PHYLIP, FASTA, NEXUS, HTML. For NCBI-formatted FASTA input sequences the number of fields in the header of PHYLIP or NEXUS output files can be altered using -oppp* or -onnn*
#	-h [max|min:max]: Entropy score cut-off; (range from 0 to 1; default: 0:0.5): The higher the score the more the disorder
#	-g [col_rate|raw_rate:col_rate]: Gap rate cut-off; gap rate per site (col (ranges from 0 to 1; default: 0.2); remove sites with > 5% gaps, -g 0.05); gap rate per sequence (row)
#	-b integer : Minimum block size; determines conserved-regions-size to be used. equal or greater than integer. default is 5.
#	$ egrep -B 1 'N{20,}' without_gappy.fasta : Those with more than 10 strings of "N"s
#	$ 
bmge_cleanup() { 
	usage $@
	echo "BMGe starting Multiple sequence alignment cleanup..."

	for i in $@
	do
		if [ ! -f $i ]
		then
			echo "input error: file $i is non-existent!"
		elif [[ ( -f $i ) && ( `basename $i` =~ .*\.(aln|afa|fasta|fa)$ ) ]]
		then
			input_src=`dirname "$( realpath "${i}" )"`
			rename $i
			regexg='^(1|0\.[0-9][1-9])$'
			unset g_rate

			echo -e "\n\tproceeding with file `basename $i`..."
			echo -e "\n\tKindly enter the accepted maximum degree of gap rates parcentage in decimal values between 0.01 and 1, default is 0.05;"
			until [[ "$g_rate" =~ $regexg ]]
			do
				read -p "Please enter the muximum gap rate per site in up to two decimal places: " g_rate
			done

			concatenate_fasta_seqs $i
			g_ratee=`echo $g_rate | sed 's/0\.//g'`
			#java -Xmx2048m -jar ${bmge_path}BMGE.jar -i ${i} -t DNA -g ${g_rate} -b 1 -h 0:1 -of ${input_src}/${output_filename}_${g_ratee}g.${filename_ext}
			 bmge -i ${i} -t DNA -g ${g_rate} -b 1 -h 0:1 -of ${input_src}/${output_filename}_${g_ratee}g.${filename_ext}
			 trimmed_MSA=${input_src}/${output_filename}_${g_ratee}g.${filename_ext}
		else
			echo "input file error in `basename $i`: input file should be a .(aln|afa|fasta|fa) file format and extension"
			continue
		fi
	done

}


#==================================================================================================================================================================================================================

#Input: FASTA, NEXUS, CLUSTALW,PHYLIP
#output: NEWICK(.treefile), main report (.iqtree), log file (.log), model file (.model), consensus tree (.contree), % bs-support values (.splits.nex) and star-dot format (.splits), 

#automatically selects best-fit model using modelfinder (iqtree >= 1.5.4)
#Fast and effective stochastic algorithm to reconstruct phylogenetic trees by ML
#ultrafast bootstrap approximation (UFBoot) to assess branch supports: -bb(version 1.X)|-B and -bnni option to reduce risk of overestimating branch supports due to severe model violation bu UFBoot
#Assessing branch support with single branch tests. Several fast branch tests like SH-aLRT. both SH-aLRT and UFBoot can be assinged jointly in a single run

#Combine ModelFinder, tree search, SH-aLRT test and ultrafast bootstrap with 1000 replicates:
#Syntax: iqtree -s example.phy -bb 1000 -bnni -alrt 1000 --prefix ouputfilename

#Find best partition scheme followed by tree inference and ultrafast bootstrap:
#Syntax: iqtree -s example.phy -p example.nex -m MFP+MERGE -B 1000

#use 4CPUs or automatically determine the best number: -T|-nt(version 1.x)
#iqtree -s example.phy -m GTR+R4 -nt [4|AUTO]

#General
#iqtree -s example.phy -bb 1000 -bnni -alrt 1000 -nt AUTO [--prefix|-pre(-version 1.X)] ouputfilename

iqtree_phylo() { # This function based on iqtree version 1.X, performs an automatic model search using ModelTest, UFBoot, optimize each bootstrap tree using a hill-climbing nearest neighbor interchange (NNI) search, SH-aLRT and automatic detection of best fit number of cores.

	usage $@
	echo "Iqtree mode starting Phylogenetic tree inference..."
	
	for i in $@
	do
		if [ ! -f $i ]
		then
			echo "input error: file $i is non-existent!"
		elif [[ ( -f $i ) && ( `basename $i` =~ .*\.(aln|afa|fasta|fa)$ ) ]]
		then
			 #Substituting Illegal characters in taxon-names are: tabulators, carriage returns, spaces, ":", ",", ")", "(", ";", "]", "[", "'","/","+" that affect the interpretation in IQ-TREE
 			 sed -i "s/\[//g; s/\]//g; s/ /_/g; s/://g; s/;//g; s/,/__/g; s/(/__/g; s/)//g; s/'//g; s/|/-.-/g; s/\//_/g; s/\//_/g" $i

			#Bootstrap support
			unset bs_reps
			regexp='^[0-9]+$'
			until [[ "$bs_reps" =~ $regexp ]]
			do
				read -p "Please enter the number of bootstrap replicates to generate before the best-scoring topology of the final ML-tree is assigned branch support values, the 1000 is advisable : " bs_reps
			done
			rename $i

			#Setting up the output directory
			unset dest
			dest=${iqtree_dest}${output_filename}
			until [[ -d ${dest} ]]
			do
				echo "creating output directory '${dest}'..."
				mkdir ${dest}
			done

			#Actual analysis
			echo -e "\nBegining complete analysis (ML search + Bootstrapping) for `basename $i` in one step..."
			
			echo -e "To select the best fit substitution model for the sequences please choose one of the options [1|2|3] to proceed or [4] to cancel"
			optns[0]="Enter a pre-selected best-fit model"
			optns[1]="Find best-fit model with free-rate heterogeneity (Rn) from a subset of 'comma separated base models'"
			optns[2]="Find the best-fit substitution model (with free-rate heterogeneity (Rn))from all available substitution models in IQtree Followed by tree inference"
			optns[3]="Exit"
			PS3='Select option [1|2|3] to delete, or [4] to exit: '
			select optn in "${optns[@]}"
			do
				unset model
				unset model1
				#regex1 $regex2 not working.
				regexp1="^[(JC)|(GTR)|(HKY)|(SYM)|(K3Pu?)|(TVMe?)|(TNe?)|(TPM2?3?u?)].*$"
				regexp2="[(JC),?|(GTR),?|(HKY),?|(SYM),?|(K3Pu?f?),?|(TVMe?f?),?|(TNe?f?),?|(F81),?|(K2P),?|(TIMe?f?),?]$"
				case $optn in
					${optns[0]})
						until [[ "$model" =~ $regexp1 ]]
						do
							read -p "Please enter substitution model to be used:: " model
						done
						break
						;;
					${optns[1]})
						until [[ "$model1" =~ $regexp2 ]]
						do
							read -p "Please enter the selected subset of base models to be tested for the best-fit substitution model to be used. syntax: GTR,JC,...:: " model1
							model="MFP -mset ${model1}"
						done
						break
						;;
					${optns[2]})
						model="MFP"
						break
						;;
					${optns[3]})
						echo -e "Exiting selection of substitution models..."
						break
						;;
					*)
						echo "INVALID choice $REPLY"
						;;
						
				esac
			done

			iqtree -s ${i} -m ${model} -bb ${bs_reps} -bnni -alrt ${bs_reps} -nt AUTO -pre ${dest}/${output_filename} 

			if [ $? -eq 0 ]
			then
				echo -e "\nOutput phylogeny files stored in ${dest}/${output_filename}\n...Phylogenetic analysis DONE!!!"
			fi
			sed -i "s/-\.-/|/g" ${dest}/${output_filename}.treefile
			sed -i "s/-\.-/|/g" ${i}

		else
			echo "input file error in `basename $i`: input file should be a FASTA or relaxed or interleaved PHYLIP file format"
			continue
		fi
	done
}

### Uncomment the following code to run this script as:
### Syntax:	$ align_trim_phylogen.sh input1 input2 input3 ...

#for i in "$@"
#do
#	source /path/to/align_trim_phylogen.sh
#	mafft_local $i <<- EOF
#	2
#	1
#	EOF
#	bmge_cleanup $aligned_file <<- EOF
#	0.05
#	EOF
#	iqtree_phylo $trimmed_MSA <<- EOF
#	2500
#	3
#	EOF
#done
