#include "mrdir.h"
#include "mropencv.h"
#include "mrutil.h"
#include "fstream"
#include "random"
#include <chrono>
#include "AnnotationFile.h"
#include "DataSetConfig.h"
#if _WIN32
const std::string rootdir = "../../Datasets";
const int numofrand = 1;
#else
const std::string rootdir = "/home/yanhe/data";

const int numofrand = 200;
#endif

//const std::string datasetprefix = "/home/yanhe/data/";
const std::string datasetprefix ="D:/Detection/Datasets/";


const std::string datasetname = "Mobile2017";
const std::string datasetdir = rootdir + "/" + datasetname;
const std::string videodir = datasetdir + "/" + "videos";
const std::string origimagedir = datasetdir + "/" + "origimages";
const std::string bgsdir = rootdir + "/" + "bgs";
std::vector<std::string>bgsfiles;
const bool userandbg = true;
DatasetConfig ds;
int startindex = 0;

int generatefromonevideo(const string videopath, const string label)
{
	cv::VideoCapture capture(videopath);
	cv::Mat img,gray, thd;
	while (true)
	{
		capture >> img;
		if (!img.data)
			break;
		cv::cvtColor(img, gray, CV_BGR2GRAY);
		cv::threshold(gray, thd, 0, 255, CV_THRESH_OTSU);
		std::vector<std::vector<cv::Point>>contours;
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
				cout << "\r" <<filename<<"             ";
			}
		}
		else
		{
			std::string filename = int2string(startindex) + ".jpg";//
			af.filename = filename;
			saveimg = img.clone();
			std::string filepath = ds.datasetdir + "/" + ds.imagedir + "/" + filename;
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
			cout << "\r" << filename << "             ";
		}
		startindex++;
	}
	return 0;
}

int generatefromvideodir(const std::string dir, const std::string label)
{
	auto files = getAllFilesinDir(dir);
	cout << dir << endl;
#pragma omp parallel for
	for (int i = 0; i < files.size(); i++)
	{
		string videpath = dir + "/" + files[i];
		cout << files[i] << endl;
		generatefromonevideo(videpath, label);
		cout << endl;
	}
	return 0;
}

int generatefromvideos()
{
	bgsfiles = getAllFilesinDir(bgsdir);
	auto subdirs = getAllSubdirs(videodir);
	for (int i = 0; i < subdirs.size(); i++)
	{
		std::string subdir = videodir + "/" + subdirs[i];
		generatefromvideodir(subdir, subdirs[i]);
	}
	cout << endl;
	return 0;
}

int generatetrainvaltxt(const float trainratio=0.7,const float valratio=0.2,const float testratio=0.1)
{
	std::vector<std::vector<std::string>>filebylabels;
	for (int i = 0; i < AnnotationFile::labelmap.size(); i++)
	{
		std::vector<std::string>files1label;
		filebylabels.push_back(files1label);
	}
	std::string imgdir = ds.datasetdir + "/" + ds.imagedir;
	auto files = getAllFilesinDir(imgdir);
	for (int i = 0; i < files.size(); i++)
	{
		AnnotationFile af;
		std::string annopath = ds.datasetdir + "/" + ds.annotationdir + "/" + files[i];
		annopath = annopath.substr(0, annopath.length() - 3) + "xml";
		af.load_file(annopath);
		int label = AnnotationFile::labelmap[af.objects[0].name];
		filebylabels[label].push_back(files[i]);
	}
	if (ds.bsavetxt)
	{
		ofstream ftrainval(ds.datasetdir + "/trainval.txt");
		ofstream ftest(ds.datasetdir + "/test.txt");
		for (int i = 0; i < filebylabels.size(); i++)
		{
			auto file1label = filebylabels[i];
			unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
			shuffle(file1label.begin(), file1label.end(), std::default_random_engine(seed));
			for (int j = 0; j < file1label.size(); j++)
			{
				string filepath = datasetprefix + datasetname + "/images/" + file1label[j];
				if (j < (trainratio + valratio)*file1label.size())
				{
					ftrainval << filepath << endl;
				}
				else if (j < (trainratio + valratio + testratio)*file1label.size())
				{
					ftest << filepath << endl;
				}
			}
		}
		ftrainval.close();
		ftest.close();
	}
	if (ds.bsavexml)
	{
		std::string imagesetsdir = ds.datasetdir + "/" + "ImageSets";
		if (!EXISTS(imagesetsdir.c_str()))
		{
			MKDIR(imagesetsdir.c_str());
		}
		std::string maindir = imagesetsdir + "/" + "Main";
		if (!EXISTS(maindir.c_str()))
		{
			MKDIR(maindir.c_str());
		}
		ofstream ftrain(maindir + "/train.txt");
		ofstream fval(maindir + "/val.txt");
		ofstream ftrainval(maindir +"/trainval.txt");
		ofstream ftest(maindir + "/test.txt");
		for (int i = 0; i < filebylabels.size(); i++)
		{
			auto file1label = filebylabels[i];
			unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
			shuffle(file1label.begin(), file1label.end(), std::default_random_engine(seed));
			for (int j = 0; j < file1label.size(); j++)
			{
				string filepath = file1label[j];
				filepath = filepath.substr(0, filepath.length() - 4);
				if (j < trainratio*file1label.size())
				{
					ftrain << filepath << endl;
					ftrainval << filepath << endl;
				}
				else if (j < (trainratio + valratio)*file1label.size())
				{
					fval << filepath << endl;
					ftrainval << filepath << endl;
				}
				else if (j < (trainratio + valratio + testratio)*file1label.size())
				{
					ftest << filepath << endl;
				}
			}
		}
		ftrain.close();
		fval.close();
		ftrainval.close();
		ftest.close();
	}
	return 0;
}

int generatefromoneimage(const string imagepath, const string label)
{
	cv::Mat img, gray, thd;
	img = cv::imread(imagepath);
	if (!img.data)
		return -1;
	cv::cvtColor(img, gray, CV_BGR2GRAY);
	cv::threshold(gray, thd, 0, 255, CV_THRESH_OTSU);
	std::vector<std::vector<cv::Point>>contours;
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
			string filename = int2string(startindex) + "_" + int2string(k) + ".jpg";//
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
			cout << "\r" << filename << "             ";
		}
	}
	else
	{
		std::string filename = int2string(startindex) + ".jpg";//
		af.filename = filename;
		saveimg = img.clone();
		std::string filepath = ds.datasetdir + "/" + ds.imagedir + "/" + filename;
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
		cout << "\r" << filename << "             ";
	}
	startindex++;
	return 0;
}

int generatefrmimagedir(const std::string dir, const std::string label)
{
	auto files = getAllFilesinDir(dir);
	cout << dir << endl;
	for (int i = 0; i < files.size(); i++)
	{
		string imagepath = dir + "/" + files[i];
		cout << files[i] << endl;
		generatefromoneimage(imagepath, label);
		cout << endl;
	}
	return 0;
}

int generatefromimages()
{
	bgsfiles = getAllFilesinDir(bgsdir);
	auto subdirs = getAllSubdirs(origimagedir);
	for (int i = 0; i < subdirs.size(); i++)
	{
		std::string subdir = origimagedir + "/" + subdirs[i];
		generatefrmimagedir(subdir, subdirs[i]);
	}
	cout << endl;
	return 0;
}
int main()
{
	srand((unsigned)time(0));
	ds.init(datasetdir);
	generatefromvideos();
//	generatefromimages();
	generatetrainvaltxt();
	return 0;
}