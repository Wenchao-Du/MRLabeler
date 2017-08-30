#pragma once
#include "string"
#include "vector"
using namespace std;
class DatasetConfig
{
public:
	DatasetConfig()
	{
	}
	void init(const string dir = "./");
	void load(const string configpath);
	string rootdir="./";
	string datasetdir;
	string configfile = "mrconfig.xml";
	string datasetname="MRDatasets";
	string year="2017";
	string imagedir = "images";
	string annotationdir="Annotations";
	string labelsdir = "labels";	
	int lastlabeledindex=0;
	string currentlabelingclass;
	vector<string>classes;
	bool bsavexml=true;
	bool bsavetxt=true;
	void vocinit();
	void mrfaceinit();
	void marketinit();
	int load_file(const string configpath);
	void save_file(const string configpath);
private:
	string intermediatedir = "intermediate";
};
