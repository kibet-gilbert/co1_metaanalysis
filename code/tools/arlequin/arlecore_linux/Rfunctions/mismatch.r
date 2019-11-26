################################################################################
# mismatch function  -  get matrix of values
#
# Author: Heidi Lischer
# Date: 10.2008
################################################################################


mismatch <- function(xmlText){
  # convert string data (table with names) to a numeric matrix -----------------
  # split string -------
  tagData2 <- as.character(xmlText)

  tagData3 <- strsplit(tagData2, "\n")
  tagMatrix <- as.matrix(as.data.frame(tagData3))
  tagMatrix <- gsub(" + ", " ", tagMatrix)   # trim white space
  tagMatrix <- subset(tagMatrix, tagMatrix[,1] != "") #trim empty lines
  tagMatrix <- tagMatrix[4:nrow(tagMatrix)]
  Data <- strsplit(tagMatrix, " ")

  Row <- length(Data)-1

  # to numeric matrix -----------
  Matrix <- as.matrix(as.data.frame(Data[1]))
  Matrix <- subset(Matrix, Matrix[,1] != "")
  Matrix <- as.numeric(Matrix)
  numericMatrix <- t(as.matrix(Matrix))

  for(n in 2:(Row)){
    nextrow <- as.matrix(as.data.frame(Data[n]))
    nextrow <- subset(nextrow, nextrow[,1] != "")
    nextrow <- as.numeric(nextrow)
    numericMatrix <- rbind(numericMatrix, t(as.matrix(nextrow)))
  }
   
   return(numericMatrix)
}
