
// RecognitionTextView.h : CRecognitionTextView 类的接口
//

#pragma once
#include "Tools.h"


class CRecognitionTextView : public CScrollView
{
protected: // 仅从序列化创建
	CRecognitionTextView();
	DECLARE_DYNCREATE(CRecognitionTextView)

	// 特性
public:
	CRecognitionTextDoc* GetDocument() const;

	// 操作
public:

	// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // 构造后第一次调用
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

	// 实现
public:
	virtual ~CRecognitionTextView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	// 生成的消息映射函数
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();

private:
	Tools tools;					//声明工具类
	CString filePath;				//文件路径
	IplImage *src;
	char fileName[200][50];			//文件名存储数组
	int index;						//记录文件在文件数组中的位置
	int fileCount;
	int removeList[100];			//删除列表
	int rIndex;
	CPoint downPoint;
	CPoint upPoint;
	CString fileDictionary;
	int srcWidth;					//图片的原宽度
	int srcHeight;					//图片的原高度
	int xSt;
	float resizeX;
	float resizeY;
	HBITMAP hBitmap;
	CScrollBar m_VScrollBar;
	SCROLLINFO si;
	boolean isSized;				//记录图片大小是否进行调整过
	char words[7536][3];		//用于存储加载的文字
	static const int REC_CHARAC_DIS = 20;			//方框与真实文字之间水平方向上的距离
	boolean isCutted;
private:
	int m_nViewHeight;
	int m_nVScrollPos;
	int m_nVPageSize;
	int LINESIZE;
				

	//int m_MAX_W, m_MAX_H;
	//int m_vtop, m_hleft;
	//int m_NowBmpW, m_NowBmpH;
	

public:
	afx_msg void OnLoadImage();				//加载图片处理函数
	afx_msg void OnPreviousPage();			//上一页处理函数
	afx_msg void OnNextPage();				//下一页处理函数
	afx_msg void OnDeletePage();			//删除页处理函数
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);		//鼠标左键按下处理函数
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);		//鼠标右键按下处理函数
	afx_msg void OnAdd();					//添加矩形处理函数
	afx_msg void OnDelete();				//删除矩形处理函数
public:
	void drawRectangle(int move_x, int move_y);
	void clearMem();
	void initData();
	void drawCharacter(int move_x, int move_y);

	afx_msg void OnDeal();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnLoadcharacter();
	afx_msg void OnSavewords();
	afx_msg void OnImagecut();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
};

#ifndef _DEBUG  // RecognitionTextView.cpp 中的调试版本
inline CRecognitionTextDoc* CRecognitionTextView::GetDocument() const
{ return reinterpret_cast<CRecognitionTextDoc*>(m_pDocument); }
#endif

