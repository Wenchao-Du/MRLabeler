#include "mrdir.h"
#include "mropencv.h"
#include "mrutil.h"
#include "fstream"
#include "random"
#include <chrono>
using namespace std;
#include "AnnotationFile.h"
#include "DataSetConfig.h"
#if _WIN32
const string rootdir = "../../Datasets";
#else
const string rootdir = "/home/yanhe/data";
#endif
const string datasetdir = rootdir + "/" + "Market2017";
const string videodir = datasetdir + "/" + "videos";
const string bgsdir = datasetdir + "/" + "bgs";
vector<string>bgsfiles;
const bool userandbg = true;
DatasetConfig ds;

#if _WIN32
const int numofrand = 2;
#else
const int numofrand = 1000;
#endif

int startindex = 0;

int generatefromvideo(const string videopath, const string label)
{
	cv::VideoCapture capture(videopath);
	cv::Mat img;
	cv::Mat gray, thd;
	while (true)
	{
		capture >> img;
		if (!img.data)
			break;
		cv::cvtColor(img, gray, CV_BGR2GRAY);
		cv::threshold(gray, thd, 0, 255, CV_THRESH_OTSU);
		vector<vector<Point>>contours;
		cv::findContours(thd, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

		int maxcontourindex = 0, maxarea = 0;
		for (int j = 0; j < contours.size(); j++)
		{
			if (contours[j].size() > maxarea)
			{
				maxarea = contours[j].size();
				maxcontourindex = j;
			}
		}
		cv::Rect r = cv::boundingRect(contours[maxcontourindex]);
		cv::Mat c = img.clone();
		cv::drawContours(c, contours, maxcontourindex, CV_RGB(255, 0, 0));
		AnnotationFile af;		
		af.set_width(img.cols);
		af.set_height(img.rows);
		af.set_depth(img.channels());
		vector<Object>objects;
		for (int k = 0; k < 1; k++)
		{
			Object object;
			object.xmin = r.x;
			object.ymin = r.y;
			object.xmax = r.x + r.width;
			object.ymax = r.y + r.height;
			object.name = label;
			objects.push_back(object);
		}
		af.objects = objects;
		cv::Mat mask = cv::Mat::zeros(img.rows, img.cols, CV_8UC1);
		cv::drawContours(mask, contours, maxcontourindex, Scalar(255), CV_FILLED);
		cv::Mat saveimg = img.clone();
		if (userandbg)
		{
			for (int k = 0; k < numofrand; k++)
			{
				string filename = int2string(startindex)+"_" + int2string(k)+ ".jpg";//
				af.filename = filename;
				int nb = rand() % bgsfiles.size();
				cv::Mat bgMat = cv::imread(bgsdir + "/" + bgsfiles[nb]);
				cv::resize(bgMat, bgMat, img.size());
				cv::Mat fgMat = saveimg(r);
				saveimg.copyTo(bgMat, mask);
				saveimg = bgMat.clone();
				string filepath = ds.datasetdir + "/" + ds.imagedir + "/" + filename;
				cv::imwrite(filepath, saveimg);
				if (ds.bsavexml)
				{
					string xmlpath = ds.datasetdir + "/" + ds.annotationdir + "/" + filename;
					xmlpath = xmlpath.substr(0, xmlpath.length() - 3) + "xml";
					af.save_xml(xmlpath);
				}
				if (ds.bsavetxt)
				{
					string txtpath = ds.datasetdir + "/" + ds.labelsdir + "/" + filename;
					txtpath = txtpath.substr(0, txtpath.length() - 3) + "txt";
					af.save_txt(txtpath);
				}
				cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b" << startindex << ":" << filename;
			}
		}
		startindex++;
	}
	return 0;
}

int generatefromdir(const string dir, const string label)
{
	auto files = getAllFilesinDir(dir);
	for (int i = 0; i < files.size(); i++)
	{
		string videpath = dir + "/" + files[i];
		generatefromvideo(videpath, label);
	}
	return 0;
}

int generatedata()
{
	bgsfiles = getAllFilesinDir(bgsdir);
	auto subdirs = getAllSubdirs(videodir);
	for (int i = 0; i < subdirs.size(); i++)
	{
		string subdir = videodir + "/" + subdirs[i];
		generatefromdir(subdir, subdirs[i]);
	}
	return 0;
}

int generatetrainvaltxt(const float trainratio=0.9,const float testratio=0.1)
{
	vector<vector<string>>filebylabels;
	for (int i = 0; i < AnnotationFile::labelmap.size(); i++)
	{
		vector<string>files1label;
		filebylabels.push_back(files1label);
	}
	string imgdir = ds.datasetdir + "/" + ds.imagedir;
	auto files = getAllFilesinDir(imgdir);
	for (int i = 0; i < files.size(); i++)
	{
		AnnotationFile af;
		string annopath = ds.datasetdir+"/"+ds.annotationdir + "/" + files[i];
		annopath = annopath.substr(0, annopath.length() - 3) + "xml";
		af.load_file(annopath);
		int label = AnnotationFile::labelmap[af.objects[0].name];
		filebylabels[label].push_back(files[i]);
	}
	ofstream ftrain(ds.datasetdir+"/trainval.txt");
	ofstream ftest(ds.datasetdir+"/test.txt");
	for (int i = 0; i < filebylabels.size(); i++)
	{
		auto file1label = filebylabels[i];
		unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
		shuffle(file1label.begin(), file1label.end(), std::default_random_engine(seed));
		for (int j = 0; j < file1label.size(); j++)
		{
			string filepath = "/home/yanhe/data/Market2017/images/" + file1label[j];
			if (j < trainratio*file1label.size())
			{
				ftrain << filepath << endl;
			}
			else if (j < (trainratio + testratio)*file1label.size())
			{
				ftest << filepath << endl;
			}
		}
	}
	ftrain.close();
	ftest.close();
	return 0;
}

int main()
{
	ds.init(datasetdir);
	generatedata();
	generatetrainvaltxt();
	return 0;
}