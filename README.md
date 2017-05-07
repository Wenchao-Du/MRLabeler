# MRLabeler

#### VOC YOLO 数据集标注工具

![VOCLabeler](VOCLabeler.png)

## 编译方法

* 1.按照[MRHead](https://github.com/imistyrain/MRHead)的方法搭建好跨平台OpenCV编译环境

* 2.用VS2013打开MRLabeler.sln编译即可

本项目从[mrconfig.xml](mrconfig.xml)加载要标注数据集的相关信息，并将原标注一并显示，通过鼠标选中并拖动框的位置，点击下一张(>按钮)或者上一张(<按钮)保存。

你也可以直接在编辑框输入要跳转的索引，直接跳到要标注的位置。

组合框用于设置标注的类别，每次画框前要先对这个进行设置。

数据集配置文件中各个字段的含义如下：

```
<?xml version="1.0"?>
<dataset>
	<name>IBM</name>数据集名称，自己定义
	<year>0712</year>数据集年代，为支持VOC而用
	<rootdir>E:/IBM</rootdir>数据集存放的目录
	<imagedir>Image</imagedir>数据集图片文件夹路径，相对于rootdir的路径
	<annotationdir>Annotations</annotationdir>原标注文件夹路径，相对于rootdir路径
	<labelsdir>../labels</labelsdir>YOLO格式标注文件夹路径，相对于本项目的路径
	<currentlabelingclass>car</currentlabelingclass>当前要标注的类别名称
	<lastlabeledindex>0</lastlabeledindex>最后标注的类别索引
	<bsavexml>1</bsavexml>是否保存VOC格式标注，默认保存
	<bsavetxt>1</bsavetxt>是否保存YOLO格式标注，默认保存
	<classes>所有的类别，每个类别独占一行
		<class>face</class>
		<class>mask</class>
	</classes>
</dataset>
```