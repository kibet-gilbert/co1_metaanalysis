#!/bin/bash

#"""this script has fuctions for phylogenetic inferencing"""


co1_path=~/bioinformatics/github/co1_metaanalysis/
fasttree_dest=${co1_path}data/output/phylogen/fasttree_output/
raxml_dest=${co1_path}data/output/phylogen/raxml_output/

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

#=====================================================================================
#Using FastTree to infer a tree for a nucleotide alignment with the GTR+CAT model, use
#syntax:	FastTree -gtr -gamma -nt alignment_file > tree_file 

#Input formats: FastTree reads multiple sequence alignments in fasta format or in interleaved phylip format.
#Output formats: FastTree outputs trees in Newick format.



phylo_fastree() { #
	usage $@
        echo "fasttree starting alinment..."

        for i in $@
        do
                if [ ! -f $i ]
                then
                        echo "input error: file $i is non-existent!"
                elif [[ ( -f $i ) && ( `basename $i` =~ .*\.(afa|fasta|fa) ) ]]
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

# Using RAxML (Randomized Axelerated Maximum Likelihood) to infer a phylogenetic tree from MSA nucleotide sequences alignment.

#Syntax:	raxmlHPC -m GTRGAMMA -p 12345 -s dna.phy -# 20 -n T6 

# args:		:raxmlHPC - calls raxml program
#		:-m -defines the model of evolution GTR and the variable evolutionary rate per site approximation CAT or model GAMMA
#		:-n -The file name appendix passed via -n is arbitrary, for this run all output files will be named RAxML_FileTypeName.T1 for instance.
#		:-p 12345 -forces starting trees by providing a fixed random number seed (RAxML uses randomized stepwise addition parsimony trees, i.e., it will not generate the same starting tree every time) OR -t startingTree.txt -to pass a starting tree OR -p 12345 -s dna.phy -# 20 -to conduct multiple searches for the best tree.
#		:-s dna.phy - defines the input sequences OR -s protein.phy OR -s binary.phy

#To get the support values for the tree a boostrap search needs to be conducted.

#syntax:	raxmlHPC -m GTRGAMMA -p 12345 -b 12345 -# 100 -s dna.phy -n T14 

#args		:-b 12345 -# 100 -provides a bootstrap random number seed via -b 12345 and the number of bootstrap replicates we want to compute via -# 100. RAxML also allows for automatically determining a sufficient number of bootstrap replicates, in this case you would replace -# 100 by one of the bootstrap convergence criteria -# autoFC, -# autoMRE, -# autoMR, -# autoMRE_IGN. The computed bootstrap replicate trees will be printed to a file called RAxML_bootstrap.T14.
#We can now use them to draw bipartitions on the best ML tree as follows: 
#	raxmlHPC -m GTRCAT -p 12345 -f b -t RAxML_bestTree.T13 -z RAxML_bootstrap.T14 -n T15.
#	This call will produce to output files that can be visualized with Dendroscope:  RAxML_bipartitions.T15 (support values assigned to nodes) and RAxML_bipartitionsBranchLabels.T15 (support values assigned to branches of the tree).
#Note that, for unrooted trees the correct representation is actually the one with support values assigned to branches and not nodes of the tree!

#Rapid Bootstrapping: use -x instead of -b to provide a bootstrap random number seed
#	raxmlHPC -m GTRGAMMA -p 12345 -x 12345 -# 100 -s dna.phy -n T19 

#allows you to do a complete analysis (ML search + Bootstrapping) in one single step. will do 100 rapid Bootstrap searches, 20 ML searches and return the best ML tree with support values to you via one single program call. By typing:
#	raxmlHPC -f a -m GTRGAMMA -p 12345 -x 12345 -# 100 -s dna.phy -n T20 

