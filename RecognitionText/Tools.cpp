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
		MessageBox(NULL,TEXT("�򿪴���"),TEXT("YES"),MB_OK);
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
	//�������������
	for(int i=0;i<8;i++)
	{
		y=yy+ y_y[i];   //��ֱ����
		x=xx+ x_x[i];   //ˮƽ����

		//�ж��Ƿ�Խ��
		if(y < Y + 5 || y >= YY-3 ||x < X + 5 || x >= XX - 6)
			continue;
		//�ݹ����
		if(src->imageData[y*widthStep+x]==0)
		{
			//��
			if(outLine[0].Right < x)
				outLine[0].Right = x;
			//��
			if(outLine[0].Left > x)
				outLine[0].Left = x;
			//��
			if(outLine[0].Up > y)
				outLine[0].Up = y;
			//��
			if(outLine[0].Down < y)
				outLine[0].Down = y;
			//��Ǹõ� ����ݹ�
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
	//�Ը��н��еݹ����
	for(int i=X + 6 ; i< XX - 6 ; i++)
	{
		//�жϸ����Ƿ����ֵ� ���ټ���
		if(count_y[i] == 0)
			continue;
		for(int j=beg;j< end ;j++)
		{
			if((uchar)src->imageData[j*widthStep + i] == 0)
			{
				//��ʼ������Χ��¼�ĵ�
				outLine[0].Right = i;
				outLine[0].Left = i;
				outLine[0].Up = j;
				outLine[0].Down = j;
				//��Ǹĵ�
				src->imageData[j*widthStep + i] = 128;
				//����ݹ麯��
				sum[0] = 1;
				FindOther(src,j,i,outLine,sum);

				//printf("%d\t",sum[0]);
				if(sum[0]<5)
					continue;

				//��ȡ�������ҽ��
				Out[count].Up = outLine[0].Up;
				Out[count].Down = outLine[0].Down;
				Out[count].Left = outLine[0].Left;
				Out[count].Right = outLine[0].Right;
				Out[count].code = 1;//��־״̬ ���ںϼ���ʹ��
				count++;//����
			}
		}
	}

	return count;
}

int Tools::compute(int x1,int y1,int x2,int y2)
{
	//�����������
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
		//��ֵ���Դ��� �����ʾ���������ںϼ��� 
		flag = 1;
		for(int i = 0; i < count ;i++)
		{
			//���״̬ �ж��Ƿ����������ں���
			if(Out[i].code == 0)
				continue;
			//��ȡ�����ϳ��ı�
			ST = (Out[i].Down - Out[i].Up) > (Out[i].Right - Out[i].Left)?(Out[i].Down - Out[i].Up) : (Out[i].Right - Out[i].Left);
			//ȡ���num�����������ںϼ��� ��ֵ�����޸�
			num = (i+7) < count ? (i+7):count;
			//
			for(int j = i+1 ;j<  num;j ++)
			{
				//���״̬ �ж��Ƿ����������ں���
				if(Out[j].code == 0)
					continue;
				//��ȡ�����ϳ��ı�
				st = (Out[j].Right - Out[j].Left) > (Out[j].Down - Out[j].Up) ? (Out[j].Right - Out[j].Left) : (Out[j].Down - Out[j].Up);
				//ȡ��Сֵ��Ϊ��ֵ
				st = st < ST ? st : ST;

				//λ�ù�ϵ ���ص�
				//�Ϸ�
				if(Out[i].Up >= Out[j].Down)
				{
					//���Ϸ�
					if(Out[i].Right >= Out[j].Left )
						dist = Out[i].Up - Out[j].Down;
					//���Ϸ�
					else if(Out[i].Right < Out[j].Left )
						dist = compute(Out[i].Up,Out[i].Right,Out[j].Down,Out[j].Left);
				}
				//�·�
				else if(Out[i].Down <= Out[j].Up)
				{
					//���·�
					if(Out[i].Right >= Out[j].Left )
						dist = Out[j].Up - Out[i].Down;
					//���·�
					else if(Out[i].Right < Out[j].Left)
						dist = compute(Out[i].Down,Out[i].Right,Out[j].Up,Out[j].Left);

				}
				//���ҷ�
				else if(Out[i].Right <= Out[j].Left)
					dist = Out[j].Left  - Out[i].Right;
				//������� ���ǿ����ں���������
				else 
					dist = 0;

				//����ֵ�Ƚ� 2��ֵ���Ը���
				if(dist > st*2)
						continue;
				//���������ں�
				Out[i].Up = Out[i].Up < Out[j].Up ? Out[i].Up : Out[j].Up;

				Out[i].Down = Out[i].Down > Out[j].Down ? Out[i].Down : Out[j].Down;

				Out[i].Left = Out[i].Left < Out[j].Left ? Out[i].Left : Out[j].Left;

				Out[i].Right = Out[i].Right > Out[j].Right ? Out[i].Right : Out[j].Right;

				Out[j].code = 0;

				//�˳���ѭ��
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
	//��ʼ�����ҵĽṹ��
	int *count_y = new int[width];
	int count = 0;
	int column;
	int Num;
	struct OutLine outline[60];

	//��ʼ������ṹ��
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
			//��¼���еĿ�ʼ
			outlinesinfo.BegEnd[k] = Xline[k];
			outlinesinfo.BegEnd[k+1] = Xline[k+1];
			//��ʼֵ
			memset(count_y,0,sizeof(int)*width);
			//���и����еĴ�ֱͶӰͳ��
			for(int i=X;i<XX;i++)
			{
				for(int j = Xline[k] ; j< Xline[k+1] ;j++)
				{
					if(src->imageData[j * widthStep +i] == 0)
						count_y[i]++;
				}
			}
			//��ȡ�����ֵ ���������и߶�0.15������ ��ֵ�����޸�
			int Min = (Xline[k+1] - Xline[k]) * 0.15;
			//��ʼ��
			Num =0;
			//��¼������ֵ����Ŀ
			for(int i = X ; i < XX ;i++)
			{
				if(count_y[i] > Min)
					Num++;
			}
			//�����Ŀ̫��
			if(Num < width * 0.03)
			{
				outlinesinfo.outlines[k/2] = NULL;
				outlinesinfo.columns[k/2] = -1;
				continue;
			}
			//������ú������Ҹ�����������
			Num = FindST(src,Xline[k],Xline[k+1],count_y,outline);

			//���øú����Ը��е����������ںϼ���
			doOutLine(Num,outline);
			//���ںϾ͵ý���������鴢��
			outlinesinfo.outlines[k/2] = (struct OutLine *)malloc(sizeof(struct OutLine)*(Num+2));

			column = 0;
			for(int i = 0;i< Num;i++)
			{
				//�����Ϊ1��������¼
				if(outline[i].code == 1)
				{
					//���ڹ�С�ĵ����ȥ�� ����ֵ�������¼���õ���ֵ
					if((outline[i].Right-outline[i].Left+1)*(outline[i].Down - outline[i].Up+1) < 8)
						continue;
					//�����Ľ��п���
					outlinesinfo.outlines[k/2][column].Up = outline[i].Up;
					outlinesinfo.outlines[k/2][column].Down = outline[i].Down;
					outlinesinfo.outlines[k/2][column].Left = outline[i].Left;
					outlinesinfo.outlines[k/2][column].Right = outline[i].Right;
					outlinesinfo.outlines[k/2][column].code = column;
					column++;
				}
			}
			//�����д��ṹ��
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
	//����ռ�
	int *count_x = new int[src->height];
	memset(count_x,0,sizeof(int)*src->height);

	//ˮƽͶӰ����
	for(int i =X; i<XX ;i++)
	{
		for(int j = Y ;j<YY ;j++)
		{
			if((uchar)src->imageData[j*widthStep + i] == 0)
				count_x[j]++;
		}
	}

	//��¼���еĿ�ʼ
	int mark_x[60];
	int mark_x_count =0;
	int flag_x=0;
	//
	for(int i = Y ; i < YY ;i++)
	{
		//�п�ʼ��ֵ�����޸� ���ж����ĸ���
		if(count_x[i] > 4 && flag_x ==0)
		{
			//�жϸ��п�ʼ����һ�н����ľ��� �����ں����� ������о���С��ʮ
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
		//�н��� �н������ж����� 3 ��ֵ�����޸�
		else if(count_x[i] < 3 && flag_x ==1)
		{
			//�ж��н����Ϳ�ʼ�ľ��� ����ȥ�� ��ֵ�����޸�
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

	//������
	IplImage * newSrc = pyrDown(src);
	cvSmooth(newSrc,newSrc);
	//��ȡ�����Ե��Ϣ
	IplImage *edge = cvCreateImage(cvSize(newSrc->width,newSrc->height),8,1);
	getEdge(newSrc,edge);

	//��ֵ��
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







