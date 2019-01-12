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
arthropods Kenya Uganda Tanzania Rwanda Burundi
```
 This results in [over 36000 published records](http://www.boldsystems.org/index.php/Public_SearchTerms)

**With the following summary (NB: as of Mon Nov 12, 2018, 14:58:44):**

>_Found 36,436 published records, with 36,436 records with sequences, forming 10,210 BINs (clusters), with specimens from 6 countries, deposited in 126 institutions._
>_Of these records, 8,865 have species names, and represent 2,241 species._

To quickly download the latest of data set in .tsv format click [**API link**](http://www.boldsystems.org/index.php/API_Public/combined?geo=Kenya|Uganda|Tanzania|Rwanda|Burundi&taxon=arthropoda&format=tsv)

Other datasets that have not been shared publicly will be sought from individual Project Managers (_The list will be provided soon_).

## Workflow
![Workflow](https://github.com/kibet-gilbert/co1_metaanalysis/blob/master/workflow.png)

## Tools
### Programming languages:
1. R
2. Bash (and awk)
3. Python (Python2, Python3 and ipython)

### Multiple Sequence Alignment
1. [Muscle](http://www.drive5.com/muscle/)
2. [T_Coffee](http://www.tcoffee.org/)
3. [MAFFT Version 7](https://mafft.cbrc.jp/alignment/software/)
4. [SATé](https://github.com/sate-dev/sate-core) for [sate-tools-linux](https://github.com/sate-dev/sate-tools-linux)
5. [PASTA](https://github.com/smirarab/pasta) [(Tutorial)](https://github.com/smirarab/pasta/blob/master/pasta-doc/pasta-tutorial.md)
6. Other tools; [SEPP](https://github.com/smirarab/sepp), [HMMER](http://hmmer.org/)

### Phylogenetic Inference
1. [RAxML](https://cme.h-its.org/exelixis/web/software/raxml/index.html)
2. [FastTree](http://www.microbesonline.org/fasttree/)

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

The metadata downloaded and stored in './data/input/eafro_data' as
>bold_data.tsv (bold2.tsv is just an exact copy of bold_data.tsv file that we are working with as a test data set file)
or bold_data.xml or bold_fasta.fas

Reference sequences from other select african countries are stored in './data/input/afro_data'

### Code
The scripts and some cloned tools (edited) are found in './code/'

The scripts to clean up and sort the metadata is:
>data_cleanup.R or 01.data_cleanup.ipynb (jupyter notebook copy)

It generates copies of the final tidy files as R dataframes and saves them in .tsv format

The script to convert the .tsv files to .fasta format is:
>process_all_input_files.sh (calls ./buildfasta_jb.awk script)


License: [CC-BY](https://creativecommons.org/licenses/by/3.0/)
