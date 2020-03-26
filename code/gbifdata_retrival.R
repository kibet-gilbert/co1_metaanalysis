#!/usr/bin/Rscript


### installing tidyverse package and loading libraries
#Checking if a package is installed and if not installed it is installed. if installed it is loaded. "install.packages("tidyverse")" installs tidyverse
requiredPackages = c("rgbif","tidyverse", "raster", "sys")
cat("\nChecking if needed packages are installed... 'rgbif' and 'tidyverse'. \nIncase of errors, install each individually and troubleshoot any errors that may occur")
for(p in requiredPackages)
    if(p %in% rownames(installed.packages()) == FALSE) {
        install.packages(p)
    } else {
        cat("\nExcellent", p, " already installed. loading", p, "...")
        suppressMessages(library(p, character.only = T))
    }
    cat("\nDone...\n")

## Parsing command line argument
args = commandArgs(trailingOnly=TRUE)
search_name = args[1]
search_taxon = args[2]
GBIF_USER = args[3]
GBIF_EMAIL = args[4]
GBIF_PWD = args[5]
#filename = file_path_sans_ext(args[1])
#input_src=dirname(file_path_as_absolute(args[1]))

#searching gbif database for key
key <- name_backbone( name = search_name, paste(search_taxon,"=", search_name, sep=" "), 
                      rank = NULL)$usageKey
cat("\n\tThe gbif backbone search result key for ", search_name, "is", key,
   ".\n\tWill proceed now with Occurance search of the records classified under the key")

#Searching the database for occurence data based on the key
search <- occ_search(taxonKey = key, return = "all")
cat("\n\tThe search result are...")
search

#Setting the download-output directory
cat("\n\tSetting the download-output directory")
output_dir="/home/kibet/bioinformatics/github/co1_metaanalysis/data/input/gbif"
setwd(output_dir)
dir.create(file.path(paste(search_name,"-",
                           key, sep = "")))
output_file_dir=paste(output_dir,"/", 
                  search_name, "-", key, sep= "" )

#Spin up a download request for GBIF occurrence data
#DWCA format
dowload_key <- occ_download(pred("taxonKey",key),
                            format = "DWCA",
                            user = GBIF_USER,
                            pwd = GBIF_PWD,
                            email = GBIF_EMAIL)
dowload.key <- dowload_key[1]

#SIMPLE_CSV format
dowload_key1 <- occ_download(pred("taxonKey",key),
                            format = "SIMPLE_CSV",
                            user = GBIF_USER,
                            pwd = GBIF_PWD,
                            email = GBIF_EMAIL)
dowload.key1 <- dowload_key1[1]

#Downloading the data from the database.
wait_time <- search$meta$count / 6000 * 60
cat("\nWill pause operation for", wait_time,
    "seconds as the download is download request is being worked on")
Sys.sleep(wait_time)
cat("\n\tThank you for waiting, we will proceed and download now\n\t")

#Downloading DWCA
gbif_dowload <- occ_download_get(dowload.key,
                                 path = output_file_dir, 
                                 overwrite = T )
res.zip <- paste(output_file_dir, "/", dowload.key, ".zip", sep = "")
unzip(res.zip, exdir = output_file_dir, overwrite = TRUE)

#Downloading SIMPLE_CSV
occ_download_get(dowload.key1, path = output_file_dir, overwrite = T)
res1.zip <- paste(output_file_dir, "/", dowload.key1, ".zip", sep = "")
unzip(res1.zip, exdir = output_file_dir, overwrite = TRUE)

#Citations from GBIF
cat("\n\tThe citations of the data including individual DOIs for the data, citations for every ndividual dataset included are as follows::\n")
gbif_citation(gbif_dowload)