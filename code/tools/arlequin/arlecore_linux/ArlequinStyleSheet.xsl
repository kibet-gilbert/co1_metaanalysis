<?xml version="1.0"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">


 <xsl:template match="/">
  <html>
   <body>
    <xsl:apply-templates/>
   </body>
  </html>
 </xsl:template>


 <!-- header style xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx -->
 <xsl:template match="header">
  <h2 style="font-family:'Courier New'">
   <xsl:call-template name="cr2br">
    <xsl:with-param name="text" select="text()" />
   </xsl:call-template>
  </h2>
 </xsl:template>


 <!-- run style xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx -->
 <xsl:template match="run">
  <p style="font-weight:bold; font-size:13px; font-family:'Courier New' ; color:blue">
   <xsl:call-template name="cr2br">
    <xsl:with-param name="text" select="text()" />
   </xsl:call-template>
  </p>
 </xsl:template> 

 
 <!-- title style xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx -->
 <xsl:template match="title">
  <p style="font-family:'Courier New'; font-size:13px;">
   <xsl:call-template name="cr2br">
    <xsl:with-param name="text" select="text()" />
   </xsl:call-template>
  </p>
 </xsl:template>


 <!-- title2 style xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx -->
 <xsl:template match="title2">
  <p style="font-weight:bold; font-family:'Courier New'">
   <xsl:call-template name="cr2br">
    <xsl:with-param name="text" select="text()" />
   </xsl:call-template>
  </p>
 </xsl:template>


 <!-- A stylexxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx -->
 <xsl:template match="A">
  <A>
    <xsl:attribute name="NAME">
      <xsl:value-of select="@NAME" />
    </xsl:attribute>   
   </A>
 </xsl:template>


 <!-- reference style xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx -->
 <xsl:template match="Reference">
  <i style="color:gray; font-size:13px; font-family:'Courier New'">
   <xsl:call-template name="cr2br">
    <xsl:with-param name="text" select="text()" />
   </xsl:call-template>
  </i>
  <br />
 </xsl:template>


 <!-- data style xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx -->
 <xsl:template match="data">
  <PRE>
   <xsl:value-of select="."/>
  </PRE>
 </xsl:template>





 <!-- special tags to extract data with R xxxxxxxxxxxxxxxxx -->

 <xsl:template match="mismatchSpatialExp">
  <PRE>
   <xsl:value-of select="."/>
  </PRE>

  <!-- insert graphic if it exist -->
  <xsl:if test="@graphicExist">
    <xsl:variable name="filePath">Graphics/spatialMismatch <xsl:value-of select="@pop"/>_<xsl:value-of select="@time"/>.png</xsl:variable>
    <img src="{$filePath}" alt="" width ="650" height="650" />
  </xsl:if>

 </xsl:template>



 <xsl:template match="mismatchSpatialExpCI_0.010000">
  <PRE>
   <xsl:value-of select="."/>
  </PRE>
 </xsl:template>



 <xsl:template match="mismatchSpatialExpCI_0.050000">
  <PRE>
   <xsl:value-of select="."/>
  </PRE>
 </xsl:template>



 <xsl:template match="mismatchSpatialExpCI_0.100000">
  <PRE>
   <xsl:value-of select="."/>
  </PRE>
 </xsl:template>



 <xsl:template match="mismatchDemogExp">
  <PRE>
   <xsl:value-of select="."/>
  </PRE>

  <!-- insert graphic if it exist -->
  <xsl:if test="@graphicExist">
    <xsl:variable name="filePath">Graphics/demographicMismatch <xsl:value-of select="@pop"/>_<xsl:value-of select="@time"/>.png</xsl:variable>
    <img src="{$filePath}" alt="" width ="650" height="650"/>
  </xsl:if>

 </xsl:template>



 <xsl:template match="mismatchDemogExpCI_0.010000">
  <PRE>
   <xsl:value-of select="."/>
  </PRE>
 </xsl:template>



 <xsl:template match="mismatchDemogExpCI_0.050000">
  <PRE>
   <xsl:value-of select="."/>
  </PRE>
 </xsl:template>



 <xsl:template match="mismatchDemogExpCI_0.100000">
  <PRE>
   <xsl:value-of select="."/>
  </PRE>
 </xsl:template>



 <xsl:template match="exactTestPopLabels">
  <PRE>
   <xsl:value-of select="."/>
  </PRE>
 </xsl:template>



 <xsl:template match="pairDistPopLabels">
  <PRE>
   <xsl:value-of select="."/>
  </PRE>
 </xsl:template>


 <xsl:template match="PairFstMat">
  <PRE>
   <xsl:value-of select="."/>
  </PRE>

  <!-- insert graphic if it exist -->
  <xsl:if test="@graphicExist">
    <xsl:variable name="filePath">Graphics/pairFstMatrix <xsl:value-of select="@time"/>.png</xsl:variable>
    <img src="{$filePath}" alt="" width ="650" height="650"/>
  </xsl:if>

 </xsl:template>


 <xsl:template match="PairFstPvalMat">
  <PRE>
   <xsl:value-of select="."/>
  </PRE>
 </xsl:template>



 <xsl:template match="obsHapFreq">
  <PRE>
   <xsl:value-of select="."/>
  </PRE>
 </xsl:template>


 <xsl:template match="expHapFreq">
  <PRE>
   <xsl:value-of select="."/>
  </PRE>

  <!-- insert graphic if it exist -->
  <xsl:if test="@graphicExist">
    <xsl:variable name="filePath">Graphics/obsExpHapFreq <xsl:value-of select="@pop"/>_<xsl:value-of select="@time"/>.png</xsl:variable>
    <img src="{$filePath}" alt="" width ="650" height="650"/>
  </xsl:if>

 </xsl:template>



 <xsl:template match="hapDistMatrix">
  <PRE>
   <xsl:value-of select="."/>
  </PRE>

  <!-- insert hapDist graphic if it exist -->
  <xsl:if test="@graphicExist">
    <xsl:variable name="filePath">Graphics/hapDistMatrix <xsl:value-of select="@time"/>.png</xsl:variable>
    <img src="{$filePath}" alt="" width ="650" height="650"/>
  </xsl:if> 

  <!-- insert hapDist graphic between/ within pop if it exist -->
  <xsl:if test="@completeGraphicExist">
    <xsl:variable name="filePath">Graphics/hapDistMatrix_withinBetweenComplete <xsl:value-of select="@time"/>.png</xsl:variable>
    <img src="{$filePath}" alt="" width ="650" height="650"/>
  </xsl:if> 
    
 </xsl:template>


 <xsl:template match="hapDistMatrixLabels">
  <PRE>
   <xsl:value-of select="."/>
  </PRE>
 </xsl:template>



 <xsl:template match="interHapDistMatrix">
  <PRE>
   <xsl:value-of select="."/>
  </PRE>

  <!-- insert graphic if it exist -->
  <xsl:if test="@graphicExist">
    <xsl:variable name="filePath">Graphics/interHapDistMatrix <xsl:value-of select="@pop"/>_<xsl:value-of select="@time"/>.png</xsl:variable>
    <img src="{$filePath}" alt="" width ="650" height="650"/>
  </xsl:if> 

 </xsl:template>


 <xsl:template match="interHapDistMatrixLabels">
  <PRE>
   <xsl:value-of select="."/>
  </PRE>
 </xsl:template>



 <xsl:template match="genotPopLabels">
  <PRE>
   <xsl:value-of select="."/>
  </PRE>
 </xsl:template>


 <xsl:template match="genotypeLikelihoodMatrix">
  <PRE>
   <xsl:value-of select="."/>
  </PRE>

  <!-- insert graphic if it exist -->
  <xsl:if test="@graphicExist">
    <xsl:variable name="filePath">Graphics/genotLikelihoodMatrix <xsl:value-of select="@pop"/>_<xsl:value-of select="@time"/>.png</xsl:variable>
    <img src="{$filePath}" alt="" width ="650" height="650"/>
  </xsl:if> 

 </xsl:template>



 <xsl:template match="absHapFreq">
  <PRE>
   <xsl:value-of select="."/>
  </PRE> 
 </xsl:template>



 <xsl:template match="relHapFreq">
  <PRE>
   <xsl:value-of select="."/>
  </PRE>

  <!-- insert graphic if it exist -->
  <xsl:if test="@graphicExist">
    <xsl:variable name="filePath">Graphics/relativeHapFreq <xsl:value-of select="@time"/>.png</xsl:variable>
    <img src="{$filePath}" alt="" width ="650" height="650"/>
  </xsl:if> 
 
 </xsl:template>



 <xsl:template match="sumAllelicSizeRange">
  <PRE>
   <xsl:value-of select="."/>
  </PRE>

  <!-- insert graphic if it exist -->
  <xsl:if test="@graphicExist">
    <xsl:variable name="filePath">Graphics/sumAllelicSizeRange <xsl:value-of select="@time"/>.png</xsl:variable>
    <img src="{$filePath}" alt="" width ="650" height="650"/>
  </xsl:if>

 </xsl:template>



 <xsl:template match="sumExpHeterozygosity">
  <PRE>
   <xsl:value-of select="."/>
  </PRE>

  <!-- insert graphic if it exist -->
  <xsl:if test="@graphicExist">
    <xsl:variable name="filePath">Graphics/sumExpectedHeterozygosity <xsl:value-of select="@time"/>.png</xsl:variable>
    <img src="{$filePath}" alt="" width ="650" height="650"/>
  </xsl:if> 
 
 </xsl:template>



 <xsl:template match="sumNumAlleles">
  <PRE>
   <xsl:value-of select="."/>
  </PRE>

  <!-- insert graphic if it exist -->
  <xsl:if test="@graphicExist">
    <xsl:variable name="filePath">Graphics/sumNumAlleles <xsl:value-of select="@time"/>.png</xsl:variable>
    <img src="{$filePath}" alt="" width ="650" height="650"/>
  </xsl:if>

 </xsl:template>



 <xsl:template match="sumGWIndex">
  <PRE>
   <xsl:value-of select="."/>
  </PRE>

  <!-- insert graphic if it exist -->
  <xsl:if test="@graphicExist">
    <xsl:variable name="filePath">Graphics/sumGWIndex <xsl:value-of select="@time"/>.png</xsl:variable>
    <img src="{$filePath}" alt="" width ="650" height="650"/>
  </xsl:if>

 </xsl:template>



 <xsl:template match="sumModGWIndex">
  <PRE>
   <xsl:value-of select="."/>
  </PRE>

  <!-- insert graphic if it exist -->
  <xsl:if test="@graphicExist">
    <xsl:variable name="filePath">Graphics/sumModGWIndex <xsl:value-of select="@time"/>.png</xsl:variable>
    <img src="{$filePath}" alt="" width ="650" height="650"/>
  </xsl:if>

 </xsl:template>



 <xsl:template match="sumThetaH">
  <PRE>
   <xsl:value-of select="."/>
  </PRE>

  <!-- insert graphic if it exist -->
  <xsl:if test="@graphicExist">
    <xsl:variable name="filePath">Graphics/sumThetaH <xsl:value-of select="@time"/>.png</xsl:variable>
    <img src="{$filePath}" alt="" width ="650" height="650"/>
  </xsl:if>

 </xsl:template>



 <xsl:template match="sumMolecDivIndexes">
  <PRE>
   <xsl:value-of select="."/>
  </PRE>

  <!-- insert graphic if it exist -->
  <xsl:if test="@graphicExist">
    <xsl:variable name="filePath">Graphics/sumMolecularDivIndexes <xsl:value-of select="@time"/>.png</xsl:variable>
    <img src="{$filePath}" alt="" width ="650" height="650"/>
  </xsl:if> 

 </xsl:template>



 <xsl:template match="tauMatrix">
  <PRE>
   <xsl:value-of select="."/>
  </PRE>

  <!-- insert graphic if it exist -->
  <xsl:if test="@graphicExist">
    <xsl:variable name="filePath">Graphics/tauMatrixFunction <xsl:value-of select="@time"/>.png</xsl:variable>
    <img src="{$filePath}" alt="" width ="650" height="650"/>
  </xsl:if> 

 </xsl:template>



 <xsl:template match="pairwiseDifferenceMatrix">
  <PRE>
   <xsl:value-of select="."/>
  </PRE>

  <!-- insert graphic if it exist -->
  <xsl:if test="@graphicExist">
    <xsl:variable name="filePath">Graphics/pairwiseDiffMatrix <xsl:value-of select="@time"/>.png</xsl:variable>
    <img src="{$filePath}" alt="" width ="650" height="650"/>
  </xsl:if> 

 </xsl:template>



 <xsl:template match="ancestralPopSize">
  <PRE>
   <xsl:value-of select="."/>
  </PRE>

  <!-- insert graphic if it exist -->
  <xsl:if test="@graphicExist">
    <xsl:variable name="filePath">Graphics/ancestralPopulationSize <xsl:value-of select="@time"/>.png</xsl:variable>
    <img src="{$filePath}" alt="" width ="650" height="650"/>
  </xsl:if> 
 
 </xsl:template>



 <xsl:template match="coancestryCoefficients">
  <PRE>
   <xsl:value-of select="."/>
  </PRE>

  <!-- insert graphic if it exist -->
  <xsl:if test="@graphicExist">
    <xsl:variable name="filePath">Graphics/coancestryCoeff <xsl:value-of select="@time"/>.png</xsl:variable>
    <img src="{$filePath}" alt="" width ="650" height="650"/>
  </xsl:if> 

 </xsl:template>



 <xsl:template match="slatkinFst">
  <PRE>
   <xsl:value-of select="."/>
  </PRE> 

  <!-- insert graphic if it exist -->
  <xsl:if test="@graphicExist">
    <xsl:variable name="filePath">Graphics/slatkinFstFunction <xsl:value-of select="@time"/>.png</xsl:variable>
    <img src="{$filePath}" alt="" width ="650" height="650"/>
  </xsl:if> 

 </xsl:template>


 <xsl:template match="detSel_FST_CI">
  <PRE>
   <xsl:value-of select="."/>
  </PRE> 
 </xsl:template>


 <xsl:template match="detSel_FCT_CI">
  <PRE>
   <xsl:value-of select="."/>
  </PRE>
 </xsl:template>


 <xsl:template match="detSel_FStat_Pval">
  <PRE>
   <xsl:value-of select="."/>
  </PRE>
  <!-- insert graphic if it exist -->
  <xsl:if test="@fst_Selection_GraphicExist">
    <xsl:variable name="filePath">Graphics/lociSelection_FST <xsl:value-of select="@time"/>.png</xsl:variable>
    <xsl:variable name="filePath2">Graphics/lociSelection_Names_FST <xsl:value-of select="@time"/>.png</xsl:variable>
    <img src="{$filePath}" alt="" width ="650" height="650"/>
    <img src="{$filePath2}" alt="" width ="650" height="650"/>
  </xsl:if> 

  <xsl:if test="@fct_Selection_GraphicExist">
    <xsl:variable name="filePath">Graphics/lociSelection_FCT <xsl:value-of select="@time"/>.png</xsl:variable>
    <xsl:variable name="filePath2">Graphics/lociSelection_Names_FCT <xsl:value-of select="@time"/>.png</xsl:variable>
    <img src="{$filePath}" alt="" width ="650" height="650" />
    <img src="{$filePath2}" alt="" width ="650" height="650" />
  </xsl:if> 
 
 </xsl:template>



 <!-- replace all carriage returns with <br /> xxxxxxxxxxxxxxxxxxxx -->
 <xsl:template name="cr2br">
  <xsl:param name="text" />
  <xsl:choose>
   <xsl:when test="contains($text, '&#xA;')">
    <xsl:value-of select="substring-before($text, '&#xA;')" />
    <br />
    <xsl:call-template name="cr2br">
    <xsl:with-param name="text" select="substring-after($text, '&#xA;')" />
    </xsl:call-template>
   </xsl:when>
   <xsl:otherwise>
    <xsl:value-of select="$text" />
   </xsl:otherwise>
  </xsl:choose>
 </xsl:template>


</xsl:stylesheet>