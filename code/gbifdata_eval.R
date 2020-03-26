#!/usr/bin/Rscript

### installing tidyverse package and loading libraries
#Checking if a package is installed and if not installed it is installed. if installed it is loaded. "install.packages("tidyverse")" installs tidyverse
requiredPackages = c("rgbif","tidyverse", "raster", "sys")
cat("\nChecking if needed packages are installed... 'rgbif' and 
'tidyverse'. \nIncase of errors, install each individually and 
troubleshoot any errors that may occur")
for(p in requiredPackages)
    if(p %in% rownames(installed.packages()) == FALSE) {
        install.packages(p)
    } else {
        cat("\nExcellent", p, " already installed. loading", p, "...")
        suppressMessages(library(p, character.only = T))
    }
    cat("\nDone...\n")
  
## loading rgbif downloads. 
args = commandArgs(trailingOnly=TRUE)
search_name = args[1]

# Loading SIMPLE_CSV file
output_dir<-Sys.glob(paste("/home/kibet/bioinformatics/github/co1_metaanalysis/data/input/gbif/", 
                  search_name,"*", sep=""))
setwd(output_dir)
data_files <- Sys.glob("*.csv")
res.csv <- data_files[which.max(file.mtime(data_files))]
cat("\n\tLoading", res.csv)
gbif_data <- read.delim(res.csv, stringsAsFactors = F, header = T,
                        na.strings = "", quote = "")

# Loading DWCA occurence file
occ.tsv <- paste(output_dir, "/", "occurrence.txt", sep = "")
cat("\n\tLoading", occ.tsv)
gbif_occ <- read.delim(occ.tsv, stringsAsFactors = F, header = T,
                       na.strings = "", sep = "\t", quote = "")

# Loading DWCA verbatim.txt
verb.tsv <- paste(output_dir, "/", "verbatim.txt", sep = "")
cat("\n\tLoading", verb.tsv)
gbif_verb <- read.delim(verb.tsv, stringsAsFactors = F, header = T,
                        na.strings = "", quote = "")

# First, extract Rows with associated sequences from occurrence.txt
gbif_seq <- subset(gbif_occ, !is.na(associatedSequences))
cat("\n\t", nrow(gbif_seq), "rows in occurrence.txt have associated nucleotide sequences")

# Second, extract records from SIMPLE_CSV file with associatedSequences based on filtered gbifIDs above
gbif_data_seq <- subset(gbif_data, gbifID %in% gbif_seq$gbifID)

# Merge The two dataframes avoiding any dublicate columns
gbif_data.seq <- merge(gbif_data_seq, gbif_seq, 
                       sort = T, all.y=T)

# Creating a dataframe of gbifID, assocuiatedSequences and adding six new columns to it: BOLDprocessiduri, BOLDprocessid, BOLDclusteruri, BOLDcluster, GenBankuri, GenBank

dplyr::select(gbif_data.seq, gbifID, associatedSequences) %>% 
mutate(BOLDprocessiduri = NA_character_, BOLDprocessid = NA_character_,
       BOLDclusteruri = NA_character_, BOLDcluster = NA_character_, 
       GenBankuri = NA_character_, GenBank = NA_character_) -> genbank
# Adding values to the BOLDprocessiduri, BOLDclusteruri, GenBankuri columns of the genbank dataframe
BOLD_process <- "processid=|^[:upper:]{4,6}[:digit:]{1,6}-[:digit:]{2}$"
BOLD_cluster <- "clusteruri="
genbank_acce <- "nuccore|^[:upper:]{1,2}_?[:digit:]{5,8}\\.?[:digit:]?$|GenBank"
n <- 1
while(n <= nrow(genbank)) {
    for(i in unlist(strsplit(as.character(genbank$associatedSequences[n]),'\\|'))) {
        if (stringr::str_detect(i, BOLD_process)) {
            genbank[n, "BOLDprocessiduri"] <- i
        } else if (stringr::str_detect(i, BOLD_cluster)) {
            genbank[n, "BOLDclusteruri"] <- i
        } else if (stringr::str_detect(i, genbank_acce)) {
            genbank[n, "GenBankuri"] <- i
        } else {
            cat("\n\nThe value in ", n," position of genbank$associatedSequences",
                " does not fit any pattern or is empty \n\t>>", i)
            genbank$BOLDprocessiduri[n] <- NA_character_
            genbank$BOLDclusteruri[n] <- NA_character_
            genbank$GenBankuri[n] <- NA_character_
        }
    }
    n <- n+1
}


#  Adding values to the BOLDprocessid, BOLDcluster, GenBank columns of the genbank dataframe
BOLD_processID <- "^[:upper:]{4,6}[:digit:]{1,6}-[:digit:]{2}$"
BOLD_clusterID <- "^BOLD:[:alnum:]{3,10}$"
genbank_accession <- "^[:upper:]{1,2}_?[:digit:]{5,8}\\.?[:digit:]?$"
n <- 1
while(n <= nrow(genbank)) {
    if(!is.na(genbank$BOLDprocessiduri[n])) for(i in unlist(strsplit(as.character(genbank$BOLDprocessiduri[n]),'processid='))) {
        if (stringr::str_detect(i, BOLD_processID)) {
            genbank[n, "BOLDprocessid"] <- i
        }
    }
    if(!is.na(genbank$BOLDclusteruri[n])) for(i in unlist(strsplit(as.character(genbank$BOLDclusteruri[n]),'clusteruri='))) {
        if (stringr::str_detect(i, BOLD_clusterID)) {
            genbank[n, "BOLDcluster"] <- i
        }
    }
    if(!is.na(genbank$GenBankuri[n])) for(i in unlist(strsplit(as.character(gsub("\\.1 ", " ", genbank$GenBankuri[n])),'nuccore/|\\s|-SUPPRESSED'))) {
        if (stringr::str_detect(i, genbank_accession)) {
            genbank[n, "GenBank"] <- i
        }
    }
    n <- n+1
}

# Merging genbank dataframe: Has URLs (BOLDprocessiduri, BOLDclusteruri and Genbankuri) and identifiers(BOLDprocessid, BOLDcluster and GenBank accession numbers) with gbif_data.seq dataframe: Has 240 variables
gbifAssociatedSequences <- merge(gbif_data.seq, genbank, 
                                 sort = T, all=T)
headers.df <- dplyr::select(gbifAssociatedSequences, gbifID, BOLDprocessid, GenBank, 
                            phylum, class, order, family, genus, species, 
                            countryCode, locality, decimalLatitude, decimalLongitude, elevation)
cat("\n\tThe represented genera are:")
as.data.frame(table(c(headers.df$genus)))
#as.data.frame(table(c(gbifAssociatedSequences$genus)))

#  Filtering out records with BOLD ProcessIDs - Sequences contained in BOLD database
gbif_genbank <- subset(gbifAssociatedSequences, !is.na(GenBank))
gbif_BOLD <- subset(gbifAssociatedSequences, !is.na(BOLDprocessid))
gbif_genbankOnly <- subset(gbifAssociatedSequences, !(GenBank %in% gbif_BOLD$GenBank))
gbif_BOLDOnly <- subset(gbif_BOLD, is.na(GenBank))
    
cat("\n", nrow(gbif_genbank), "records have GenBank Accession numbers,\n",
    nrow(gbif_BOLD), "records have BOLD processIDs, while\n",
    nrow(gbif_genbankOnly), "records have only GenBank Accession numbers and\n",
    nrow(gbif_BOLDOnly),"records have only BOLD processIDs. A total of\n",
    nrow(gbifAssociatedSequences), "records have associated sequences.")
    
cat("\n There are", length(unique(gbif_genbank$GenBank)),
    "unique GenBank accession numbers,\n", 
    length(unique(gbif_genbankOnly$GenBank)),
    "are not associated with any BOLD ProcessIDs and\n",
    length(unique(gbif_BOLD$GenBank)),
    "are associated with records with BOLD processIDS")
    
cat("\n",nrow(gbif_genbankOnly),"records have only GenBank accession numbers of which\n",
    length(unique(gbif_genbankOnly$GenBank)), "have unique accession numbers")

# Extracting GenBank accesion codes
genbank_accessions <- gbif_genbank$GenBank
cat("\nOf the", nrow(genbank), "records with associated sequences\n",
    length(genbank_accessions),
    "records have GenBank accession numbers and of this,\n",
    length(unique(genbank_accessions)),
    "accession numbers are unique, with only\n",
    length(unique(gbif_genbankOnly$GenBank)),
    "unique records not linked to any BOLDprocessIDs")
genbankAccessionNumbers <- unique(genbank_accessions)
gbif_genbank_AccessionNos <- unique(gbif_genbankOnly$GenBank)
    
#Writing the dataframes to tables in the drive location
    
#Writing the dataframe gbifAssociatedSequences to file
write.table(gbifAssociatedSequences,
            file = paste(search_name, "_gbifAssociatedSequences.tsv", sep =""),
            row.names = FALSE, col.names= TRUE, sep = "\t", quote=FALSE )
#writing the vector genbankAccessionNumbers to file
writeLines(genbankAccessionNumbers,
            con = paste(search_name, "_genbankAccessionNumbers.txt", sep =""),
            sep = "\n" )
#Writing the vector gbif_genbank_AccessionNos
writeLines(gbif_genbank_AccessionNos,
            con = paste(search_name, "_minusBOLD_genbankAccessionNumbers.txt", sep =""),
            sep = "\n" )

#Writing the dataframe headers to a FASTA format to file
write.table(headers.df,
            file = paste(search_name, "_gbifheaders.tsv", sep =""),
            row.names = FALSE, col.names= TRUE, sep = "\t", quote=FALSE )