################################################################################
# absoluteHapFreq function  - (matrix)
#
# Author: Heidi Lischer
# Date: 11.2008
################################################################################


absoluteHapFreq <- function(xmlText) {
  # convert string data (table with names) to a numeric matrix -----------------
  # split string----
  tagData2 <- as.character(xmlText)

  tagData3 <- strsplit(tagData2, "\n")
  tagMatrix <- as.matrix(as.data.frame(tagData3))
  tagMatrix <- gsub(" + ", "\t", tagMatrix)   # trim white space
  tagMatrix <- subset(tagMatrix, tagMatrix[,1] != "") #trim empty lines

  # pop names ------------
  Names <- tagMatrix[2]
  Names <- strsplit(Names, "\t")
  Names <- as.matrix(as.data.frame(Names))
  Names <- subset(Names, Names[,1] != "")
  Names <- Names[1:nrow(Names)]


  #data-------------------------------------
  tagMatrix <- tagMatrix[5:nrow(tagMatrix)]
  Data <- strsplit(tagMatrix, "\t")
  

  Row <- length(Data)
  
  # to numeric matrix----  
  i <- 1
  Matrix <- as.matrix(as.data.frame(Data[i]))
  Matrix <- subset(Matrix, Matrix[,1] != "")
  
  # jump over lines without data
  while(length(Matrix) == 1){
    i <- i + 1
    Matrix <- as.matrix(as.data.frame(Data[i]))
    Matrix <- subset(Matrix, Matrix[,1] != "")  
  }
  
  HapNames <- Matrix[1,]
  Matrix <- Matrix[1:nrow(Matrix),]
  absHapMatrix <- t(as.matrix(Matrix))

  if(i <= Row){
    for(n in (i+1):(Row)){
      nextrow <- as.matrix(as.data.frame(Data[n]))
  
      # jump over empty lines and lines without data
      if((nextrow[1,1] != "") & (length(nextrow) != 1)){
        nextrow <- nextrow[1:nrow(nextrow),]         
        absHapMatrix <- rbind(absHapMatrix, t(as.matrix(nextrow)))
      }
    }
  }
  
  #add population names to the absHapMatrix
  absHapMatrix <- rbind(Names, absHapMatrix)


  #return absHapMatrix
  return(absHapMatrix) 
}