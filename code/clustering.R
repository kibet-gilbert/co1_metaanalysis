#!/usr/bin/Rscript

#this Rscript is meant for denoising and clustering sequences using DADA2 R package.

### installing DADA2 package and loading libraries
#Checking if a package is installed and if not installed it is installed. if installed it is loaded. "install.packages("tidyverse")" installs tidyverse
if("dada2" %in% rownames(installed.packages()) == FALSE) {
        install.packages("BiocManager")
	BiocManager::install("dada2")
} else {
        cat("\nExcellent dada2 already installed.\nproceeding with resolving sequence variants that may differ by as little as one nucleotide\n")
        suppressMessages(library(dada2)) ## loads dada2 package
}

