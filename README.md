# co1_metaanalysis

## Preview
The co1_metaanalysis is a Master's of science in Molecular Biology and Bioinformatics project, Department of Biochemistry, [Jomo Kenyatta University of Agriculture and Technology(Jkuat)](http://www.jkuat.ac.ke/)

It is conducted at the **[international centre for insect physiology and ecology(_icipe_)](http://www.icipe.org/)**  based on data from [BOLD Systems](http://www.boldsystems.org/)

This project is conducted with the supervision and support of:
1. [Dr Scott Miller](https://entomology.si.edu/StaffPages/MillerS.html)
2. [Dr Jandouwe Villinger](http://www.icipe.org/about/staff/jandouwe-villinger)
3. [Dr Steven Ger(JKUAT)](https://scholar.google.com/citations?user=Qdp8yCsAAAAJ&hl=en)
4. [Dr Caleb Kipkurui Kibet](https://github.com/kipkurui)
5. [Dr Jean-Baka Domelevo Entfellner](https://github.com/jean-baka)
6. [Dr Dan Masiga](http://www.icipe.org/about/staff/daniel-masiga)

## Aim of the project

The aim of the study is to determine the phylogenetic diversity and phylogeographic distribution of voucher arthropods within East Africa and provide a detailed reference for future research on arthropods in the region.

Further analyses focus on identification of potential disease vector species among biting flies (Order Diptera) and crop pest species such as fruit flies (family Tephriditae) that have not yet been implicated with but may have an impact on human, animal and crop health.

## Data acquisition
The test data to COI barcode sequences and metadata are exported from the [boldsystems public data portal](http://www.boldsystems.org/index.php/Public_BINSearch?searchtype=records) using the following search syntax (criteria) in the search box:
```
arthropoda Kenya Uganda Tanzania Rwanda Burundi Ethiopia "South Sudan"
```
 This results in [over 36000 published records](http://www.boldsystems.org/index.php/Public_SearchTerms) from East Africa.
**With the following summary (NB: as of Mon Nov 12, 2018, 14:58:44):**

>_Found 36,436 published records, with 36,436 records with sequences, forming 10,210 BINs (clusters), with specimens from 6 countries, deposited in 126 institutions._
>_Of these records, 8,865 have species names, and represent 2,241 species._

To quickly download the latest of data set from East Africa in .tsv format click [**API link**](http://www.boldsystems.org/index.php/API_Public/combined?geo=Kenya|Uganda|Tanzania|Rwanda|Burundi|Ethiopia|"South%20Sudan"&taxon=arthropoda&format=tsv) (warning!!! Clicking thislink will automatically start downloading data)

Other datasets that have not been shared publicly will be sought from individual Project Managers (_The list will be provided soon_).
  1. DS-KENFRUIT dataset (1427) managed by Dr. Scott Miller (Kenya (1410), Madagascar (10), Nigeria (5), Burkina Faso (1), United States (1))
  2. DS-MPALALEP dataset (2472) managed by Dr. Scott miller (All kenyan)
  3. IDRCK IDRC kenya dataset (1704) managed by Dr. Daniel Masiga

To download the COI-5P datasets from countries listed in the file `countries`, we used the `bolddata_retrival` function in the `process_all_input_files.sh` script. The downloaded data are in .XML format files.

## Workflow.
![Workflow](https://github.com/kibet-gilbert/co1_metaanalysis/blob/master/workflow.png)

### Programming languages:
1. R : tidyverse, r-essentials, rgbif, diveRsity, finePOP
2. Bash (and awk)
3. Python (Python2, Python3 and ipython): BeautifulSoup4, lxml, pandas

### Data Retrival.
Data used in this project are retrieved from three major puplicly accessible databases:
1. [Barcode of Life Datasystems](http://www.boldsystems.org/) via [the URL generator API](http://www.boldsystems.org/index.php/api_urlgenerator/)
2. [Global Biodiversity Information Facility](https://www.gbif.org) via its GBIF API using [the rgbif R package](https://www.gbif.org/tool/81747/rgbif) to search and retrieve data. For an overview of rgbif functions go to [reference](https://docs.ropensci.org/rgbif/reference/index.html): an overview of all rgbif functions.
3. [GenBank Nucleotide Database](https://www.ncbi.nlm.nih.gov/genbank/) using the [Entrez Direct package](https://www.ncbi.nlm.nih.gov/books/NBK179288/) to access [Entrez Programming Utilities (E-utilities)](https://www.ncbi.nlm.nih.gov/books/NBK25501/) the pubplic API to the NCBI entrez system

Bash fuctions have been written to simplify usage of this API packages, namely: `bolddata_retrival`, `gbifdata_retrival` and `genbankdata_retrival`
### Classification of sequences.
On avarage, less than 20% of all downloaded sequences have species taxonomic identities and a small fraction of this are misclassified. Therefore to give species identities to the sequences and assess the accuracy of the identities for those with species taxon classification, a classification analysis is done. The tool used is:
1. [**Ribosomal Database Project classifier (RDPClassifier)**](https://github.com/rdpstaff/classifier), a naive Bayasian classifier originally developed to do rapid taxonomic placement of 16S rRNA sequences as descriped by [Cole et al.](https://doi.org/10.1093/nar/gkt1244), is used to  assign taxon names to CO1 metabarcode sequences that have missing taxa values. It has been trained to species rank using [CO1 Eukaryote v3.2 training set](https://github.com/terrimporter/CO1Classifier/releases/tag/v3.2) as described by [Porter and Hajibabaei](http://dx.doi.org/10.1146/annurev-ecolsys-102209-144621)

### Multiple Sequence Alignment tools.
1. [**Muscle.**](http://www.drive5.com/muscle/)
It is problematic to align large number of sequences using global alignment algorithims used by muscle as explained in [Very large alignments are usually a bad idea](http://www.drive5.com/muscle/manual/bigalignments.html). Clustering highly identical (95% or 90% identity) help reduce the the sequences and challanges faced.
2. [**T_Coffee**](https://github.com/cbcrg/tcoffee). [The regressive mode of T-Coffee](https://github.com/cbcrg/tcoffee/blob/master/docs/tcoffee_quickstart_regressive.rst) is [described as most suitable for large datasets](https://www.biorxiv.org/content/10.1101/490235v1.full) by E. G. Nogales et. al (2018). Our assesment proved that it is relatively accurate compared to MUSCLE but relatively slow compared to PASTA
3. [**MAFFT Version 7**](https://mafft.cbrc.jp/alignment/software/). For large datasets: [Tips for handling a large dataset](https://mafft.cbrc.jp/alignment/software/tips.html). More published by [T. Nakamura et. al (2018)](https://academic.oup.com/bioinformatics/article/34/14/2490/4916099). This tool is reliably accurate but very slow
4. [SATé](https://github.com/sate-dev/sate-core) for [sate-tools-linux](https://github.com/sate-dev/sate-tools-linux)
5. [**PASTA**](https://github.com/smirarab/pasta) [(Tutorial)](https://github.com/smirarab/pasta/blob/master/pasta-doc/pasta-tutorial.md)
6. Other tools; [SEPP](https://github.com/smirarab/sepp), [UPP](https://github.com/smirarab/sepp/blob/master/README.UPP.md) and [HMMER](http://hmmer.org/)
7. Visualized the multiple sequence alignments using [**jalview**](http://www.jalview.org/download) for datasets upto a few thousands and [**Seaview**](http://doua.prabi.fr/software/seaview) by [Gouy et al.](https://academic.oup.com/mbe/article/27/2/221/970247) for bigger data sets. Seaview uses the FLTK project (installed separately) for its user interface.

#### MSA evaluation methods used in T_Coffee:
We used the following [sequence based methods](https://tcoffee.readthedocs.io/en/latest/tcoffee_main_documentation.html#sequence-based-methods) to evaluate our MSAs:
1. [**Computing the CORE index of any alignment**](https://tcoffee.readthedocs.io/en/latest/tcoffee_main_documentation.html#computing-the-local-core-index).
2. Evaluating the [**Transitive Consistency Score (TCS)**](https://tcoffee.readthedocs.io/en/latest/tcoffee_main_documentation.html#transitive-consistency-score-tcs) of an MSA. The scores generated here are usefull in filtering our sequences and in phylogenetic inference based on herogenous site evolutionary rates.

### Phylogenetic Inference tools.
1. The substitution model used can be selected using [**ModelTest-NG**](https://github.com/ddarriba/modeltest) described by [Darriba et.al](https://doi.org/10.1093/molbev/msz189).
2. [**BMGE (Block Mapping and Gathering with Entropy)**](ftp://ftp.pasteur.fr/pub/gensoft/projects/BMGE/): A tool for selection of phylogenetic informative regions from MSAs as documented [**HERE**]( http://gensoft.pasteur.fr/docs/BMGE/1.12/BMGE_doc.pdf). Removes ambiguously aligned regions, highly variable (saturated) characters altimately improving overall performance of the phylogenetic reconstruction  method as described by [Criscuolo et. al. (2010)](https://doi.org/10.1186/1471-2148-10-210)
3. [**RAxML (Randomized Axelerated Maximum Likelihood)**](https://cme.h-its.org/exelixis/web/software/raxml/index.html): Highly accurate, computer intensive but, a little slow. Removes duplicate sequences (Headers/nucleotide-sequences) then infers the tree. described in [Stamakis et. al. (2014)]( https://doi.org/10.1093/bioinformatics/btu033)
4.  [**RAxML-ng (RAxML Next Generation)**](https://github.com/amkozlov/raxml-ng): a fast, scalable and user-friendly tool for maximum likelihood phylogenetic inference. "A from-scratch re-implementation of the established greedy tree search algorithm of RAxML/ExaML" with "improved accuracy, flexibility, speed, scalability, and usability compared with RAxML/ExaML". Desribed by [Kozlov et. al. (2019)](https://doi.org/10.1093/bioinformatics/btz305) as having higher scoring trees than IQtree on "taxon-rich datasets"
5. [**IQtree**](http://www.iqtree.org/doc/): An "Efficient search algorithm: Fast and effective stochastic algorithm to reconstruct phylogenetic trees by maximum likelihood". Has a "Ultrafast" bootstrap algorithm 10-40 times faster than RAxML, "Ultafast model selection" algorithm (ModelFinder) that is automatic, 10-100 times faster than jModelTest and ProTest, support Bid Data Analysis and other attributes as descriped by [Nguyen et. al.](https://doi.org/10.1093/molbev/msu300).
6. [**FastTree**](http://www.microbesonline.org/fasttree/): Fast and less computer intensive, but not so accurate.

The resultant computer readable formats of the phylogenetic trees are either of the following [Newick](http://evolution.genetics.washington.edu/phylip/newicktree.html), [NEXUS](http://en.wikipedia.org/wiki/Nexus_file) and [PhyloXML](http://en.wikipedia.org/wiki/PhyloXML).

Programs to be used to visualize and edit phylogenetic trees:
1. [**Archaeopteryx**](http://www.phylosoft.org/archaeopteryx/), See Archaeopteryx documentation [here](https://sites.google.com/site/cmzmasek/home/software/archaeopteryx/documentation).
2. [**FigTree**](http://tree.bio.ed.ac.uk/software/figtree/).
3. Also attempted these other options: [**(i) Dendroscope**](http://ab.inf.uni-tuebingen.de/software/dendroscope/),
[(ii) Jstree](http://lh3lh3.users.sourceforge.net/jstree.shtml) or [(iii) PhyloWidget](http://www.phylowidget.org/) can be used.

#### Phylogenetic Tree Analysis Tools:
1. [**Evolutionary Placement Algorithm (EPA)**](https://academic.oup.com/sysbio/article/60/3/291/1667010): Used to root a tree (adding outgroups). Integrated into RAxML, it can classify a bunch of environmental sequences into a reference tree using thorough read insertions given a non-comprehensive reference tree and an alignment containing all sequences (reference + query)
2. [**Evolutionary Placement Algorithm -NG (EPA-NG)**](https://doi.org/10.1093/sysbio/syy054): "A complete reimplementation of EPA in RAxML-EPA. Faster, scalable (upto 2048 cores) and accomodate more parameters. Source code and documentation are in [GitHub](https://github.com/Pbdas/epa-ng) by [Barbera et. al]( https://doi.org/10.1093/sysbio/syy054)
3. [**Bio.Phylo**](https://doi.org/10.1186/1471-2105-13-209): Biopython's Phylo package as descriped in the [Phylo Cookbook](https://biopython.org/wiki/Phylo_cookbook) and [tutorial](http://biopython.org/DIST/docs/tutorial/Tutorial.html#htoc204), uses other Python packages; MatPlotlib and NetworkX. Can be used to study the tree and manipulate it's nodes

#### Species Delimitation:
1. [**Multi-rate Poisson Tree Processes (mPTP)**](https://doi.org/10.1093/bioinformatics/btx025): It computes putative species through maximum likelihood species delimitation using inferred phylogenetic trees. Uses a more accurate, faster and scalable multi-rate [“Poisson Tree Processes” (PTP) method](https://cme.h-its.org/exelixis/web/software/PTP/index.html). Also supports Markov Chain Monte Carlo sampling to compute support values for each OTU providing "a comprehensive evaluation of the inferred delimitation in just a few seconds for millions of steps, independently of tree size" [Kapli. P. et. al](https://doi.org/10.1093/bioinformatics/btx025). The [code is available GitHub](https://github.com/Pas-Kapli/mptp) and the [documentaion wiki here](https://github.com/Pas-Kapli/mptp/wiki).

### Population Structure.
#### Haplotype Network.
To elucidate the population structure, first a haplotype network needs to be infered. There are severally GUI tools: DNASP5/6 (Windows and MAC), [PopART (Windows, MAC and Linux)](http://popart.otago.ac.nz/documentation.shtml). But command line tools are limited and majority of those availavle are R packages. One is the haploNet fuction in the pegas R package.
##### pegas R package
[Population and Evolutionary Genetics Analysis System (pegas)](https://doi.org/10.1093/bioinformatics/btp696) is an [R package](https://cran.r-project.org/web/packages/pegas/index.html) that relies on [ape](https://cran.r-project.org/web/packages/ape/), [adegenet](https://cran.r-project.org/web/packages/adegenet/) and other dependencies.
The first step is to load and parse a FASTA file: the `ape::read.dna()` function of ape package is used here.
1. **pairwise distances of sequences**
It may be valueble to compute pairwise distances between the DNA sequnces using `ape::dist.dna()` function. This will generate a matrix of pairwise distances. The substitution model used can be selected using [ModelTest-NG](https://github.com/ddarriba/modeltest) described by [Darriba et.al](https://doi.org/10.1093/molbev/msz189).
2. **Haplotype Extraction and Frequencies**
The identification of haplotypes and friquencies is done using `pegas::haplotype()` function of the pegas R package.
3. **Haplotype Network**
This can be computed using `pegas::haploNet()` function and the plot is visualized using `plot()` function, with the legend displayed using `legend()` function.

#### Phylogeographic Differentiation
**Testing for phylogeographic differentiation**
Second is to test if there is any empirical semblance of phylogeographic differentiation using statisical tests. There are two options here too GUI based like PERMUTcpSSR (Windows ONLY) or command line based like diveRsity package in R. An alternative to diveRsity R is finePOP  R package by [Nakamichi et. al.]( https://doi.org/10.1111/1755-0998.12663) that is based on empirical Bayes estimation
##### diveRsity R package
[diveRsity](https://cran.r-project.org/web/packages/diveRsity/index.html): "An R package for the estimation and exploration of population genetics parameters and their associated errors" by [Keenan et al., 2013](https://doi.org/10.1111/2041-210X.12067).
Using `diveRsity::fastDivPart()` function one can calculate Jost's D (Jost, 2008) and Wright's FST (Weir & Cockerham, 1984) statistics to assess population differentiation using a number of permutation replicates (e.g 10,000). This function accepts only [`genpop` file format](https://genepop.curtin.edu.au/help_input.html) in either three or two digit format.

##### finePOP R package
[finePOP](https://cran.r-project.org/web/packages/FinePop/index.html) implements a more rigorous empirical bayes approach to estimate population
genetic statistical measures of (i) Wright's FST measure of genetic divergence; (ii) Henrick's G'ST  and (iii) Jost D. finePOP  R package by [Nakamichi et. al.]( https://doi.org/10.1111/1755-0998.12663) is ideal for high gene-flow species as "the weak genetic signal of population differentiation hinders the precise estimation of population genetic parameters"

Third is to estimate number of population clusters (K) based on geographical and genetic distance. This generates haplogroups that are generally phylogenetically closely related within clusters and geographically clustered. Again two options are available: GUI based tools like SAMOVA in SPADS1 (Windows ONLY) and command line based like GENELAND, an R package.

Fourth, AMOVA test is conducted to test the percentage of variation attributable to; (1) groups, (2) localities and (3) differences among individual within groups. This can be done in ARLEQUIN R package.

Fifth is to test the significance of correlation between mean genetic distances (Kimura two parameter model, 2KP) and geographic distances (Km) where possible. This can be done with Mantel Tests in Ade4 package in R.

Lastly is to conduct statistical tests to acertain the evolutionary nutrality in terms of Tajima's D and Fu's F. Also of interest is the haplotype diversity (h), nucleotide diversity (π) and number of segregating sites in each cluster.

### Phylogeographic Analysis tools.
1. [BASTA (BAyesian STructured coalescent Approximation)](https://doi.org/10.1371/journal.pgen.1005421), an approach implemented in [BEAST2 bayesian Package](https://bitbucket.org/nicofmay/basta-bayesian-structured-coalescent-approximation/src/master/) that is used to infer migration trends and evolutionary history of sampled lineages.
2. Other tools; [geocoding script](https://github.com/paolo-gratton/Gratton_et_al_JBiogeogr_2016) by [Gratton et al., 2017]( https://doi.org/10.1111/jbi.12786)

### Other tools
Besides tools mentioned above, the tools below proved useful:
1. [PGDSpider](http://www.cmpg.unibe.ch/software/PGDSpider/): Very useful a Java program that can read 27 different file formats and export data into 29 in molecular sequence format convertion/transformation by [Lischer et al., 2012](https://doi.org/10.1093/bioinformatics/btr642).

## Resources
COI sequences preparation protocol;
1. [Prosser et.al. 2016](https://doi.org/10.1111/1755-0998.12474)
2. [Wilson 2012 DNA barcoding insects](https://doi.org/10.1007/978-1-61779-591-6_3)

### Data
Both input and output (results) are found in './data/' directory

The metadata downloaded and stored in './data/input/input_data/bold_africa' each file comes from a country it is named after. The test dataset, from East Africa, is stored in './data/input/test_data/' as bold_data.tsv ( Other formats of the same test dataset are bold_data.xml or bold_fasta.fas (a fasta file with sequences)

All the downloaded data is stored in bold_africa, parsed using BeautifulSoup4 and lxml in python3, converted to a pandas dataframe and saved as raw .tsv text files in same directory using the './code/xml_to_tsv.py' script via './code/process_all_input_files.sh' script function 'build_tsv'. Reference sequences from other select african countries are sorted, cleaned and stored in './data/input/input_data/clean_africa' while East African data is processed and stored in './data/input/input_data/clean_eafrica/' using '.code/data_cleanup.R' Rscript via './code/process_all_input_files.sh' script function 'clean_sort_tsv'

Finally, additional double-checking and some cleaning is done in Vim to remove sequnces with errors and dublicates, after listing dublicates using the command:
>grep ">" <filename\> | sort | uniq -cd #prints the whole header if repeated

>grep ">" afroCOI_Under500_data.fasta| awk 'BEGIN{ FS="|" } {print $1 } {echo $1 }' | sort | uniq -cd #prints only the identifier (processID) if repeated

### Code
The scripts and some cloned tools (edited) are found in './code/'

The scripts to clean up and sort the metadata is:
>data_cleanup.R or 01.data_cleanup.ipynb (jupyter notebook copy)

It generates copies of the final tidy files as R dataframes and saves them in .tsv format

The script to convert the .tsv files to .fasta format is:
>process_all_input_files.sh (calls ./buildfasta_jb.awk script)


License: [CC-BY](https://creativecommons.org/licenses/by/3.0/)
