# dat = read.table("bold2.tsv", header=T, sep="\t", stringsAsFactors = F) # for some reason, fails to parse record number 73 (line 74 of original file)
dat2 = read.delim("bold2.tsv", stringsAsFactors = F, header = T, na.strings = "") # seems to work ok. bold2.tsv does not contain any '\r' characters

unique(dat2$markercode)
# filtering rows corresponding to COI-5P markers:
COI_data = subset(dat2, markercode == "COI-5P" & !is.na(nucleotides))
nrow(dat2)
nrow(COI_data) # we have 35990 usable sequences for COI-5P marker
# to install all the tidyverse galaxy of packages at once:
install.packages("tidyverse")
library(dplyr)
library(magrittr)
# once dplyr (and magrittr if necessary) is installed:
COI_data %>% mutate(seqlen = nchar(nucleotides)) %>% filter(seqlen > 620) -> resulting_dataframe # creates an additional column and filters
summary(resulting_dataframe)
