#!/usr/bin/env Rscript

#This script is has code used in anaysing the population structure from genetic data using pegas r package - "Population and Evolutionary Genetics Analysis System"

#testing and installing packages:

requiredPackages = c("pegas","ape","adegenet")
cat("\nChecking if needed packages are installed... 'pegas','ape' and 'adegenet'\nDependencies are ‘e1071’, ‘gtools’, ‘classInt’, ‘units’, ‘gdata’, ‘httpuv’, ‘htmltools’, ‘later’, ‘promises’, ‘rlang’, ‘fastmap’, ‘segmented’, ‘sp’, ‘spData’, ‘sf’, ‘deldir’, ‘LearnBayes’, ‘coda’, ‘expm’, ‘gmodels’, ‘permute’, ‘ade4’, ‘shiny’, ‘seqinr’, ‘spdep’, ‘vegan’\n\nIncase of errors installing this packages then kindly try to install the individually and trouble shoot any errors.")
for(p in requiredPackages)
	if(p %in% rownames(installed.packages()) == FALSE) {
        install.packages(p)
} else {
        cat("\nExcellent", p, " already installed. loading", p, "...")
        ## loads dplyr and magrittr packages
        suppressMessages(library(p, character.only = T))
}
cat("\nDone...\n")
