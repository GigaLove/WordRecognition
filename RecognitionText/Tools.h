#pragma once
class Tools
{
public:
	Tools(void);
	~Tools(void);
	int findFile(char FilePath[],char FileName[][50]);
	IplImage * pyrDown(IplImage *src);
	void getEdge(IplImage *src,IplImage *new_src);
	int partition(int i,int j,int temp,int *data);
	void quicksort(int i,int j,int *data);
	int Otsu(IplImage *src);
	void OtsuTheld(IplImage *src,IplImage *new_src);
	void FindOther(IplImage *src,int xx,int yy,struct OutLine outLine,struct OutLine *tempoutline);
	void GetWords(IplImage *src,struct OutLine outLine,OutLines *outlines);
	void SetInOrder(OutLines outlines,OutLineSs *outLineSs,Lines *lines);
	void GetLines(int *xValue,int Num,Lines *lines);
	int deal(CString filePath,IplImage *src,OutLineSs *outlineSs,Lines *lines,struct OutLine outline);
};

