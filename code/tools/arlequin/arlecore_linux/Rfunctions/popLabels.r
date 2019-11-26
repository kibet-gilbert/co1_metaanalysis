################################################################################
# popLabels function
#
# Author: Heidi Lischer
# Date: 10.2008
################################################################################


popLabels <- function(popText){
  # split text and get matrix
  tagData2 <- as.character(popText)
  tagData3 <- strsplit(tagData2, "\n")
  tagMatrix <- as.matrix(as.data.frame(tagData3))
  tagMatrix <- gsub(" + ", "\t", tagMatrix)   # trim white space
  tagMatrix <- subset(tagMatrix, tagMatrix[,1] != "")

  # pop names --------
  Names <- tagMatrix[3: nrow(tagMatrix), 1]
  Names <- strsplit(Names, "\t")
  Labels <- as.matrix(as.data.frame(Names)[3,])

  return(Labels)
}
