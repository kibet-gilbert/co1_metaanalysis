#!/usr/bin/env python

"""This script converts the xml records of downloaded bold_data.xml files to bold_data.tsv file format files with 80 columns"""


import os
import sys
import pandas as pd
from bs4 import BeautifulSoup as b

#To parse a document, pass it into the BeautifulSoup constructor. You can pass in a string or an open filehandle

#df1=pd.DataFrame()

#for each_file in files_xlm: 
with open((sys.argv[1]), "r") as f: # opening xml filehandle
    content = f.read()
soup = b(content, "xml")    

list1 = []
for values in (soup.find_all("record")):
    #record_id
    try:
        #if values.find("record_id") is None:
        #    recordID = ""
        #else:
            recordID = values.find("record_id").text
    except AttributeError:
        recordID = ""
    #processid
    try:
        #if values.find("processid") is None:
        #     processid = ""
        # else:
             processid = values.find("processid").text
    except AttributeError:
        processid = ""
    #bin_uri
    try:
        #if values.find("bin_uri") is None:
        #    bin_uri = ""
        #else:
            bin_uri = values.find("bin_uri").text
    except AttributeError:
        bin_uri = ""
    #sampleid
    try:
        #if values.find("sampleid") is None:
        #    sampleid = ""
        #else:
            sampleid = values.find("sampleid").text
    except AttributeError:
        sampleid = ""
    #catalognum
    try:
        #if values.find("catalognum") is None:
        #    catalognum = ""
        #else:
            catalognum = values.find("catalognum").text
    except AttributeError:
        catalognum = ""
    #fieldnum
    try:
        #if values.find("fieldnum") is None:
        #    fieldnum = ""
        #else:
            fieldnum = values.find("fieldnum").text
    except AttributeError:
        fieldnum = ""
    #collection_code
    try:
        #if values.find("collection_code") is None:
        #    collection_code = ""
        #else:
            collection_code = values.find("collection_code").text
    except AttributeError:
        collection_code = ""
    #institution_storing
    try:
        #if values.find("institution_storing") is None:
        #    institution_storing = ""
        #else:
            institution_storing = values.find("institution_storing").text
    except AttributeError:
        institution_storing = ""
    #identification_provided_by
    try:
        #if values.find("identification_provided_by") is None:
        #    identification_provided_by = ""
        #else:
            identification_provided_by = values.find("identification_provided_by").text
    except AttributeError:
        identification_provided_by = ""
    #phylum_taxID
    try:
        #if values.phylum.find("taxID") is None:
        #    phylum_taxID = ""    
        #else:
            phylum_taxID = values.phylum.find("taxID").text
    except AttributeError:
        phylum_taxID = ""
    #phylum_name
    try:
        #if values.phylum.find("name") is None:
        #    phylum_name = ""
        #else:
            phylum_name = values.phylum.find("name").text
    except AttributeError:
        phylum_name = ""
    #Class
    try:
        #if values.Class.find("taxID") is None:
        #    class_taxID = ""
        #else:
            class_taxID = values.Class.find("taxID").text
    except AttributeError:
        class_taxID = ""
    try:
        #if values.Class.find("name") is None:
        #    class_name = ""
        #else:
            class_name = values.Class.find("name").text
    except AttributeError:
        class_name = ""
    #order
    try:
        #if values.order.find("taxID") is None:
        #    order_taxID = ""
        #else:
            order_taxID = values.order.find("taxID").text
    except AttributeError:
        order_taxID = ""
    try:
        #if values.order.find("name") is None:
        #    order_name = ""
        #else:
            order_name = values.order.find("name").text
    except AttributeError:
        order_name = ""
    #family
    try:
        #if values.family.find("taxID") is None:
        #    family_taxID = ""
        #else:
            family_taxID = values.family.find("taxID").text
    except AttributeError:
        family_taxID = ""
    try:
        #if values.family.find("name") is None:
        #    family_name = ""
        #else:
            family_name = values.family.find("name").text
    except AttributeError:
        family_name = ""
    #subfamily
    try:
        #if values.subfamily.find("taxID") is None:
        #    subfamily_taxID = ""
        #else:
            subfamily_taxID = values.subfamily.find("taxID").text
    except AttributeError:
        subfamily_taxID = ""
    try:
        #if values.subfamily.find("name") is None:
        #    subfamily_name = ""
        #else:
            subfamily_name = values.subfamily.find("name").text
    except AttributeError:
        subfamily_name = ""
    #genus
    try:
        #if values.genus.find("taxID") is None:
        #    genus_taxID = ""
        #else:
            genus_taxID = values.genus.find("taxID").text
    except AttributeError:
        genus_taxID = ""
    try:
        #if values.genus.find("name") is None:
        #    genus_name = ""
        #else:
            genus_name = values.genus.find("name").text
    except AttributeError:
        genus_name = ""
    #species
    try:
        #if values.species.find("taxID") is None:
        #    species_taxID = ""
        #else:
            species_taxID = values.species.find("taxID").text
    except AttributeError:
        species_taxID = ""
    try:
        #if values.species.find("name") is None:
        #    species_name = ""
        #else:
            species_name = values.species.find("name").text
    except AttributeError:
        species_name = ""
    #subspecies
    try:
        #if values.subspecies.find("taxID") is None:
        #    subspecies_taxID = ""
        #else:
            subspecies_taxID = values.subspecies.find("taxID").text
    except AttributeError:
        subspecies_taxID = ""
    try:
        #if values.subspecies.find("name") is None:
        #    subspecies_name = ""
        #else:
            subspecies_name = values.subspecies.find("name").text
    except AttributeError:
        subspecies_name = ""
    #identification_method
    try:
        #if values.find("method") is None:
        #    identification_method = ""
        #else:
            identification_method = values.find("method").text
    except AttributeError:
        identification_method = ""
    #identification_reference
    try:
        #if values.find("reference") is None:
        #    identification_reference = ""
        #else:
            identification_reference = values.find("reference").text
    except AttributeError:
        identification_reference = ""
    #tax_note
    try:
        #if values.find("note") is None:
        #    tax_note = ""
        #else:
            tax_note = values.find("note").text
    except AttributeError:
        tax_note = ""
    #voucher_status
    try:
        #if values.find("voucher_status") is None:
        #    voucher_status = ""
        #else:
            voucher_status = values.find("voucher_status").text
    except AttributeError:
        voucher_status = ""
    #tissue_type
    try:
        #if values.find("tissue_type") is None:
        #    tissue_type = ""
        #else:
            tissue_type = values.find("tissue_type").text
    except AttributeError:
        tissue_type = ""
    #collection_event_id
    try:
        #if values.find("collection_event_id") is None:
        #    collection_event_id = ""
        #else:
            collection_event_id = values.find("collection_event_id").text
    except AttributeError:
        collection_event_id = ""
    #collectors
    try:
        #if values.find("collectors") is None:
        #    collectors = ""
        #else:
            collectors = values.find("collectors").text
    except AttributeError:
        collectors = ""
    #collectiondate_start
    try:
        #if values.find("collectiondate_start") is None:
        #    collectiondate_start = ""
        #else:
            collectiondate_start = values.find("collectiondate_start").text
    except AttributeError:
        collectiondate_start = ""
    #collectiondate_end
    try:
        #if values.find("collectiondate_end") is None:
        #    collectiondate_end = ""
        #else:
            collectiondate_end = values.find("collectiondate_end").text
    except AttributeError:
        collectiondate_end = ""
    #collectiontime
    try:
        #if values.find("collectiontime") is None:
        #    collectiontime = ""
        #else:
            collectiontime = values.find("collectiontime").text
    except AttributeError:
        collectiontime = ""
    #collection_note
    try:
        #if values.find("collection_note") is None:
        #    collection_note = ""
        #else:
            collection_note = values.find("collection_note").text
    except AttributeError:
        collection_note = ""
    #site_code
    try:
        #if values.find("site_code") is None:
        #    site_code = ""
        #else:
            site_code = values.find("site_code").text
    except AttributeError:
        site_code = ""
    #sampling_protocol
    try:
        #if values.find("sampling_protocol") is None:
        #    sampling_protocol = ""
        #else:
            sampling_protocol = values.find("sampling_protocol").text
    except AttributeError:
        sampling_protocol = ""
    #lifestage
    try:
        #if values.find("lifestage") is None:
        #    lifestage = ""
        #else:
            lifestage = values.find("lifestage").text
    except AttributeError:
        lifestage = ""
    #sex
    try:
        #if values.find("sex") is None:
        #    sex = ""
        #else:
            sex = values.find("sex").text
    except AttributeError:
        sex = ""
    #reproduction
    try:
        #if values.find("reproduction") is None:
        #    reproduction = ""
        #else:
            reproduction = values.find("reproduction").text
    except AttributeError:
        reproduction = ""
    #habitat
    try:
        #if values.find("habitat") is None:
        #    habitat = ""
        #else:
            habitat = values.find("habitat").text
    except AttributeError:
        habitat = ""
    #associated_spacimens
    try:
        #if values.find("associated_specimens") is None:
        #    associated_specimens = ""
        #else:
            associated_specimens = values.find("associated_specimens").text
    except AttributeError:
        associated_specimens = ""
    #associated_taxa
    try:
        #if values.find("associated_taxa") is None:
        #    associated_taxa = ""
        #else:
            associated_taxa = values.find("associated_taxa").text
    except AttributeError:
        associated_taxa = ""
    #extrainfo
    try:
        #if values.find("extrainfo") is None:
        #    extrainfo = ""
        #else:
            extrainfo = values.find("extrainfo").text
    except AttributeError:
        extrainfo = ""
    #notes
    try:
        #if values.find("notes") is None:
        #    notes = ""
        #else:
            notes = values.find("notes").text
    except AttributeError:
        notes = ""
    #lat
    try:
        #if values.find("lat") is None:
        #    lat = ""
        #else:
            lat = values.find("lat").text
    except AttributeError:
        lat = ""
    #lon
    try:
        #if values.find("lon") is None:
        #    lon = ""
        #else:
            lon = values.find("lon").text
    except AttributeError:
        lon = ""
    #coord_source
    try:
        #if values.find("coord_source") is None:
        #    coord_source = ""
        #else:
            coord_source = values.find("coord_source").text
    except AttributeError:
        coord_source = ""
    #coord_accuracy
    try:
        #if values.find("coord_accuracy") is None:
        #    coord_accuracy = ""
        #else:
            coord_accuracy = values.find("coord_accuracy").text
    except AttributeError:
        coord_accuracy = ""
    #elev
    try:
        #if values.find("elev") is None:
        #    elev = ""
        #else:
            elev = values.find("elev").text
    except AttributeError:
        elev = ""
    #depth
    try:
        #if values.find("depth") is None:
        #    depth = ""
        #else:
            depth = values.find("depth").text
    except AttributeError:
        depth = ""
    #elev_accuracy
    try:
        #if values.find("elev_accuracy") is None:
        #    elev_accuracy = ""
        #else:
            elev_accuracy = values.find("elev_accuracy").text
    except AttributeError:
        elev_accuracy = ""
    #depth_accuracy
    try:
        #if values.find("depth_accuracy") is None:
        #    depth_accuracy = ""
        #else:
            depth_accuracy = values.find("depth_accuracy").text
    except AttributeError:
        depth_accuracy = ""
    #country
    try:
        #if values.find("country") is None:
        #    country = ""
        #else:
            country = values.find("country").text
    except AttributeError:
        country = ""
    #province_state
    try:
        #if values.find("province_state") is None:
        #    province_state = ""
        #else:
            province_state = values.find("province_state").text
    except AttributeError:
        province_state = ""
    #region
    try:
        #if values.find("region") is None:
        #    region = ""
        #else:
            region = values.find("region").text
    except AttributeError:
        region = ""
    #sector
    try:
        #if values.find("sector") is None:
        #    sector = ""
        #else:
            sector = values.find("sector").text
    except AttributeError:
        sector = ""
    #exactsite
    try:
        #if values.find("exactsite") is None:
        #    exactsite = ""
        #else:
            exactsite = values.find("exactsite").text
    except AttributeError:
        exactsite = ""
    #image_ids
    try:
        #if values.find("mediaID") is None:
        #    image_ids = ""
        #else:
            image_ids = values.find("mediaID").text
    except AttributeError:
        image_ids = ""
    #image_urls
    try:
        #if values.find("image_file") is None:
        #    image_urls = ""
        #else:
            image_urls = values.find("image_file").text
    except AttributeError:
        image_urls = ""
    #media_descriptors
    try:
        #if values.find("media_descriptor") is None:
        #    media_descriptors = ""
        #else:
            media_descriptors = values.find("media_descriptor").text
    except AttributeError:
        media_descriptors = ""
    #captions
    try:
        #if values.find("caption") is None:
        #    captions = ""
        #else:
            captions = values.find("caption").text
    except AttributeError:
        captions = ""
    #copyright_holders
    try:
        #if values.find("copyright_holder") is None:
        #    copyright_holders = ""
        #else:
            copyright_holders = values.find("copyright_holder").text
    except AttributeError:
        copyright_holders = ""
    #copyright_years
    try:
        #if values.find("copyright_year") is None:
        #    copyright_years = ""
        #else:
            copyright_years = values.find("copyright_year").text
    except AttributeError:
        copyright_years = ""
    #copyright_licenses
    try:
        #if values.find("copyright_license") is None:
        #    copyright_licenses = ""
        #else:
            copyright_licenses = values.find("copyright_license").text
    except AttributeError:
        copyright_licenses = ""
    #copyright_institutions
    try:
        #if values.find("copyright_institution") is None:
        #    copyright_institutions = ""
        #else:
            copyright_institutions = values.find("copyright_institution").text
    except AttributeError:
        copyright_institutions = ""
    #photographers
    try:
        #if values.find("photographer") is None:
        #    photographers = ""
        #else:
            photographers = values.find("photographer").text
    except AttributeError:
        photographers = ""
    #sequenceID
    try:
        #if values.find("sequenceID") is None:
        #    sequenceID = ""
        #else:
            sequenceID = values.find("sequenceID").text
    except AttributeError:
        sequenceID = ""
    #markercode
    try:
        #if values.sequences.find("markercode") is None:
        #    markercode = ""
        #else:
            markercode = values.sequences.find("markercode").text
    except AttributeError:
        markercode = ""
    #genbank_accession
    try:
        #if values.find("genbank_accession") is None:
        #    genbank_accession = ""
        #else:
            genbank_accession = values.find("genbank_accession").text
    except AttributeError:
        genbank_accession = ""
    #nucleotides
    try:
        #if values.find("nucleotides") is None:
        #    nucleotides = ""
        #else:
            nucleotides = values.find("nucleotides").text
    except AttributeError:
        nucleotides = ""
    #trace_ids
    try:
        #if values.find("trace_id") is None:
        #    trace_ids = ""
        #else:
            trace_ids = values.find("trace_id").text
    except AttributeError:
        trace_ids = ""
    #trace_names
    try:
        #if values.find("trace_name") is None:
        #    trace_names = ""
        #else:
            trace_names = values.find("trace_name").text
    except AttributeError:
         trace_names = ""
    #trace_links
    try:
        #if values.find("trace_link") is None:
        #    trace_links = ""
        #else:
            trace_links = values.find("trace_link").text
    except AttributeError:
        trace_links = ""
    #run_dates
    try:
        #if values.find("run_date") is None:
        #    run_dates = ""
        #else:
            run_dates = values.find("run_date").text
    except AttributeError:
        run_dates = ""
    #sequencing_centers
    try:
        #if values.find("sequencing_center") is None:
        #    sequencing_centers = ""
        #else:
            sequencing_centers = values.find("sequencing_center").text
    except AttributeError:
        sequencing_centers = ""
    #directions
    try:
        #if values.find("direction") is None:
        #    directions = ""
        #else:
            directions = values.find("direction").text
    except AttributeError:
        directions = ""
    #seq_primers
    try:
        #if values.find("seq_primer") is None:
        #    seq_primers = ""
        #else:
            seq_primers = values.find("seq_primer").text
    except AttributeError:
        seq_primers = ""
    #marker_codes
    try:
        #if values.tracefiles.find("markercode") is None:
        #    marker_codes = ""
        #else:
            marker_codes = values.tracefiles.find("markercode").text
    except AttributeError:
        marker_codes = ""
        
    list1.append("\t".join([processid, sampleid, recordID, catalognum, fieldnum, institution_storing, collection_code, bin_uri, phylum_taxID, phylum_name, class_taxID, class_name, order_taxID, order_name, family_taxID, family_name, subfamily_taxID, subfamily_name, genus_taxID, genus_name, species_taxID, species_name, subspecies_taxID, subspecies_name, identification_provided_by, identification_method, identification_reference, tax_note, voucher_status, tissue_type, collection_event_id, collectors, collectiondate_start, collectiondate_end, collectiontime, collection_note, site_code, sampling_protocol, lifestage, sex, reproduction, habitat, associated_specimens, associated_taxa, extrainfo, notes, lat, lon, coord_source, coord_accuracy, elev, depth, elev_accuracy, depth_accuracy, country, province_state, region,sector, exactsite, image_ids, image_urls, media_descriptors, captions, copyright_holders, copyright_years, copyright_licenses, copyright_institutions, photographers, sequenceID, markercode, genbank_accession, nucleotides, trace_ids, trace_names, trace_links, run_dates, sequencing_centers, directions, seq_primers, marker_codes]))
    df  = pd.DataFrame(list1)
    #return df



    df=df[0].str.split('\t', expand=True)
    df.columns = ['processid','sampleid','recordID','catalognum','fieldnum','institution_storing','collection_code','bin_uri','phylum_taxID','phylum_name','class_taxID','class_name','order_taxID','order_name','family_taxID','family_name','subfamily_taxID','subfamily_name','genus_taxID','genus_name','species_taxID','species_name','subspecies_taxID','subspecies_name','identification_provided_by','identification_method','identification_reference','tax_note','voucher_status','tissue_type','collection_event_id','collectors','collectiondate_start','collectiondate_end','collectiontime','collection_note','site_code','sampling_protocol','lifestage','sex','reproduction','habitat','associated_specimens','associated_taxa','extrainfo','notes','lat','lon','coord_source','coord_accuracy','elev','depth','elev_accuracy','depth_accuracy','country','province_state','region','sector','exactsite','image_ids','image_urls','media_descriptors','captions','copyright_holders','copyright_years','copyright_licenses','copyright_institutions','photographers','sequenceID','markercode','genbank_accession','nucleotides','trace_ids','trace_names','trace_links','run_dates','sequencing_centers','directions','seq_primers','marker_codes']
    df.to_csv("output.tsv", sep = "\t", index=False)
