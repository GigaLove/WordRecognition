#include "stdafx.h"
#include "Tools.h"
using std::string;
#include <atlconv.h>


Tools::Tools(void)
{
	//fileDictionary = "F:\\photos\\fp\\";
}


Tools::~Tools(void)
{
}

int Tools::findFile(char FilePath[],char FileName[][50])
{
	long handle;
	int FileCount=0;
	struct _finddata_t fileinfo;

	handle=_findfirst(FilePath,&fileinfo);
	if (handle == -1)
	{
		MessageBox(NULL,TEXT("打开错误"),TEXT("YES"),MB_OK);
		return -1;
	}
	strcpy_s(FileName[FileCount],(const char*)fileinfo.name);
	//printf("%s\n",FileName[FileCount]);
	FileCount ++;

	while(!_findnext(handle,&fileinfo))
	{
		strcpy_s(FileName[FileCount],(const char*)fileinfo.name);
		//printf("%s\n",FileName[FileCount]);
		FileCount++;
	}

	_findclose(handle);

	return FileCount;
}

IplImage * Tools::pyrDown(IplImage *src)
{

	IplImage *pyr[10];
	pyr[0] = src;
	int time = 0;

	for(time = 0 ;time < 10;time++)
	{
		if(pyr[time]->width*pyr[time]->height < 3000000)
			break;
		pyr[time+1] = cvCreateImage(cvSize(pyr[time]->width/2,pyr[time]->height/2),8,1);
		cvPyrDown(pyr[time],pyr[time+1]);
	}

	for(int i = 1 ;i< time ;i++)
		cvReleaseImage(&pyr[i]);

	return pyr[time];
}

void Tools::getEdge(IplImage *src,IplImage *new_src)
{
	int width = src ->width;
	int height = src ->height;
	int widthStep = src->widthStep;

	/*int x[] = {-2,-1,0,1,2, -2,-1,0,1,2, -2,-1,1,2, -2,-1,0,1,2, -2,-1,0,1,2};
	int y[] = {-2,-2,-2,-2,-2, -1,-1,-1,-1,-1, 0,0,0,0, 1,1,1,1,1, 2,2,2,2,2};*/
	int x[] = {-1,0,1,-1,1,-1,0,1};
	int y[] = {-1,-1,-1,0,0,1,1,1};

	int temp;
	int temp_x;
	int temp_y;
	int Max1,Max2;
	//
#pragma omp parallel num_threads(4)
	{
#pragma omp for private(temp,temp_x,temp_y,Max1,Max2)
		for(int i=0 ; i< width ;i++)
		{
			for(int j = 0;j<height ;j++)
			{
				Max1 = 0;
				Max2 = 0;
				for(int k = 0; k<8; k++)
				{
					temp_y = j+y[k];
					temp_x = i+x[k];
					if(temp_y<0 || temp_y>=height)
						continue;
					if(temp_x<0 && temp_x>=width)
						continue;

					temp = (uchar)(src->imageData[j*widthStep + i]) - (uchar)(src->imageData[temp_y*widthStep + temp_x]);

					if(temp  < Max1)
					{
						Max1 = temp;
						Max2 = Max1;
					}
					else if(temp < Max2)
						Max2 = temp;

				}
				new_src->imageData[j*widthStep + i] = abs(Max1 + Max2);
				/* if(uchar(new_src->imageData[j*widthStep + i]) < abs(Max))
				new_src->imageData[j*widthStep + i]=0;
				else
				new_src->imageData[j*widthStep + i] = uchar(new_src->imageData[j*widthStep + i]) - abs(Max);*/
			}
		}
	}
}

//快排
int Tools::partition(int i,int j,int temp,int *data)/*划分*/
{
	int L,R ;
	L = i;
	R = j;
	do
	{
		while(L < R && data[R] > temp)
			R--;

		data[L]=data[R];

		while(L < R && data[L] <= temp)
			L++;

		data[R]=data[L];
	}while(L < R);

	data[L]=temp;

	return L;
}

void Tools::quicksort(int i,int j,int *data)/*快速排序*/
{
	int pivotindex=0,k=0;
	int temp;

	if((j-i)==1)
	{
		if(data[i]>data[j])//交换
		{
			temp=data[i];
			data[i]=data[j];
			data[j]=temp;
		}
		return;
	}
	else
	{
		if(data[i]>=data[i+1])
			pivotindex=i;
		else
			pivotindex=i+1;

		temp = data[pivotindex];
		k = partition (pivotindex,j,temp,data);

		if(i<k-1)
			quicksort(i,k-1,data);
		if(k<j)
			quicksort(k,j,data);
	}
}
//归一化像素值
void Tools::Normal(IplImage *src)
{
	//找到最大的值
	int Min = 255;
	int Max = 0;
	for(int i = 0; i< src->width ;i++)
	{
		for(int j = 0;j<src->height;j++)
		{
			if((uchar)src->imageData[j*src->widthStep + i] > Max)
				Max = (uchar)src->imageData[j*src->widthStep + i];
			
			if((uchar)src->imageData[j*src->widthStep + i] < Min )
				Min = (uchar)src->imageData[j*src->widthStep + i];
		}
	}

	float R = 255.0/(Max - Min);

	for(int i = 0; i< src->width ;i++)
	{
		for(int j = 0;j<src->height;j++)
		{
			src->imageData[j*src->widthStep + i]  = ((uchar)src->imageData[j*src->widthStep + i] - Min)*R;
		}
	}
}

int Tools::Otsu(IplImage *src)
{
	int width = src ->width;
	int height = src->height;
	int widthStep = src->widthStep;

	int hist[256] ={0};
	int MatrixHist[256] = {0};

	for(int i = 0 ;i< width ;i++)
	{
		for(int j = 0; j< height ;j++)
		{
			hist[(uchar)(src->imageData[j*widthStep + i])]++;
		}
	}

	int count=0;
	for(int i=0;i<255 ;i++)
	{
		MatrixHist[i]=i*hist[i];
		count += MatrixHist[i];
	}

	float u1;
	float u2;
	int theld = 1;
	float sb=0.0;
	float max_sb=0.0;
	float sum1=hist[0];
	float sum2;
	float sum = width*height;
	float Mah1=MatrixHist[0]; 
	float Mah2=0;
	//
	for(int i = 1 ;i<256 ;i++)
	{

		sum1 += hist[i];
		Mah1 += MatrixHist[i];

		if(sum1 > 0)
		{

			sum2 = sum - sum1;
			Mah2 = count - Mah1;
			if(sum2==0)
				break;

			u1 = Mah1 /sum1;
			u2 = Mah2 /sum2;

			sb = sum1/sum * sum2/sum *(u1-u2)*(u1-u2);
			if(sb > max_sb)
			{
				max_sb = sb;
				theld = i;
			}
		}
	}

	return theld;
}


void Tools::OtsuTheld(IplImage *src,IplImage *new_src)
{
	int width = src->width;
	int height = src ->height;
	int widthStep = src->widthStep;

	int theld = Otsu(src);

	if(theld <= 10)
		theld = 11;

#pragma omp parallel num_threads(4)
	{
#pragma omp for
		for(int i = 0;i<width ;i++)
		{
			for(int j = 0;j<height ;j++)
			{
				if( uchar(src->imageData[j*widthStep + i]) >= theld)
					new_src->imageData[j*widthStep + i] = 0;
				else 
					new_src->imageData[j*widthStep + i] = 255;
			}
		}
	}
}

void Tools::FindOther(IplImage *src,int xx,int yy,struct OutLine outLine,struct OutLine *tempoutline)
{
	int x_x[8]={-1,0,1,-1,1,-1,0,1};
	int y_y[8]={-1,-1,-1,0,0,1,1,1};

	int x,y;
	int xSt = outLine.xSt;
	int xEnd = outLine.xEnd;
	int ySt = outLine.ySt;
	int yEnd = outLine.yEnd;
	int widthStep = src->widthStep;
	//八邻域进行搜索

	for(int i=0;i<8;i++)
	{
		y=yy+ y_y[i];   //垂直方向
		x=xx+ x_x[i];   //水平方向

		//判断是否越界
		if(y < ySt || y >= yEnd ||x < xSt || x >= xEnd)
			continue;
		//递归查找
		if(src->imageData[y*widthStep+x]==0)
		{
			//右
			if(x < tempoutline[0].xSt)
				tempoutline[0].xSt = x;
			//左
			if(x > tempoutline[0].xEnd)
				tempoutline[0].xEnd  = x;
			//上
			if(y < tempoutline[0].ySt)
				tempoutline[0].ySt = y;
			//下
			if(y > tempoutline[0].yEnd)
				tempoutline[0].yEnd = y;
			//标记该点 进入递归
			src->imageData[y*widthStep+x] = 128;
			tempoutline[0].Code ++ ;
			FindOther(src,x,y,outLine,tempoutline);
		}
	}
}

void Tools::GetWords(IplImage *src,struct OutLine outLine,OutLines *outlines)
{
	int xSt = outLine.xSt;
	int xEnd = outLine.xEnd;
	int ySt = outLine.ySt;
	int yEnd = outLine.yEnd;
	int widthStep = src->widthStep;

	struct OutLine tempoutline;
	for(int i = xSt;i < xEnd ;i++)
	{
		for(int j = ySt; j< yEnd ;j++)
		{
			if((uchar)src->imageData[j*widthStep + i] == 0)
			{
				tempoutline.Code = 1;

				tempoutline.xSt = i;
				tempoutline.xEnd = i;
				tempoutline.ySt = j;
				tempoutline.yEnd = j;

				src->imageData[j*widthStep + i] = 128;
				FindOther(src,i,j,outLine,&tempoutline);

				if(tempoutline.Code > 5)
					outlines->push_back(tempoutline);
			}
		}
	}
}

void Tools::SetInOrder(OutLines outlines,OutLineSs *outLineSs,Lines *lines)
{
	int line;
	int Dis;
	int wordline;

	//数据进行分行
	for(int i = 0;i< outlines.size();i++)
	{
		Dis = 1000;
		//计算该值得几何中心
		wordline = (outlines[i].ySt + outlines[i].yEnd)/2;

		//和所有行计算距离
		for(int j = 0;j< lines->size();j++)
		{
			if(abs(wordline - lines->at(j)) < Dis)
			{
				line = j;
				Dis = abs(wordline - lines->at(j));
			}
		}
		outlines[i].Line = line;
	}


	bool st;
	int temp;
	int tempCode;
	int tempLeft;
	int Threld = 10;
	int Code = 0;
	OutLines tempouelines;
	//对每一行进行统计排序
	for(int i = 0;i<lines->size();i++)
	{
		tempLeft = 0;
		while(1)
		{
			st = true;
			temp = 3000;
			//
			for(int j=0; j<outlines.size(); j++)
			{
				if(outlines[j].Line == i)
				{
					if(outlines[j].xSt >= tempLeft && outlines[j].xSt < temp)
					{
						tempCode = j;
						temp = outlines[j].xSt;//找到当前最左边的笔画
						st =false;
					}
				}
			}

			//printf("%d\t%d\t%d\n",tempLeft,tempCode,temp);
			if(st)
			{
				outLineSs->push_back(tempouelines);
				tempouelines.clear();
				break;
			}

			//进行操作
			if(tempouelines.size() == 0)
			{
				outlines[tempCode].Code = Code;
				tempLeft = outlines[tempCode].xSt;
				tempouelines.push_back(outlines[tempCode]);
				outlines[tempCode].Line = -1;
				Code ++;
			}
			//否则判断是否融合
			else
			{
				outlines[tempCode].Code = Code;

				//printf("%d\t%d\n",tempouelines[tempouelines.size() -1].xEnd,outlines[tempCode].xSt - Threld);
				//融合
				if(tempouelines[tempouelines.size() -1].xEnd >= outlines[tempCode].xSt - Threld)
				{
					tempouelines[tempouelines.size() -1].xEnd = tempouelines[tempouelines.size()-1].xEnd > outlines[tempCode].xEnd 
						?  tempouelines[tempouelines.size()-1].xEnd : outlines[tempCode].xEnd;

					tempouelines[tempouelines.size() -1].ySt = tempouelines[tempouelines.size()-1].ySt < outlines[tempCode].ySt 
						?  tempouelines[tempouelines.size()-1].ySt : outlines[tempCode].ySt;

					tempouelines[tempouelines.size() -1].yEnd = tempouelines[tempouelines.size()-1].yEnd > outlines[tempCode].yEnd 
						?  tempouelines[tempouelines.size()-1].yEnd : outlines[tempCode].yEnd;
				}

				else
				{
					tempLeft = outlines[tempCode].xSt;
					tempouelines.push_back(outlines[tempCode]);
					Code ++;
				}
				outlines[tempCode].Line = -1;
			}
		}

		//检查大小位置
		/*for(int j = 0;j< outLineSs->at(i).size(); j++)
		{
			printf("%d\n",outLineSs->at(i).at(j).Code);
		}*/
	}

	Code = 0;
	for(int i = 0;i<outLineSs->size() ;i++)
	{
		for(int j = 0 ;j < outLineSs->at(i).size();j++)
		{
			if((outLineSs->at(i).at(j).xEnd - outLineSs->at(i).at(j).xSt)*
				(outLineSs->at(i).at(j).yEnd - outLineSs->at(i).at(j).ySt) < 20)
			{
				outLineSs->at(i).at(j).Code = -1;
				Code ++;
			}
			else
			{
				outLineSs->at(i).at(j).Code = outLineSs->at(i).at(j).Code - Code;
			}
		}
	}
}
//现在X方向进行融合 然后再X方向进行分组
//猜测每一行之间的间隔大约在十 到儿子
void Tools::GetLines(int *xValue,int Num,Lines *lines)
{
	int Sum = xValue[0];
	int Count = 1;

	for(int i = 1;i < Num;i++)
	{
		if(xValue[i] - xValue[i-1] < 20)
		{
			Sum += xValue[i];
			Count ++;
		}

		else
		{
			Sum = Sum/Count;
			lines->push_back(Sum);

			Sum = xValue[i];
			Count = 1;
		}

		if(i == Num -1)
		{
			Sum = Sum/Count;
			lines->push_back(Sum);
		}
	}
}
IplImage* Tools::deal(CString filePath,IplImage *src,OutLineSs *outlineSs,Lines *lines,struct OutLine outLine,bool isCutted)
{

	char * filePathName;
	int n = filePath.GetLength();
	USES_CONVERSION;
	filePathName = T2A(filePath);

	src = cvLoadImage(filePathName,0);
	if(src == NULL)
		return NULL;
	//降采样
	IplImage * newSrc = pyrDown(src);
	Normal(newSrc);
	cvSmooth(newSrc,newSrc);
	//获取特殊边缘信息
	IplImage *edge = cvCreateImage(cvSize(newSrc->width,newSrc->height),8,1);
	getEdge(newSrc,edge);

	//二值化
	//cvSmooth(edge,edge);
	IplImage *threld = cvCreateImage(cvSize(newSrc->width,newSrc->height),8,1);
	OtsuTheld(edge,threld);

	//
	int time = src->width/newSrc->width;
	if(!isCutted)
	{
		outLine.xSt = 10;
		outLine.xEnd = newSrc->width - 10;
		outLine.ySt = 10;
		outLine.yEnd = newSrc->height - 10;
	}

	else
	{
		outLine.xSt = outLine.xSt/time;
		outLine.xEnd = outLine.xEnd/time;
		outLine.ySt = outLine.ySt/time;
		outLine.yEnd = outLine.yEnd/time;
	}
	//储存初次获得的轮廓
	OutLines outLines;
	GetWords(threld,outLine,&outLines);
 
	//获取y方向的分布值
	int *xValue = new int[outLines.size()];
	for(int i = 0;i < outLines.size();i++)
	{
		//printf("%d\t%d\t%d\t%d\n",outLines[i].xEnd,outLines[i].xSt,outLines[i].ySt,outLines[i].yEnd);
		xValue[i] = (outLines[i].ySt + outLines[i].yEnd)/2;
	}
	quicksort(0,outLines.size()-1,xValue);

	//获得行数
	GetLines(xValue,outLines.size(),lines);
	//获得排序的结果
	SetInOrder(outLines,outlineSs,lines);

	for(int i = 0; i<outlineSs->size();i++)
	{
		for(int j = 0; j<outlineSs->at(i).size();j++)
		{
			outlineSs->at(i).at(j).xSt = outlineSs->at(i).at(j).xSt*time;
			outlineSs->at(i).at(j).xEnd = outlineSs->at(i).at(j).xEnd*time;
			outlineSs->at(i).at(j).ySt = outlineSs->at(i).at(j).ySt*time;
			outlineSs->at(i).at(j).yEnd = outlineSs->at(i).at(j).yEnd*time;
		}
		lines->at(i) = lines->at(i)*time;
	}

	outLines.clear();
	cvReleaseImage(&newSrc);
	cvReleaseImage(&edge);
	cvReleaseImage(&threld);

	return src;
}



