#!/usr/bin/Rscript


### installing tidyverse package and loading libraries
#Checking if a package is installed and if not installed it is installed. if installed it is loaded. "install.packages("tidyverse")" installs tidyverse
cat("\nChecking if needed packages are installed... 'tidyverse','dplyr' and 'magrittr'")
if("tidyverse" %in% rownames(installed.packages()) == FALSE) {
        install.packages("tidyverse")
} else {
        cat("\nExcellent tidyverse already installed.\nproceeding with clean_up and sorting\n")
	## loads dplyr and magrittr packages
	suppressMessages(library(dplyr))
	suppressMessages(library(magrittr))
	suppressMessages(library(tools))
}


## loading BOLD results in tab delimited format to a dataframe object. 
args = commandArgs(trailingOnly=TRUE)
filename_ext = file_ext(args[1])
filename = file_path_sans_ext(args[1])
input_src=dirname(file_path_as_absolute(args[1]))

bold_dataframe = read.delim(args[1], stringsAsFactors = F, header = T, na.strings = "") ## loads bold2.tsv as a dataframe object. works ok, bold2.tsv does not contain any '\r' characters

### Taking a closer look at the bold_dataframe$nucleotides field
bold_data = subset(bold_dataframe, !is.na(nucleotides))
## Introducing a field "seqlen1" that has the number of nucleotides in the COI_data
bold_data %>% mutate(seqlen1 = nchar(nucleotides)) -> resulting_dataframe1 

## REMOVING '-'characters from nucleotide sequences and creating a field of unalinged nucleotide sequences (resulting_dataframe2$unaligned_nucleotides)
resulting_dataframe1 %>% mutate(unaligned_nucleotides = gsub('-', '', resulting_dataframe1$nucleotides, ignore.case = FALSE, perl = FALSE, fixed = FALSE, useBytes = FALSE)) -> resulting_dataframe2 

## Introducing a field seqlen2 with number nucleotides in resulting_dataframe2$unaligned_nucleotides field
resulting_dataframe2 %>% mutate(seqlen2 = nchar(unaligned_nucleotides)) -> resulting_dataframe3 

### taking a closer look at bold_dataframe$markercode

## filtering records corresponding to COI-5P markers
COI_data = subset(resulting_dataframe3, markercode == "COI-5P" & !is.na(nucleotides))
non_COI_data = subset(resulting_dataframe3, markercode != "COI-5P" & !is.na(nucleotides))

###Generating a file with all 'COI-5P' sequences
COI_data -> COI_all_data
cat("\nOf all",nrow(bold_dataframe),"retrieved records, ",length(COI_all_data$unaligned_nucleotides)," have 'COI-5P' marker sequences. These are the only ones retained for downstream analysis::\n")


###Introducing a filter to remove sequences with less than 500 nucleotides
COI_all_data %>% filter(seqlen2 >= 500 ) -> COI_Over499_data; cat("\t",length(COI_Over499_data$unaligned_nucleotides),"sequences have 500 or more nucleotide bases\n")

### Introducing a filter to remove any sequence with less than 500 and more than 700 nucleotides
COI_all_data %>% filter(seqlen2 >= 500 & seqlen2 <= 700) -> COI_500to700_data; cat("\t",length(COI_500to700_data$unaligned_nucleotides),"sequences have 500 to 700 bases\n") ## 

### Introducing a filter to remove any sequence with less than 650 and over 660 nucleotides
COI_all_data %>% filter(seqlen2 >= 650 & seqlen2 <= 660) -> COI_650to660_data; cat("\t",length(COI_650to660_data$unaligned_nucleotides),"sequences have 650 to 660 bases\n") 

### Introducing a filter to remove any sequence with over 500 nucleotides
COI_all_data %>% filter(seqlen2 < 500) -> COI_Under500_data; cat("\t",length(COI_Under500_data$unaligned_nucleotides),"sequences have less than 500 bases\n")

### Introducing a filter to remove any sequence with less than 700 nucleotides
COI_all_data %>% filter(seqlen2 > 700) -> COI_Over700_data; cat("\t",length(COI_Over700_data$unaligned_nucleotides),"sequences have more than 700 bases\n")

### Assessing gps co-ordinates
cat( "\t", nrow(subset (COI_all_data, is.na(lat))), "records lack latitude co-ordinates \n\t",
    nrow(subset (COI_all_data, is.na(lon))), "records lack longitude co-ordinates")

### Summary
cat( "\n\t",nrow(subset(bold_dataframe, markercode != "COI-5P")), 
    " records have non-COI-5P markersequences \n\t",
    nrow(subset(bold_dataframe,  is.na(nucleotides) )),
    " lack nucleotide sequence values (is.na(nucleotides))")

### Printing copies of the final tidy files as dataframes in .tsv format
output_list = c("COI_all_data", "non_COI_data", "COI_Over499_data", "COI_500to700_data", "COI_650to660_data", "COI_Over700_data", "COI_Under500_data")
output_list_names = c("_all_data", "_NonCOI_data", "_Over499_data", "_500to700_data", "_650to660_data", "_Over700_data", "_Under500_data")

datalist = lapply(output_list, get)
names(datalist) <- paste(input_src,"/", filename, output_list_names, sep= "" )
for (i in 1:length(datalist)) {write.table(datalist[i], file = paste(names(datalist[i]), ".tsv", sep = ""), row.names = FALSE, col.names= TRUE, sep = "\t", quote=FALSE)}
