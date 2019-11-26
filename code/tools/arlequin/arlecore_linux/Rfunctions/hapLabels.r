################################################################################
# hapLabels function
#
# Author: Heidi Lischer
# Date: 10.2008
################################################################################


hapLabels <- function(xmlText){
  tagData2 <- as.character(xmlText)
  tagData3 <- strsplit(tagData2, "\n")
  tagMatrix <- as.matrix(as.data.frame(tagData3))
  tagMatrix <- gsub(" + ", "\t", tagMatrix)   # trim white space
  tagMatrix <- subset(tagMatrix, tagMatrix[,1] != "") #trim empty lines
  
  # get pop names --------------
  Names <- tagMatrix[2: nrow(tagMatrix), 1]
  Names <- strsplit(Names, "\t")
  HapLabels <- as.matrix(as.data.frame(Names)[1,])
  
  return(HapLabels)
  
}


      
