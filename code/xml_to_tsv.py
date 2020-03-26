#!/usr/bin/env python

"""This script converts the xml records of downloaded bold_data.xml files to bold_data.tsv file format files with 80 columns"""


import os
import sys
import numpy
import pandas as pd
import lxml
from bs4 import BeautifulSoup as b

#To parse a document, pass it into the BeautifulSoup constructor. You can pass in a string or an open filehandle

def xml2tsv( *bold_xml_file ):
    """ 
    This function converts a BOLD.xml file to a BOLD.TSV file. The BOLD input file is the file downloaded from BOLD in XML format.
    Usage: xml2tsv [file1 file2 file3...filen]
    """
    for var in bold_xml_file:
        #for each_file in files_xlm:
        #with open((sys.argv[1]), "r") as f: # opening xml filehandle
        with open(var, "r") as f:
            content = f.read()
            soup = b(content, "xml")    
            
            list1 = []
            for values in (soup.find_all("record")):
                #record_id
                try:
                    recordID = values.find("record_id").text
                except AttributeError:
                    recordID = ""
                #processid
                try:
                    processid = values.find("processid").text
                except AttributeError:
                    processid = ""
                #bin_uri
                try:
                    bin_uri = values.find("bin_uri").text
                except AttributeError:
                    bin_uri = ""
                #sampleid
                try:
                    sampleid = values.find("sampleid").text
                except AttributeError:
                    sampleid = ""
                #catalognum
                try:
                    catalognum = values.find("catalognum").text
                except AttributeError:
                    catalognum = ""
                #fieldnum
                try:
                    fieldnum = values.find("fieldnum").text
                except AttributeError:
                    fieldnum = ""
                #collection_code
                try:
                    collection_code = values.find("collection_code").text
                except AttributeError:
                    collection_code = ""
                #institution_storing
                try:
                    institution_storing = values.find("institution_storing").text
                except AttributeError:
                    institution_storing = ""
                #identification_provided_by
                try:
                    identification_provided_by = values.find("identification_provided_by").text
                except AttributeError:
                    identification_provided_by = ""
                #phylum_taxID
                try:
                    phylum_taxID = values.phylum.find("taxID").text
                except AttributeError:
                    phylum_taxID = ""
                #phylum_name
                try:
                    phylum_name = values.phylum.find("name").text
                except AttributeError:
                    phylum_name = ""
                #Class
                try:
                    class_taxID = values.Class.find("taxID").text
                except AttributeError:
                    class_taxID = ""
                try:
                    class_name = values.Class.find("name").text
                except AttributeError:
                    class_name = ""
                #order
                try:
                    order_taxID = values.order.find("taxID").text
                except AttributeError:
                    order_taxID = ""
                try:
                    order_name = values.order.find("name").text
                except AttributeError:
                    order_name = ""
                #family
                try:
                    family_taxID = values.family.find("taxID").text
                except AttributeError:
                    family_taxID = ""
                try:
                    family_name = values.family.find("name").text
                except AttributeError:
                    family_name = ""
                #subfamily
                try:
                    subfamily_taxID = values.subfamily.find("taxID").text
                except AttributeError:
                    subfamily_taxID = ""
                try:
                    subfamily_name = values.subfamily.find("name").text
                except AttributeError:
                    subfamily_name = ""
                #genus
                try:
                    genus_taxID = values.genus.find("taxID").text
                except AttributeError:
                    genus_taxID = ""
                try:
                    genus_name = values.genus.find("name").text
                except AttributeError:
                    genus_name = ""
                #species
                try:
                    species_taxID = values.species.find("taxID").text
                except AttributeError:
                    species_taxID = ""
                try:
                    species_name = values.species.find("name").text
                except AttributeError:
                    species_name = ""
                #subspecies
                try:
                    subspecies_taxID = values.subspecies.find("taxID").text
                except AttributeError:
                    subspecies_taxID = ""
                try:
                    subspecies_name = values.subspecies.find("name").text
                except AttributeError:
                    subspecies_name = ""
                #identification_method
                try:
                    identification_method = values.find("method").text
                except AttributeError:
                    identification_method = ""
                #identification_reference
                try:
                    identification_reference = values.find("reference").text
                except AttributeError:
                    identification_reference = ""
                #tax_note
                try:
                    tax_note = values.find("note").text
                except AttributeError:
                    tax_note = ""
                #voucher_status
                try:
                    voucher_status = values.find("voucher_status").text
                except AttributeError:
                    voucher_status = ""
                #tissue_type
                try:
                    tissue_type = values.find("tissue_type").text
                except AttributeError:
                    tissue_type = ""
                #collection_event_id
                try:
                    collection_event_id = values.find("collection_event_id").text
                except AttributeError:
                    collection_event_id = ""
                #collectors
                try:
                    collectors = values.find("collectors").text
                except AttributeError:
                    collectors = ""
                #collectiondate_start
                try:
                    collectiondate_start = values.find("collectiondate_start").text
                except AttributeError:
                    collectiondate_start = ""
                #collectiondate_end
                try:
                    collectiondate_end = values.find("collectiondate_end").text
                except AttributeError:
                    collectiondate_end = ""
                #collectiontime
                try:
                    collectiontime = values.find("collectiontime").text
                except AttributeError:
                    collectiontime = ""
                #collection_note
                try:
                    collection_note = values.find("collection_note").text
                except AttributeError:
                    collection_note = ""
                #site_code
                try:
                    site_code = values.find("site_code").text
                except AttributeError:
                    site_code = ""
                #sampling_protocol
                try:
                    sampling_protocol = values.find("sampling_protocol").text
                except AttributeError:
                    sampling_protocol = ""
                #lifestage
                try:
                    lifestage = values.find("lifestage").text
                except AttributeError:
                    lifestage = ""
                #sex
                try:
                    sex = values.find("sex").text
                except AttributeError:
                    sex = ""
                #reproduction
                try:
                    reproduction = values.find("reproduction").text
                except AttributeError:
                    reproduction = ""
                #habitat
                try:
                    habitat = values.find("habitat").text
                except AttributeError:
                    habitat = ""
                #associated_spacimens
                try:
                    associated_specimens = values.find("associated_specimens").text
                except AttributeError:
                    associated_specimens = ""
                #associated_taxa
                try:
                    associated_taxa = values.find("associated_taxa").text
                except AttributeError:
                    associated_taxa = ""
                #extrainfo
                try:
                    extrainfo = values.find("extrainfo").text
                except AttributeError:
                    extrainfo = ""
                #notes
                try:
                    notes = values.find("notes").text
                except AttributeError:
                    notes = ""
                #lat
                try:
                    lat = values.find("lat").text
                except AttributeError:
                    lat = ""
                #lon
                try:
                    lon = values.find("lon").text
                except AttributeError:
                    lon = ""
                #coord_source
                try:
                    coord_source = values.find("coord_source").text
                except AttributeError:
                    coord_source = ""
                #coord_accuracy
                try:
                    coord_accuracy = values.find("coord_accuracy").text
                except AttributeError:
                    coord_accuracy = ""
                #elev
                try:
                    elev = values.find("elev").text
                except AttributeError:
                    elev = ""
                #depth
                try:
                    depth = values.find("depth").text
                except AttributeError:
                    depth = ""
                #elev_accuracy
                try:
                    elev_accuracy = values.find("elev_accuracy").text
                except AttributeError:
                    elev_accuracy = ""
                #depth_accuracy
                try:
                    depth_accuracy = values.find("depth_accuracy").text
                except AttributeError:
                    depth_accuracy = ""
                #country
                try:
                    country = values.find("country").text
                except AttributeError:
                    country = ""
                #province_state
                try:
                    province_state = values.find("province_state").text
                except AttributeError:
                    province_state = ""
                #region
                try:
                    region = values.find("region").text
                except AttributeError:
                    region = ""
                #sector
                try:
                    sector = values.find("sector").text
                except AttributeError:
                    sector = ""
                #exactsite
                try:
                    exactsite = values.find("exactsite").text
                except AttributeError:
                    exactsite = ""
                #image_ids
                try:
                    image_ids = values.find("mediaID").text
                except AttributeError:
                    image_ids = ""
                #image_urls
                try:
                    image_urls = values.find("image_file").text
                except AttributeError:
                    image_urls = ""
                #media_descriptors
                try:
                    media_descriptors = values.find("media_descriptor").text
                except AttributeError:
                    media_descriptors = ""
                #captions
                try:
                    captions = values.find("caption").text
                except AttributeError:
                    captions = ""
                #copyright_holders
                try:
                    copyright_holders = values.find("copyright_holder").text
                except AttributeError:
                    copyright_holders = ""
                #copyright_years
                try:
                    copyright_years = values.find("copyright_year").text
                except AttributeError:
                    copyright_years = ""
                #copyright_licenses
                try:
                    copyright_licenses = values.find("copyright_license").text
                except AttributeError:
                    copyright_licenses = ""
                #copyright_institutions
                try:
                    copyright_institutions = values.find("copyright_institution").text
                except AttributeError:
                    copyright_institutions = ""
                #photographers
                try:
                    photographers = values.find("photographer").text
                except AttributeError:
                    photographers = ""
                #sequenceID
                try:
                    sequenceID = values.find("sequenceID").text
                except AttributeError:
                    sequenceID = ""
                #markercode
                try:
                    markercode = values.sequences.find("markercode").text
                except AttributeError:
                    markercode = ""
                #genbank_accession
                try:
                    genbank_accession = values.find("genbank_accession").text
                except AttributeError:
                    genbank_accession = ""
                #nucleotides
                try:
                    nucleotides = values.find("nucleotides").text
                except AttributeError:
                    nucleotides = ""
                #trace_ids
                try:
                    trace_ids = values.find("trace_id").text
                except AttributeError:
                    trace_ids = ""
                #trace_names
                try:
                    trace_names = values.find("trace_name").text
                except AttributeError:
                    trace_names = ""
                #trace_links
                try:
                    trace_links = values.find("trace_link").text
                except AttributeError:
                    trace_links = ""
                #run_dates
                try:
                    run_dates = values.find("run_date").text
                except AttributeError:
                    run_dates = ""
                #sequencing_centers
                try:
                    sequencing_centers = values.find("sequencing_center").text
                except AttributeError:
                    sequencing_centers = ""
                #directions
                try:
                    directions = values.find("direction").text
                except AttributeError:
                    directions = ""
                #seq_primers
                try:
                    seq_primers = values.find("seq_primer").text
                except AttributeError:
                    seq_primers = ""
                #marker_codes
                try:
                    marker_codes = values.tracefiles.find("markercode").text
                except AttributeError:
                    marker_codes = ""
                    
                list1.append("\t".join([processid, sampleid, recordID, catalognum, fieldnum, institution_storing, collection_code, bin_uri, phylum_taxID, phylum_name, class_taxID, class_name, order_taxID, order_name, family_taxID, family_name, subfamily_taxID, subfamily_name, genus_taxID, genus_name, species_taxID, species_name, subspecies_taxID, subspecies_name, identification_provided_by, identification_method, identification_reference, tax_note, voucher_status, tissue_type, collection_event_id, collectors, collectiondate_start, collectiondate_end, collectiontime, collection_note, site_code, sampling_protocol, lifestage, sex, reproduction, habitat, associated_specimens, associated_taxa, extrainfo, notes, lat, lon, coord_source, coord_accuracy, elev, depth, elev_accuracy, depth_accuracy, country, province_state, region,sector, exactsite, image_ids, image_urls, media_descriptors, captions, copyright_holders, copyright_years, copyright_licenses, copyright_institutions, photographers, sequenceID, markercode, genbank_accession, nucleotides, trace_ids, trace_names, trace_links, run_dates, sequencing_centers, directions, seq_primers, marker_codes]))
                df  = pd.DataFrame(list1)
                #return df
                
                df=df[0].str.split('\t', expand=True)
                df.columns = ['processid','sampleid','recordID','catalognum','fieldnum','institution_storing','collection_code','bin_uri','phylum_taxID','phylum_name','class_taxID','class_name','order_taxID','order_name','family_taxID','family_name','subfamily_taxID','subfamily_name','genus_taxID','genus_name','species_taxID','species_name','subspecies_taxID','subspecies_name','identification_provided_by','identification_method','identification_reference','tax_note','voucher_status','tissue_type','collection_event_id','collectors','collectiondate_start','collectiondate_end','collectiontime','collection_note','site_code','sampling_protocol','lifestage','sex','reproduction','habitat','associated_specimens','associated_taxa','extrainfo','notes','lat','lon','coord_source','coord_accuracy','elev','depth','elev_accuracy','depth_accuracy','country','province_state','region','sector','exactsite','image_ids','image_urls','media_descriptors','captions','copyright_holders','copyright_years','copyright_licenses','copyright_institutions','photographers','sequenceID','markercode','genbank_accession','nucleotides','trace_ids','trace_names','trace_links','run_dates','sequencing_centers','directions','seq_primers','marker_codes']
                df.to_csv("output.tsv", sep = "\t", index=False)

if __name__ =="__main__":
    import sys
    xml2tsv(sys.argv[1])

