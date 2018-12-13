# co1_metaanalysis

## Preview.
The co1_metaanalysis is an Master's of science in Molecular Biology and Bioinformatics project, Department of Biochemistry, [Jomo kenyatta University of Agriculture and Technology(Jkuat)](http://www.jkuat.ac.ke/)

It is based at **[international centre for insect physiology and ecology(_icipe_)](http://www.icipe.org/)** and is done in collaboration with [BOLD Systems](http://www.boldsystems.org/)

This project is conducted with the supervision and support of;
1. [Dr Scott Miller](https://entomology.si.edu/StaffPages/MillerS.html)
2. [Dr Jandouwe Villinger](http://www.icipe.org/about/staff/jandouwe-villinger)
3. [Dr Steven Ger(jkuat)](https://scholar.google.com/citations?user=Qdp8yCsAAAAJ&hl=en)
4. [Dr Caleb Kipkurui Kibet](https://github.com/kipkurui)
5. [Dr Jean-Baka Domelevo Entfellner](https://github.com/jean-baka)
6. [Dr Dan Masiga](http://www.icipe.org/about/staff/daniel-masiga)

## Aim of the project

This study will determine the phylogenetic diversity and phylogeographic distribution of these voucher arthropods within eastern Africa to provide a detailed reference for future research on arthropods in the region.
Further analyses will focus on identification of potential disease vector species among biting flies (Order Diptera) and crop pest species such as fruit flies (family Tephriditae) that have not yet been implicated with, but may have impact on human, animal and crop health.

## Data acquisition
The co1 barcode sequence metadata will be exported from the [boldsystems public data portal](http://www.boldsystems.org/index.php/Public_BINSearch?searchtype=records) using the following search cretaria in the search box:
```
arthropoda Kenya Uganda Tanzania Rwanda Burundi
```
 This results in [over 36000 published records](http://www.boldsystems.org/index.php/Public_SearchTerms)

**With the following summary (NB: as of Mon Nov 12 2018 14:58:44):**

>_Found 36,436 published records, with 36,436 records with sequences, forming 10,210 BINs (clusters), with specimens from 6 countries, deposited in 126 institutions._
>_Of these records, 8,865 have species names, and represent 2,241 species._

To quickly download a the latest dataset in .tsv format click [**API link**](http://www.boldsystems.org/index.php/API_Public/combined?geo=Kenya|Uganda|Tanzania|Rwanda|Burundi&taxon=arthropoda&format=tsv)

Other datasets that have not been shared publicly will be sort from individual Project Managers (_The list will be provided soon_).

## Workflow
![Workflow](https://github.com/kibet-gilbert/co1_metaanalysis/blob/master/workflow.png)

## Tools
### Programming languages:
1. R
2. Bash (and awk)
3. Python (Python2,Python3 and ipython)

### Multiple Sequence Alignment
1. Muscle
2. T_Coffee
3. MAFFT
4. [SATé](https://github.com/sate-dev/sate-core) for [sate-tools-linux](https://github.com/sate-dev/sate-tools-linux)
5. [PASTA](https://github.com/smirarab/pasta)
6. Other tools; [SEPP](https://github.com/smirarab/sepp)

### Phylogenetic Inference
1. RAxML
2. FastTree

### Phylogeographic Analysis
1. BASTA (BEAST2 bayesian Package)
2. Other tools; [geocoding script(Gratton et al., 2017)](https://github.com/paolo-gratton/Gratton_et_al_JBiogeogr_2016)

## Resources
COI sequences preparation protocol;
1. [Prosser et.al. 2016](https://github.com/kibet-gilbert/co1_metaanalysis/blob/master/Prosser_et-al_2016_NGS_DNA_type_specimens_MER.pdf)
2. [Wilson 2012 DNA barcoding insects](https://github.com/kibet-gilbert/co1_metaanalysis/blob/master/Wilson_2012_DNA_barcoding_insects.pdf)

The metadata downloaded and stored as
>bold_data.tsv
>(bold2.tsv is just a true copy of bold_data.tsv file that we are working with as a test data set file)

or
>bold_data.xml

or
>bold_fasta.fas

The script to clean up and sort the metadata is
>data_cleanup.R

License: [CC-BY](https://creativecommons.org/licenses/by/3.0/)
