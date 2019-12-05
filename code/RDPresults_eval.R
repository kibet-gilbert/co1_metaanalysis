#!/usr/bin/Rscript

### installing tidyverse package and loading libraries
#Checking if a package is installed and if not installed it is installed. if installed it is loaded. "install.packages("tidyverse")" installs tidyverse

cat("\nChecking if needed packages are installed... tidyverse packages: 'ggplot2','dplyr' and 'magrittr'")
if("tidyverse" %in% rownames(installed.packages()) == FALSE) {
        install.packages("tidyverse")
} else {
        cat("\nExcellent tidyverse already installed.\nproceeding with summary analysis of RDPclassifier results...\n")
	## loading dplyr, ggplot2 and magrittr packages
    suppressMessages(library(dplyr))
    suppressMessages(library(ggplot2))
    suppressMessages(library(magrittr))
    suppressMessages(library(tools))
}

## loading RDPclassifier results in tab delimited format to a dataframe object. 
args = commandArgs(trailingOnly=TRUE)
filename_ext = file_ext(args[1])
filename = file_path_sans_ext(args[1])
input_src=dirname(file_path_as_absolute(args[1]))

df_taxa = read.delim(args[1], sep = "\t", stringsAsFactors = F, dec = ".", header = T)

## listing all families with over 20 values represented in the dataset.
Prefamily_df <- as.data.frame(table(c(subset(df_taxa, seq_len >= 500)$Family)), row.names = NULL)
colnames(Prefamily_df) <- c("Family","prefamfreq")
cat(nrow(subset(Prefamily_df, prefamfreq >= 20)),
    "predefined famil(y/ies) (seq_len >= 500), with frequency of over 20, is/are found in this clade, out of",
   nrow(Prefamily_df),
   "represented famil(y/ies) (seq_len >= 500)\n\n")
#subset(Prefamily_df, prefamfreq >= 20)

Asfamily_df <- as.data.frame(table(c(subset(df_taxa, Family_sc == 1.0 & seq_len >= 500)$As_family)), row.names = NULL)
colnames(Asfamily_df) <- c("As_family","asfamfreq")
cat(nrow(subset(Asfamily_df, asfamfreq >= 20)),
    "assigned families (BS score = 1.0 & seq_len >= 500), with frequency of over 20, are found in this clade out of",
   nrow(Asfamily_df),
   "represented assigned families (seq_len >= 500)\n\n")
#subset(Asfamily_df, asfamfreq >= 20)
family_df <- merge(Prefamily_df, Asfamily_df, by.x="Family", by.y="As_family")
subset(family_df, prefamfreq >= 20 | asfamfreq >= 20)

## listing all genera with over 5 values represented in the dataset.
Pregenus_df <- as.data.frame(table(c(subset(df_taxa, seq_len >= 500)$Genus)), row.names = NULL)
colnames(Pregenus_df) <- c("Genus","pregenusfreq")
cat(nrow(subset(Pregenus_df, pregenusfreq >= 5)),
    "predefined genera (seq_len >= 500 $ frequency >= 5) are found in this clade, out of",
   nrow(Pregenus_df),
   "represented genera (seq_len >= 500)\n\n")
#subset(Pregenus_df, pregenusfreq >= 10)

Asgenus_df <- as.data.frame(table(c(subset(df_taxa, Genus_sc == 1.0 & seq_len >= 500)$As_genus)), row.names = NULL)
colnames(Asgenus_df) <- c("As_genus","asgenusfreq")
cat(nrow(subset(Asgenus_df, asgenusfreq >= 5)),
    "assigned genera (BS score = 1.0 & seq_len >= 500), with frequency of over 5, are found in this clade out of",
   nrow(Asgenus_df),
   "represented genera (seq_len >= 500)\n\n")
#subset(Asgenus_df, asgenusfreq >= 10)
genus_df <- merge(Pregenus_df, Asgenus_df, by.x="Genus", by.y="As_genus")
subset(genus_df, pregenusfreq >= 5 | asgenusfreq >= 5)


#Detailed analysis of the distribution of variables as a assinged by RDPclassifier
## Total number of records within the file vs those with over 500 base-pairs
cat(nrow(df_taxa), "total number of records with",
    nrow(subset(df_taxa, seq_len >= 500)),
    "records having over 500 base-pairs\n")
# Total number of sequences with predefined species taxa vs
# those with over 500 base-pairs
cat(nrow(subset(df_taxa, Species != "NA")),
    "records with a predefined species from which",
    nrow(subset(df_taxa, Species != "NA" & seq_len >= 500 )),
    "have over 500 base pairs\n\n")
# All records assigned species classification
cat(nrow(subset(df_taxa, Species_sc >= 0)),
    "records were assigned species classification with bootstrap support ranging from 0 to 1\n\n")
# All records assigned species classification of BS support of 1, >= 0.95 and >= 0.70
cat(nrow(subset(df_taxa, Species_sc == 1.0)),
    "records were classiffied with a bootsrap of = 1,",
   nrow(subset(df_taxa, Species_sc >= 0.95)),
    "with >= 0.95 bootstrap support and",
   nrow(subset(df_taxa, Species_sc >= 0.70)),
   "with >= 0.7 bootstrap support\n\n")
# All records with predefined species and
# assigned species classification at BS support of 1, >= 0.95 and >= 0.70
cat(nrow(subset(df_taxa, Species != "NA" & Species_sc == 1.0)),
    "records, with predefined species, were classiffied with a bootsrap of = 1,",
   nrow(subset(df_taxa, Species != "NA" & Species_sc >= 0.95)),
    "with >= 0.95 bootstrap support and",
   nrow(subset(df_taxa, Species != "NA" & Species_sc >= 0.70)),
   "with >= 0.7 bootstrap support\n\n")
# All records with predefined species, seq-length of >= 500 and
# assigned species classification at BS support of 1, >= 0.95 and >= 0.70
cat(nrow(subset(df_taxa, seq_len >= 500 & Species_sc == 1.0)),
    "records, with >= 500 seq. length, were classiffied with a bootsrap of = 1,",
   nrow(subset(df_taxa, seq_len >= 500 & Species_sc >= 0.95)),
    "with >= 0.95 bootstrap support and",
   nrow(subset(df_taxa, seq_len >= 500 & Species_sc >= 0.70)),
   "with >= 0.7 bootstrap support\n\n")
# All records without predefined species, but with seq-length of >= 500 and
# assigned species classification at BS support of 1, >= 0.95 and >= 0.70
cat(nrow(subset(df_taxa, Species != "NA" & seq_len >= 500 & Species_sc == 1.0)),
    "records, with predefined species and >= 500 seq. length, were classiffied with a bootsrap of = 1,",
   nrow(subset(df_taxa, Species != "NA" & seq_len >= 500 & Species_sc >= 0.95)),
    "with >= 0.95 bootstrap support and",
   nrow(subset(df_taxa, Species != "NA" & seq_len >= 500 & Species_sc >= 0.70)),
   "with >= 0.7 bootstrap support\n\n")

# All records with a predifined species or RDP species classification BS support of 1.0
df_taxa %>% filter(Species != "NA" | Species_sc == 1.0) -> df_100per_classified
# All records with a predifined species or RDP species classification BS support of 0.95
df_taxa %>% filter(Species != "NA" | Species_sc >= 0.95) -> df_95per_classified
cat(nrow(df_100per_classified),
    "records either had predefined species,",
    "or were classified with a bootstrap value of 1.0,",
   nrow(df_95per_classified),
    "with bootstrap value of >=0.95 and",
   nrow(subset(df_taxa, Species != "NA" | Species_sc >= 0.7)),
   "with boostrap value of >=0.7\n\n")
cat(nrow(subset(df_taxa, Species_sc == 1.0 & seq_len >= 500 )),
    "records classified into species have sequences with over 500 bases, with",
    nrow(subset(df_taxa, is.na(Species) & Species_sc == 1.0 & seq_len >= 500 )),
    "having no prior species classification\n")
cat(nrow(subset(df_taxa, Species_sc == 1.0 & Species != df_taxa$As_species)),
    "records with a species classication confidence of 1.0",
    "have differing assinged species from predefined species,",
   nrow(subset(
       df_taxa, seq_len >= 500 & Species_sc == 1.0 & Species != df_taxa$As_species)),
    "of which have over 500 base-pairs\n\n")
df_taxa_clash <- subset(
    df_taxa, seq_len >= 500 & Species_sc == 1.0 & Species != df_taxa$As_species)
#names(lapply("df_taxa_clash",get)) <- paste(input_src,filename,sep="")
setwd(input_src)
write.table(df_taxa_clash, file = paste(filename, "_clash.tsv", sep =""),
            row.names = FALSE, col.names= TRUE, sep = "\t", quote=FALSE )
write.table(family_df, file = paste(filename, "_families.tsv", sep =""),
            row.names = FALSE, col.names= TRUE, sep = "\t", quote=FALSE )
write.table(genus_df, file = paste(filename, "_genera.tsv", sep =""),
            row.names = FALSE, col.names= TRUE, sep = "\t", quote=FALSE )
#head(df_classified)
#head(subset(df_taxa, Species != "NA" & Species_sc == 0))
