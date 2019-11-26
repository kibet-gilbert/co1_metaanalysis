################################################################################
# interHapLabels function
#
# Author: Heidi Lischer
# Date: 10.2008
################################################################################


interHapLabels <- function(xmlText){
  tagData2 <- as.character(xmlText)
  tagData3 <- strsplit(tagData2, "\n")
  tagMatrix <- as.matrix(as.data.frame(tagData3))
  tagMatrix <- gsub(":", " ", tagMatrix)
  tagMatrix <- gsub(" + ", "\t", tagMatrix)   # trim white space
  tagMatrix <- subset(tagMatrix, tagMatrix[,1] != "") #trim empty lines
  
  Names <- strsplit(tagMatrix, "\t")
  InterHapLabels <- as.matrix(as.data.frame(Names)[2,])
  
  return(InterHapLabels)
  
}


      
