#include "DataSetConfig.h"
#include "pugixml.hpp"
#include "iostream"
#include "mrdir.h"
const string defaultdatasetpath = "mrconfig.xml";

int DatasetConfig::load_file(const string configpath)
{
	pugi::xml_document doc;
	doc.load_file(configpath.c_str());
	pugi::xml_node rootnode = doc.child("dataset");
	datasetname = rootnode.child("name").text().get();
	year = rootnode.child("year").text().get();
	rootdir = rootnode.child("rootdir").text().get();
	imagedir = rootnode.child("imagedir").text().get();
	annotationdir = rootnode.child("annotationdir").text().get();
	labelsdir = rootnode.child("labelsdir").text().get();

	currentlabelingclass = rootnode.child("currentlabelingclass").text().get();
	lastlabeledindex = rootnode.child("lastlabeledindex").text().as_int();
	bsavexml = rootnode.child("bsavexml").text().as_bool();
	bsavetxt = rootnode.child("bsavetxt").text().as_bool();
	pugi::xml_node classesnode = rootnode.child("classes");
	classes.clear();

	for (auto it = classesnode.first_child(); it; it = it.next_sibling())
	{
		classes.push_back(it.text().get());
	}
	return 0;
}
void DatasetConfig::save_file(const string configpath)
{
	pugi::xml_document doc;
	pugi::xml_node rootnode = doc.append_child("dataset");
	rootnode.append_child("name").text().set(datasetname.c_str());
	rootnode.append_child("year").text().set(year.c_str());	
	rootnode.append_child("rootdir").text().set(rootdir.c_str());
	rootnode.append_child("imagedir").text().set(imagedir.c_str());
	rootnode.append_child("annotationdir").text().set(annotationdir.c_str());
	rootnode.append_child("labelsdir").text().set(labelsdir.c_str());
	rootnode.append_child("currentlabelingclass").text().set(currentlabelingclass.c_str());
	rootnode.append_child("lastlabeledindex").text().set(lastlabeledindex);
	rootnode.append_child("bsavexml").text().set(bsavexml);
	rootnode.append_child("bsavetxt").text().set(bsavetxt);
	pugi::xml_node classesnode = rootnode.append_child("classes");
	for (int i = 0; i < classes.size(); i++)
	{
		classesnode.append_child("class").text().set(classes[i].c_str());
	}
	doc.save_file(configpath.c_str());
}

void DatasetConfig::init()
{
	if (exist(defaultdatasetpath.c_str()))
		load_file(defaultdatasetpath);
	else
	{
		mrfaceinit();
		save_file(defaultdatasetpath);
	}	
}

void DatasetConfig::vocinit()
{
	datasetname = "VOC";
	year = "0712";
	currentlabelingclass = "car";
	lastlabeledindex = 0;
	rootdir = "E:/Detection/darknetv2/data/voc/VOCdevkit/VOC2007";
	annotationdir = "Annotations";
	imagedir = "JPEGImages";
	labelsdir = "labels";
	string classnames[] = { "aeroplane", "bicycle", "bird",
		"boat", "bottle", "bus", "car", "cat", "chair", "cow",
		"diningtable", "dog", "horse", "motorbike", "person",
		"pottedplant", "sheep", "sofa", "train", "tvmonitor" };
	bsavexml = true;
	bsavetxt = true;
	classes.clear();
	for (int i = 0; i < 20; i++)
	{
		classes.push_back(classnames[i]);
	}
}

void DatasetConfig::mrfaceinit()
{
	datasetname = "MRFace";
	year = "2017";
	currentlabelingclass = "face";
	lastlabeledindex = 0;
	rootdir = "./";
	annotationdir = "Annotations";
	imagedir = "images";
	labelsdir = "labels";
	string classnames[] = { "face" };
	bsavexml = true;
	bsavetxt = true;
	classes.clear();
	for (int i = 0; i < 1; i++)
	{
		classes.push_back(classnames[i]);
	}
}