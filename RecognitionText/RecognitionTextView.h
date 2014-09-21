
// RecognitionTextView.h : CRecognitionTextView ��Ľӿ�
//

#pragma once
#include "Tools.h"


class CRecognitionTextView : public CScrollView
{
protected: // �������л�����
	CRecognitionTextView();
	DECLARE_DYNCREATE(CRecognitionTextView)

	// ����
public:
	CRecognitionTextDoc* GetDocument() const;

	// ����
public:

	// ��д
public:
	virtual void OnDraw(CDC* pDC);  // ��д�Ի��Ƹ���ͼ
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // ������һ�ε���
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

	// ʵ��
public:
	virtual ~CRecognitionTextView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	// ���ɵ���Ϣӳ�亯��
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();

private:
	Tools tools;					//����������
	CString filePath;				//�ļ�·��
	IplImage *src;
	char fileName[200][50];			//�ļ����洢����
	int index;						//��¼�ļ����ļ������е�λ��
	int fileCount;
	int removeList[100];			//ɾ���б�
	int rIndex;
	CPoint downPoint;
	CPoint upPoint;
	CString fileDictionary;
	int srcWidth;					//ͼƬ��ԭ���
	int srcHeight;					//ͼƬ��ԭ�߶�
	int xSt;
	float resizeX;
	float resizeY;
	HBITMAP hBitmap;
	CScrollBar m_VScrollBar;
	SCROLLINFO si;
	boolean isSized;				//��¼ͼƬ��С�Ƿ���е�����
	char words[7536][3];		//���ڴ洢���ص�����
	static const int REC_CHARAC_DIS = 20;			//��������ʵ����֮��ˮƽ�����ϵľ���
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
	afx_msg void OnLoadImage();				//����ͼƬ������
	afx_msg void OnPreviousPage();			//��һҳ������
	afx_msg void OnNextPage();				//��һҳ������
	afx_msg void OnDeletePage();			//ɾ��ҳ������
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);		//���������´�����
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);		//����Ҽ����´�����
	afx_msg void OnAdd();					//��Ӿ��δ�����
	afx_msg void OnDelete();				//ɾ�����δ�����
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

#ifndef _DEBUG  // RecognitionTextView.cpp �еĵ��԰汾
inline CRecognitionTextDoc* CRecognitionTextView::GetDocument() const
{ return reinterpret_cast<CRecognitionTextDoc*>(m_pDocument); }
#endif

