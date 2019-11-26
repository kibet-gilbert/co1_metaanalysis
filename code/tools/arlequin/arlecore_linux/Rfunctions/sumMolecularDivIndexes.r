################################################################################
# sumMolecularDivIndexes function  -  (lines: Thetas)
#
# Author: Heidi Lischer
# Date: 10.2008
################################################################################


sumMolecularDivIndexes <- function(xmlText, timeAttr, outfile=outfiles){
  # convert string data (table with names) to a numeric matrix------------------
  # split string----
  tagData2 <- as.character(xmlText)
  tagData3 <- strsplit(tagData2, "\n")
  tagMatrix <- as.matrix(as.data.frame(tagData3))
  tagMatrix <- gsub(" + ", "\t", tagMatrix)   # trim white space
  tagMatrix <- subset(tagMatrix, tagMatrix[,1] != "") #trim empty lines

  # names-----------------------
  Names <- tagMatrix[2]
  Names <- strsplit(Names, "\t")
  Names <- as.matrix(as.data.frame(Names))
  Names <- subset(Names, Names[,1] != "")
  Names <- Names[2:(nrow(Names)-2)]

  # data------------------------
  tagMatrix <- tagMatrix[4:nrow(tagMatrix)]
  Data <- strsplit(tagMatrix, "\t")

  # differentiate between 2 different styles
  Row <- length(Data)
  if(Row > 10){
    RowStart <- 11
  } else {
    RowStart <- 1
  }
  
  # to numeric matrix-----------
  Matrix <- as.matrix(as.data.frame(Data[RowStart]))
  Matrix <- gsub("N.A.", "NA", Matrix)
  Matrix <- subset(Matrix, Matrix[,1] != "")
  Theta <- Matrix[1]
  Matrix <- Matrix[2:(nrow(Matrix)-2)]
  Matrix <- as.numeric(Matrix)
  numericMatrix <- t(as.matrix(Matrix))

  for(n in (RowStart+1):(Row)){
    nextrow <- as.matrix(as.data.frame(Data[n]))
    nextrow <- gsub("N.A.", "NA", nextrow)
    nextrow <- subset(nextrow, nextrow[,1] != "")
    nextrowTheta <- nextrow[1]
    nextrow <- nextrow[2:(nrow(nextrow)-2)]
    nextrow <- as.numeric(nextrow)
    numericMatrix <- rbind(numericMatrix, t(as.matrix(nextrow)))
    Theta <- rbind(Theta, nextrowTheta)
  }
  
  Max <- max(numericMatrix, na.rm=TRUE)
  Min <- 0
  
  if(min(numericMatrix, na.rm=TRUE) < 0){
    Min <- min(numericMatrix, na.rm=TRUE)
  }
  
  if(Min == Max){
    Max <- Max + 1
  } 


  # draw plot ------------------------------------------------------------------
  outfileGraphic <- paste(outfile, "sumMolecularDivIndexes ", timeAttr, ".png",
                            sep="")  
  
  #save graphic
  png(outfileGraphic, width=1300, height=1300, res=144)
  
    #two or more pop are in the data (lines) ------------
    if(ncol(numericMatrix) != 1){
      plot(Min, type="l", xlab="", ylab="",
             main="Molecular diversity indices",  ylim=c(Min, Max),
             xlim=c(1,ncol(numericMatrix)), col="white", axes=FALSE, cex.lab=1.2)
             
         axis(side=2, at=c(0:Max),ylim=c(0:Max), labels=c(0:Max))
         mtext(side=2, text=expression(bold(theta)), cex=1.6, line=2.5)
         
         axis(side=1, at=c(1:ncol(numericMatrix)), labels=Names, las=2,
                cex.axis=0.8)
         mtext(side=1, text="Population", font=2, line=3.5)

         box()
    
         #add the different lines (Thetas)
         legendText <- NULL
         legendCol <- NULL
         j <- 1  
         
         for(i in 1:nrow(Theta)){
           #Theta k
           if(Theta[i] == "Theta_k"){ 
             lines(numericMatrix[i,], lwd=2)
             lines(numericMatrix[(i+1),], lty=2)
             lines(numericMatrix[(i+2),], lty=2)
             
             legendText[j] <- expression(paste(theta[k], " (95% CI)"))
             legendCol[j] <- "black"
             j <- j + 1
             i <- i + 2
           }
           
           #Theta H
           if(Theta[i] == "Theta_H"){
             lines(numericMatrix[i,], col="red3", lwd=2)
             lines((numericMatrix[i,]+ numericMatrix[(i+1),]), lty=2, col="red3")
             lines((numericMatrix[i,]- numericMatrix[(i+1),]), lty=2, col="red3")
             
             legendText[j] <- expression(paste(theta[H], " (+/- 1 sd)"))
             legendCol[j] <- "red3"
             j <- j + 1             
             i <- i + 1
           }
           
           #Theta S
           if(Theta[i] == "Theta_S"){            
             lines(numericMatrix[i,], col="blue", lwd=2)
             lines((numericMatrix[i,]+ numericMatrix[(i+1),]), lty=2, col="blue")
             lines((numericMatrix[i,]- numericMatrix[(i+1),]), lty=2, col="blue")
             
             legendText[j] <- expression(paste(theta[S], " (+/- 1 sd)"))
             legendCol[j] <- "blue"
             j <- j + 1
             i <- i + 1
           }           
           
           #Theta pi
           if(Theta[i] == "Theta_pi"){        
             lines(numericMatrix[i,], col="green3", lwd=2)
             lines((numericMatrix[i,]+ numericMatrix[(i+1),]), lty=2, col="green3")
             lines((numericMatrix[i,]- numericMatrix[(i+1),]), lty=2, col="green3")
             
             legendText[j] <- expression(paste(theta[pi], " (+/- 1 sd)"))
             legendCol[j] <- "green3"
             j <- j + 1
             i <- i + 1
           }
         }
         
         #add legend
         legend("topleft", legendText, lty=1, lwd=2, bty="n",
                 cex=1.1, col=legendCol)
    
    
    # if only one pop is in the data (points) -------------             
    } else {
      plot(Min, type="p", xlab="", ylab="",
             main="Molecular diversity indices", ylim=c(Min, Max),
             xlim=c(1, ncol(numericMatrix)), col="white", axes=FALSE)
             
         axis(side=2, at=c(0:Max),ylim=c(0:Max), labels=c(0:Max))
         mtext(side=2, text=expression(bold(theta)), cex=1.6, line=2.5)
          
         axis(side=1, at=c(1:ncol(numericMatrix)),labels=Names)
         mtext(side=1, text="Population", font=2, line=2.5)
         box()
    
         # add the different points (Thetas)
         legendText <- NULL
         legendCol <- NULL
         j <- 1  
         
         for(i in 1:nrow(Theta)){
          
           #Theta k
           if(Theta[i] == "Theta_k"){ 
             points(numericMatrix[i,], pch=16)
             points(numericMatrix[(i+1),])
             points(numericMatrix[(i+2),])
             
             legendText[j] <- expression(paste(theta[k], " (95% CI)"))
             legendCol[j] <- "black"
             j <- j + 1
             i <- i + 2
           }
           
           #Theta H
           if(Theta[i] == "Theta_H"){
             points(numericMatrix[i,], col="red3", pch=16)
             points((numericMatrix[i,]+ numericMatrix[(i+1),]), col="red3")
             points((numericMatrix[i,]- numericMatrix[(i+1),]), col="red3")
             
             legendText[j] <- expression(paste(theta[H], " (+/- 1 sd)"))
             legendCol[j] <- "red3"
             j <- j + 1             
             i <- i + 1
           }
           
           #Theta S
           if(Theta[i] == "Theta_S"){            
             points(numericMatrix[i,], col="blue", pch=16)
             points((numericMatrix[i,]+ numericMatrix[(i+1),]), col="blue")
             points((numericMatrix[i,]- numericMatrix[(i+1),]), col="blue")
             
             legendText[j] <- expression(paste(theta[S], " (+/- 1 sd)"))
             legendCol[j] <- "blue"
             j <- j + 1
             i <- i + 1
           }           
           
           #Theta pi
           if(Theta[i] == "Theta_pi"){         
             points(numericMatrix[i,], col="green3", pch=16)
             points((numericMatrix[i,]+ numericMatrix[(i+1),]), col="green3")
             points((numericMatrix[i,]- numericMatrix[(i+1),]), col="green3")
             
             legendText[j] <- expression(paste(theta[pi], " (+/- 1 sd)"))
             legendCol[j] <- "green3"
             j <- j + 1
             i <- i + 1
           }
         }
    
         #add  the legend
         legend("topleft", legendText, pch=16, bty="n",
                 cex=1.1, col=legendCol)
    }
  
  dev.off()


}