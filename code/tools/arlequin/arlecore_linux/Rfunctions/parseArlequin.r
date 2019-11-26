################################################################################
# parseArlequin function
#
# Author: Heidi Lischer
# Date: 10.2008
# Modified: 04.2013
################################################################################


parseArlequin <- function(infile, outfiles, sourcePath){
  # open XML package------------------------------------------------------------
  try({
    library(XML, lib.loc=paste(substr(sourcePath, 0, nchar(sourcePath)-1), "\\R_3.5", sep=""))
  })
  try({
    library(XML, lib.loc=paste(substr(sourcePath, 0, nchar(sourcePath)-1), "\\R_3.0", sep=""))
  })
  try({
    library(XML, lib.loc=paste(substr(sourcePath, 0, nchar(sourcePath)-1), "\\R_2.12", sep=""))
  })
  try({
    library(XML, lib.loc=paste(substr(sourcePath, 0, nchar(sourcePath)-1), "\\R_2.11", sep=""))
  })
  
  try({
 #   library(XML)
  })

  # get XML file into an R Structure--------------------------------------------
  doc = xmlTreeParse(infile, useInternal = TRUE)
  root = xmlName(xmlRoot(doc))        #get root name



  
  # spatial mismatch -----------------------------------------------------------
  source(paste(sourcePath, "mismatch.r", sep=""))
  mismatchSpatial_0.01 <- list()
  mismatchSpatial_0.05 <- list()
  mismatchSpatial_0.1 <- list()
  
  # get mismatchSpatialExpCI_0.010000 --------------------
  mismatchSpatialExpCI_0.01 <- getNodeSet(doc,"//mismatchSpatialExpCI_0.010000")
  
  if(length(mismatchSpatialExpCI_0.01) != 0){
    spatialNames <- list() 
    
    for(i in 1:length(mismatchSpatialExpCI_0.01)){
      xmlText <- sapply(mismatchSpatialExpCI_0.01[i], xmlValue)
      
      timeAttr <- sapply(mismatchSpatialExpCI_0.01[i], xmlGetAttr, "time")
      timeAttr <- gsub("/|:", "-", timeAttr)
      sapply(mismatchSpatialExpCI_0.01[i], addAttributes,"time"=timeAttr)
       
      popAttr <- sapply(mismatchSpatialExpCI_0.01[i], xmlGetAttr, "pop")
      popAttr <- gsub("/|:", "-", popAttr)
      sapply(mismatchSpatialExpCI_0.01[i], addAttributes,"pop"=popAttr)
      
      spatialNames[i] <- paste(timeAttr, popAttr)
      
      try({
        mismatchSpatial_0.01[[i]] <- mismatch(xmlText)
      })
    }
    names(mismatchSpatial_0.01) <- spatialNames
  }
  

  # get mismatchSpatialExpCI_0.050000 ---------------------
  mismatchSpatialExpCI_0.05 <- getNodeSet(doc,"//mismatchSpatialExpCI_0.050000")
  
  if(length(mismatchSpatialExpCI_0.05) != 0){
    spatialNames <- list()
    
    for(i in 1:length(mismatchSpatialExpCI_0.05)){
      xmlText <- sapply(mismatchSpatialExpCI_0.05[i], xmlValue)
      
      timeAttr <- sapply(mismatchSpatialExpCI_0.05[i], xmlGetAttr, "time")
      timeAttr <- gsub("/|:", "-", timeAttr)
      sapply(mismatchSpatialExpCI_0.05[i], addAttributes,"time"=timeAttr)
      
      popAttr <- sapply(mismatchSpatialExpCI_0.05[i], xmlGetAttr, "pop")
      popAttr <- gsub("/|:", "-", popAttr)
      sapply(mismatchSpatialExpCI_0.05[i], addAttributes,"pop"=popAttr)
      
      spatialNames[i] <- paste(timeAttr, popAttr)
      
      try({
        mismatchSpatial_0.05[[i]] <- mismatch(xmlText)
      })
    }
    names(mismatchSpatial_0.05) <- spatialNames
  }
  
  
  # get mismatchSpatialExpCI_0.100000 ----------------------
  mismatchSpatialExpCI_0.1 <- getNodeSet(doc, "//mismatchSpatialExpCI_0.100000")
  
  if(length(mismatchSpatialExpCI_0.1) != 0){
    spatialNames <- list()
    
    for(i in 1:length(mismatchSpatialExpCI_0.1)){
      xmlText <- sapply(mismatchSpatialExpCI_0.1[i], xmlValue)
      
      timeAttr <- sapply(mismatchSpatialExpCI_0.1[i], xmlGetAttr, "time")
      timeAttr <- gsub("/|:", "-", timeAttr)
      sapply(mismatchSpatialExpCI_0.1[i], addAttributes,"time"=timeAttr)
      
      popAttr <- sapply(mismatchSpatialExpCI_0.1[i], xmlGetAttr, "pop")
      popAttr <- gsub("/|:", "-", popAttr)
      sapply(mismatchSpatialExpCI_0.1[i], addAttributes,"pop"=popAttr)
      
      spatialNames[i] <- paste(timeAttr, popAttr)
      
      try({
        mismatchSpatial_0.1[[i]] <- mismatch(xmlText)
      })
    }
    names(mismatchSpatial_0.1) <- spatialNames
  }
  
  
  # get mismatchSpatialExp ---------------------------------
  mismatchSpatialExp <- getNodeSet(doc, "//mismatchSpatialExp")

  if(length(mismatchSpatialExp) != 0){
    source(paste(sourcePath, "mismatch_Graphic.r", sep=""))

    for(i in 1:length(mismatchSpatialExp)){
      xmlText <- sapply(mismatchSpatialExp[i], xmlValue)

      timeAttr <- sapply(mismatchSpatialExp[i], xmlGetAttr, "time")
      timeAttr <- gsub("/|:", "-", timeAttr)
      sapply(mismatchSpatialExp[i], addAttributes,"time"=timeAttr)

      popAttr <- sapply(mismatchSpatialExp[i], xmlGetAttr, "pop")
      popAttr <- gsub("/|:", "-", popAttr)
      sapply(mismatchSpatialExp[i], addAttributes,"pop"=popAttr)

      spatialName <- paste(timeAttr, popAttr)
      
      try({
        mismatchMatrix <- mismatch(xmlText)
        
        mismatch_Graphic(mismatchMatrix, mismatchSpatial_0.01[[spatialName]],
                            mismatchSpatial_0.05[[spatialName]],
                            mismatchSpatial_0.1[[spatialName]],
                            spatial=TRUE, timeAttr, popAttr, outfiles)

        sapply(mismatchSpatialExp[i], addAttributes, "graphicExist"="yes")
      })
    }
  }

  
  # demographic mismatch -------------------------------------------------------
  mismatchDemog_0.01 <- list()
  mismatchDemog_0.05 <- list()
  mismatchDemog_0.1 <- list()
  
  # get mismatchDemogExpCI_0.010000 ----------------------
  
  mismatchDemogExpCI_0.01 <- getNodeSet(doc, "//mismatchDemogExpCI_0.010000")
  
  if(length(mismatchDemogExpCI_0.01) != 0){
    demogNames <- list() 
    
    for(i in 1:length(mismatchDemogExpCI_0.01)){
      xmlText <- sapply(mismatchDemogExpCI_0.01[i], xmlValue)
      
      timeAttr <- sapply(mismatchDemogExpCI_0.01[i], xmlGetAttr, "time")
      timeAttr <- gsub("/|:", "-", timeAttr)
      sapply(mismatchDemogExpCI_0.01[i], addAttributes,"time"=timeAttr)
      
      popAttr <- sapply(mismatchDemogExpCI_0.01[i], xmlGetAttr, "pop")
      popAttr <- gsub("/|:", "-", popAttr)
      sapply(mismatchDemogExpCI_0.01[i], addAttributes,"pop"=popAttr)
      
      demogNames[i] <- paste(timeAttr, popAttr)
      
      try({
        mismatchDemog_0.01[[i]] <- mismatch(xmlText)
      })
    }
    names(mismatchDemog_0.01) <- demogNames
  }
  
  
  # get mismatchDemogExpCI_0.050000 -----------------------
  mismatchDemogExpCI_0.05 <- getNodeSet(doc, "//mismatchDemogExpCI_0.050000")
  
  if(length(mismatchDemogExpCI_0.05) != 0){
    
    for(i in 1:length(mismatchDemogExpCI_0.05)){
      xmlText <- sapply(mismatchDemogExpCI_0.05[i], xmlValue)
      
      timeAttr <- sapply(mismatchDemogExpCI_0.05[i], xmlGetAttr, "time")
      timeAttr <- gsub("/|:", "-", timeAttr)
      sapply(mismatchDemogExpCI_0.05[i], addAttributes,"time"=timeAttr)
      
      popAttr <- sapply(mismatchDemogExpCI_0.05[i], xmlGetAttr, "pop")
      popAttr <- gsub("/|:", "-", popAttr)
      sapply(mismatchDemogExpCI_0.05[i], addAttributes,"pop"=popAttr)
      
      try({
        mismatchDemog_0.05[[i]] <- mismatch(xmlText)
      })
    }
    names(mismatchDemog_0.05) <- demogNames
  }
  
  
  # get mismatchDemogExpCI_0.100000 ------------------------
  mismatchDemogExpCI_0.1 <- getNodeSet(doc, "//mismatchDemogExpCI_0.100000")
  
  if(length(mismatchDemogExpCI_0.1) != 0){
    
    for(i in 1:length(mismatchDemogExpCI_0.1)){
      xmlText <- sapply(mismatchDemogExpCI_0.1[i], xmlValue)
      
      timeAttr <- sapply(mismatchDemogExpCI_0.1[i], xmlGetAttr, "time")
      timeAttr <- gsub("/|:", "-", timeAttr)
      sapply(mismatchDemogExpCI_0.1[i], addAttributes,"time"=timeAttr)
      
      popAttr <- sapply(mismatchDemogExpCI_0.1[i], xmlGetAttr, "pop")
      popAttr <- gsub("/|:", "-", popAttr)
      sapply(mismatchDemogExpCI_0.1[i], addAttributes,"pop"=popAttr)
      
     try({
        mismatchDemog_0.1[[i]] <- mismatch(xmlText)
      })
    }
    names(mismatchDemog_0.1) <- demogNames
  }
  

  # get mismatchDemogExp ---------------------------------
  mismatchDemogExp <- getNodeSet(doc, "//mismatchDemogExp")

  if(length(mismatchDemogExp) != 0){
    source(paste(sourcePath, "mismatch_Graphic.r", sep=""))

    for(i in 1:length(mismatchDemogExp)){
      xmlText <- sapply(mismatchDemogExp[i], xmlValue)

      timeAttr <- sapply(mismatchDemogExp[i], xmlGetAttr, "time")
      timeAttr <- gsub("/|:", "-", timeAttr)
      sapply(mismatchDemogExp[i], addAttributes,"time"=timeAttr)

      popAttr <- sapply(mismatchDemogExp[i], xmlGetAttr, "pop")
      popAttr <- gsub("/|:", "-", popAttr)
      sapply(mismatchDemogExp[i], addAttributes,"pop"=popAttr)

      demogName <- paste(timeAttr, popAttr)

      try({
        mismatchMatrix <- mismatch(xmlText)

        mismatch_Graphic(mismatchMatrix, mismatchDemog_0.01[[demogName]],
                            mismatchDemog_0.05[[demogName]],
                            mismatchDemog_0.1[[demogName]],
                            spatial=FALSE, timeAttr, popAttr, outfiles)

        sapply(mismatchDemogExp[i], addAttributes, "graphicExist"="yes")
      })
    }
  }
  
  
  
  # get pairDistPopLabels ------------------------------------------------------
  LabelsList <- list()
  pairDistPopLabels <- getNodeSet(doc, "//pairDistPopLabels")
  DuplicateAttrLabelsList <- list()
  
  if(length(pairDistPopLabels) != 0){
    source(paste(sourcePath, "popLabels.r", sep=""))
    LabelNames <- list()
    
    for(i in 1:length(pairDistPopLabels)){
      xmlText <- sapply(pairDistPopLabels[i], xmlValue)
      
      timeAttr <- sapply(pairDistPopLabels[i], xmlGetAttr, "time")
      timeAttr <- gsub("/|:", "-", timeAttr)
      
      try({
        LabelsList[[i]] <- popLabels(xmlText)
        LabelNames[[i]] <- timeAttr  
      })               
    }
        
    names(LabelsList) <- LabelNames

    # check for duplicates in timeAttr ----------
    uniqueTimeAttr <- unique(LabelNames)

    for(k in uniqueTimeAttr){
        indexList <- vector()
        for(l in 1:length(LabelNames)){
            if(LabelNames[l] == k){
                indexList[[length(indexList)+1]] <- l
            }
        }
        DuplicateAttrLabelsList[[length(DuplicateAttrLabelsList)+1]] <- indexList
    }
    names(DuplicateAttrLabelsList) <- uniqueTimeAttr
  }


  # get PairFstMat -------------------------------------------------------------
  PairFstMat <- getNodeSet(doc, "//PairFstMat")
  
  if(length(PairFstMat) != 0){
    source(paste(sourcePath, "pairFstMatrix.r", sep=""))  

    timeAttrList <- vector()
    for(i in 1:length(PairFstMat)){    
      xmlText <- sapply(PairFstMat[i], xmlValue)
      
      timeAttr <- sapply(PairFstMat[i], xmlGetAttr, "time")
      timeAttr <- gsub("/|:", "-", timeAttr)
      sapply(PairFstMat[i], addAttributes,"time"=timeAttr)
      
      if(any(timeAttrList == timeAttr) && length(DuplicateAttrLabelsList[[timeAttr]]) > 1){
         tryCatch({
            pairFstMatrix(xmlText, LabelsList[[DuplicateAttrLabelsList[[timeAttr]][2]]], paste(timeAttr, "_2", sep=""), outfiles)

            sapply(PairFstMat[i], addAttributes, "graphicExist"="yes")
         })
      } else {
         try({
            pairFstMatrix(xmlText, LabelsList[[timeAttr]], timeAttr, outfiles)

            sapply(PairFstMat[i], addAttributes, "graphicExist"="yes")
        })
      }
      timeAttrList[i] <- timeAttr
    }
  }
  
  
  # get obsHapFreq -------------------------------------------------------------
  obsHapFreqList <- list()
  obsHapFreq <- getNodeSet(doc, "//obsHapFreq")
  
  if(length(obsHapFreq) != 0){
    source(paste(sourcePath, "observedHapFreq.r", sep=""))
    obsHapAttrList <- list()
    
    for(i in 1:length(obsHapFreq)){
      xmlText <- sapply(obsHapFreq[i], xmlValue)
      
      timeAttr <- sapply(obsHapFreq[i], xmlGetAttr, "time")
      timeAttr <- gsub("/|:", "-", timeAttr)
      
      popAttr <- sapply(obsHapFreq[i], xmlGetAttr, "pop") 
      popAttr <- gsub("/|:", "-", popAttr)
      sapply(obsHapFreq[i], addAttributes, "pop"=popAttr)
      
      try({    
        obsHapFreqList[[i]] <- observedHapFreq(xmlText)
        obsHapAttrList[[i]] <- paste(popAttr, timeAttr)
      })
    }
    
    names(obsHapFreqList) <- obsHapAttrList
  }


  # get expHapFreq -------------------------------------------------------------
  expHapFreq <- getNodeSet(doc, "//expHapFreq")
  
  if(length(expHapFreq) != 0){
    source(paste(sourcePath, "expectedHapFreq.r", sep=""))
    
    for(i in 1:length(expHapFreq)){
      xmlText <- sapply(expHapFreq[i], xmlValue)
      
      timeAttr <- sapply(expHapFreq[i], xmlGetAttr, "time")
      timeAttr <- gsub("/|:", "-", timeAttr)
      sapply(expHapFreq[i], addAttributes, "time"=timeAttr)
      
      popAttr <- sapply(expHapFreq[i], xmlGetAttr, "pop")
      popAttr <- gsub("/|:", "-", popAttr)
      sapply(expHapFreq[i], addAttributes, "pop"=popAttr)
      
      try({
        expectedHapFreq(xmlText, obsHapFreqList[[paste(popAttr, timeAttr)]],
                         timeAttr, popAttr, outfiles)
 
        sapply(expHapFreq[i], addAttributes, "graphicExist"="yes")
      })
    }
  }

  
  # get absHapFreq -------------------------------------------------------------
  absHapFreq <- getNodeSet(doc, "//absHapFreq")
  absHapList <- list()
  absHapAttr <- list()
  
  if(length(absHapFreq) != 0){
    source(paste(sourcePath, "absoluteHapFreq.r", sep=""))
    
    for(i in 1:length(absHapFreq)){
      absHapFrequency <- NULL
      
      xmlText <- sapply(absHapFreq[i], xmlValue)
      
      timeAttr <- sapply(absHapFreq[i], xmlGetAttr, "time")
      timeAttr <- gsub("/|:", "-", timeAttr)
      sapply(absHapFreq[i], addAttributes, "time"=timeAttr)
      
      try({
        absHapFrequency <- absoluteHapFreq(xmlText)
        
        # add data to the list ------------                                               
        if(!is.null(absHapFrequency)){
          absHapList[[i]] <- absHapFrequency          
          absHapAttr[[i]] <- timeAttr
        }
      })
    }
    
    #name the lists with the time attributes
    names(absHapList) <- absHapAttr
  }


  # get hapDistMatrix and hapDistMatrixLabels ----------------------------------    
  hapDistMatrix <- getNodeSet(doc, "//hapDistMatrix")
  hapDistMatrixLabels <- getNodeSet(doc, "//hapDistMatrixLabels")      
  
  if(length(hapDistMatrix) != 0){
    source(paste(sourcePath, "haplotypeDistMatrix.r", sep=""))
    source(paste(sourcePath, "hapLabels.r", sep=""))
    
    for(i in 1:length(hapDistMatrix)){
      HapLabels <- NULL
      distanceMatrix <- NULL
      
      labelText <- sapply(hapDistMatrixLabels[i], xmlValue)
      
      timeAttr <- sapply(hapDistMatrix[i], xmlGetAttr, "time")
      timeAttr <- gsub("/|:", "-", timeAttr)
      sapply(hapDistMatrix[i], addAttributes, "time"=timeAttr)
      
      xmlText <- sapply(hapDistMatrix[i], xmlValue) 
      
      try({
        HapLabels <- hapLabels(labelText)      
        distanceMatrix <- haplotypeDistMatrix(xmlText, HapLabels,timeAttr, 
                                                     outfiles)
                                                     
        sapply(hapDistMatrix[i], addAttributes, "graphicExist"="yes")    
          
                                                     
        # if absHapMatrix exist
        # draw a  haplotype matrix plot between/within populations  ------------                                               
        if(!is.null(distanceMatrix) & !is.null(absHapList[timeAttr][[1]])){
          source(paste(sourcePath,"hapDistMatrix_withinBetweenComplete.r",
                        sep=""))        
        
          hapDistMatrix_withinBetweenComplete(HapLabels, distanceMatrix,
                                   absHapList[[timeAttr]], timeAttr, outfiles)
          
          sapply(hapDistMatrix[i], addAttributes,"completeGraphicExist"="yes")          
        }
      })   
    }
  }   
  
  
  # get interHapDistMatrix and interHapDistMatrixLabels ------------------------
  interHapDistMatrix <- getNodeSet(doc, "//interHapDistMatrix")
  interHapDistMatrixLabels <- getNodeSet(doc, "//interHapDistMatrixLabels") 
  
  if(length(interHapDistMatrix) != 0){
    source(paste(sourcePath, "interHaplotypeDistMatrix.r", sep=""))
    source(paste(sourcePath, "interHapLabels.r", sep=""))
   
    for(i in 1:length(interHapDistMatrix)){    
      timeAttr <- sapply(interHapDistMatrix[i], xmlGetAttr, "time")
      timeAttr <- gsub("/|:", "-", timeAttr)
      sapply(interHapDistMatrix[i], addAttributes, "time"=timeAttr)
      
      #get interHapDistMatrix Labels ------------------
      xmlText <- sapply(interHapDistMatrixLabels[i], xmlValue)
      
      try({
        InterHapLabels <- interHapLabels(xmlText)
        
        #get interHapDistMatrix Data ---------------------
        xmlText <- sapply(interHapDistMatrix[i], xmlValue)
        
        popAttr <- sapply(interHapDistMatrix[i], xmlGetAttr, "pop")
        popAttr <- gsub("/|:", "-", popAttr)
        sapply(interHapDistMatrix[i], addAttributes, "pop"=popAttr)
        
        distMatrix <- interHaplotypeDistMatrix(xmlText, InterHapLabels,timeAttr,
                                                  popAttr, outfiles)
                                                  
        sapply(interHapDistMatrix[i], addAttributes, "graphicExist"="yes")
      
      })
    }
  }

  
  # get genotPopLabels ---------------------------------------------------------
  genotPopLabelsList <- list()
  genotPopLabels <- getNodeSet(doc, "//genotPopLabels")
  
  if(length(genotPopLabels) != 0){
    source(paste(sourcePath, "popLabels.r", sep=""))
    genotPopLabelNames <- list()
    
    for(i in 1:length(genotPopLabels)){
      xmlText <- sapply(genotPopLabels[i], xmlValue)
      timeAttr <- sapply(genotPopLabels[i], xmlGetAttr, "time")
      timeAttr <- gsub("/|:", "-", timeAttr)
      
      try({
        genotPopLabelsList[[i]] <- popLabels(xmlText)       
        genotPopLabelNames[[i]] <- timeAttr
      })
    }
    
    #names the genotPopLabels list
    names(genotPopLabelsList) <- genotPopLabelNames
  }
  
  
  # get genotypeLikelihoodMatrix -----------------------------------------------
  genotypeLikelihoodMatrix <- getNodeSet(doc, "//genotypeLikelihoodMatrix")
  
  if(length(genotypeLikelihoodMatrix) != 0){
    source(paste(sourcePath, "genotLikelihoodMatrix.r", sep=""))
    
    for(i in 1:length(genotypeLikelihoodMatrix)){
      xmlText <- sapply(genotypeLikelihoodMatrix[i], xmlValue)
      
      timeAttr <- sapply(genotypeLikelihoodMatrix[i], xmlGetAttr, "time")
      timeAttr <- gsub("/|:", "-", timeAttr)
      sapply(genotypeLikelihoodMatrix[i], addAttributes, "time"=timeAttr)
      
      popAttr <- sapply(genotypeLikelihoodMatrix[i], xmlGetAttr, "pop")
      popAttr <- gsub("/|:", "-", popAttr)
      sapply(genotypeLikelihoodMatrix[i], addAttributes, "pop"=popAttr)
      
      try({
        genotLikelihoodMatrix(xmlText, genotPopLabelsList[[timeAttr]], timeAttr,
                               popAttr, outfiles)
                               
        sapply(genotypeLikelihoodMatrix[i],addAttributes,"graphicExist"="yes") 
      })
    }
  } 
  

  # get relHapFreq -------------------------------------------------------------
  relHapFreq <- getNodeSet(doc, "//relHapFreq")
  
  if(length(relHapFreq) != 0){
    source(paste(sourcePath, "relativeHapFreq.r", sep=""))
    
    for(i in 1:length(relHapFreq)){
      xmlText <- sapply(relHapFreq[i], xmlValue)
      
      timeAttr <- sapply(relHapFreq[i], xmlGetAttr, "time")
      timeAttr <- gsub("/|:", "-", timeAttr)
      sapply(relHapFreq[i], addAttributes, "time"=timeAttr)
      
      try({
        relativeHapFreq(xmlText, timeAttr, outfiles)

        sapply(relHapFreq[i] ,addAttributes, "graphicExist"="yes")
      })
    }
  }


  # get sumExpHeterozygosity ---------------------------------------------------
  sumExpHeterozygosity <- getNodeSet(doc, "//sumExpHeterozygosity")
  
  if(length(sumExpHeterozygosity) != 0){
    source(paste(sourcePath, "sumExpectedHeterozygosity.r", sep=""))
    
    for(i in 1:length(sumExpHeterozygosity)){
      xmlText <- sapply(sumExpHeterozygosity[i], xmlValue)
      
      timeAttr <- sapply(sumExpHeterozygosity[i], xmlGetAttr, "time")
      timeAttr <- gsub("/|:", "-", timeAttr)
      sapply(sumExpHeterozygosity[i], addAttributes, "time"=timeAttr)
      
      try({
        sumExpectedHeterozygosity(xmlText, timeAttr, outfiles)
        
        sapply(sumExpHeterozygosity[i] ,addAttributes, "graphicExist"="yes")
      })
    }
  }
  
  
  # get sumAllelicSizeRange ----------------------------------------------------
  sumAllelicSizeRange <- getNodeSet(doc, "//sumAllelicSizeRange")
  
  if(length(sumAllelicSizeRange) != 0){
    source(paste(sourcePath, "sumAllelicSizeRangeFunction.r", sep=""))
    
    for(i in 1:length(sumAllelicSizeRange)){
      xmlText <- sapply(sumAllelicSizeRange[i], xmlValue)
      
      timeAttr <- sapply(sumAllelicSizeRange[i], xmlGetAttr, "time")
      timeAttr <- gsub("/|:", "-", timeAttr)
      sapply(sumAllelicSizeRange[i], addAttributes, "time"=timeAttr)
      
      try({
        sumAllelicSizeRangeFunction(xmlText, timeAttr, outfiles)
         
        sapply(sumAllelicSizeRange[i], addAttributes, "graphicExist"="yes")
      })
    }
  }
  

  # get sumNumAlleles ----------------------------------------------------------
  sumNumAlleles <- getNodeSet(doc, "//sumNumAlleles")
  
  if(length(sumNumAlleles) != 0){
    source(paste(sourcePath, "sumNumAllelesFunction.r", sep=""))
    
    for(i in 1:length(sumNumAlleles)){
      xmlText <- sapply(sumNumAlleles[i], xmlValue)
      
      timeAttr <- sapply(sumNumAlleles[i], xmlGetAttr, "time")
      timeAttr <- gsub("/|:", "-", timeAttr)
      sapply(sumNumAlleles[i], addAttributes, "time"=timeAttr)
      
      try({
        sumNumAllelesFunction(xmlText, timeAttr, outfiles)
         
        sapply(sumNumAlleles[i], addAttributes, "graphicExist"="yes")
      })
    }
  }
  
  
  # get sumGWIndex -------------------------------------------------------------
  sumGWIndex <- getNodeSet(doc, "//sumGWIndex")
  
  if(length(sumGWIndex) != 0){
    source(paste(sourcePath, "sumGWIndexFunction.r", sep=""))
    
    for(i in 1:length(sumGWIndex)){
      xmlText <- sapply(sumGWIndex[i], xmlValue)
      
      timeAttr <- sapply(sumGWIndex[i], xmlGetAttr, "time")
      timeAttr <- gsub("/|:", "-", timeAttr)
      sapply(sumGWIndex[i], addAttributes, "time"=timeAttr)
      
      try({
        sumGWIndexFunction(xmlText, timeAttr, outfiles)
         
        sapply(sumGWIndex[i], addAttributes, "graphicExist"="yes")
      })
    }
  }
  
  
  # get sumModGWIndex ----------------------------------------------------------
  sumModGWIndex <- getNodeSet(doc, "//sumModGWIndex")
  
  if(length(sumModGWIndex) != 0){
    source(paste(sourcePath, "sumModGWIndexFunction.r", sep=""))
    
    for(i in 1:length(sumModGWIndex)){
      xmlText <- sapply(sumModGWIndex[i], xmlValue)
      
      timeAttr <- sapply(sumModGWIndex[i], xmlGetAttr, "time")
      timeAttr <- gsub("/|:", "-", timeAttr)
      sapply(sumModGWIndex[i], addAttributes, "time"=timeAttr)
      
      try({
        sumModGWIndexFunction(xmlText, timeAttr, outfiles)
         
        sapply(sumModGWIndex[i], addAttributes, "graphicExist"="yes")
      })
    }
  }
  
  
  # get sumThetaH --------------------------------------------------------------
  sumThetaH <- getNodeSet(doc, "//sumThetaH")
  
  if(length(sumThetaH) != 0){
    source(paste(sourcePath, "sumThetaHFunction.r", sep=""))
    
    for(i in 1:length(sumThetaH)){
      xmlText <- sapply(sumThetaH[i], xmlValue)
      
      timeAttr <- sapply(sumThetaH[i], xmlGetAttr, "time")
      timeAttr <- gsub("/|:", "-", timeAttr)
      sapply(sumThetaH[i], addAttributes, "time"=timeAttr)
      
      try({
        sumThetaHFunction(xmlText, timeAttr, outfiles)
         
        sapply(sumThetaH[i], addAttributes, "graphicExist"="yes")
      })
    }
  }
  
  
  # get sumMolecDivIndexes -----------------------------------------------------
  sumMolecDivIndexes <- getNodeSet(doc, "//sumMolecDivIndexes")
  
  if(length(sumMolecDivIndexes) != 0){
    source(paste(sourcePath, "sumMolecularDivIndexes.r", sep=""))
    
    for(i in 1:length(sumMolecDivIndexes)){
      xmlText <- sapply(sumMolecDivIndexes[i], xmlValue)
      
      timeAttr <- sapply(sumMolecDivIndexes[i], xmlGetAttr, "time")
      timeAttr <- gsub("/|:", "-", timeAttr)
      sapply(sumMolecDivIndexes[i], addAttributes, "time"=timeAttr)
      
      try({
        sumMolecularDivIndexes(xmlText,timeAttr, outfiles)
        
        sapply(sumMolecDivIndexes[i] ,addAttributes, "graphicExist"="yes")
      })
    }
  }


  # get tauMatrix --------------------------------------------------------------
  tauMatrix <- getNodeSet(doc, "//tauMatrix")
  
  if(length(tauMatrix) != 0){
    source(paste(sourcePath, "tauMatrixFunction.r", sep=""))
    
    timeAttrList <- vector()
    for(i in 1:length(tauMatrix)){
      xmlText <- sapply(tauMatrix[i], xmlValue)
      
      timeAttr <- sapply(tauMatrix[i], xmlGetAttr, "time")
      timeAttr <- gsub("/|:", "-", timeAttr)
      sapply(tauMatrix[i], addAttributes, "time"=timeAttr)
      
      if(any(timeAttrList == timeAttr) && length(DuplicateAttrLabelsList[[timeAttr]]) > 1){
         tryCatch({
            tauMatrixFunction(xmlText, LabelsList[[DuplicateAttrLabelsList[[timeAttr]][2]]], paste(timeAttr, "_2", sep=""), outfiles)

            sapply(tauMatrix[i], addAttributes, "graphicExist"="yes")
         })
      } else {
          try({
            tauMatrixFunction(xmlText, LabelsList[[timeAttr]], timeAttr, outfiles)

            sapply(tauMatrix[i] ,addAttributes, "graphicExist"="yes")
          })
      }
      timeAttrList[i] <- timeAttr
    }
  }


  # get pairwiseDifferenceMatrix -----------------------------------------------
  pairwiseDifferenceMatrix <- getNodeSet(doc, "//pairwiseDifferenceMatrix")
  
  if(length(pairwiseDifferenceMatrix) != 0){
    source(paste(sourcePath, "pairwiseDiffMatrix.r", sep=""))
    
    timeAttrList <- vector()
    for(i in 1:length(pairwiseDifferenceMatrix)){
      xmlText <- sapply(pairwiseDifferenceMatrix[i], xmlValue)
      
      timeAttr <- sapply(pairwiseDifferenceMatrix[i], xmlGetAttr, "time")
      timeAttr <- gsub("/|:", "-", timeAttr)
      sapply(pairwiseDifferenceMatrix[i], addAttributes, "time"=timeAttr)
      
      if(any(timeAttrList == timeAttr) && length(DuplicateAttrLabelsList[[timeAttr]]) > 1){
         tryCatch({
            pairwiseDiffMatrix(xmlText, LabelsList[[DuplicateAttrLabelsList[[timeAttr]][2]]], paste(timeAttr, "_2", sep=""), outfiles)

            sapply(pairwiseDifferenceMatrix[i],addAttributes,"graphicExist"="yes")
         })
      } else {
          try({
            pairwiseDiffMatrix(xmlText, LabelsList[[timeAttr]], timeAttr, outfiles)

            sapply(pairwiseDifferenceMatrix[i],addAttributes,"graphicExist"="yes")
          })
      }
      timeAttrList[i] <- timeAttr
    }
  }


  # get ancestralPopSize -------------------------------------------------------
  ancestralPopSize <- getNodeSet(doc, "//ancestralPopSize")
  
  if(length(ancestralPopSize) != 0){
    source(paste(sourcePath, "ancestralPopulationSize.r", sep=""))
    
    timeAttrList <- vector()
    for(i in 1:length(ancestralPopSize)){
      xmlText <- sapply(ancestralPopSize[i], xmlValue)
      
      timeAttr <- sapply(ancestralPopSize[i], xmlGetAttr, "time")
      timeAttr <- gsub("/|:", "-", timeAttr)
      sapply(ancestralPopSize[i], addAttributes, "time"=timeAttr)
      
      if(any(timeAttrList == timeAttr) && length(DuplicateAttrLabelsList[[timeAttr]]) > 1){
         tryCatch({
            ancestralPopulationSize(xmlText, LabelsList[[DuplicateAttrLabelsList[[timeAttr]][2]]], paste(timeAttr, "_2", sep=""), outfiles)

            sapply(ancestralPopSize[i], addAttributes, "graphicExist"="yes")
         })
      } else {
          try({
            ancestralPopulationSize(xmlText, LabelsList[[timeAttr]], timeAttr,
                                      outfiles)

            sapply(ancestralPopSize[i] ,addAttributes, "graphicExist"="yes")
          })
      }
      timeAttrList[i] <- timeAttr
    }
  }


  # get coancestryCoefficients -------------------------------------------------
  coancestryCoefficients <- getNodeSet(doc, "//coancestryCoefficients")
  
  if(length(coancestryCoefficients) != 0){
    source(paste(sourcePath, "coancestryCoeff.r", sep=""))
    
    timeAttrList <- vector()
    for(i in 1:length(coancestryCoefficients)){
      xmlText <- sapply(coancestryCoefficients[i], xmlValue)
      
      timeAttr <- sapply(coancestryCoefficients[i], xmlGetAttr, "time")
      timeAttr <- gsub("/|:", "-", timeAttr)
      sapply(coancestryCoefficients[i], addAttributes, "time"=timeAttr)
      
      if(any(timeAttrList == timeAttr) && length(DuplicateAttrLabelsList[[timeAttr]]) > 1){
         tryCatch({
            coancestryCoeff(xmlText, LabelsList[[DuplicateAttrLabelsList[[timeAttr]][2]]], paste(timeAttr, "_2", sep=""), outfiles)

            sapply(coancestryCoefficients[i], addAttributes, "graphicExist"="yes")
         })
      } else {
          try({
            coancestryCoeff(xmlText, LabelsList[[timeAttr]], timeAttr, outfiles)

            sapply(coancestryCoefficients[i] ,addAttributes, "graphicExist"="yes")
          })
      }
      timeAttrList[i] <- timeAttr
    }
  }


  # get slatkinFst -------------------------------------------------------------
  slatkinFst <- getNodeSet(doc, "//slatkinFst")
  
  if(length(slatkinFst) != 0){
    source(paste(sourcePath, "slatkinFstFunction.r", sep=""))
    
    timeAttrList <- vector()
    for(i in 1:length(slatkinFst)){
      xmlText <- sapply(slatkinFst[i], xmlValue)
      
      timeAttr <- sapply(slatkinFst[i], xmlGetAttr, "time")
      timeAttr <- gsub("/|:", "-", timeAttr)
      sapply(slatkinFst[i], addAttributes, "time"=timeAttr)
      
      if(any(timeAttrList == timeAttr) && length(DuplicateAttrLabelsList[[timeAttr]]) > 1){
         tryCatch({
            slatkinFstFunction(xmlText, LabelsList[[DuplicateAttrLabelsList[[timeAttr]][2]]], paste(timeAttr, "_2", sep=""), outfiles)

            sapply(slatkinFst[i], addAttributes, "graphicExist"="yes")
         })
      } else {
          try({
            slatkinFstFunction(xmlText, LabelsList[[timeAttr]], timeAttr, outfiles)

            sapply(slatkinFst[i] ,addAttributes, "graphicExist"="yes")
          })
      }
      timeAttrList[i] <- timeAttr
    }
  }
  
  
  # get FST CI -----------------------------------------------------------------
  fst_ci <- getNodeSet(doc, "//detSel_FST_CI")
  fst_ci_List <- list()
  fst_ci_Attr <- list()
  
  if(length(fst_ci) != 0){
    source(paste(sourcePath, "fst_ci_List.r", sep=""))
    
    for(i in 1:length(fst_ci)){
      fst_ci_values <- NULL
      
      xmlText <- sapply(fst_ci[i], xmlValue)
      
      timeAttr <- sapply(fst_ci[i], xmlGetAttr, "time")
      timeAttr <- gsub("/|:", "-", timeAttr)
      sapply(fst_ci[i], addAttributes, "time"=timeAttr)
      
      try({
        fst_ci_values <- fst_ci_List_Function(xmlText)
        
        # add data to the list ------------                                               
        if(!is.null(fst_ci_values)){
          fst_ci_List[[i]] <- fst_ci_values          
          fst_ci_Attr[[i]] <- timeAttr
        }         
      })
    }     
    #name the lists with the time attributes
    names(fst_ci_List) <- fst_ci_Attr
  }
  
  
  # get FCT_CI -----------------------------------------------------------------
  fct_ci <- getNodeSet(doc, "//detSel_FCT_CI")
  fct_ci_List <- list()
  fct_ci_Attr <- list()
  
  if(length(fct_ci) != 0){
    source(paste(sourcePath, "fct_ci_List.r", sep=""))
    
    for(i in 1:length(fct_ci)){
      fStat_Pvalues <- NULL
      
      xmlText <- sapply(fct_ci[i], xmlValue)
      
      timeAttr <- sapply(fct_ci[i], xmlGetAttr, "time")
      timeAttr <- gsub("/|:", "-", timeAttr)
      sapply(fct_ci[i], addAttributes, "time"=timeAttr)
      
      try({
        fct_ci_values <- fct_ci_List_Function(xmlText)
        
        # add data to the list ------------                                               
        if(!is.null(fct_ci_values)){
          fct_ci_List[[i]] <- fct_ci_values          
          fct_ci_Attr[[i]] <- timeAttr
        }        
      })
    }     
    #name the lists with the time attributes
    names(fct_ci_List) <- fct_ci_Attr
  }
  
  
  
  # get fStat_Pval -------------------------------------------------------------
  fStat_Pval <- getNodeSet(doc, "//detSel_FStat_Pval")
  
  if(length(fStat_Pval) != 0){
    source(paste(sourcePath, "fStat_Pvalues_Func.r", sep=""))
    
    for(i in 1:length(fStat_Pval)){
      fStat_Pvalues <- NULL
      
      xmlText <- sapply(fStat_Pval[i], xmlValue)
      
      timeAttr <- sapply(fStat_Pval[i], xmlGetAttr, "time")
      timeAttr <- gsub("/|:", "-", timeAttr)
      sapply(fStat_Pval[i], addAttributes, "time"=timeAttr)
      
      try({
        fStat_Pvalues <- fStat_Pvalues_Func(xmlText)
                                              
        # if fst_ci_List exist
        # draw a  loci selection graph  ------------                                               
        if(!is.null(fStat_Pvalues) & !is.null(fst_ci_List[timeAttr][[1]])){
          source(paste(sourcePath,"lociSelection.r", sep=""))        
        
          lociSelection(fStat_Pvalues, fst_ci_List[[timeAttr]], timeAttr,
                         outfiles, "FST")
          
          sapply(fStat_Pval[i],addAttributes,"fst_Selection_GraphicExist"="yes")
        }
        
        # if fct_ci_List exist
        # draw a  loci selection graph  ------------                        
        if(!is.null(fStat_Pvalues) & !is.null(fct_ci_List[timeAttr][[1]])){
          source(paste(sourcePath,"lociSelection.r", sep=""))        
        
          lociSelection(fStat_Pvalues, fct_ci_List[[timeAttr]], timeAttr,
                         outfiles, "FCT")
          
          sapply(fStat_Pval[i],addAttributes,"fct_Selection_GraphicExist"="yes")
        }

      })
    }
  }
  
  
  #save the changed input file
  saveXML(doc, file=infile)
  
}