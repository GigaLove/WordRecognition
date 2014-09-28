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

	handle=_findfirst(FilePath, &fileinfo);
	if (handle == -1)
	{
		MessageBox(NULL,TEXT("打开错误"),TEXT("YES"),MB_OK);
		return -1;
	}
	strcpy_s(FileName[FileCount],(const char*)fileinfo.name);
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

void Tools::FindOther(IplImage *src,int yy,int xx,struct OutLine outLine[],int *count)
{
	int x_x[8]={-1,0,1,-1,1,-1,0,1};
	int y_y[8]={-1,-1,-1,0,0,1,1,1};

	int x,y;
	int widthStep = src->widthStep;
	//八邻域进行搜索
	for(int i=0;i<8;i++)
	{
		y=yy+ y_y[i];   //垂直方向
		x=xx+ x_x[i];   //水平方向

		//判断是否越界
		if(y < Y + 5 || y >= YY-3 ||x < X + 5 || x >= XX - 6)
			continue;
		//递归查找
		if(src->imageData[y*widthStep+x]==0)
		{
			//右
			if(outLine[0].Right < x)
				outLine[0].Right = x;
			//左
			if(outLine[0].Left > x)
				outLine[0].Left = x;
			//上
			if(outLine[0].Up > y)
				outLine[0].Up = y;
			//下
			if(outLine[0].Down < y)
				outLine[0].Down = y;
			//标记该点 进入递归
			src->imageData[y*widthStep+x] = 128;
			count[0]++;
			FindOther(src,y,x,outLine,count);
		}
	}
}

int Tools::FindST(IplImage *src,int beg, int end ,int count_y[],struct OutLine Out[])
{
	int widthStep = src->widthStep;

	int count = 0;
	int sum[1];
	struct OutLine outLine[1];
	//对该行进行递归查找
	for(int i=X + 6 ; i< XX - 6 ; i++)
	{
		//判断改行是否有字点 减少计算
		if(count_y[i] == 0)
			continue;
		for(int j=beg;j< end ;j++)
		{
			if((uchar)src->imageData[j*widthStep + i] == 0)
			{
				//初始轮廓外围记录的点
				outLine[0].Right = i;
				outLine[0].Left = i;
				outLine[0].Up = j;
				outLine[0].Down = j;
				//标记改点
				src->imageData[j*widthStep + i] = 128;
				//进入递归函数
				sum[0] = 1;
				FindOther(src,j,i,outLine,sum);

				//printf("%d\t",sum[0]);
				if(sum[0]<5)
					continue;

				//获取轮廓查找结果
				Out[count].Up = outLine[0].Up;
				Out[count].Down = outLine[0].Down;
				Out[count].Left = outLine[0].Left;
				Out[count].Right = outLine[0].Right;
				Out[count].code = 1;//标志状态 在融合计算使用
				count++;//计数
			}
		}
	}

	return count;
}

int Tools::compute(int x1,int y1,int x2,int y2)
{
	//计算两点距离
	return int(sqrt(double(y2-y1)*(y2-y1) + (x2-x1)*(x2-x1)));
}

void Tools::doOutLine(int count,struct OutLine Out[])
{
	int dist;
	int st;
	int num;
	int ST;
	int flag;
	do{
		//该值可以从设 这里表示进行两轮融合计算 
		flag = 1;
		for(int i = 0; i < count ;i++)
		{
			//检测状态 判断是否被其他轮廓融合了
			if(Out[i].code == 0)
				continue;
			//获取轮廓较长的边
			ST = (Out[i].Down - Out[i].Up) > (Out[i].Right - Out[i].Left)?(Out[i].Down - Out[i].Up) : (Out[i].Right - Out[i].Left);
			//取与后方num个轮廓进行融合计算 该值可以修改
			num = (i+7) < count ? (i+7):count;
			//
			for(int j = i+1 ;j<  num;j ++)
			{
				//检测状态 判断是否被其他轮廓融合了
				if(Out[j].code == 0)
					continue;
				//获取轮廓较长的边
				st = (Out[j].Right - Out[j].Left) > (Out[j].Down - Out[j].Up) ? (Out[j].Right - Out[j].Left) : (Out[j].Down - Out[j].Up);
				//取较小值作为阈值
				st = st < ST ? st : ST;

				//位置关系 无重叠
				//上方
				if(Out[i].Up >= Out[j].Down)
				{
					//正上方
					if(Out[i].Right >= Out[j].Left )
						dist = Out[i].Up - Out[j].Down;
					//右上方
					else if(Out[i].Right < Out[j].Left )
						dist = compute(Out[i].Up,Out[i].Right,Out[j].Down,Out[j].Left);
				}
				//下方
				else if(Out[i].Down <= Out[j].Up)
				{
					//正下方
					if(Out[i].Right >= Out[j].Left )
						dist = Out[j].Up - Out[i].Down;
					//右下方
					else if(Out[i].Right < Out[j].Left)
						dist = compute(Out[i].Down,Out[i].Right,Out[j].Up,Out[j].Left);

				}
				//正右方
				else if(Out[i].Right <= Out[j].Left)
					dist = Out[j].Left  - Out[i].Right;
				//其他情况 既是可以融合两个轮廓
				else 
					dist = 0;

				//与阈值比较 2的值可以更改
				if(dist > st*2)
						continue;
				//进行轮廓融合
				Out[i].Up = Out[i].Up < Out[j].Up ? Out[i].Up : Out[j].Up;

				Out[i].Down = Out[i].Down > Out[j].Down ? Out[i].Down : Out[j].Down;

				Out[i].Left = Out[i].Left < Out[j].Left ? Out[i].Left : Out[j].Left;

				Out[i].Right = Out[i].Right > Out[j].Right ? Out[i].Right : Out[j].Right;

				Out[j].code = 0;

				//退出内循环
				i--;
				break;
			}
		}

		flag--;
	}while(flag > 0);
}

struct OutLinesInfo Tools::childFunction(IplImage *src,int Xline[],int count_x)
{
	int width = XX - X;
	int height = YY - Y;
	int widthStep = src ->widthStep;
	//初始化查找的结构体
	int *count_y = new int[width];
	int count = 0;
	int column;
	int Num;
	struct OutLine outline[60];

	//初始化储存结构体
	struct OutLinesInfo outlinesinfo;
	outlinesinfo.rows = count_x/2;
	outlinesinfo.BegEnd = new int[count_x];
	outlinesinfo.columns = new int[count_x/2];
	outlinesinfo.outlines = (struct OutLine**)malloc(sizeof(struct OutLine*)*(count_x/2));

#pragma omp parallel num_threads(4)
	{
#pragma omp for private(count_y,column,Num,outline,count)
		for(int k = 0; k < count_x ;k+=2)
		{
			//记录改行的开始
			outlinesinfo.BegEnd[k] = Xline[k];
			outlinesinfo.BegEnd[k+1] = Xline[k+1];
			//初始值
			memset(count_y,0,sizeof(int)*width);
			//进行该字行的垂直投影统计
			for(int i=X;i<XX;i++)
			{
				for(int j = Xline[k] ; j< Xline[k+1] ;j++)
				{
					if(src->imageData[j * widthStep +i] == 0)
						count_y[i]++;
				}
			}
			//获取检测阈值 即超过改行高度0.15的列数 该值可以修改
			int Min = (Xline[k+1] - Xline[k]) * 0.15;
			//初始化
			Num =0;
			//记录超过阈值的数目
			for(int i = X ; i < XX ;i++)
			{
				if(count_y[i] > Min)
					Num++;
			}
			//如果数目太少
			if(Num < width * 0.03)
			{
				outlinesinfo.outlines[k/2] = NULL;
				outlinesinfo.columns[k/2] = -1;
				continue;
			}
			//否则调用函数查找改行所有轮廓
			Num = FindST(src,Xline[k],Xline[k+1],count_y,outline);

			//调用该函数对改行的轮廓进行融合计算
			doOutLine(Num,outline);
			//将融合就得结果放入数组储存
			outlinesinfo.outlines[k/2] = (struct OutLine *)malloc(sizeof(struct OutLine)*(Num+2));

			column = 0;
			for(int i = 0;i< Num;i++)
			{
				//将结果为1的轮廓记录
				if(outline[i].code == 1)
				{
					//对于过小的点进行去除 该阈值可以重新计算得到新值
					if((outline[i].Right-outline[i].Left+1)*(outline[i].Down - outline[i].Up+1) < 8)
						continue;
					//其他的进行拷贝
					outlinesinfo.outlines[k/2][column].Up = outline[i].Up;
					outlinesinfo.outlines[k/2][column].Down = outline[i].Down;
					outlinesinfo.outlines[k/2][column].Left = outline[i].Left;
					outlinesinfo.outlines[k/2][column].Right = outline[i].Right;
					outlinesinfo.outlines[k/2][column].code = column;
					column++;
				}
			}
			//将结果写入结构体
			outlinesinfo.columns[k/2] = column;
		}
	}
	return outlinesinfo;
}

struct OutLinesInfo Tools::function(IplImage *src)
{
	int width = XX - X;  
	int height = YY - Y;
	int widthStep = src ->widthStep;
	//申请空间
	int *count_x = new int[src->height];
	memset(count_x,0,sizeof(int)*src->height);

	//水平投影计数
	for(int i =X; i<XX ;i++)
	{
		for(int j = Y ;j<YY ;j++)
		{
			if((uchar)src->imageData[j*widthStep + i] == 0)
				count_x[j]++;
		}
	}

	//记录各行的开始
	int mark_x[60];
	int mark_x_count =0;
	int flag_x=0;
	//
	for(int i = Y ; i < YY ;i++)
	{
		//行开始该值可以修改 改行多于四个点
		if(count_x[i] > 4 && flag_x ==0)
		{
			//判断该行开始与上一行结束的距离 否则融合两行 如果两行距离小于十
			if(mark_x_count==0||i - mark_x[mark_x_count-1] >10)
			{
				mark_x[mark_x_count] = i-2;
				flag_x = 1;
				mark_x_count++;
			}
			else
			{
				flag_x = 1;
				mark_x_count--;
			}
		}
		//行结束 行结束的判断条件 3 该值可以修改
		else if(count_x[i] < 3 && flag_x ==1)
		{
			//判断行结束和开始的距离 否则去掉 该值可以修改
			if(i-mark_x[mark_x_count-1] > 15)
			{
				mark_x[mark_x_count] = i+2;
				flag_x = 0;
				mark_x_count++;
			}
			else
			{
				flag_x = 0;mark_x_count--;
			}
		}
	}
	return childFunction(src,mark_x,mark_x_count);
}

IplImage* Tools::deal(CString filePath,bool isCut,int ux,int dx,int uy,int dy)
{
	char * filePathName;
	int n = filePath.GetLength();
	USES_CONVERSION;
	filePathName = T2A(filePath);

	IplImage *src = cvLoadImage(filePathName,0);

	//降采样
	IplImage * newSrc = pyrDown(src);
	cvSmooth(newSrc,newSrc);
	//获取特殊边缘信息
	IplImage *edge = cvCreateImage(cvSize(newSrc->width,newSrc->height),8,1);
	getEdge(newSrc,edge);

	//二值化
	cvSmooth(edge,edge);
	IplImage *theld = cvCreateImage(cvSize(newSrc->width,newSrc->height),8,1);
	OtsuTheld(edge,theld);

	int time = src->width/newSrc->width;

	if(isCut)
	{
		X = ux/time;
		XX = dx/time;
		Y = uy/time;
		YY = dy/time;
	}
	else
	{
		X = 0;
		XX = newSrc->width;
		Y = 0;
		YY = newSrc->height;
	}
	outlinesinfo = function(theld);

	for(int i = 0 ;i < outlinesinfo.rows;i++)
	{
		if(outlinesinfo.columns[i] < 0 )
			continue;

		for(int j = 0;j < outlinesinfo.columns[i];j++)
		{
			outlinesinfo.outlines[i][j].Up = outlinesinfo.outlines[i][j].Up*time;
			outlinesinfo.outlines[i][j].Down = outlinesinfo.outlines[i][j].Down*time;
			outlinesinfo.outlines[i][j].Left = outlinesinfo.outlines[i][j].Left*time;
			outlinesinfo.outlines[i][j].Right = outlinesinfo.outlines[i][j].Right*time;
		}
	}

	cvReleaseImage(&newSrc);
	cvReleaseImage(&edge);
	cvReleaseImage(&theld);

	return src;
}







