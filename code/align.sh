#!/bin/bash

#"""This script is a module with alignment instructions as fuctions, methods and properties."""

co1_path=~/bioinformatics/github/co1_metaanalysis/
muscle_dest=${co1_path}data/output/alignment/muscle_output/
mafft_dest=${co1_path}data/output/alignment/mafft_output/
makemergetable=${co1_path}code/makemergetable.rb
tcoffee_dest=${co1_path}data/output/alignment/t_coffee_output/

usage() { #checks if the positional arguments (input files) for execution of the script are defined
	if [ $# -eq 0 ]
	then
		echo "Input error..."
		echo "Usage: $0 file1.fasta[file2.fasta file3.fasta ...]"
		exit 1
		
	fi
}

rename() { #generates output file names with same input filename prefix. The suffix (".suffix") is set in individual functions.
	input_filename=`basename $i`
	output_filename=${input_filename%.*}
}


#========================================================================================
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
		elif [[ ( -f $i ) && ( `basename $i` == *.fasta ) ]]
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
		elif [[ ( -f $i ) && ( `basename $i` == *.afa ) ]]
		then
			rename
			echo -e "\nproceeding with file `basename $i`..."
			muscle -in $i -fastaout ${muscle_dest}./refined/\r${output_filename}.afa -clwout ${muscle_dest}./refined/\r${output_filename}.aln -refine
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
	unset $in1_outname && echo "good: output filename var in1_outname is empty"
	unset $in2_outname && echo "good: output filename var in2_outname is empty"


	for i in $@
        do
		if [ $# -ne 2 ]
		then
			echo "input file error: only two input files are allowed!"
			break
		elif [[ ( -f $i ) && ( `basename $i` == *.afa ) ]]
		then
			if [ -z $in1_outname ]
			then 
				rename $i
				in1_outname=$output_filename
			else
				rename $i
				in2_outname=$output_filename
			fi
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
		muscle -profile -in1 $1 -in2 $2 -fastaout ${muscle_dest}./merged/${in1_outname}_${in2_outname}.afa -clwout ${muscle_dest}./merged/${in1_outname}_${in2_outname}.aln
	else
		echo " A error with output_filenames: in1_outname and in2_outname "
	fi
}



#========================================================================================
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
                elif [[ ( -f $i ) && ( `basename $i` == *.fasta ) ]]
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
						;;
					clustal_output_format)
						echo -e "\nGenerating a clustal format output\n"
						mafft --large --globalpair --thread -1 --reorder --clustalout $1 > ${mafft_dest}aligned/${output_filename}.aln
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
                elif [[ ( -f $i ) && ( `basename $i` == *.fasta) ]]
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
				exit 1
                        fi
                elif [ ! -f $i ]
                then
                        echo "input error: file $i is non-existent!"
                        exit 1
                else
                        echo "input file error in `basename $i`: input file should be a .fasta file format"
                        exit 1
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
	echo -e "\nwarning: Each sub-MSA is forecd to form a monophyletic cluster in version 7.043 and higher (2013/May/26)."
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


#==============================================================================

### t_coffee: the regressive mode of T-Coffee is meant to align very large datasets with a high accuracy. It starts by aligning the most distantly related sequences first. It uses this guide tree to extract the N most diverse sequences. In this first intermediate MSA, each sequence is either a leaf or the representative of a subtree. The algorithm is re-aplied recursively onto every representative sequence until all sequences have been incorporated in an internediate MSA of max size N. The final MSA is then obtained by merging all the intermediate MSAs into the final MSA.

#Fast and accurate: the regressive alignment is used to align the sequences in FASTA format. The tree is estimated using the mbed method of Clustal Omega (-reg_tree=mbed), the size of the groups is 100 (-reg_nseq=100) and the method used to align the groups is Clustal Omega:

#syntax:	$ t_coffee -reg -seq proteases_large.fasta -reg_nseq 100 -reg_tree mbed -reg_method clustalo_msa -outfile proteases_large.aln -outtree proteases_large.mbed

#		-seq: provide sequences. must be in FASTA format
#		-reg_tree: defines the method to be used to estimste the tree
#		-outtree: defines the name of newly computed out tree. mbed method of Clustal Omega is used.
#		-outfile**: defines the name of output file of the MSA
#		-reg_nseq: sets the max size of the subsequence alignments; the groups is 100
#		-reg_thread: sets max threads to be used
#		-reg_method**: defines the method to be used to estimate MSA: Clustal Omega


tcoffee_large() {
	usage $@
	echo "t-coffee starting alinment..."
	
	for i in $@
	do
		if [ ! -f $i ]
		then
			echo "input error: file $i is non-existent!"
		elif [[ ( -f $i ) && ( `basename $i` == *.fasta ) ]]
		then
			rename
			echo -e "\nproceeding with file `basename $i`..."
			t_coffee -reg -seq $i -reg_nseq 100 -reg_tree mbed -reg_method clustalo_msa -outfile ${tcoffee_dest}aligned/${output_filename}.aln -newtree ${tcoffee_dest}trees/${output_filename}.mbed
		else
			echo "input file error in `basename $i`: input file should be a .fasta file format"
			continue
			
		fi
	done

}
