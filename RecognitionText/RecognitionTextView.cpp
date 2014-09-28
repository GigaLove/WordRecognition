
// RecognitionTextView.cpp : CRecognitionTextView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "RecognitionText.h"
#endif

#include "RecognitionTextDoc.h"
#include "RecognitionTextView.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CRecognitionTextView

IMPLEMENT_DYNCREATE(CRecognitionTextView, CScrollView)

	BEGIN_MESSAGE_MAP(CRecognitionTextView, CScrollView)
		// 标准打印命令
		ON_COMMAND(ID_FILE_PRINT, &CScrollView::OnFilePrint)
		ON_COMMAND(ID_FILE_PRINT_DIRECT, &CScrollView::OnFilePrint)
		ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CRecognitionTextView::OnFilePrintPreview)
		ON_WM_CONTEXTMENU()
		ON_WM_RBUTTONUP()
		ON_WM_PAINT()
		ON_COMMAND(ID_Load_Image, &CRecognitionTextView::OnLoadImage)
		ON_COMMAND(ID_Previous_Page, &CRecognitionTextView::OnPreviousPage)
		ON_COMMAND(ID_Next_Page, &CRecognitionTextView::OnNextPage)
		ON_COMMAND(ID_Delete_Page, &CRecognitionTextView::OnDeletePage)
		ON_WM_LBUTTONDOWN()
		ON_WM_LBUTTONUP()
		ON_COMMAND(ID_Add, &CRecognitionTextView::OnAdd)
		ON_COMMAND(ID_Delete, &CRecognitionTextView::OnDelete)
		ON_COMMAND(ID_Deal, &CRecognitionTextView::OnDeal)
		ON_WM_SIZE()
		ON_WM_VSCROLL()
		ON_WM_HSCROLL()
		ON_COMMAND(ID_LoadCharacter, &CRecognitionTextView::OnLoadcharacter)
		ON_COMMAND(ID_SaveWords, &CRecognitionTextView::OnSavewords)
		ON_COMMAND(ID_ImageCut, &CRecognitionTextView::OnImagecut)
		ON_WM_MOUSEWHEEL()
	END_MESSAGE_MAP()

	// CRecognitionTextView 构造/析构

	CRecognitionTextView::CRecognitionTextView()
	{
		// TODO: 在此处添加构造代码
		initData();
		resizeX = 2;
		resizeY = 2;
		LINESIZE = 60;
		src = NULL;
		//可以不需要初始化
		downX = 0;
		downY = 0;
		upX = 0;
		upY = 0;
		for (int i=0; i<7536; i++)
		{
			for (int j=0;j<3;j++)
			{
				words[i][j] = '\0';
			}
		}
	}

	void CRecognitionTextView::initData()
	{
		tools.outlinesinfo.rows = -1;
		index = 0;
		fileCount = 0;
		rIndex = 0;
		fileDictionary = "";
		filePath = "";
		isSized = false;
		m_nVScrollPos = 0;		
		clsCutInfo();
	}

	void CRecognitionTextView::clsCutInfo()
	{
		isCutted = false;
		cutHeight = 0;
	}

	CRecognitionTextView::~CRecognitionTextView()
	{
	}

	BOOL CRecognitionTextView::PreCreateWindow(CREATESTRUCT& cs)
	{
		// TODO: 在此处通过修改
		//  CREATESTRUCT cs 来修改窗口类或样式

		return CScrollView::PreCreateWindow(cs);
	}

	// CRecognitionTextView 绘制

	void CRecognitionTextView::OnDraw(CDC* /*pDC*/)
	{
		CRecognitionTextDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		if (!pDoc)
			return;

		// TODO: 在此处为本机数据添加绘制代码
	}

	void CRecognitionTextView::OnInitialUpdate()
	{
		CScrollView::OnInitialUpdate();

		//CSize sizeTotal;
		//// TODO: 计算此视图的合计大小
		//sizeTotal.cx = sizeTotal.cy = 100;
		//SetScrollSizes(MM_TEXT, sizeTotal);
	}


	// CRecognitionTextView 打印


	void CRecognitionTextView::OnFilePrintPreview()
	{
#ifndef SHARED_HANDLERS
		AFXPrintPreview(this);
#endif
	}

	BOOL CRecognitionTextView::OnPreparePrinting(CPrintInfo* pInfo)
	{
		// 默认准备
		return DoPreparePrinting(pInfo);
	}

	void CRecognitionTextView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
	{
		// TODO: 添加额外的打印前进行的初始化过程
	}

	void CRecognitionTextView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
	{
		// TODO: 添加打印后进行的清理过程
	}

	void CRecognitionTextView::OnRButtonUp(UINT /* nFlags */, CPoint point)
	{
		ClientToScreen(&point);
		OnContextMenu(this, point);
	}

	void CRecognitionTextView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
	{
#ifndef SHARED_HANDLERS
		theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
	}


	// CRecognitionTextView 诊断

#ifdef _DEBUG
	void CRecognitionTextView::AssertValid() const
	{
		CScrollView::AssertValid();
	}

	void CRecognitionTextView::Dump(CDumpContext& dc) const
	{
		CScrollView::Dump(dc);
	}

	CRecognitionTextDoc* CRecognitionTextView::GetDocument() const // 非调试版本是内联的
	{
		ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CRecognitionTextDoc)));
		return (CRecognitionTextDoc*)m_pDocument;
	}
#endif //_DEBUG


	void CRecognitionTextView::OnPaint()
	{
		CPaintDC dc(this); // device context for painting
		// TODO: 在此处添加消息处理程序代码
		// 不为绘图消息调用 CScrollView::OnPaint()
		dc.SetWindowOrg(0,m_nVScrollPos);
		hBitmap=(HBITMAP)LoadImage(NULL,filePath,IMAGE_BITMAP,0,0,LR_CREATEDIBSECTION|LR_DEFAULTSIZE|LR_LOADFROMFILE);
		if(hBitmap != NULL)
		{
			//获取设备的上下文
			CDC* pdc = GetDC();
			pdc->SetWindowOrg(0, m_nVScrollPos);
			CBitmap   bitmap;
			//将bitmap对象与获取的句柄绑定起来
			bitmap.Detach();
			bitmap.Attach(hBitmap);
			CDC memDC;
			//根据已有的上下文，再度创建上下文
			memDC.CreateCompatibleDC(pdc);
			//选择GDI绘图对象
			memDC.SelectObject(&bitmap);
			BITMAP bitinfo;
			bitmap.GetBitmap(&bitinfo);
			srcWidth = bitinfo.bmWidth;
			srcHeight = bitinfo.bmHeight;
			//从指定设备上下文拷贝位图 
			//pdc->BitBlt(0, 0, srcWidth, srcHeight, &memDC, 0, 0, SRCCOPY);
			//获取窗体大小
			CRect rect;
			GetClientRect(&rect);

			int winWidth = rect.Width();
			//int winHeight = rect.Height;
			//计算为中间位置的起点
			xSt = 0 > (winWidth - bitinfo.bmWidth / resizeX)/2 ? 0:(winWidth - bitinfo.bmWidth / resizeX)/2;
			//设置图像拉伸模式
			pdc->SetStretchBltMode(HALFTONE);
			//拉伸图像
			pdc->StretchBlt(xSt,0,bitinfo.bmWidth / resizeX,bitinfo.bmHeight / resizeY,&memDC,0,0,
				bitinfo.bmWidth,bitinfo.bmHeight,SRCCOPY);
			//删除GDI对象
			bitmap.DeleteObject();
			//删除设备上下文环境
			memDC.DeleteDC();
			if(!isSized)
			{
				CRect rect;
				GetClientRect(&rect);
				//m_nViewWidth = rect.Width();
				m_nViewHeight = srcHeight/2;
				m_nVPageSize = rect.Height();
				si.fMask = SIF_ALL;
				si.nMin = 0;
				si.nMax = srcHeight / 2;
				si.nPos = 0;
				si.nPage = rect.Height();
				SetScrollInfo(SB_VERT, &si, TRUE);
				isSized = true;
			}
			drawRectangle(0, m_nVScrollPos);
			drawCharacter(0, m_nVScrollPos);
		}

	}

	//加载图片处理函数
	void CRecognitionTextView::OnLoadImage()
	{
		// TODO: 在此添加命令处理程序代
		static TCHAR BASED_CODE szFilter[] = _T("Map Files (*.bmp)|*.bmp||");			//进行文件过滤
		CFileDialog fileDialog(TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, this);
		if(fileDialog.DoModal() == IDOK)
		{
			initData();
			CString temp;
			temp = fileDialog.GetPathName();
			fileDictionary = temp.Left(temp.ReverseFind('\\')) + CString("\\");
			MessageBox(fileDictionary);
		}
		else
		{
			return;
		}

		fileCount = tools.findFile(fileDictionary + "*.bmp", fileName);			//获取当前文件夹下文件数，并将文件名添加到数组中

		if(fileCount == -1)
		{
			MessageBox(TEXT("图片路径有误"));
		}
		filePath = fileDictionary + (CString)fileName[0];
		OnPaint();		//进行绘制第一幅图片
	}

	//上一页处理函数
	void CRecognitionTextView::OnPreviousPage()
	{
		// TODO: 在此添加命令处理程序代码
		if (fileCount > 0)
		{
			if(index > 0)			//当前图片下标有效
			{
				if(tools.outlinesinfo.rows >-1)		//判断当前图片是否处理过，处理过的话为true
				{
					clearMem();			//清空结构体指针
				}
				int tempIndex = index;
				index -= 1;

				for (int i = rIndex-1;i>=0;i--)		//过滤删除页数的index
				{
					if(index == removeList[i])
					{
						index--;
					}
				}
				if (index < 0)
				{
					index = tempIndex;
					MessageBox(TEXT("前续页数已被删除，这已经是第一张图片"));
					return;
				}
				filePath = fileDictionary + (CString)fileName[index];
				//加载图片
				clsCutInfo();
				OnPaint();
			}
			else{
				MessageBox(TEXT("这是第一张图片"));
			}	
		} else {
			MessageBox(TEXT("请先加载图片"));
		}

	}

	//下一页处理函数
	void CRecognitionTextView::OnNextPage()
	{
		// TODO: 在此添加命令处理程序代码
		if (fileCount > 0)
		{
			if(index < fileCount-1)
			{
				if(tools.outlinesinfo.rows >-1)		//判断当前图片是否处理过，处理过的话为true
				{
					clearMem();			//清空结构体指针
				}
				int tempIndex = index;
				index += 1;
				for (int i=0;i<rIndex;i++)					//过滤删除页数的index
				{    
					if(index == removeList[i]){
						index++;
					}
				}
				if (index >= fileCount)
				{
					index = tempIndex;
					MessageBox(TEXT("后续页数已被删除，这已经是最后一张图片"));
					return;
				}
				filePath = fileDictionary + (CString)fileName[index];
				//加载图片
				clsCutInfo();
				OnPaint();
			}
			else{
				MessageBox(TEXT("这已经是最后一张图片"));
			}
		} else {
			MessageBox(TEXT("请先加载图片"));
		}
	}

	//清空结构体指针
	void CRecognitionTextView::clearMem()
	{
		//释放结构体指针
		for(int i = 0 ;i < tools.outlinesinfo.rows;i++)
		{
			if(tools.outlinesinfo.outlines[i] != NULL)
			{	
				free(tools.outlinesinfo.outlines[i]);		//释放行指针
				tools.outlinesinfo.outlines[i] = NULL;
			}
		}

		free(tools.outlinesinfo.columns);
		free(tools.outlinesinfo.BegEnd);
		free(tools.outlinesinfo.outlines);

		tools.outlinesinfo.rows = -1;
		tools.outlinesinfo.BegEnd = NULL;
		tools.outlinesinfo.columns = NULL;
		tools.outlinesinfo.outlines = NULL;
	}

	//删除页数处理函数
	void CRecognitionTextView::OnDeletePage()
	{
		// TODO: 在此添加命令处理程序代码
		if (fileCount > 0)
		{
			removeList[rIndex] = index;			//将当前图片的index添加到移除列表
			rIndex ++;
			if(index < fileCount - 1){
				OnNextPage();
			} else {
				OnPreviousPage();	
			}
		} else {
			MessageBox(TEXT("请先加载图片"));
		}
		//释放空间
		cvReleaseImage(&src);
	}

	//鼠标左键按下处理函数
	void CRecognitionTextView::OnLButtonDown(UINT nFlags, CPoint point)
	{
		// TODO: 在此添加消息处理程序代码和/或调用默认值
		downPoint = point;
		downX = (point.x - xSt) * resizeX;
		downY = (point.y + m_nVScrollPos) * resizeY;
		isCutted = false;
		CScrollView::OnLButtonDown(nFlags, point);

	}

	//鼠标左键松开处理函数
	void CRecognitionTextView::OnLButtonUp(UINT nFlags, CPoint point)
	{
		// TODO: 在此添加消息处理程序代码和/或调用默认值
		upPoint = point;
		upX = (point.x - xSt) * resizeX;
		upY = (point.y  + m_nVScrollPos)*resizeY;
		CScrollView::OnLButtonUp(nFlags, point);
	}

	//添加矩形处理函数
	void CRecognitionTextView::OnAdd()
	{
		int k = -1;
		int distance = 1000;
		//获取实际位置
		int up = int((downPoint.y+m_nVScrollPos) * resizeY);
		int down = int((upPoint.y+m_nVScrollPos) * resizeY);
		int left = int(downPoint.x - xSt)* resizeX;
		int right = int(upPoint.x- xSt) * resizeX;

		//判断在哪一行
		for(int i = 0; i < tools.outlinesinfo.rows;i++)
		{
			if(tools.outlinesinfo.columns[i] == -1)
				continue;
			if(abs(up - tools.outlinesinfo.BegEnd[i*2])+abs(down - tools.outlinesinfo.BegEnd[i*2 + 1]) < distance)
			{
				distance = abs(up - tools.outlinesinfo.BegEnd[i*2])+abs(down - tools.outlinesinfo.BegEnd[i*2 + 1]);
				k = i;
			}
		}

		if (k >= 0)
		{
			tools.outlinesinfo.outlines[k][tools.outlinesinfo.columns[k]].Up = up;
			tools.outlinesinfo.outlines[k][tools.outlinesinfo.columns[k]].Down = down;
			tools.outlinesinfo.outlines[k][tools.outlinesinfo.columns[k]].Left = left;
			tools.outlinesinfo.outlines[k][tools.outlinesinfo.columns[k]].Right = right;

			//判断处于第几个位置
			int code = -1;
			for(int i = 0; i<tools.outlinesinfo.columns[k];i++)
			{
				//将大于它的位置 往后移一个
				if(left < tools.outlinesinfo.outlines[k][i].Left)
				{
					if(tools.outlinesinfo.outlines[k][i].code != -1)
						tools.outlinesinfo.outlines[k][i].code ++;

					if(code != -1)
						code  = i;
				}
			}

			tools.outlinesinfo.outlines[k][tools.outlinesinfo.columns[k]].code = code;
			tools.outlinesinfo.columns[k] ++;

			//drawRectangle(0, m_nVScrollPos);
			OnPaint();
		}
	}

	//删除矩形处理函数
	void CRecognitionTextView::OnDelete()
	{
		// TODO: 在此添加命令处理程序代码
		for(int i=0; i< tools.outlinesinfo.rows; i++)
		{
			if(tools.outlinesinfo.columns[i]==-1)
				continue;

			for(int j = 0; j< tools.outlinesinfo.columns[i] ;j++)
			{
				if ((downPoint.x- xSt)*resizeX < tools.outlinesinfo.outlines[i][j].Left
					&& (downPoint.y+m_nVScrollPos)*resizeY < tools.outlinesinfo.outlines[i][j].Up 
					&& (upPoint.x- xSt)*resizeX > tools.outlinesinfo.outlines[i][j].Right
					&& (upPoint.y+m_nVScrollPos)*resizeY > tools.outlinesinfo.outlines[i][j].Down)
				{
					tools.outlinesinfo.outlines[i][j].code = -1;
				}
			}
		}

		//加载图片
		//hBitmap=(HBITMAP)LoadImage(NULL,filePath,IMAGE_BITMAP,0,0,LR_CREATEDIBSECTION|LR_DEFAULTSIZE|LR_LOADFROMFILE);
		OnPaint();
		//drawRectangle(0, m_nVScrollPos);
	}

	//绘制矩形处理函数
	void CRecognitionTextView::drawRectangle(int move_x, int move_y)
	{
		CClientDC dc(this);				//获取客户区环境
		CBrush *pBrush=CBrush::FromHandle((HBRUSH)
			GetStockObject(NULL_BRUSH));

		CBrush *pOldBrush=dc.SelectObject(pBrush);

		for(int i=0; i< tools.outlinesinfo.rows; i++)
		{
			if(tools.outlinesinfo.columns[i]==-1)
				continue;

			for(int j = 0; j< tools.outlinesinfo.columns[i] ;j++)
			{
				if (tools.outlinesinfo.outlines[i][j].code >= 0)
				{
					dc.Rectangle(CRect(CPoint(int(tools.outlinesinfo.outlines[i][j].Left/resizeX - move_x)+xSt, int(tools.outlinesinfo.outlines[i][j].Up/resizeY - move_y)), CPoint(int(tools.outlinesinfo.outlines[i][j].Right/resizeX - move_x)+xSt, int(tools.outlinesinfo.outlines[i][j].Down/resizeY - move_y))));
				}
			}
		}
	}

	//画字符 就是按顺序花字符 可能有错
	void CRecognitionTextView::drawCharacter(int move_x, int move_y)
	{
		CClientDC dc(this);
		int ST = index;
		int l = 0;
		int m = 0;
		int number  = 0;
		//进入行循环
		for(int i = 0;i < tools.outlinesinfo.rows;i++)
		{
			if(tools.outlinesinfo.columns[i]<0)
				continue;
			//进入列循环 画字
			l = 0;
			for(int j = 0;j < tools.outlinesinfo.columns[i]; j++)
			{
				//判断第 j个字是什么
				for(int k = 0; k < tools.outlinesinfo.columns[i];k++)
				{
					if(j == tools.outlinesinfo.outlines[i][k].code )
					{
						dc.TextOutW(int(tools.outlinesinfo.outlines[i][k].Right/resizeX - move_x)+xSt +20, tools.outlinesinfo.outlines[i][k].Up/resizeY - move_y, CString(words[ST*150+m*10+l]));
						//在下方画出序号
						char num[5];
						_itoa(number,num,10);
						dc.TextOutW(int(tools.outlinesinfo.outlines[i][k].Left/resizeX - move_x)+xSt, tools.outlinesinfo.outlines[i][k].Down/resizeY - move_y + 20 , CString(num));
						//计算该字的总序号
						tools.outlinesinfo.outlines[i][k].No = number + ST*150;
						number ++;
						l++;
					}
				}
			}
			m++;
		}
	} 


	//处理图片的事件处理函数
	void CRecognitionTextView::OnDeal()
	{
		// TODO: 在此添加命令处理程序代码
		if(filePath !="")
		{
			src = tools.deal(filePath,isCutted,downX,upX,downY,upY);
			if (src == NULL)
				MessageBox(TEXT("加载失败！"));
			else
			{
				drawRectangle(0, m_nVScrollPos);
				if (words[0][0] != '\0')
				{
					drawCharacter(0, m_nVScrollPos);
				} else {
					MessageBox(_T("请先加载文字"));
				}
				OnPaint();
			}
		}
		else
		{
			MessageBox(TEXT("未加载图片！"));
		}
	}


	void CRecognitionTextView::OnSize(UINT nType, int cx, int cy)
	{
		if(isSized)
		{
			CScrollView::OnSize(nType, cx, cy);
			int nVScrollMax = 0;
			m_nVScrollPos = m_nVPageSize = 0;
			if (cy < m_nViewHeight){
				nVScrollMax = m_nViewHeight -1;
				m_nVPageSize = cy;
				if(m_nVScrollPos > m_nViewHeight - 1 - m_nVPageSize)
				{
					m_nVScrollPos = m_nViewHeight - 1 - m_nVPageSize;
				}
			}
			si.fMask = SIF_ALL;
			si.nMin = 0;
			si.nMax = nVScrollMax;
			si.nPos = m_nVScrollPos;
			si.nPage = m_nVPageSize;
			SetScrollInfo(SB_VERT, &si, TRUE);
		}
		// TODO: 在此处添加消息处理程序代码
	}


	void CRecognitionTextView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
	{
		// TODO: 在此添加消息处理程序代码和/或调用默认值
		int nDelta;
		switch (nSBCode)
		{
		case SB_LINEUP:
			nDelta = -LINESIZE;
			break;
		case SB_LINEDOWN:
			nDelta = LINESIZE;
			break;
		case SB_PAGEUP:
			nDelta = -m_nVPageSize;
			break;
		case SB_PAGEDOWN:
			nDelta = m_nVPageSize;
			break;
		case SB_THUMBTRACK:
			nDelta = int(nPos) - m_nVScrollPos;
			break;
		default:
			return;
		}
		int nScrollPos = m_nVScrollPos + nDelta;
		int nMaxPos = m_nViewHeight - m_nVPageSize;
		if (nScrollPos < 0){
			nDelta = - m_nVScrollPos;
		}
		else if(nScrollPos > nMaxPos){
			nDelta = 0;
		}
		if (nDelta != 0){
			m_nVScrollPos += nDelta;
			SetScrollPos(SB_VERT, m_nVScrollPos, TRUE);
			ScrollWindow(0, -nDelta);
		}	

	}


	void CRecognitionTextView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
	{
		// TODO: 在此添加消息处理程序代码和/或调用默认值
		CScrollView::OnHScroll(nSBCode, nPos, pScrollBar);
	}

	//加载文字处理函数
	void CRecognitionTextView::OnLoadcharacter()
	{
		// TODO: 在此添加命令处理程序代码
		if (fileCount > 0)
		{
			static TCHAR BASED_CODE szFilter[] = _T("Map Files (*.dat)|*.dat||");			//进行文件过滤
			CFileDialog fileDialog(TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, this);
			if(fileDialog.DoModal() == IDOK){
				CString datFileName;
				datFileName = fileDialog.GetPathName();			
				MessageBox(datFileName);
				USES_CONVERSION;			//声明标识符
				FILE *file = fopen(T2A(datFileName), "rb");		//调用函数库实现CString到const char*转换
				if(!file)
				{
					printf("打开文件失败");
					return;
				}
				for(int i = 0 ;i<7356;i++)
				{
					//读取字符
					fread(words[i], 2, 1, file);
					words[i][2] = '\0';

					printf("%s\t",words[i]);
				}
				fclose(file);
			}
		} 
		else 
		{
			MessageBox(_T("请先加载图片"));
		}
	}

	//用来储存结果 一类字一个文件  一共就有7千多个文件
	void CRecognitionTextView::OnSavewords()
	{
		CvMat* submat;
		IplImage *header;
		IplImage * newsrc;
		//按顺序储存 一张图的字 
		//建立7356个文件夹 每一储存一类文件图片
		for(int i = 0;i < tools.outlinesinfo.rows;i++)
		{
			if(tools.outlinesinfo.columns[i]<0)
				continue;
			//进入列循环 画字
			for(int j = 0;j < tools.outlinesinfo.columns[i]; j++)
			{
				//判断第 j个字是什么
				if(tools.outlinesinfo.outlines[i][j].code >= 0)
				{
					int new_width = tools.outlinesinfo.outlines[i][j].Right - tools.outlinesinfo.outlines[i][j].Left;
					int new_height = tools.outlinesinfo.outlines[i][j].Down - tools.outlinesinfo.outlines[i][j].Up;

					submat = new CvMat();
					header = cvCreateImageHeader(cvSize(new_width,new_height),8,1);

					CvRect rect = cvRect(tools.outlinesinfo.outlines[i][j].Left, tools.outlinesinfo.outlines[i][j].Up,new_width,new_height);

					cvGetSubRect(src,submat,rect);

					newsrc = cvGetImage(submat,header);
					//产生该字的文件名
					char file[30] = "F:\\RS\\";
					char num[6];
					_itoa(tools.outlinesinfo.outlines[i][j].No,num,10);//计算其序数
					strcat(file,num);
					//判断文件夹是否存在
					if(_access(file,0))
						_mkdir(file);
					//产生随机数 生成该字的名 随机命名
					srand((unsigned)time(NULL));
					char filefullname[30];

					do
					{
						_itoa(rand()%65356,num,10);
						strcpy(filefullname,file);
						strcat(filefullname,"\\");
						strcat(filefullname,num);
						strcat(filefullname,".bmp");
					}while(!_access(filefullname,0));

					cvSaveImage(filefullname,newsrc);
				}
			}
		}

		MessageBox(_T("图片保存成功"));
		cvReleaseImage(&src);
		OnNextPage();
	}


	void CRecognitionTextView::OnImagecut()
	{
		// TODO: 在此添加命令处理程序代码
		if (fileCount >0) {
			if (downX < 0)	
				downX = 0;
			if (downX > srcWidth) 
				downX = srcWidth;
			if (upX < 0)	
				upX = 0;
			if (upX > srcWidth) 
				upX = srcWidth;
			
			if (downY < 0)
				downY = 0;
			if (downY > srcHeight)
				downY = srcHeight;
			if (upY < 0)
				upY = 0;
			if (upY > srcHeight)
				upY = srcHeight;
			isCutted = true;

		} else {
			MessageBox(_T("请先加载图片"));
		}
	}


	BOOL CRecognitionTextView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
	{
		// TODO: 在此添加消息处理程序代码和/或调用默认值
		if (zDelta < 0)
		{
			OnVScroll(SB_LINEDOWN, 0, &m_VScrollBar);
		}
		else if (zDelta > 0)
		{
			OnVScroll(SB_LINEUP, 0, &m_VScrollBar);
		}
		return CScrollView::OnMouseWheel(nFlags, zDelta, pt);
	}
