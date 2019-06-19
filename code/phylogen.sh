#!/bin/bash

#"""this script has fuctions for phylogenetic inferencing"""


co1_path=~/bioinformatics/github/co1_metaanalysis/
fasttree_dest=${co1_path}data/output/phylogen/fasttree_output/
raxml_dest=${co1_path}data/output/phylogen/raxml_output/

source ${co1_path}code/process_all_input_files.sh
#usage() { #checks if the positional arguments (input files) for execution of the script are defined
#        if [ $# -eq 0 ]
#        then
#                echo "Input error..."
#                echo "Usage: ${FUNCNAME[1]} file1.fasta[file2.fasta file3.fasta ...]"
#                exit 1
#
#        fi
#}
#
#rename() { #generates output file names with same input filename prefix. The suffix (".suffix") is set in individual functions.
#        input_filename=`basename $i`
#        output_filename=${input_filename%.*}
#}

#=====================================================================================
#Using FastTree to infer a tree for a nucleotide alignment with the GTR+CAT model, use
#syntax:	FastTree -gtr -gamma -nt alignment_file > tree_file 

#To quickly estimate the reliability of each split in the tree, FastTree uses the Shimodaira-Hasegawa test on the three alternate topologies (NNIs) around that split.
#Input formats: FastTree reads multiple sequence alignments in fasta format or in interleaved phylip format.
#Output formats: FastTree outputs trees in Newick format.



fastree_phylo() { #
	usage $@
        echo "fasttree starting Phylogenetic tree inference..."

        for i in $@
        do
                if [ ! -f $i ]
                then
                        echo "input error: file $i is non-existent!"
                elif [[ ( -f $i ) && ( `basename $i` =~ .*\.(aln|afa|fasta|fa) ) ]]
                then
                        rename
                        echo -e "\nproceeding with file `basename $i`..."
			fasttree -gtr -gamma -nt $i > ${fasttree_dest}${output_filename}.tree
                else
                        echo "input file error in `basename $i`: input file should be a .fasta file format"
                        continue
                fi
        done

}

#=====================================================================================

# Using RAxML (Randomized Axelerated Maximum Likelihood) to infer a phylogenetic tree from MSA nucleotide sequences alignment

#Syntax:	raxmlHPC -m GTRGAMMA -p 12345 -# 20 [-b 12345 -# 100] -s dna.phy -n T6 

#	:-m -defines the model of evolution, GTR and the variable evolutionary rate per site approximation, CAT or model GAMMA
#	:-n -defines file name appendix, all output files; RAxML_FileTypeName.T6
#	:-p 12345 -forces starting trees by providing a fixed random number seed (RAxML uses randomized stepwise addition parsimony trees, it will not generate the same starting tree every time); OR -t startingTree.txt -to pass a starting tree OR;-p 12345 -s dna.phy -# 20 conducts multiple searches for the best tree.
#	:-s dna.phy - defines the input sequences; -s protein.phy or -s binary.phy
#	:-b 12345 -# 100 -provides a bootstrap random number seed via -b 12345 and the number of bootstrap replicates we want to compute via -# 100. For automatic determination of a sufficient number of bootstrap replicates; replace -# 100 by one of bootstrap convergence criteria: -# autoFC, -# autoMRE, -# autoMR, -# autoMRE_IGN #The computed bootstrap replicate trees will be printed to a file called RAxML_bootstrap.T14 and can used to draw bipartitions on the best ML tree as follows:
#	raxmlHPC -m GTRCAT -p 12345 -f b -t RAxML_bestTree.T13 -z RAxML_bootstrap.T14 -n T15

#	This call will produce to output files that can be visualized with Dendroscope: RAxML_bipartitions.T15 (support values assigned to nodes) and RAxML_bipartitionsBranchLabels.T15 (support values assigned to branches of the tree). Note that, for unrooted trees the correct representation is actually the one with support values assigned to branches and not nodes of the tree!

#a hybrid/combined PThreads ↔ MPI parallelization that uses MPI to distribute bootstrap replicates or independent tree searches to different shared memory nodes in a cluster while it uses PThreads to parallelize the likelihood calculations of single tree searches. We call this coarse grain (MPI) and fine-grain (PThreads) parallelism: Define the exact number of CPUs avilable in the system via the -T option. (this will run nicely on my laptop that has two cores) the parallel efficiency of the PThreads version of RAxML depends on the alignment length (number of distinct patterns in your alignment); use one core/thread per 500 DNA
#	raxmlHPC-HYBRID-AVX2 -T 2 -m GTRGAMMA -p 12345 -# 20 -b 12345 -# autoMRE -s dna.phy
#CAT approximation of  rate heterogneity  works very well on datasets with more than 50 taxa.
#       raxmlHPC-HYBRID-AVX2 -T 2 -o outgroup1,outgroup2 -m GTRCAT -p 12345 -# 20 -b 12345 -# 100 -s dna.phy

raxml_phylo(){ # This function performs a maximum likelihood search of the phylogenetic trees. It starts by conducting multiple (20) searches for the best starting tree (-p 12345 -s indna.phy -# 20).
	usage $@
	echo "RAxML starting Phylogenetic tree inference..."

	for i in $@
	do
		if [ ! -f $i ]
		then
			echo "input error: file $i is non-existent!"
		elif [[ ( -f $i ) && ( `basename $i` =~ .*\.(aln|afa|fasta|fa) ) ]]
		then
			rename
			echo -e "\nDeleting unwanten records from `basename $i`.
..\n"
			unset Choice
			read -p "Please enter Yes or NO:: " Choice
			regexp1='^[n|N|No|NO|no|]$'
			until [[ "$Choice" =~ $regexp1 ]]
			do
				read -p "Please enter Yes or NO:: " Choice
				delete_unwanted $i
			done

			select bootstrap_option in autoMRE 20 100 1000
			do
				echo -e "\nProceeding with file `basename $i`...\nBeginning best starting tree search and bootstrap search, This may take a while...\n"
				raxmlHPC-HYBRID-AVX2 -T 2 -m GTRGAMMA -p 12345 -# 20 -b 12345 -# ${bootstrap_option} -s $i -w ${raxml_dest} -n ${output_filename}
			done
			echo -e "\nBest stating tree search and bootstrap search DONE...\nBest tree written to ${raxml_dest}RAxML_bestTree.${output_filename} \nAnd Bootstrap replicate trees written to ${raxml_dest}RAxML_bootstrap.${output_filename}\nProceeding with drawing bipartitions on the best ML tree...\n"
			raxmlHPC-AVX2 -m GTRCAT -p 12345 -f b -t ${raxml_dest}RAxML_bestTree.${output_filename} -z ${raxml_dest}RAxML_bootstrap.${output_filename} -w ${raxml_dest} -n${output_filename}1
			echo -e "\nThe best tree ${raxml_dest}RAxML_bestTree.${output_filename} annotated with support values has been generated and printed to ${raxml_dest}RAxML_bipartitions.${output_filename}1 for nodes and ${raxml_dest}RAxML_bipartitionsBranchLabels.${output_filename}1 for branches\nProceeding to build a consensus tree...\n"
			raxmlHPC-AVX2 -m GTRCAT -J MRE -z ${raxml_dest}RAxML_bootstrap.${output_filename} -w ${raxml_dest} -n MRE_CONS
			echo -e "\nDONE generating the consensus tree and printed to ${raxml_dest}RAxML_bootstrap.MRE_CONS\n"
		else
			echo "input file error in `basename $i`: input file should be a FASTA or relaxed or interleaved PHYLIP file format"
			continue
		fi
	done	
}

#outgroups: allow the tree to be rooted at the branch leading to the outgroup. can contain one or more than one taxon and if the outgroups ceases to be monophyletic RAxML prints out a respective warning:
#       raxmlHPC-AVX2 -p 12345 -o Mouse -m GTRGAMMA -s dna.phy -n T30 #one outgroup
#       raxmlHPC-AVX2 -p 12345 -o Mouse,Rat -m GTRGAMMA -s dna.phy -n T31 #two

#Rapid Bootstrapping: use -x instead of -b to provide a bootstrap random number seed
#       raxmlHPC -m GTRGAMMA -p 12345 -x 12345 -# 100 -s dna.phy -n T19

#rapid bootstrapping allows you to do a complete analysis (ML search + Bootstrapping) in one single step. To do 100 rapid Bootstrap searches, 20 ML searches and return the best ML tree with support values to you by typing:
#       raxmlHPC -f a -m GTRGAMMA -p 12345 -# 20 -x 12345 -# 100 -s dna.phy -n T20


