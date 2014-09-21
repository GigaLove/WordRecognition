#pragma once
class Tools
{
public:
	Tools(void);
	~Tools(void);
	int findFile(char FilePath[],char FileName[][50]);
	IplImage * pyrDown(IplImage *src);
	void getEdge(IplImage *src,IplImage *new_src);
	int Otsu(IplImage *src);
	void OtsuTheld(IplImage *src,IplImage *new_src);
	void FindOther(IplImage *src,int yy,int xx,struct OutLine outLine[],int *count);
	int FindST(IplImage *src,int beg, int end ,int count_y[],struct OutLine Out[]);
	int compute(int x1,int y1,int x2,int y2);
	void doOutLine(int count,struct OutLine Out[]);
	struct OutLinesInfo childFunction(IplImage *src,int Xline[],int count_x);
	struct OutLinesInfo function(IplImage *src);
	IplImage *deal(CString filePath);

public:
	char fileName[200][50];
	int fileNumber;
	struct OutLinesInfo outlinesinfo;
};

