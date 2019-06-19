# co1_metaanalysis

## Preview
The co1_metaanalysis is a Master's of science in Molecular Biology and Bioinformatics project, Department of Biochemistry, [Jomo Kenyatta University of Agriculture and Technology(Jkuat)](http://www.jkuat.ac.ke/)

It is conducted at the **[international centre for insect physiology and ecology(_icipe_)](http://www.icipe.org/)**  in collaboration with [BOLD Systems](http://www.boldsystems.org/)

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
The co1 barcode sequence metadata will be exported from the [boldsystems public data portal](http://www.boldsystems.org/index.php/Public_BINSearch?searchtype=records) using the following search syntax (criteria) in the search box:
```
arthropoda Kenya Uganda Tanzania Rwanda Burundi Ethiopia "South Sudan"
```
 This results in [over 36000 published records](http://www.boldsystems.org/index.php/Public_SearchTerms) from East Africa.
**With the following summary (NB: as of Mon Nov 12, 2018, 14:58:44):**

>_Found 36,436 published records, with 36,436 records with sequences, forming 10,210 BINs (clusters), with specimens from 6 countries, deposited in 126 institutions._
>_Of these records, 8,865 have species names, and represent 2,241 species._

To quickly download the latest of data set from East Africa in .tsv format click [**API link**](http://www.boldsystems.org/index.php/API_Public/combined?geo=Kenya|Uganda|Tanzania|Rwanda|Burundi&taxon=arthropoda&format=tsv)

Other datasets that have not been shared publicly will be sought from individual Project Managers (_The list will be provided soon_).
  1. DS-KENFRUIT dataset (1427) managed by Dr. Scott Miller (Kenya (1410), Madagascar (10), Nigeria (5), Burkina Faso (1), United States (1))
  2. DS-MPALALEP dataset (2472) managed by Dr. Scott miller (All kenyan)
  3. IDRCK IDRC kenya dataset (1704) managed by Dr. Daniel Masiga

To download all any datasets from select countries in the file co1_metaanalysis/code/countries we used the command
>source ./code/process_all_input_files.sh && bolddata_retrival ./code/countries

## Workflow
![Workflow](https://github.com/kibet-gilbert/co1_metaanalysis/blob/master/workflow.png)

### Programming languages:
1. R : tidyverse and r-essentials
2. Bash (and awk)
3. Python (Python2, Python3 and ipython): BeautifulSoup4, lxml, pandas

### Multiple Sequence Alignment
1. [Muscle.](http://www.drive5.com/muscle/)
It is problematic to align large number of sequences using global alignment algorithims used by muscle as explained in [Very large alignments are usually a bad idea](http://www.drive5.com/muscle/manual/bigalignments.html). Clustering highly identical (95% or 90% identity) help reduce the the sequences and challanges faced.
2. [T_Coffee](https://github.com/cbcrg/tcoffee). [The regressive mode of T-Coffee](https://github.com/cbcrg/tcoffee/blob/master/docs/tcoffee_quickstart_regressive.rst) is [described as most suitable for large datasets](https://www.biorxiv.org/content/10.1101/490235v1.full) by E. G. Nogales et. al (2018).
3. [MAFFT Version 7](https://mafft.cbrc.jp/alignment/software/). For large datasets: [Tips for handling a large dataset](https://mafft.cbrc.jp/alignment/software/tips.html). More published by [T. Nakamura et. al (2018)](https://academic.oup.com/bioinformatics/article/34/14/2490/4916099)
4. [SATé](https://github.com/sate-dev/sate-core) for [sate-tools-linux](https://github.com/sate-dev/sate-tools-linux)
5. [PASTA](https://github.com/smirarab/pasta) [(Tutorial)](https://github.com/smirarab/pasta/blob/master/pasta-doc/pasta-tutorial.md)
6. Other tools; [SEPP](https://github.com/smirarab/sepp), [HMMER](http://hmmer.org/)
7. To view the multiple sequence alignments use [jalview](http://www.jalview.org/download) for datasets upto a few thousands but for bigger data sets use [Seaview](http://doua.prabi.fr/software/seaview) by [Gouy et al.](https://academic.oup.com/mbe/article/27/2/221/970247). Seaview uses the FLTK (installed separately) project for its user interface.

#### MSA evaluation:
This is can be done based on two approaches: 1. Based on a reference MSA (which we don't have) or 2. Based on analysing the alignments themselves. In the later we use the following [sequence based methods](https://tcoffee.readthedocs.io/en/latest/tcoffee_main_documentation.html#sequence-based-methods):
1. [Computing the CORE index of any alignment
](https://tcoffee.readthedocs.io/en/latest/tcoffee_main_documentation.html#computing-the-local-core-index).
2. Evaluating the [Transitive Consistency Score (TCS)](https://tcoffee.readthedocs.io/en/latest/tcoffee_main_documentation.html#transitive-consistency-score-tcs) of an MSA. The scores generated here are usefull in filtering our sequences and in phylogenetic inference based on herogenous site evolutionary rates.

### Phylogenetic Inference
1. [RAxML](https://cme.h-its.org/exelixis/web/software/raxml/index.html)
2. [FastTree](http://www.microbesonline.org/fasttree/)

The available/applicable computer readable formats of the phylogenetic trees are [Newick](http://evolution.genetics.washington.edu/phylip/newicktree.html), [NEXUS](http://en.wikipedia.org/wiki/Nexus_file) and [PhyloXML](http://en.wikipedia.org/wiki/PhyloXML).

Programs to be used to visualize and edit phylogenetic trees:
[FigTree](http://tree.bio.ed.ac.uk/software/figtree/).
Other options being; [Archaeopteryx](http://www.phylosoft.org/archaeopteryx/), [Dendroscope](http://ab.inf.uni-tuebingen.de/software/dendroscope/),
[Jstree](http://lh3lh3.users.sourceforge.net/jstree.shtml) or [PhyloWidget](http://www.phylowidget.org/)

### Phylogeographic Analysis
1. BASTA [(BEAST2 bayesian Package)](http://www.beast2.org/)
2. Other tools; [geocoding script(Gratton et al., 2017)](https://github.com/paolo-gratton/Gratton_et_al_JBiogeogr_2016)

## Resources
COI sequences preparation protocol;
1. [Prosser et.al. 2016](https://github.com/kibet-gilbert/co1_metaanalysis/blob/master/Prosser_et-al_2016_NGS_DNA_type_specimens_MER.pdf)
2. [Wilson 2012 DNA barcoding insects](https://github.com/kibet-gilbert/co1_metaanalysis/blob/master/Wilson_2012_DNA_barcoding_insects.pdf)

### Data
Both input and output (results) are found in './data/'

The metadata downloaded and stored in './data/input/input_data/bold_africa' each file comes from a country it is named after. The test dataset, from East Africa, is stored in './data/input/test_data/' as bold_data.tsv (bold2.tsv is just an exact copy of bold_data.tsv. Other formats of the same test dataset are bold_data.xml or bold_fasta.fas (a fasta file with sequences)

All the downloaded data is stored in bold_africa, parsed using BeautifulSoup4 and lxml in python3, converted to a pandas dataframe and saved as raw .tsv text files in same directory using the './code/xml_to_tsv.py' script via './code/process_all_input_files.sh' script function 'build_tsv'. Reference sequences from other select african countries are sorted, cleaned and stored in './data/input/input_data/clean_africa' while East African data is processed and stored in './data/input/input_data/clean_eafrica/' using '.code/data_cleanup.R' Rscript via './code/process_all_input_files.sh' script function 'clean_sort_tsv'

Finally additional cleaning is done in Vim to remove sequnces with errors and dublicates, by listing dublicates using the command:
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
