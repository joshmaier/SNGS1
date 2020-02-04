/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*   Files: omp_main.cpp clusters.cpp  clusters.h utils.h utils.cpp          */
/*   			dbscan.cpp dbscan.h kdtree2.cpp kdtree2.hpp          */
/*		    						             */
/*   Description: an openmp implementation of dbscan clustering algorithm    */
/*				using the disjoint set data structure        */
/*                                                                           */
/*   Author:  Md. Mostofa Ali Patwary                                        */
/*            EECS Department, Northwestern University                       */
/*            email: mpatwary@eecs.northwestern.edu                          */
/*                                                                           */
/*   Copyright, 2012, Northwestern University                                */
/*   See COPYRIGHT notice in top-level directory.                            */
/*                                                                           */
/*   Please cite the following publication if you use this package 	     */
/* 									     */
/*   Md. Mostofa Ali Patwary, Diana Palsetia, Ankit Agrawal, Wei-keng Liao,  */
/*   Fredrik Manne, and Alok Choudhary, "A New Scalable Parallel DBSCAN      */
/*   Algorithm Using the Disjoint Set Data Structure", Proceedings of the    */
/*   International Conference on High Performance Computing, Networking,     */
/*   Storage and Analysis (Supercomputing, SC'12), pp.62:1-62:11, 2012.	     */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

//./omp_dbscan -i clus50k.bin -b -m 5 -n 1700 -e 25 -t 1 -o test

#include "dbscan.h"
#include "utils.h"
#include "kdtree2.hpp"
static void usage(char *argv0) 
{
    const char *params =
	"Usage: %s [switches] -i filename -b -m minpts -n seeds -e epsilon -o output -t threads\n"
    	"	-i filename	: file containing input data to be clustered\n"
    	"	-b		: input file is in binary format (default no)\n"
	"	-m minpts	: input parameter of DBSCAN, min points to form a cluster, e.g. 2\n"
	"   -n seeds    : input parameter of Sow-n-Grow, number of seeds to be chosen from each partition of data\n"
	"	-e epsilon	: input parameter of DBSCAN, radius or threshold on neighbourhoods retrieved, e.g. 0.8\n"
	"	-o output	: clustering results, format, (each line, point id, clusterid)\n"
	"	-t threads	: number of threads to be employed\n\n";
    fprintf(stderr, params, argv0);
    exit(-1);
}
int main(int argc, char** argv)
{
	double	seconds;
	int 	opt;
	int 	minPts, threads;
	int seeds;
	int classical = 0;
	double 	eps;
	char* 	outfilename = NULL;
	int     isBinaryFile;
	char*   infilename = NULL;
	// some default values
	minPts 		= -1;
	eps		= -1;
	seeds = -1;
	isBinaryFile 	= 0;
	outfilename 	= NULL;
	infilename	= NULL;
	threads 	= -1;
    	while ((opt=getopt(argc,argv,"i:t:d:p:m:n:e:o:v:z:bxghncul"))!= EOF)
    	{
		switch (opt)
        	{
        		case 'i':
            		infilename = optarg;
               		break;
				case 't':
					threads = atoi(optarg);
					break;
	    		case 'b':
		            isBinaryFile = 1;
            		break;
           		case 'm':
		            minPts = atoi(optarg);
            		break;
            	case 'n':
            		seeds = atoi(optarg);
            		break;
		        case 'e':
            		eps  = atof(optarg);
            		break;
				case 'o':
					outfilename = optarg;
					break;
				case 'c':
					classical = 1;
					break;
            	case '?':
           			usage(argv[0]);
	           		break;
           		default:
           			usage(argv[0]);
           			break;
    		}
	}
	if(infilename == NULL || minPts < 0 || eps < 0 || seeds < 0 || threads < 1)
	{
		usage(argv[0]);
		exit(-1);
	}
	omp_set_num_threads(threads);
	NWUClustering::ClusteringAlgo dbs;
	cout << "Input parameters " << " minPts " << minPts << " eps " << eps << endl;
	double start;
	cout << "Reading points from file: " << infilename << endl;
	if(dbs.read_file(infilename, isBinaryFile) == -1)
			exit(-1);
	ofstream outputfile;
	outputfile.open("runs.txt");
	double time;
	
		
	cout << "***** SEEDS = " << seeds << " *****" << endl;
	outputfile << "****** SEEDS = " << seeds << endl;
	dbs.set_dbscan_params(eps, minPts, seeds);
	if(dbs.read_file(infilename, isBinaryFile) == -1)
		exit(-1);
	dbs.build_kdtree();
	start = omp_get_wtime();
	run_dbscan_algo_uf(dbs);
	time = omp_get_wtime() - start;
	outputfile << "DBSCAN (total) took " << time << " seconds." << endl;
	dbs.writeClusters_uf(outputfile);
	outputfile << endl;
	

	
	// For testing purposes only, please ignore
	/*for(int n=100; n<15001; n=n+100){
		seeds = n;
		//omp_set_num_threads(threads);
		NWUClustering::ClusteringAlgo dbs;
		cout << "***** SEEDS = " << n << " *****" << endl;
		outputfile << "****** SEEDS = " << n << endl;

		dbs.set_dbscan_params(eps, minPts, seeds);
		if(dbs.read_file(infilename, isBinaryFile) == -1)
			exit(-1);
		dbs.build_kdtree();
		start = omp_get_wtime();
		run_dbscan_algo_uf(dbs);
		time = omp_get_wtime() - start;
		outputfile << "DBSCAN (total) took " << time << " seconds." << endl;
		dbs.writeClusters_uf(outputfile);
		outputfile << endl;

	} */

	outputfile.close();
	/*if(outfilename != NULL)
	{
		ofstream outfile;
		outfile.open(outfilename);
		dbs.(outfile);
		dbs.writeClusters(outfile);
		outfile.close();
	}*/
	return 0;
}
