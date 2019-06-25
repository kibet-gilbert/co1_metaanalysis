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
#syntax:	$FastTree -gtr -gamma -nt alignment_file > tree_file 

#To quickly estimate the reliability of each split in the tree, FastTree uses the Shimodaira-Hasegawa test on the three alternate topologies (NNIs) around that split.
#Input formats: FastTree reads multiple sequence alignments in fasta format or in interleaved phylip format.
#Output formats: FastTree outputs trees in Newick format.



fasttree_phylo() { #
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

#Syntax:	$raxmlHPC -m GTRGAMMA -p 12345 -# 20 [-b 12345 -# 100] -s dna.phy -n T6 

#	:-m -defines the model of evolution, GTR and the variable evolutionary rate per site approximation, CAT or model GAMMA
#	:-n -defines file name appendix, all output files; RAxML_FileTypeName.T6
#	:-p 12345 -forces starting trees by providing a fixed random number seed (RAxML uses randomized stepwise addition parsimony trees, it will not generate the same starting tree every time); OR -t startingTree.txt -to pass a starting tree OR;-p 12345 -s dna.phy -# 20 conducts multiple searches for the best tree.
#	:-s dna.phy - defines the input sequences; -s protein.phy or -s binary.phy
#	:-w relative path to the output Directory
#	:-b 12345 -# 100 -provides a bootstrap random number seed via -b 12345 and the number of bootstrap replicates we want to compute via -# 100. For automatic determination of a sufficient number of bootstrap replicates; replace -# 100 by one of bootstrap convergence criteria: -# autoFC, -# autoMRE, -# autoMR, -# autoMRE_IGN #The computed bootstrap replicate trees will be printed to a file called RAxML_bootstrap.T14 and can used to draw bipartitions on the best ML tree as follows:
#	$raxmlHPC -m GTRCAT -p 12345 -f b -t RAxML_bestTree.T13 -z RAxML_bootstrap.T14 -n T15

#	This call will produce to output files that can be visualized with Dendroscope: RAxML_bipartitions.T15 (support values assigned to nodes) and RAxML_bipartitionsBranchLabels.T15 (support values assigned to branches of the tree). Note that, for unrooted trees the correct representation is actually the one with support values assigned to branches and not nodes of the tree!

#a hybrid/combined PThreads ↔ MPI parallelization that uses MPI to distribute bootstrap replicates or independent tree searches to different shared memory nodes in a cluster while it uses PThreads to parallelize the likelihood calculations of single tree searches. We call this coarse grain (MPI) and fine-grain (PThreads) parallelism: Define the exact number of CPUs avilable in the system via the -T option. (this will run nicely on my laptop that has two cores) the parallel efficiency of the PThreads version of RAxML depends on the alignment length (number of distinct patterns in your alignment); use one core/thread per 500 DNA
#	$raxmlHPC-HYBRID-AVX2 -T 2 -m GTRGAMMA -p 12345 -# 20 -b 12345 -# autoMRE -s dna.phy
#CAT approximation of  rate heterogneity  works very well on datasets with more than 50 taxa.
#       $raxmlHPC-HYBRID-AVX2 -T 2 -o outgroup1,outgroup2 -m GTRCAT -p 12345 -# 20 -b 12345 -# 100 -s dna.phy

raxml_phylo_hard(){ # This function performs a maximum likelihood search of the phylogenetic trees. It starts by conducting a given number -# multiple searches for the best starting tree (-p 12345 -s indna.phy -# 20). Then conducts a given number bootstrap searches. and finally writes confidence values to the best tree besides computing a consensus tree.
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

			#Deleting unwanted records
			echo -e "\nDeleting unwanted records from `basename $i`..."
			unset Choice
			read -p "Please enter [Yes] or [NO] to proceed:: " Choice
			regexp1='^[n|N|No|NO|no]$'
			until [[ "$Choice" =~ $regexp1 ]]
			do
				#read -p "Please enter Yes or NO:: " Choice
				if [[ "${Choice}"=="y" || "${Choice}"=="Y" || "${Choice}"=="Yes" || "${Choice}"=="YES" || "${Choice}"=="yes" ]]
				then
					delete_unwanted $i
				fi
				read -p "Please enter [Yes] or [NO] to delete more unwanted records:: " Choice
			done

			unset rate_heterogeneity
			echo -e "Please select the rate heterogeneity model or approximation to use from the following options, enter [1] or [2]:"
			select rate_heterogeneity in GTRGAMMA GTRCAT
			do
	
				#Maximum Likelihood tree search
				echo -e "\nFinding the Best-Known Likelihood tree on `basename $i` MSA...\nSelect the number of ML searches to be conducted for best scoring tree..."
				select MLtree_searches in 10 20 100 200
				do
					echo -e "\nProceeding with file `basename $i`...\nFinding the best-scoring ML tree for the DNA alignment.."
					raxmlHPC-HYBRID-AVX2 -f d -T 2 -m ${rate_heterogeneity} -p 12345 -# ${MLtree_searches} -s $i -w ${raxml_dest} -n ${output_filename}
					break
				done
				echo -e "\nBest-scoring ML tree search DONE...\nThe best scoring ML tree written to ${raxml_dest}RAxML_bestTree.${output_filename}\n\nProceeding with bootsrap search..."
				
				#Bootsrap search
				echo -e "Please select the number of bootstrap_runs or selection method from the following options:"
				select bootstrap_option in autoMRE autoMRE_ING 20 100 1000
				do
					echo -e "\nBegining bootstrap search, This may take a while...\n"
					raxmlHPC-HYBRID-AVX2 -f d -T 2 -m ${rate_heterogeneity} -p 12345 -b 12345 -# ${bootstrap_option} -s $i -w ${raxml_dest} -n ${output_filename}1
					break
				done
				echo -e "\nBootstrap search DONE..."
				
				#Obtaining Confidence Values
				echo -e "\nProceeding with drawing bipartitions on the best ML tree to obtain a topology with support values...\n"
				
				raxmlHPC-AVX2 -m ${rate_heterogeneity} -p 12345 -f b -t ${raxml_dest}RAxML_bestTree.${output_filename} -z ${raxml_dest}RAxML_bootstrap.${output_filename}1 -w ${raxml_dest} -n ${output_filename}_BS_tree
				
				#Computing Extended Majority Rule (MRE) Consensus tree
				echo -e "\nProceeding to build a consensus tree...\n"
				
				raxmlHPC-AVX2 -m ${rate_heterogeneity} -J MRE -z ${raxml_dest}RAxML_bootstrap.${output_filename}1 -w ${raxml_dest} -n ${output_filename}_MRE-CONS
				echo -e "DONE generating the consensus tree"
				break
			done
		else
			echo "input file error in `basename $i`: input file should be a FASTA or relaxed or interleaved PHYLIP file format"
			continue
		fi
	done	
}


#=====================================================================================
#Rapid Bootstrapping: use -x instead of -b to provide a bootstrap random number seed
#       raxmlHPC -m GTRGAMMA -p 12345 -x 12345 -# 100 -s dna.phy -n T19

#rapid bootstrapping allows you to do a complete analysis (ML search + Bootstrapping) in one single step. To do 100 rapid Bootstrap searches, 20 ML searches (using every 5th BS tree) and return the best ML tree with support values to you by typing:
#       raxmlHPC -f a -m GTRGAMMA -p 12345 -x 12345 -# 100 -s dna.phy -n T20

raxml_phylo_easy(){ # This function conduct a full ML analysis, i.e., a certain number of BS replicates and a search for a best-scoring tree on the original alignment and output the bootstrapped trees (RAxML_bootstrap.TEST), the best scoring ML tree (RAxML_bestTree.TEST) and the BS support values drawn on the best-scoring tree as node labels (RAxML_bipartitions.TEST) as well as, more correctly since support values refer to branches as branch labels (RAxML_bipartitionsBranchLabels.
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

			#Deleting unwanted records
			echo -e "\nDeleting unwanted records from `basename $i`..."
			unset Choice
			read -p "Please enter [Yes] or [NO] to proceed:: " Choice
			regexp1='^[n|N|No|NO|no]$'
			until [[ "$Choice" =~ $regexp1 ]]
			do
				if [[ "${Choice}"=="y" || "${Choice}"=="Y" || "${Choice}"=="Yes" || "${Choice}"=="YES" || "${Choice}"=="yes" ]]
				then
					delete_unwanted $i
				fi
				read -p "Please enter [Yes] or [NO] to delete more unwanted records:: " Choice
			done
			
			#Full analysis rapid Bootstrapping and Maximum Likelihood search
			echo -e "Please select the number of bootstrap_runs or selection method from the following options:"
			select bootstrap_option in autoMRE autoMRE_ING 20 100 1000
			do
				unset rate_heterogeneity
				echo -e "Please select the rate heterogeneity model or approximation to use from the following options, enter [1] or [2]:"
				select rate_heterogeneity in GTRGAMMA GTRCAT
				do
					echo -e "\nBegining complete analysis (ML search + Bootstrapping) for `basename $i` in one step..."
					raxmlHPC-HYBRID-AVX2 -f a -T 2 -m ${rate_heterogeneity} -p 12345 -x 12345 -# ${bootstrap_option} -s $i -w ${raxml_dest} -n ${output_filename}
					#raxmlHPC-HYBRID-AVX2 -f a -T 2 -m GTRGAMMA -p 12345 -x 12345 -# ${bootstrap_option}­-s $i -w ${raxml_dest} -n ${output_filename}
					break
				done
				break
			done
			
			echo -e "\nComplete analysis done"
		else
			echo "input file error in `basename $i`: input file should be a FASTA or relaxed or interleaved PHYLIP file format"
			continue
		fi
	done
}


#=====================================================================================
# OUTGROUPS: allow the tree to be rooted at the branch leading to the outgroup.
#There are two options:
#	1. Add outgroups before ML and bootstrap analysis: can contain one or more than one taxon and if the outgroups ceases to be monophyletic RAxML prints out a respective warning:
#	$raxmlHPC-AVX2 -p 12345 -o Mouse -m GTRGAMMA -s dna.phy -n T30 #one outgroup
#	$raxmlHPC-AVX2 -p 12345 -o Mouse,Rat -m GTRGAMMA -s dna.phy -n T31 #two

#	2. Avoid outgroups in the initial ML and Bootstrap analyses: First build a tree (ML+BS search)for only the ingroup, then, use EPA (2011) to place outgroups. Many advantages: can test/use different outgroups; if the outgroup pertubs the ingroup analysis, no need to re-run; avoid ougroup affecting branch lengths of the ingroups, resulting from how evolutionary placement algorithm has been built and; outputs placement probabilities (likelihood weights) for each outgroup
# Evolutoinary placement algorithm (EPA);
#	$raxmlHPC­-f v -s alg -t referenceTree -m GTRCAT -n TEST
# To insert different sequences into same refence tree frequently and efficiently, avoiding estimation of model parameters and branch lengths everytime do:
# Generate a model file containing the model parameters for th reference tree: stored in RAxML_binaryModelParameters.PARAMS, which can be read into EPA
#	$raxmlHPC -f e -m GTRGAMMA -s referenceAlignment -t refenceTree -n PARAMS
#	$raxmlHPC -f v -R RAxML_binaryModelParameters.PARAMS -t RAxML_results.PARAMS -s alg -m GTRGAMMA -n TEST2
# For binary model files, the models of rate heterogeneity must be the same. Anwhen using GTRCAT, disable pattern compression using -H flag in both runs.

raxml_rooting(){ # This funtion generates a model filewith model parameters for reference tree then places the outgroup/s in the tree
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

			unset rate_heterogeneity
			echo -e "Please select the rate heterogeneity model or approximation to use from the following options, enter [1] or [2]:"
			select rate_heterogeneity in GTRGAMMA GTRCAT
			do
				unset tree
				unset outgroup
				echo -e "\nBeginning rerooting of the tree. Please enter the path to the tree file to be  rerooted..."
				read -p "Please enter the phylogenetic tree::  " tree
				case $rate_heterogeneity in
					GTRGAMMA)
						echo -e "\nBeginning rooting based on ${rate_heterogeneity} rate heterogeneity model...\nGenerating reference tree model parameters binary file..."
						raxmlHPC-AVX2 -f e -m ${rate_heterogeneity} -s ${i} -t ${tree} -w ${raxml_dest} -n ${output_filename}_PARAMS
						echo -e "\nBinary files generation DONE.\nProceeding with outgroup insertion...\nPlease enter the path to the file containing the outgroup, it should be a single sequence/record..."
						read -p "Please enter the outgroup file:: " outgroup
						raxmlHPC-AVX2 -f v -m ${rate_heterogeneity} -R ${raxml_dest}RAxML_binaryModelParameters.${output_filename}_PARAMS -t ${raxml_dest}RAxML_results.${output_filename}_PARAMS -s ${outgroup} -n ${output_filename}_rooted
						echo -e "Thorough outgroup insertion DONE."
						break
						;;
					GTRCAT)
						echo -e "\nBeginning rooting based on ${rate_heterogeneity} rate heterogeneity model...\nGenerating reference tree model parameters binary file..."
						raxmlHPC-AVX2 -f e -H -m ${rate_heterogeneity} -s ${i} -t ${tree} -w ${raxml_dest} -n ${output_filename}_PARAMS
						echo -e "\nBinary files generation DONE.\nProceeding with outgroup insertion...\nPlease enter the path to the file containing the outgroup, it should be a single sequence/record..."
						read -p "Please enter the outgroup file:: " outgroup
						raxmlHPC-AVX2 -f v -H -m ${rate_heterogeneity} -R ${raxml_dest}RAxML_binaryModelParameters.${output_filename}_PARAMS -t ${raxml_dest}RAxML_results.${output_filename}_PARAMS -s ${outgroup} -n ${output_filename}_rooted
						echo -e "Thorough outgroup insertion DONE."
						break
						;;
				esac
			done

		else
			echo "input file error in `basename $i`: input file should be a FASTA or relaxed or interleaved PHYLIP file format"
			continue
		fi
	done
}
