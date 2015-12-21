# svMum
A pipeline to call duplicates from whole genome alignment.

Download and compile the programs -

 ```
	g++ -Wall -std=c++0x mlib.cpp svmum.cpp -o svmum
	g++ -Wall script_maker.cpp -o scriptmaker
 ```

Othe programs needed for this pipeline:

 You will need <a href="http://mummer.sourceforge.net/">MUMmer</a>,  <a href="https://github.com/arq5x/bedtools2/blob/master/README.md">bedtools</a> , and <a href="http://www.repeatmasker.org/"> Repeatmasker</a> to use this pipeline. Additionally, the program fasplitter from <a href = "https://github.com/mahulchak/Assembly-utils">Assembly-utils</a> is required to split the fasta file.

Here is an example of how to use svMUM pipeline to obtain a list of duplicates sequences from whole gnome alignment.

1. Run fasplitter on the reference genome to split the contigs/chromosomes/scaffolds into component fasta files. This is necessary because mummer can be run on a single reference sequence at a time.

 ``` 
	fasplitter ref.fasta Y
 ```
Using the 'Y' switch in fasplitter will ensure that the new fasta files have '.fa' in their names.

2. Create a list of the new fasta files

 ```
	ls *.fa > list_of_fa

 ```
 TIP: to avoid listing the reference assembly fasta file in the list, use ".fasta" as the file extension for the reference assembly.

3. Run scriptmaker to generate duplicate calling scripts for all the component fasta files.

 ```   
	scriptmaker ref.fasta list_of_fa
 ```

4. Generate the list of all scripts and then add bash command to each of them.

 ```
	ls job_* > list_of_jobs

	sed -i 's/^/bash /g' list_of_jobs

 ```

5. Now run all the job scripts in parallel or serial mode depending on how many cores you have. If you are running in serial mode (i.e. want to use 1 core) , do -

 ```
	bash list_of_jobs
 ```
 For using the parallel mode, you can use GNU parallel -

 ```
	cat list_of_jobs | parallel -j NPROC
 ```
 Replace 'NPROC' with the number of processors you want to use. If you have 4 processors, use 4.

6. Concatenate all the output files.

 ```
	cat *.tsv > all_chrom.tsv
 ```

7. The 'all_chrom.tsv' file has both TE and duplicates in it. To separate TE from duplicates, you will need <a href="https://github.com/arq5x/bedtools2/blob/master/README.md">bedtools</a> and a file with TE annotations for the reference genome.
 TIP: You can use Repeatmasker to generate the TE annotation file if you already don't have a TE annotation file.

 After you get the TE annotation file, you can use the following commands to obtain the list of sequences that are single copy in the reference genome but more than one copy in the other genome.

 ```
	cat all_chrom.tsv| awk '{if($10-$9 >100) print $0}' | sort -u -k9,9 |sort -k1,1 -k2,2n >name_all_singleton

	cat name_all_singleton |sort -u -k2,2n| awk '{print $1"\t"$2"\t"$3}' |sort -k2,2n |bedtools merge -i stdin >name_all_hits

	sort -k1,1 -k2,2n TE.bed |bedtools subtract -A -a name_all_hits -b stdin |bedtools merge  -i stdin |awk '{if ($3-$2>100) print $0}' > name_all_predicted_dups
 ```
 Replace 'name' with a unique identifier.

