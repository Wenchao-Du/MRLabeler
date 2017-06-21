#pragma once
#include "vector"
using namespace std;
class DatasetConfig
{
public:
	DatasetConfig()
	{
		init();
	}
	string datasetname;
	string year;	
	string rootdir;
	string annotationdir;
	string imagedir;
	string labelsdir = "labels";
	string currentlabelingclass;
	int lastlabeledindex;
	vector<string>classes;
	bool bsavexml;
	bool bsavetxt;
	void vocinit();
	void mrfaceinit();
	int load_file(const string configpath);
	void save_file(const string configpath);
private:
	string intermediatedir = "intermediate";
	void init();		
};
