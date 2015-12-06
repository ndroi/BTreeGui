#ifndef QTBTREE_H
#define QTBTREE_H
#include <QtWidgets/QMainWindow>
#include<QtWidgets\qdialog.h>
#include<QtGui\qpainter.h>
#include "ui_qtbtree.h"
#include "ui_NewBtreeDLG.h"
#include"ui_InsAndDel.h"
#include"BTree.cpp"
#include<vector>
class DrawAll;
class InsAndDelDLG :public QDialog
{
	Q_OBJECT
public:
	InsAndDelDLG(QWidget *parent = 0);
	~InsAndDelDLG();
	Ui::Dialog ui;
};
class NewBtreeDLG :public QDialog
{
	Q_OBJECT
public:
	NewBtreeDLG(QWidget *parent = 0);
	~NewBtreeDLG();
	int getM();
	int getNum();
	vector<int> getKeys();
private:
	Ui::NewBtreeDLG ui;
public:
	void accept();
};
class QtBtree : public QMainWindow
{
	Q_OBJECT

public:
    bool ok = false;
	QtBtree(QWidget *parent = 0);
	~QtBtree();
	NewBtreeDLG* nbdlg;
	BTree<int>* bt=NULL;
	DrawAll* da=NULL;
	void paintEvent(QPaintEvent* event);
private:
	Ui::QtBtreeClass ui;
	private slots:
	void NewBtree();
	void InsDel();
};
class DrawNode//管理一个结点
{
public:
	static const int w = 20;//每个矩形的宽度和高度,这两个各值一般不变
	static const int h = 20;
	QPainter* p;//画笔
	QPoint pt;//该节点所在的坐标,(左上角)
	QPoint parent;//父节点所在的位置
	DrawNode();//
	void draw();
	vector<int>keys;//该节点的所有键值，即要画出n个矩形块
	QPoint getPoint(int i);//获取第i个子节点应该连接的点
	int getKeysNum();//获取keys个数
};
class DrawRow//管理一行节点
{
public:
	static const int d = 15;//节点之间的距离
	QPoint pt; //该节点排所在的坐标, (左上角)
	QPainter* p;//画笔
	DrawRow* parentRow=NULL;//父节点排，用来查询本排节点的父节点坐标
	DrawRow();
	vector<DrawNode> rowNodes;
	void getNode(DrawNode dn);//获取DrawNode对象
	void draw();
	// 获取DrawRow在画板上的实际长度
	int GetLen();
	// 得到第index个子节点的父节点的坐标
	QPoint getParentPoint(int index);
};
class DrawAll//管理所有节点排
{
public:
	static const int d = 40;//节点排之间的距离
	QPoint pt; //画图所在的坐标, (左上角)
	QPainter* p;//画笔
	DrawAll();
	vector<DrawRow> rows;
	void getRow(DrawRow dr);//获取DrawRow对象
	void draw();
	void Create(BTree<int>* bt);
	bool isNeedNewRow();
};
#endif // QTBTREE_H
