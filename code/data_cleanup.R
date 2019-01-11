### installing tidyverse package and loading libraries
install.packages("tidyverse") ## installs tidyverse

library(dplyr);library(magrittr) ## loads dplyr and magrittr packages


bold_dataframe = read.delim("../data/input/eafro_data/bold2.tsv", stringsAsFactors = F, header = T, na.strings = "") ## loads bold2.tsv as a dataframe object. works ok, bold2.tsv does not contain any '\r' characters

str(bold_dataframe) ## structure summary of bold_dataframe data

### taking a closer look at bold_dataframe$markercode
unique(bold_dataframe$markercode) ## identify and outputs the markers present in the data
COI_data = subset(bold_dataframe, markercode == "COI-5P" & !is.na(nucleotides)) ## filtering records corresponding to COI-5P markers
cat("COI-5P marker sequences are ",nrow(COI_data)," out of ", nrow(bold_dataframe), "sequences in the raw bold data") ## comparing the number of records before and after removing other markers


### Taking a closer look at the bold_dataframe$nucleotides field
typeof(bold_dataframe$nucleotides) ## Returns the designated data type in the bold_dataframe$nucleotides field
COI_data %>% mutate(seqlen1 = nchar(nucleotides)) -> resulting_dataframe1 ## Introducing a field "seqlen1" that has the number of nucleotides in the COI_data
unique(unlist(strsplit(COI_data$nucleotides, "", fixed = TRUE)), incomparables = FALSE) ## list all characters present in the nucleotide sequences

length(grep( '-',resulting_dataframe1$nucleotides, value= TRUE)) ## Number of nucleotide sequences with '-' characters* in them
resulting_dataframe1 %>% mutate(unaligned_nucleotides = gsub('-', '', resulting_dataframe1$nucleotides, ignore.case = FALSE, perl = FALSE, fixed = FALSE, useBytes = FALSE)) -> resulting_dataframe2 ## REMOVING '-'characters from nucleotide sequences and creating a field of unalinged nucleotide sequences (resulting_dataframe2$unaligned_nucleotides)
length(grep( '-',resulting_dataframe2$unaligned_nucleotides, value= TRUE)) ## Number of nucleotide sequences with '-' characters* in them after removal expected is '0'

resulting_dataframe2 %>% mutate(seqlen2 = nchar(unaligned_nucleotides)) -> resulting_dataframe3 ## Introducing a field seqlen2 with number nucleotides in resulting_dataframe2$unaligned_nucleotides field

str(resulting_dataframe3) ## structure summary of resulting_dataframe data

### sorting the dataframe and saving the final tidy dataset
boxplot(resulting_dataframe1$seqlen1, resulting_dataframe3$seqlen2) ## visualizing the distribution of sequence length in resulting_dataframe3$unaligned_nucleotides
hist(resulting_dataframe3$seqlen1); hist(resulting_dataframe3$seqlen2) ## visualizing the distribution of sequence length in resulting_dataframe3$unaligned_nucleotides

### Introducing a filter to remove any sequence with less than 500 and more than 700 nucleotides
resulting_dataframe3 %>% filter(seqlen2 >= 500 & seqlen2 <= 700) -> COI_clean_data; cat(length(COI_clean_data$unaligned_nucleotides),"sequences have from 500 to 700 bases") ## 

### Introducing a filter to remove any sequence with less than 650 and over 660 nucleotides
resulting_dataframe3 %>% filter(seqlen2 >= 650 & seqlen2 <= 660) -> COI_clean01_data; cat(length(COI_clean01_data$unaligned_nucleotides),"sequences have from 650 to 660 bases") 

### Introducing a filter to remove any sequence with over 500 nucleotides
resulting_dataframe3 %>% filter(resulting_dataframe3$seqlen2 < 500) -> COI_Under500_data; cat(length(COI_Under500_data$unaligned_nucleotides),"sequences have less than 500 bases")

### Introducing a filter to remove any sequence with less than 700 nucleotides
resulting_dataframe3 %>% filter(resulting_dataframe3$seqlen2 > 700) -> COI_Over700_data; cat(length(COI_Over700_data$unaligned_nucleotides),"sequences have more than 700 bases")

### Randomly sampling 100 sequences from data sets for use in testing the pipeline
COI_testa00_data <- resulting_dataframe3[sample(nrow(resulting_dataframe3), 100), ]#from resulting_dataframe3 :all COI sequences
COI_testb01_data <- COI_clean_data[sample(nrow(COI_clean_data), 100), ]#from COI_clean_data :sequences that have from 500 to 700 bases
COI_testb02_data <- COI_clean_data[sample(nrow(COI_clean_data), 100), ]#from COI_clean_data :sequences that have from 500 to 700 bases
COI_testb03_data <- COI_clean_data[sample(nrow(COI_clean_data), 100), ]#from COI_clean_data :sequences that have from 500 to 700 bases
COI_testc04_data <- COI_clean01_data[sample(nrow(COI_clean01_data), 100), ]#from COI_clean_data :sequences that have from 650 to 660 bases
COI_testc05_data <- COI_clean01_data[sample(nrow(COI_clean01_data), 100), ]#from COI_clean_data :sequences that have from 650 to 660 bases
COI_testd06_data <- COI_Under500_data[sample(nrow(COI_Under500_data), 100), ]#from COI_clean_data :sequences that have under 500 bases
COI_teste07_data <- COI_Over700_data[sample(nrow(COI_Over700_data), 100), ]#from COI_clean_data :sequences that have over 700 bases


### Printing copies of the final tidy files as dataframes in .tsv format
datalist = lapply(c("COI_clean_data", "COI_clean01_data", "COI_Over700_data", "COI_Under500_data", "COI_testa00_data", "COI_testb01_data", "COI_testb02_data", "COI_testb03_data", "COI_testc04_data", "COI_testc05_data", "COI_testd06_data", "COI_teste07_data"), get)
names(datalist) <- (c("../data/input/eafro_data/COI_clean_data", "../data/input/eafro_data/COI_clean01_data", "../data/input/eafro_data/COI_Over700_data", "../data/input/eafro_data/COI_Under500_data", "../data/input/eafro_data/COI_testa00_data", "../data/input/eafro_data/COI_testb01_data", "../data/input/eafro_data/COI_testb02_data", "../data/input/eafro_data/COI_testb03_data", "../data/input/eafro_data/COI_testc04_data", "../data/input/eafro_data/COI_testc05_data", "../data/input/eafro_data/COI_testd06_data", "../data/input/eafro_data/COI_teste07_data"))
for (i in 1:length(datalist)) {write.table(datalist[i], file = paste(names(datalist[i]), ".tsv", sep = ""), row.names = FALSE, col.names= TRUE, sep = "\t", quote=FALSE)}