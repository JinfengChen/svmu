# svmu

SVMU (Structural Variants from MUmmer) 0.2beta is part of our (Emerson and Long labs at UCI) ongoing efforts to identify comprehensive sequence variants via alignment of two  contiguous genome assemblies. It calls SNPs, small indels, as well as duplicates, large indels, and inversions from whole genome alignments using MUMmer. 
<b>It is still under development</b>. If you encounter an issue, please email me at mchakrab@uci.edu. This version replaces the earlier version of svmu. Please note that svmu has been tested with MUMmer v3.23 only. Its compatibility with MUMmer v4 has not been tested.

If you publish results obtained with this pipeline, please cite SVMU as described here https://www.nature.com/articles/s41588-017-0010-y. the version used in this paper can be found here : https://github.com/mahulchak/svmu/releases/tag/v0.1beta

1. Download and compile the programs -

 ```
	make

 ```

2. Align the reference and your sample genomes using nucmer (only tested with MUMmer 3.23): 

 ```
	nucmer -maxmatch -noextend -prefix sam2ref.mm ref.fasta sample.fasta
	
 ```
Unfortunately, svmu has a high memory footprint (we are working to reduce it) so if your svmu run crashes due to memory, you may run nucmer as follows -
 ```
	nucmer -mumreference -noextend -prefix sam2ref.mr ref.fasta sample.fasta

 ```

If you are trying to find large (>100bp) indels and inversions, mumreference works better (less time and memory) than the maxmatch algorithm. maxmatch is sometimes better for CNV detection. 

3. Run svmu on the delta file.

 ```
	svmu sam2ref.mm.delta ref.fasta sample.fasta n > sample.small.txt

 ```
  n represents the number of unique mum/syntenic blocks that should be present between two sequences to find the SVs between them. It can be 5, or 10, or 100 (a future update will likely get rid of this parameter). The program generates several files as output: 

	sv.txt: A tab delimited file that summarizes structural mutations (indels, CNVs, inversions) in the sample genome with respect to the reference genome.  

	small.txt: A tab delimited file containing SNPs and small indels that occur within syntenic blocks (or MUMs).

	cnv_all.txt: A tab delimited file with all the reference genomic regions that are present in higher copy numbers (>1) in the sample genome. Those with "trans" in their names mean either it is a transposable element or non-TE copies of a gene in different chromosomes.

	indel.txt: A tab delimited file with all the reference and quesry genomic regions that are missing in the other genome. 
	
	trans.txt: A bed file with the reference genomic regions that have been putatively translocated (may not include TEs). 

This is work in progress so do examine the output. Final goal is to facilitate variant detection from a population samples of high quality genomes. If you have an idea or suggestion (including collaboration ideas, write to me).


Note:
If you are using SVMU for your research, please keep in mind that SVMU has not been extensively tested on genomes bigger than Drosophila. So there is no gurantee that it will work well with other genomes. Currently it requires 4GB memory for yeast genome and 50GB memory for <i>D. melanogaster</i> genome.

KNOWN ISSUES:
1. If the reverse complementary strand of a chromosome was sequenced (relative to the reference), svmu will identify whole sequence as 'INV' or inverted. A simple workaround is to do grep -v 'INV' on your sv.txt file. A future fix will take care of this issue.
2. White space in fasta headers will cause segfault in svmu that is because nucmer strips all text following white space or tab present in the fasta headers.

