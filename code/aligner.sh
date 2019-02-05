#!/bin/bash

#this is the alignment script that generates the alinments

co1_path=~/bioinformatics/github/co1_metaanalysis/
source ${co1_path}code/align.sh

#MUSCLE ALINMENT FUNCTIONS

printf "\nTo select a task Enter [n] as follows:\n[1] to do MSA of input files.\n[2] to refine the alignments of input files.\n[3] to build an unified alignment from separate input files.\n[4] to exit.\n"
read -p "Your choice: " choice
case $choice in
	1)
		echo -e "\nSelect large data set aligment method from:\n"
		select option in msa_largedata_muscle msa_largedata_mafft msa_largedata_tcoffee none-exit
		do
			case $option in
				msa_largedata_muscle)
					muscle_large $@
					;;
				msa_largedata_mafft)
					mafft_GlINS1 $@
					;;
				msa_largedata_tcoffee)
					tcoffee_large $@
					;;
				none-exit)
					break 1
					;;
				*) echo "error: Invalid selection"
			esac
		done
		;;
	2)
		muscle_refine $@
		;;
	3)
		echo -e "\nSelect an MSA file operation from the following:\n"
		select option in msa_p2palnmt_muscle msa_merge_mafft msa_addseqs_mafft msa_addfragments_mafft none-exit
		do
			case $option in
				msa_p2palnmt_muscle)
					muscle_p2p $@
					;;
				msa_merge_mafft)
					mafft_merge $@
					;;
				msa_addseqs_mafft)
					mafft_add $@
					;;
				msa_addfragments_mafft)
					mafft_addfragments $@
					;;
				none-exit)
					break
					;;
				*)
					echo "error: invalid selection"
			esac
		done
		;;
	4)
		break
		;;
	*)
		echo "error: invalid selection"
esac



