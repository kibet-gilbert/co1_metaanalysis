################################################################
## R function for niche modeling and projection into the past 
##
## Dependencies: R/3.3.2
##				  java/1.7.0
##				  Maxent
##
## R dependencies: raster, rworldmap, biomod2, maxent, maps, rgdal, 
##					 rgdalUtils
##				     custom R function "_function.calcOverlap.R"
##
## Input Data: txt file with geographic coordinates
##			   Raster files of environmental variables [http://www.worldclim.org/bioclim]
##
## Author: Ariadna Morales [ariadna.biologia@gmail.com]
##		   Some sections of the modeling using biomod were adapted from 
##         a previous script by Tara Pelletier
##
## 		   Before using this version look at https://github.com/ariadnamorales
## 		   there might be an updated version of this function.
##
## Last saved version: May 26th, 2017
##
## WARNINGS: maxent.jar MUST be in workingPath
##           requires a custom function "_function.calcOverlap.R"
##			  this file contains paths for osu0942 in OSC
##
#################################################################


##------> Arguments
# speciesName				-----> Genus and specific epithet separated by "_", example: "Myotis_brandtii"
# workingPath				-----> Ouput files and folders will be created in this folder
# pathInputFiles			-----> txt file with geographic coordinates and a header "Latitude" and "Longitude"
# pathBioClim_present		-----> path to files with environmental variables, must be rater files with extension ".bil"
# pathBioClim_LGM			-----> path to files with environmental variables, must be rater files with extension ".tif"
# path_function.calcOverlap


#######################
## Example of usage  ##
#######################
##
## --------> WARNING: paths exclusive for osu0942 in OSC
## --------> modules required to be loaded manually in owens to run this function
##
##	module load R/3.3.2
##	module load java/1.7.0
##	export PATH=/users/PAS1201/osu0942/local/bin:$PATH
##	export LD_LIBRARY_PATH=/users/PAS1201/osu0942/local/lib:$LD_LIBRARY_PATH
##	export GDAL_DATA=/users/PAS1201/osu0942/local/share/gdal
##	R
##
##  workingPath<-"/users/PAS1201/osu0942/nicheModelsBats"
##  pathInputFiles<-"/users/PAS1201/osu0942/nicheModelsBats/InputFiles/uniqueGPS_bats/"
##  pathBioClim_present<-"/users/PAS1201/osu0942/BIOCLIM/current_30secs/"
##  pathBioClim_LGM<-"/users/PAS1201/osu0942/BIOCLIM/lgm_2.5min/"
##  path_function.calcOverlap<-"/users/PAS1201/osu0942/nicheModelsBats/_customFunctions/"
##
##  source("/users/PAS1201/osu0942/nicheModelsBats/_customFunctions/_function.nicheModellingProject.R")
##  speciesName<-"Cyttarops_alecto"
##  nicheModelingPresent.ProjectionPast(speciesName=speciesName, workingPath=workingPath, pathInputFiles=pathInputFiles, pathBioClim_present=pathBioClim_present, pathBioClim_LGM=pathBioClim_LGM)
##
####################### -------> example ends


##------------------------> START function
nicheModelingPresent.ProjectionPast<-function(speciesName, workingPath, pathInputFiles, pathBioClim_present, pathBioClim_LGM, path_function.calcOverlap){
		
		## set working path
		setwd(workingPath)
		print(paste("Working path:", workingPath,sep=""))

				## Print species name
		print(paste("------------------Starting analyses for ",speciesName,"------------------", sep=""))
		
		##create an outputFolder 
		## note: maxent changes "_" by ".", threfore gsub should be used to match file names
		system(paste("mkdir ", workingPath,"/",gsub("_", ".", speciesName),sep=""))
		system(paste("mkdir ", workingPath,"/",gsub("_", ".", speciesName),"/maps",sep=""))

				
		## Load libraries
		print(paste("Loading libraries",sep=""))
		library(raster)
		library(rworldmap)
		library(biomod2)
		library(maxent)
		library(maps)
		library(rgdal)
		library(gdalUtils)
		source(paste0(path_function.calcOverlap, "_function.calcOverlap.R")

			
		## Import bioclim data ---->present
		print(paste("Loading bioclim data ---> present",sep=""))
		for (i in 1:19){	
			var <-raster(paste(pathBioClim_present,"bio_",i,".bil",sep=""))
			assign(paste('bio',i,'w',sep=""),var,env=.GlobalEnv)	
		}

		## Read gps points file	---> This files were filtered before and contain only unique gps points, however this function will double-check
		print(paste("Loading gps points",sep=""))
		gps<-read.table(file=paste(pathInputFiles,speciesName,".txt",sep=""), header=TRUE)
			##format table
			#colnames(gps)[1]<-"genus"
			#colnames(gps)[2]<-"species"
			#colnames(gps)[3]<-"Latitude"		#order in GBIF
			#colnames(gps)[4]<-"Longitude"		#order in GBIF

			## remove duplicated coordinates
			gps<-subset(gps, !duplicated(Latitude))
			print("duplicated points removed")
			
			# save tables with unique GPS points
			#write.table(gps, file=paste(workingPath,"/",gsub("_", ".", speciesName),"/",speciesName,"_uniqueGPSpoints.txt",sep=""), quote=FALSE, row.names=FALSE)

		## Get min and max lat and lon values - add 10 degrees to each for mapping/projection
		## assumes points represent full range of species
		## points should be in order LON, LAT for R analyses
		max_lat <- max(gps$Latitude)
		high_lat <- max_lat + 10
		min_lat <- min(gps$Latitude)
		low_lat <-  min_lat - 10
		max_lon <- max(gps$Longitude)
		high_lon <- max_lon + 10
		min_lon <- min(gps$Longitude)
		low_lon <-	min_lon - 10

		## check points ----> A map of the gps poinst could saved
			#wmap <- getMap(resolution = "low") ---------> test with species[255] Tadarida brasiliensis
			#pdf(file=paste(workingPath,"/",gsub("_", ".", speciesName),"/maps/gpsPoints_",speciesName,".pdf",sep=""), 8,8)
			#	plot(wmap, xlim = c(low_lon, high_lon), ylim = c(low_lat, high_lat), asp=1) 
			#	points(gps$Longitude, gps$Latitude, col="red", pch=20, cex=0.5)
			#dev.off()

		## Clip biolayers
		print(paste("Clipping bioclim layers ----> present",sep=""))
		range = extent(c(low_lon, high_lon, low_lat, high_lat))
			bio_1 = crop(bio1w, range)
			bio_2 = crop(bio2w, range)
			bio_3 = crop(bio3w, range)
			bio_4 = crop(bio4w, range)
			bio_5 = crop(bio5w, range)
			bio_6 = crop(bio6w, range)
			bio_7 = crop(bio7w, range)
			bio_8 = crop(bio8w, range)
			bio_9 = crop(bio9w, range)
			bio_10 = crop(bio10w, range)
			bio_11 = crop(bio11w, range)
			bio_12 = crop(bio12w, range)
			bio_13 = crop(bio13w, range)
			bio_14 = crop(bio14w, range)
			bio_15 = crop(bio15w, range)
			bio_16 = crop(bio16w, range)
			bio_17 = crop(bio17w, range)
			bio_18 = crop(bio18w, range)
			bio_19 = crop(bio19w, range)

		bios = stack(bio_1, bio_2, bio_3, bio_4, bio_5, bio_6, bio_7, bio_8, bio_9, bio_10, bio_11, bio_12, bio_13, bio_14, bio_15, bio_16, bio_17, bio_18, bio_19)
		list = names(bios)
					
		# remove world layers to free memory space
		for(i in 1:19){
			rm(list=paste("bio",i,"w", sep=""))
		}
		print(paste("World bioclim layers were removed to free memory space----> present",sep=""))

		## Get correlations among variables and remove the high ones
		print(paste("Getting data ready for niche modeling",sep=""))
		
		## this step is SLOW
		s <- layerStats(bios, stat = "pearson", na.rm=TRUE)

		## function to convert cor matrix into useable list
		flattenSquareMatrix <- function(m) {
   			if( (class(m) != "matrix") | (nrow(m) != ncol(m))) stop("Must be a square matrix.") 
   			if(!identical(rownames(m), colnames(m))) stop("Row and column names must be equal.")
   				ut <- upper.tri(m)
   				data.frame(i = rownames(m)[row(m)[ut]],
              	j = rownames(m)[col(m)[ut]],
              	cor=t(m)[ut],
              	p=m[ut])
 		}

		## Determine which variables to drop
		f <-flattenSquareMatrix(s$'pearson correlation coefficient')
		g <- subset (f, f$cor > 0.7 | f$cor < -0.7)
		remove <- sort(unique(g$i))

		## Make new list of bioclim variables for modeling
		new_list_q <- list[! list %in% remove]
		new_list <- noquote(paste(new_list_q, collapse=", "))

		## Save list of retained variables
		write.table(new_list, file=paste(workingPath,"/",gsub("_", ".", speciesName),"/retained_bioClimVar_",speciesName,".txt",sep=""), quote=FALSE, row.names=FALSE, col.names=FALSE)

		#bioclim layers file
		bioclim = eval(parse(text = paste("stack(",new_list,")")))

		#format gps points for niche modeling
		gpspoints<- gps[c(4,3)]		#Longitude Latitude
		gpspoints<- gpspoints[!duplicated(gpspoints),]
		n<-nrow(gpspoints)

		#make presence data information
		p<-matrix(1, ncol = 1, nrow = n)

		############################
		## Niche modeling PRESENT ##
		############################
		print(paste("Performing niche modeling ----> present",sep=""))
		BiomodData <- BIOMOD_FormatingData(resp.var = p,
                                     expl.var = bioclim,
                                     resp.xy = gpspoints,
                                     resp.name = paste(speciesName,sep=""),
                                     PA.nb.rep=1,
                                     PA.nb.absences=10000, 			#test with 2000
                                     PA.strategy="random")

		print(paste("_______ BiomodData DONE",sep="")) 
		
		#define parameters for MaxEnt model
		BiomodOption <- BIOMOD_ModelingOptions(
  				MAXENT.Phillips = list( 
  				#path_to_maxent.jar = pathMaxent,
  				memory_allocated=1024,
                 maximumiterations = 1000,				### test with 100, use 1000 in analyses
                 visible = FALSE,
                 linear = TRUE,
                 quadratic = TRUE,
                 product = TRUE,
                 threshold = TRUE,
                 hinge = TRUE,
                 lq2lqptthreshold = 80,
                 l2lqthreshold = 10,
                 hingethreshold = 15,
                 beta_threshold = -1, 
                 beta_categorical = -1,
                 beta_lqp = -1,
                 beta_hinge = -1,
                 defaultprevalence = 0.5 ))
        print(paste("_______ BiomodOption DONE",sep="")) 

		#run the model with pseudo absences   --> maxent.jar file MUST be in working dir
		BiomodModelOut <- BIOMOD_Modeling(
  			BiomodData,
  			models = c('MAXENT.Phillips'),
  			models.options = BiomodOption,
  			NbRunEval=1,
  			DataSplit=70, #30% of the samples are set aside for model evaluation
  			VarImport=1, #number of iterations for evaluation variable importance
  			models.eval.meth = c('ROC', 'TSS'),
  			SaveObj = TRUE,
  			rescal.all.models = TRUE,
  			modeling.id = speciesName)
  		print(paste("_______ BiomodModel DONE",sep="")) 
  
		#get model evalutaion values
		evals <- get_evaluations(BiomodModelOut) 
		tss <- evals[2,1,1,2,1]
		roc <- evals[1,1,1,2,1]

		#get bioclim variable importance values
		variables<-get_variables_importance(BiomodModelOut)
		v<-variables[,,2,]
		v<-sort(v, decreasing=TRUE)

		#outputdata we might want later
		write.table(data.frame(n, tss, roc), file = paste(workingPath,"/",gsub("_", ".", speciesName),"/niche_values_",speciesName,".txt",sep=""), sep = "\t", row.names=FALSE, col.names=FALSE, quote=FALSE)
		write.table(v, file = paste(workingPath,"/",gsub("_", ".", speciesName),"/bioclim_values_",speciesName,".txt",sep=""), col.names=FALSE, quote=FALSE, sep = "\t")
		print(paste("_______ output tables were saved",sep="")) 

		#project model on geographic space
		projection <- BIOMOD_Projection(
  			modeling.output = BiomodModelOut,
  			new.env = bioclim,
  			proj.name = "current",
  			selected.models = 'all',
  			binary.meth = NULL,
  			compress = 'xz',
  			build.clamping.mask = FALSE,
  			output.format = '.img')
  		print(paste("_______ BIOMOD_Projection DONE",sep="")) 

		#export the projection to a raster ascii file for later use
		## note: maxent changes "_" by ".", threfore gsub should be used to match file names
		current <- raster(paste(workingPath,"/",gsub("_", ".", speciesName),"/proj_current/proj_current_",gsub("_", ".", speciesName),".img", sep=""))
		# add matrix with probabilities of coccurrence values per cell to raster
		values(current)<-c(projection@proj@val@layers[[2]]@data@values)
		writeRaster(current, file=paste(workingPath,"/",gsub("_", ".", speciesName),"/proj_current/",gsub("_", ".", speciesName),"_current",sep=""), format="ascii", overwrite=T)
		
		print(paste("_______ raster files were saved ---> present",sep=""))
		

		# define color palette for maps ----> (grey, yellow, red)
		lm.palette <- colorRampPalette(c("#636363", "#fee08b", "#b2182b"), space = "rgb")

		#plot points and projection on map and send to file
		pdf(paste(workingPath,"/",gsub("_", ".", speciesName),"/maps/",gsub("_", ".", speciesName),"_mapPresent.pdf",sep=""))
			plot(current, xlab="longitude", ylab="latitude", col=lm.palette(10), main=bquote(italic(.(speciesName))))
			points(gps$Longitude, gps$Latitude, col="blue", pch=20, cex=1)
			map(xlim = c(low_lon, high_lon), ylim = c(low_lat, high_lat), add=TRUE)
			mtext("SDM present", side=3)
		dev.off()
		print("_______ Map of projection saved ---> present")



		##################################
		## Project model into the past  ##
		##################################
		
		print(paste("Performing projection ----> present to LGM",sep=""))
		
		### Last GlaciaL Maximum
		## Import bioclim data
		print(paste("Loading and clipping bioclim data ---> LGM",sep=""))
		for (i in 1:19){	
			var <-raster(paste(pathBioClim_LGM,"mrlgmbi",i,".tif",sep=""))
			assign(paste('lgm_bio',i,'w',sep=""),var,env=.GlobalEnv)			#rename biolayers   e.g.-----> "bio1w_lgm"
			var_clipped<-crop(var, range)										#clip biolayers
			assign(paste('lgm_bio',i,sep=""),var_clipped,env=.GlobalEnv)     #rename clipped biolayers   e.g.-----> "bio1_lgm"					
		}

		## remove world layers to free memory space
		for(i in 1:19){
			rm(list=paste("lgm_bio",i,"w", sep=""))
		}
		print(paste("World bioclim layers were removed to free memory space----> LGM",sep=""))

		## Get bioclim variables for model
		lgm_list<-gsub("bio_", "lgm_bio", new_list)

		## Stack bioclim layers file
		lgm_bioclim = eval(parse(text = paste("stack(",lgm_list,")")))
		names(lgm_bioclim)<-c(new_list_q)


		## Project model     --------> LGM
		lgm_projection <- BIOMOD_Projection(
  			modeling.output = BiomodModelOut,
  			new.env = lgm_bioclim,
  			proj.name = "lgm",
  			selected.models = 'all',
  			binary.meth = NULL,
  			compress = 'xz',
  			build.clamping.mask = FALSE,
  			output.format = '.img')
  			print(paste("_______ BIOMOD_Projection LGM DONE",sep=""))
  
		## Export the projection to a raster ascii file for later use
		lgm <- raster(paste(workingPath,"/",gsub("_", ".", speciesName),"/proj_lgm/proj_lgm_",gsub("_", ".", speciesName),".img",sep=""))

		## Add matrix with probabilities of occurrence values per cell to raster
		values(lgm)<-c(lgm_projection@proj@val@layers[[2]]@data@values)
		writeRaster(lgm, file=paste(workingPath,"/",gsub("_", ".", speciesName),"/proj_lgm/",gsub("_", ".", speciesName),"_lgm",sep=""), format="ascii", overwrite=T) 
		print(paste("_______ raster files were saved ---> LGM",sep=""))

		## define color palette for maps ----> (grey, yellow, red)
		lm.palette <- colorRampPalette(c("#636363", "#fee08b", "#b2182b"), space = "rgb")

		## Plot points and projection on map and send to file
		pdf(paste(workingPath,"/",gsub("_", ".", speciesName),"/maps/",gsub("_", ".", speciesName),"_mapLGM.pdf",sep=""))
			plot(lgm, xlab="longitude", ylab="latitude", col=lm.palette(10), main=bquote(italic(.(speciesName))))
			points(gps$Longitude, gps$Latitude, col="blue", pch=20, cex=1)
			map(xlim = c(low_lon, high_lon), ylim = c(low_lat, high_lat), add=TRUE)
			mtext("SDM Last Glacial Maximum", side=3)
		dev.off()
		print("_______ Map of projection saved ---> LGM")
		
		## Plot both present and LGM projections
		pdf(paste(workingPath,"/",gsub("_", ".", speciesName),"/maps/",gsub("_", ".", speciesName),"_maps_Present-LGM.pdf",sep=""), width=14, height=7)
			attach(mtcars)
		par(mfrow=c(1,2)) 
			## plot present
			plot(current, xlab="longitude", ylab="latitude", col=lm.palette(10), main=bquote(italic(.(speciesName))))
				points(gps$Longitude, gps$Latitude, col="blue", pch=20, cex=1)
				map(xlim = c(low_lon, high_lon), ylim = c(low_lat, high_lat), add=TRUE)
				mtext("SDM present", side=3)
		
			## plot LGM
			plot(lgm, xlab="longitude", ylab="latitude", col=lm.palette(10), main=bquote(italic(.(speciesName))))
				points(gps$Longitude, gps$Latitude, col="blue", pch=20, cex=1)
				map(xlim = c(low_lon, high_lon), ylim = c(low_lat, high_lat), add=TRUE)
				mtext("SDM Last Glacial Maximum", side=3)
			dev.off()
			print("_______ Map of both projections saved")

		##############################################################
		## Calculate overlap of two raster files (present and LGM) ##
		##############################################################
		print("Calculating area of overlapping")
		
		## This function will save a map and a table with areas of overlap
		calculateAreaOverlap(speciesName=speciesName, d=current, b=lgm, threshold_probSDM=500, MIN_lon=low_lon, MAX_lon=high_lon, MIN_lat=low_lat, MAX_lat=high_lat, gpsPoints=gps)
		calculateAreaOverlap(speciesName=speciesName, d=current, b=lgm, threshold_probSDM=700, MIN_lon=low_lon, MAX_lon=high_lon, MIN_lat=low_lat, MAX_lat=high_lat, gpsPoints=gps)
		calculateAreaOverlap(speciesName=speciesName, d=current, b=lgm, threshold_probSDM=900, MIN_lon=low_lon, MAX_lon=high_lon, MIN_lat=low_lat, MAX_lat=high_lat, gpsPoints=gps)

		###########################################
		## Save all output objects in a RDA file ##
		###########################################
		save(list=c(ls()),file=paste(workingPath,"/",gsub("_", ".", speciesName),"/outR_", speciesName,".rda",sep=""))
		print(paste("-------- END of calculations (all objects were saved in a RDA file)--------",sep=""))
}
##------------------------> END function