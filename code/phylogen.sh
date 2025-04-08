#!/bin/bash

#"""this script has fuctions for phylogenetic inferencing"""


co1_path=~/bioinformatics/github/co1_metaanalysis/
bmge_path=${co1_path}code/tools/BMGE-1.12/
raxmlng_mpi=${co1_path}code/tools/raxml-ng/bin/raxml-ng-mpi
fasttree_dest=${co1_path}data/output/phylogen/fasttree_output/
raxml_dest=${co1_path}data/output/phylogen/raxml_output/
raxmlng_dest=${co1_path}data/output/phylogen/raxmlng_output/
iqtree_dest=${co1_path}data/output/phylogen/iqtree_output/
PYTHON_EXEC=$( which python ) 
EPA_ng=${co1_path}code/tools/epa-ng/bin/epa-ng

source ${co1_path}code/process_all_input_files.sh


realpath() { #
	${PYTHON_EXEC} -c "import os,sys; print(os.path.realpath(sys.argv[1]))" $1
}

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
                elif [[ ( -f $i ) && ( `basename $i` =~ .*\.(aln|afa|fasta|fa)$ ) ]]
                then
                        rename
                        echo -e "\nproceeding with file `basename $i`..."
			sed -i "s/\[//g; s/\]//g; s/ /_/g; s/://g; s/;//g; s/,/__/g; s/(/__/g; s/)//g; s/'//g; s/|/-.-/g; s/\//_/g" $i
			fasttree -gtr -gamma -nt $i > ${fasttree_dest}${output_filename}.tree |& tee -a ${fasttree_dest}aligned/${output_filename}.log
			if [ $? -eq 0 ]
			then
				echo -e "\nDONE. The ouput phylogenetic tree file has been stored in ${fasttree_dest}${output_filename}.tree"
			fi
                else
                        echo "input file error in `basename $i`: input file should be a .fasta file format"
                        continue
                fi
        done

}

#=====================================================================================
#Handling Gappy sites and entropy using BMGe
#Syntax $ java -Xmx2048m -jar BMGE-1.12/BMGE.jar -i enafroCOI_all_clean_sN10-eN10.aln -t DNA -g 0.95 -b 1 -h 0:1 -of without_gappy.fasta : the actual run
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
			rename
			regexg='^(1|0\.[0-9][1-9])$'
			unset g_rate

			echo -e "\n\tproceeding with file `basename $i`..."
			echo -e "\n\tKindly enter the accepted maximum degree of gap rates parcentage in decimal values between 0.01 and 1, recommended is 0.95, default is 0.05;"
			until [[ "$g_rate" =~ $regexg ]]
			do
				read -p "Please enter the muximum gap rate per site in up to two decimal places: " g_rate
			done

			concatenate_fasta_seqs $i
			g_ratee=`echo $g_rate | sed 's/0\.//g'`
			java -Xmx2048m -jar ${bmge_path}BMGE.jar -i ${i} -t DNA -g ${g_rate} -b 1 -h 0:1 -of ${input_src}/${output_filename}_${g_ratee}g.${filename_ext}
		else
			echo "input file error in `basename $i`: input file should be a .(aln|afa|fasta|fa) file format and extension"
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
		elif [[ ( -f $i ) && ( `basename $i` =~ .*\.(aln|afa|fasta|fa)$ ) ]]
		then
			rename

			#Substituting illegal characters
			sed -i "s/\[//g; s/\]//g; s/ /_/g; s/://g; s/;//g; s/,/__/g; s/(/__/g; s/)//g; s/'//g" $i

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

			#Setting up the output directory
			unset dest
			dest=${raxml_dest}${output_filename}
			until [[ -d ${dest} ]]
			do
				echo "creating output directory '${dest}'..."
				mkdir ${dest}
			done

			#Setting the rate heterogeneity model
			unset rate_heterogeneity
			echo -e "Please select the rate heterogeneity model or approximation to use from the following options, enter [1] or [2]:"
			select rate_heterogeneity in GTRGAMMA GTRCAT
			do
	
				#Maximum Likelihood tree search
				echo -e "\nFinding the Best-Known Likelihood tree on `basename $i` MSA...\nSelect the number of ML searches to be conducted for best scoring tree..."
				select MLtree_searches in 10 20 100 200
				do
					echo -e "\nProceeding with file `basename $i`...\nFinding the best-scoring ML tree for the DNA alignment.."
					raxmlHPC-HYBRID-AVX2 -f d -T 2 -m ${rate_heterogeneity} -p 12345 -# ${MLtree_searches} -s $i -w ${dest} -n ${output_filename}
					break
				done
				echo -e "\nBest-scoring ML tree search DONE...\nThe best scoring ML tree written to ${dest}RAxML_bestTree.${output_filename}\n\nProceeding with bootsrap search..."
				
				#Bootsrap search
				echo -e "Please select the number of bootstrap_runs or selection method from the following options:"
				select bootstrap_option in autoMRE autoMRE_ING 20 100 1000
				do
					echo -e "\nBegining bootstrap search, This may take a while...\n"
					raxmlHPC-HYBRID-AVX2 -f d -T 2 -m ${rate_heterogeneity} -p 12345 -b 12345 -# ${bootstrap_option} -s $i -w ${dest} -n ${output_filename}1
					break
				done
				echo -e "\nBootstrap search DONE..."
				
				#Obtaining Confidence Values
				echo -e "\nProceeding with drawing bipartitions on the best ML tree to obtain a topology with support values...\n"
				
				raxmlHPC-AVX2 -m ${rate_heterogeneity} -p 12345 -f b -t ${dest}RAxML_bestTree.${output_filename} -z ${dest}RAxML_bootstrap.${output_filename}1 -w ${dest} -n ${output_filename}_BS_tree
				
				#Computing Extended Majority Rule (MRE) Consensus tree
				echo -e "\nProceeding to build a consensus tree...\n"
				#Generating consensus tree
				raxmlHPC-AVX2 -m ${rate_heterogeneity} -J MRE -z ${dest}RAxML_bootstrap.${output_filename}1 -w ${dest} -n ${output_filename}_MRE-CONS
				if [ $? -eq 0 ]
				then
					echo -e "DONE generating the consensus tree"
				fi
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
		elif [[ ( -f $i ) && ( `basename $i` =~ .*\.(aln|afa|fasta|fa)$ ) ]]
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

			#Substituting Illegal characters in taxon-names are: tabulators, carriage returns, spaces, ":", ",", ")", "(", ";", "]", "[", "'" that affect the interpretation in RAxML
			sed -i "s/\[//g; s/\]//g; s/ /_/g; s/://g; s/;//g; s/,/__/g; s/(/__/g; s/)//g; s/'//g" $i
		
			#Setting up the output directory
			unset dest
			dest=${raxml_dest}${output_filename}
			until [[ -d ${dest} ]]
			do
				echo "creating output directory '${dest}'..."
				mkdir ${dest}
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
					raxmlHPC-HYBRID-AVX2 -f a -T 2 -m ${rate_heterogeneity} -p 12345 -x 12345 -# ${bootstrap_option} -s $i -w ${dest} -n ${output_filename}
					##raxmlHPC-HYBRID-AVX2 -f a -T 2 -m GTRGAMMA -p 12345 -x 12345 -# ${bootstrap_option}­-s $i -w ${dest} -n ${output_filename}
					break
				done
				break
			done
			if [ $? -eq 0 ]
			then
				echo -e "\nComplete analysis done"
			fi
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
# For binary model files, the models of rate heterogeneity must be the same. And when using GTRCAT, disable pattern compression using -H flag in both runs.

raxml_rooting(){ # This funtion generates a model filewith model parameters for reference tree then places the outgroup/s in the tree
	usage $@
	echo "RAxML starting Phylogenetic tree inference..."

	for i in $@
	do
		if [ ! -f $i ]
		then
			echo "input error: file $i is non-existent!"
		elif [[ ( -f $i ) && ( `basename $i` =~ .*\.(aln|afa|fasta|fa)$ ) ]]
		then
 			rename
			input_src=`dirname "$( realpath "${i}" )"`

			#Substituting illigal characters
			sed -i "s/\[//g; s/\]//g; s/ /_/g; s/://g; s/;//g; s/,/__/g; s/(/__/g; s/)//g; s/'//g" $i

			#Setting up the output directory
			unset dest
			dest=${raxml_dest}${output_filename}
			until [[ -d ${dest} ]]
			do
				echo "creating output directory '${dest}'..."
				mkdir ${dest}
			done

			#Setting the rate of heterogeneitu model
			unset rate_heterogeneity
			echo -e "Please select the rate heterogeneity model or approximation to use from the following options, enter [1] or [2], It should be exactly the same as the one used in tree inference:"
			select rate_heterogeneity in GTRGAMMA GTRCAT
			do
				if [ ${rate_heterogeneity} = "GTRCAT" ]; then rate_heterogeneity="GTRCAT -H"; fi
				unset tree
				unset outgroup
				echo -e "\nBeginning rerooting of the tree. Please enter the path to the tree file to be  rerooted..."
				read -p "Please enter the phylogenetic tree::  " tree
				
				#Generating reference tree model parameters binary file
				echo -e "\nBeginning rooting based on ${rate_heterogeneity} rate heterogeneity model...\nGenerating reference tree model parameters binary file..."
				raxmlHPC-AVX2 -f e -m ${rate_heterogeneity} -s ${i} -t ${tree} -w ${dest} -n ${output_filename}_PARAMS
				echo -e "\nBinary files generation DONE.\nProceeding with outgroup insertion...\nPlease enter the path to the file containing the outgroup, it should be a single sequence/record..."
				read -p "Please enter the outgroup file:: " outgroup
				#appending the outgroup to the alignment file
				cp ${i} ${input_src}/rooting_inputfile.fas
				cat ${outgroup} >> ${input_src}/rooting_inputfile.fas
				sed -i '/^[[:space:]]*$/d' ${input_src}/rooting_inputfile.fas
				#Rooting - ougroup insertiion in the tree
				raxmlHPC-AVX2 -f v -m ${rate_heterogeneity} -R ${dest}RAxML_binaryModelParameters.${output_filename}_PARAMS -t ${dest}RAxML_results.${output_filename}_PARAMS -s ${input_src}/rooting_inputfile.fas -w ${dest} -n ${output_filename}_rooted
				if [ $? -eq 0 ]
				then
					echo -e "Thorough outgroup insertion DONE."
				fi
			done

		else
			echo "input file error in `basename $i`: input file should be a FASTA or relaxed or interleaved PHYLIP file format"
			continue
		fi
	done
}


#==================================================================================================================================================================================================================

#Full ML-bootstrapping analysis:
#syntax: raxml-ng -msa FILE --all --bs-trees [value] 

raxmlng_phylo(){ # This function is based on RAxML-NG v. 0.9.0 released on 20.05.2019 by The Exelixis Lab. Supports faster bootstraping and perform all-in-one analysis: ML-tree search, non-parametric bootstrap:
        usage $@
        echo "RAxML-ng MPI mode starting Phylogenetic tree inference..."

        for i in $@
        do
                if [ ! -f $i ]
                then
                        echo "input error: file $i is non-existent!"
                elif [[ ( -f $i ) && ( `basename $i` =~ .*\.(aln|afa|fasta|fa)$ ) ]]
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
			
			#Setting up the output directory
			unset dest
			dest=${raxmlng_dest}${output_filename}
			until [[ -d ${dest} ]]
			do
				echo "creating output directory '${dest}'..."
				mkdir ${dest}
			done

                        #Substituting Illegal characters in taxon-names are: tabulators, carriage returns, spaces, ":", ",", ")", "(", ";", "]", "[", "'" that affect the interpretation in RAxML
                        sed -i "s/\[//g; s/\]//g; s/ /_/g; s/://g; s/;//g; s/,/__/g; s/(/__/g; s/)//g; s/'//g" $i

			#Setting the evolutionary model: [Substitution matrix]+[Propotions of invariant sites]+[rate heterogenaity model]
			unset evolutionary_model
			regexp='^^[a-zA-Z0-9/+]+$'
			until [[ "$evolutionary_model" =~ $regexp ]]
			do
				read -p "Please enter the evolutionary model [Substitution matrix]+[Propotions of invariant sites]+[rate heterogenaity model] to be used. Use modeltest to select 'modeltest -i [inputfile] -T raxml-ng': " evolutionary_model
			done

			#Working with large files
			if [ $( grep ">" ${i} | wc -l) -gt 200 ]
			then
				raxml-ng --parse --msa ${i} --model ${evolutionary_model} --prefix ${dest}/${output_filename}
				i=${dest}/${output_filename}.raxml.rba
			fi

			#Setting number of threads to use
			unset num_cpus
			regexp='^[0-9][1-9]*$'
			until [[ "$num_cpus" =~ $regexp ]]
			do
				read -p "Please enter the number of cpus to be used, RAxML-NG works best with one thread per physical core: " num_cpus
			done


                        #Full analysis rapid Bootstrapping and Maximum Likelihood search
                        echo -e "Please select the number of bootstrap_runs or selection method from the following options:"
                        select bootstrap_option in autoMRE 20 100 1000
                        do
				#Setting the number of starting trees
				unset st_trees
				regexp='^[0-9]+$'
				until [[ "$st_trees" =~ $regexp ]]
				do
					read -p "Please enter the number of strarting trees to generate before the best-scoring topology as the final ML-tree is picked, the : " st_trees
				done

				#Setting the branch support metric algorithm
				unset branch_support_metric
				echo -e "Please select a suitable branch support metric: Transfer Bootstrap Expectation (Lemoine et al., Nature 2018) is a recent alternative to the classical Felsenstein's bootstrap and can better reveal support for deep branches "
				select branch_support_metric in FBE TBE
				do
					echo -e "\nBegining complete analysis (ML search + Bootstrapping) for `basename $i` in one step..."
					raxml-ng-mpi --all --threads ${num_cpus} --model ${evolutionary_model} --tree pars{${st_trees}},rand{${st_trees}} --seed 12345 --bs-trees ${bootstrap_option} -bs-metric ${branch_support_metric} --msa $i --prefix ${dest}/${output_filename}
					if [ $? -eq 0 ]
					then
						echo -e "\nAnalysis Done!!!Congratulations"
					fi
					break
				done
				break
			done
                else
                        echo "input file error in `basename $i`: input file should be a FASTA or relaxed or interleaved PHYLIP file format"
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
			rename

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

#===============================================================================================================================================================#Phylogenetic placement using EPA-en (Evolutionary Placement Algorithm - ng)

# epa-ng --ref-msa $REF_MSA --tree $TREE --query $QRY_MSA --model $MODEL --outdir $output_dir --threads $CPUs

# the model can be the "filename.raxml.bestModel" file in raxml-ng or "RAxML_info.filename" in raxml8.x, OR model parameter descriptor from raxml-ng, iqtree or Raxml8.X

epang-rooting() { #Rooting using EPA-NG algorithm
	if [ $# -eq 0 ]
	then
		echo "Input error..."
		echo "Usage: ${FUNCNAME[0]} -s <refrence_aln> -t <tree> -q <query aln>"
		return 1
	fi

	unset REF_MSA
	unset TREE
	unset QUERY
	unset MODEL
	unset num_cpus

	local OPTIND=1
	while getopts 's:t:q:' key
	do
		case "${key}" in
			s)
				if [ ! -f $OPTARG ]
				then
					echo "input error: file $OPTARG is non-existent!"
				elif [[ ( -f $OPTARG ) && ( `basename $OPTARG` =~ .*\.(aln|afa|fasta|fa|fst)$ ) ]]
				then
					REF_MSA=$OPTARG
				fi
				;;
			t)
				if [ ! -f $OPTARG ]
				then
					echo "input error: file $OPTARG is non-existent!"
				elif [[ ( -f $OPTARG ) && ( `basename $OPTARG` =~ .*\.(tree|tre|newick|.*) ) ]]
				then
					TREE=$OPTARG
				fi
				;;
			q)
				if [ ! -f $OPTARG ]
				then
					echo "input error: file $OPTARG is non-existent!"
				elif [[ ( -f $OPTARG ) && ( `basename $OPTARG` =~ .*\.(aln|afa|fasta|fa|fst)$ ) ]]
				then
					QUERY=$OPTARG
				fi
				;;
			?)
				echo "Input error..."
				echo "Usage: ${FUNCNAME[0]} -s <refrence_aln> -t <tree> -q <query aln>"
				return 1
				;;
		esac
	done

	echo -e"Lets proceed with placement of $QUERY in $REF_MSA..."
	input_src=`dirname "$( realpath "${TREE}" )"`

	regexp='^[0-9][1-9]*$'
	regexp1="^[(JC)|(GTR)|(HKY)|(SYM)|(K3Pu?)|(TVMe?)|(TNe?)|(TPM2?3?u?)].*$"
	until [[ ( ( -f "$MODEL" ) && ( `basename -- "$MODEL"` =~ .*\.(tre) ) ) || ( "$MODEL" =~ $regexp1 ) ]]
	do
		read -p "Please enter substitution model parameters or file to used in tree inference:: " MODEL
	done

	until [[ "$num_cpus" =~ $regexp ]]
	do
		read -p "Please enter the number of cpus to be used: " num_cpus
	done

	echo -e "\n\tProceeding with operation..."
	${EPA_ng} --ref-msa $REF_MSA --tree $TREE --query $QUERY --model $MODEL --outdir $input_src --threads $num_cpus

	if [ $? -eq 0 ]
	then
		echo -e "\n\tDONE. The output files has been stored in ${input_src}/"
	fi

}

#===============================================================================================================================================================
#Execute ML species delimitation with single coalescent rate over all species
#	$ mptp --tree_file tree_filename --output_file output_filename --ml --single --minbr 0.0009330519
# Execute a simple ML species delimitation inference, assuming a different coalescent rate for each delimited species. 
#	$ mptp --tree_file tree_filename --output_file output_filename --mcmc 1000000 --multi --minbr 0.0009330519[default 0.0001]

#Input data: 	--tree_file tree_filename
#output:	--output_file output_filename
#[--minbr minimum-branch-length(0.0001)] - The minimum branch length. 
#[--minbr_auto fasta_alingment] - To automatically detect minimum branch length. A fasta formated alignment should be provided. The method identifies the subtrees that consist of sequences with zero p-distances and returns the minimum branch length value that should then be used with the --min_br command for either a ML or an MCMC delimitation analysis
#[--outgroup taxon1,taxon2,..] # Specify the outgroup
#[--outgroup_crop taxon1,taxon2,..] # Specifythe outgroup and removes it from final phylogeny
#	$mptp --tree_file tree_filename --minbr_auto fasta_filename --output_file output_filename [--outgroup taxon1,taxon2,..|--outgroup_crop taxon1,taxon2,..]

#Uses Markov Chain Monte Carlo sampling method to assess confidence of the ML delimitation scheme.
# --mcmc 1000000(steps) - Activate MCMC. Followed by "--single" or "--multi"
# --mcmc_sample 100(sampling-frequency) - set the frequency of MCMC sampling
# --mcmc_runs 2(runs) - number of independent MCMC analyses runs.
# [--mcmc_startrandom|--mcmc_startml|--mcmc_startnull] - starting point of MCMC
# --mcmc_burnin N - number of mcmc steps to ignore before threshold

# $ mptp --tree_file tree_filename --minbr 0.0009330519 --output_file output_filename --mcmc 1000000 --multi [--mcmc_burnin INT(1000000)] --mcmc_sample 100 [--mcmc_startrandom|--mcmc_startml|--mcmc_startnull] --mcmc_runs 2

mptp_SpDelim() { # This function performs putative species delimitation from a binary rooted phylogenetic trees. The trees can be from a Maximum Likelihood (e.g. RAxML, PhyML) or Bayesian Inference (e.g. MrBayes, ExaBayes) approach. It runs the multi-rate PTP (mPTP) model, an improved model to Poisson Tree Processes (PTP; Zhang et al., 2013).
	if [ $# -eq 0 ]
	then
		echo "Input error..."
		echo "Usage: ${FUNCNAME[0]} -t <tree> [-s <refrence_aln> ] [-r]"
		return 1
	fi

	unset TREE
	unset REF_MSA
	unset ROOTED
	unset MINBR
	unset OUTGROUP

	local OPTIND=1
	while getopts 't:rs::' key
	do
		case "${key}" in
			t)
				if [ ! -f $OPTARG ]
				then
					echo -e "\tinput error: file $OPTARG is non-existent!"
				elif [[ ( -f $OPTARG ) && ( `basename -- $OPTARG` =~ .*\.(tree|tre|newick|.*) ) ]]
				then
					TREE=$OPTARG
				fi
				;;
			r)
				echo -e "\tProvided tree $TREE is taken as rooted and if not so, the first taxon will be taken as the root"
				ROOTED="TRUE"
				;;
			s)
				if [ ! -f $OPTARG ]
				then
					echo -e "\tinput error: file $OPTARG is non-existent!"
				elif [[ ( -f $OPTARG ) && ( `basename -- $OPTARG` =~ .*\.(aln|afa|fasta|fa|fst)$ ) ]]
				then
					REF_MSA=$OPTARG
				fi
				;;
			?)
				echo "Input error..."
				echo "Usage: ${FUNCNAME[0]} -t <tree> [-r] [-s <refrence_aln>]"
				return 1
				;;
		esac
	done

	echo -e "\tLets proceed with ML delimitation of $TREE..."
	i=$TREE
	rename
	input_src=`dirname "$( realpath "${TREE}" )"`

	until [ "$ROOTED" = "TRUE" ]
	do
		unset OUTGROUP
		regexp1='^[a-zA-Z0-9/_-\ \|,\.]+$'
		until [[ "$OUTGROUP" =~ $regexp1 ]]
		do
			read -p "Please enter one or a comma-separated list of outgroup taxa (taxa1,taxa2,..) and ensure the alignment file [-s <refrence_aln>] is provided:: " OUTGROUP
		done
		mptp --tree_file $TREE --minbr_auto $REF_MSA --output_file ${input_src}/${output_filename} --outgroup $OUTGROUP #--outgroup_crop
		#? rooted tree
		#? minbr
		if [ $? -eq 0 ]
		then
			ROOTED="TRUE"
			echo -e "\n\t$TREE has been rooted"
		else
			echo -e "\n\tERROR!!! $TREE has not been rooted, please check the outgroup given to confirm that it is spelled correctly"
			#return 1
		fi
	done
	
	until [ ! -z $MINBR ]
	do
		echo -e "\n\tPhylogenetic trees are given much smaller non-zero branch-lengths for identical sequences in order to retain the binary shape. Providing them to mptp enables it to determine multifacation. To set or estimate the minimum branch length threshold select one of the options below::"
		options[0]="Input the minimum branch length threshold, the default is 0.0001"
		options[1]="Automatically detect the correct the minimum branch length from the alignment file. Ensure you provided a FASTA format alinment(-s)"
		options[2]="Exit"
		PS3='Select option [1|2] to proceed, or [3] to exit: '
		select option in "${options[@]}"
		do
			regexp='^0.[0-9]+$'
			case $option in
				${options[0]})
					until [[ "$MINBR" =~ $regexp ]]
					do
						read -p "Please enter the miminum branch length (enter 0.0001 for the default):: " MINBR
					done
					break
					;;
				${options[1]})
					echo -e "\n\tProceeding with estimation of minimum branch length threshold if $TREE tree from $REF_MSA sequences"
					mptp --tree_file $TREE --minbr_auto $REF_MSA --output_file ${input_src}/${output_filename}
					if [ $? -eq 0 ]
					then
						echo -e "\n\tMinimum branch length estimated"
					else
						return 1
					fi
					read -p "Please enter the miminum branch length from operation above (enter 0.0001 for the default):: " MINBR
					#MINBR=""
					break
					;;
				${options[2]})
					echo -e "\n\tExiting rooting of $TREE..."
					break
					;;
				*)
					echo -e "\n\tINVALID choice $REPLY"
					;;
			esac
		done
	done
	echo -e "\n\tLets proceed with ML delimitation of $TREE..."
	
	unset MCMC
	unset BURNIN
	unset SAMPLE
	unset RUNS
	regexp2='^[0-9]+$'
	echo -e	"\tEnter Markov Chain Monte Carlo (mcmc) parameters below."
	until [[ ( "$MCMC" =~ $regexp2 ) && ( "$BURNIN" =~ $regexp2 ) && ( "$SAMPLE" =~ $regexp2 ) && ( "$RUNS" =~ $regexp2 ) ]]
	do
		read -p "Please enter the number of mcmc steps (eg 1000000):: " MCMC
		read -p "Please enter the number of mcmc burnin steps below threshold to ignore (eg 1000000):: " BURNIN
		read -p "Please enter the frequency of mcmc sampling (eg 1000):: " SAMPLE
		read -p "Please enter the number of independent mcmc runs (eg 2):: " RUNS
	done

	mptp --tree_file $TREE --minbr $MINBR --output_file ${input_src}/${output_filename} --mcmc $MCMC --multi --mcmc_burnin $BURNIN --mcmc_sample $SAMPLE --mcmc_startrandom --mcmc_runs $RUNS
 	if [ $? -eq 0 ]
	then
		echo -e "\n\tDONE... ML species delimitation is finished and files output stored in ${input_src}/${output_filename}"
	fi	

}
