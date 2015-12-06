#pragma once
#include <queue>
#include<qstring.h>
using namespace std;
//B树的结点
template <typename T>
struct BTreeNode
{
	int num;               //关键字个数	
	T *K;                  //指向关键字数组
	BTreeNode<T> *parent;  //指向父亲结点
	BTreeNode<T> **A;      //指向孩子结点的指针
	BTreeNode(int n, int m, BTreeNode<T>  *p)
	{
		num = n;
		parent = p;
		K = new T[m + 1];           //最多有m-1个关键字，K0不用，Km用来当哨兵
		A = new BTreeNode *[m + 1]; //最多有m个分支，Am用来当哨兵
		for (int i = 0; i <= m; i++)
			A[i] = NULL;
	}
	~BTreeNode()
	{
		delete[] K; K = NULL;
		delete[] A; A = NULL;
	}
};

//搜索结果的三元组定义
template <typename T>
struct Triple
{
	BTreeNode<T> * node;  //关键字所在结点
	int i;                //关键字下标位置
	bool tag;             //搜索是否成功
	Triple(BTreeNode<T> *nd, int pos, bool t)
	{
		node = nd; i = pos; tag = t;
	}
};

//B树定义
template <typename T>
class BTree
{
public:
	BTree(int m, BTreeNode<T> * root);
	~BTree();
	Triple<T> Search(const T& x); //搜索
	bool Insert(const T& x);      //插入
	bool Delete(const T& x);      //删除
	void InsertKey(BTreeNode<T> *p, T k, BTreeNode<T> *a, int i);    //插入一个二元组(K,A)
	void SpliteNode(BTreeNode<T> *p, T *k, BTreeNode<T> **a, int i); //分裂结点
	void RightAdjust(BTreeNode<T> *p, BTreeNode<T> *q, int i);  //从右子女取关键字
	void LeftAdjust(BTreeNode<T> *p, BTreeNode<T> *q, int i);   //从左子女取关键字
	void LeftCompress(BTreeNode<T> *p, int i);  //往左移动1个位置
	void RightCompress(BTreeNode<T> *p, int i); //往右移动1个位置
	void MergeNode(BTreeNode<T> *p, BTreeNode<T> *q, BTreeNode<T> *pR, int i); //合并两个结点
	QString SaveBTree(); //返回QString到Btree
private:
	int m_m;                //路数，即最大子树棵数
	BTreeNode<T> *m_pRoot;  //B树的根结点
};
template<typename T>
BTree<T>::BTree(int m, BTreeNode<T> * root)
{
	m_m = m;
	m_pRoot = root;
}
template<typename T>
BTree<T>::~BTree() 
{
	if (m_pRoot != NULL)
	{
		queue<BTreeNode<T> *> nodeQueue; 
		nodeQueue.push(m_pRoot);         //放入根结点
		while (nodeQueue.size())
		{
			BTreeNode<T> * p = nodeQueue.front();
			if (p->A[0] != NULL) 
			{
				for (int i = 0; i <= p->num; i++)
					nodeQueue.push(p->A[i]);
			}
			nodeQueue.pop();
			delete p;
			p = NULL;
		}
	}
}

template <typename T>
Triple<T> BTree<T>::Search(const T &x)
{
	int i = 0;  //下标
	BTreeNode<T> *p = m_pRoot, *q = NULL;  //用来保存当前结点和它的父结点

	while (p != NULL) //一直检查到叶结点
	{
	
		int n = p->num;   //当前结点的关键字个数	
		for (i = 0; i < n && x >= p->K[i + 1]; i++)  //可以改进一下，用二分查找
			;
		if (x == p->K[i]) //是否已找到，不用判断下标，i最大为n 
			return Triple<T>(p, i, true);
		q = p;
		p = p->A[i];     //搜索下一层，Ki与Ki+1中间的指针
	}
	return Triple<T>(q, i, false); //x不在树中，找到了可以插入的结点位置
}
template <typename T>
bool BTree<T>::Insert(const T &x)
{
	if (m_pRoot == NULL) //空树
	{
		m_pRoot = new BTreeNode<T>(1, m_m, NULL);  //新的根含有1个关键字
		m_pRoot->K[1] = x;    //根的关键字
		return true;
	}

	Triple<T> triple = Search(x); 	//检查是否已存在
	if (triple.tag == true) //x已在B树中
		return false;

	BTreeNode<T> *p = triple.node, *q; //结点地址
	//构造插入的两元组(k,a) 其中k为关键字，a为右邻指针
	BTreeNode<T> *a = NULL;
	T k = x;
	int i = triple.i;

	while (1) //插入过程
	{
		if (p->num < m_m - 1) //关键字个数未到达上限，可以直接插入
		{
			InsertKey(p, k, a, i); //(k, a)插入到位置(Ki, Ai)后面
			return true;
		}
		SpliteNode(p, &k, &a, i); //将p结点分裂成两个结点
		if (p->parent != NULL)     //父结点不为空
		{
			q = p->parent; //获得父结点
			for (i = 0; i < q->num && x >= q->K[i + 1]; i++) //确定新的插入位置i
				;
			p = q;   //进入上一层
		}
		else
		{
			//已经到达了根，需要新建一个结点
			m_pRoot = new BTreeNode<T>(1, m_m, NULL);  //新的根含有1个关键字
			m_pRoot->K[1] = k; //新根的关键字
			m_pRoot->A[0] = p; //左指针
			m_pRoot->A[1] = a; //右指针
			p->parent = a->parent = m_pRoot; //更新左右指针的父结点
			return true;
		}
	}
}
template <typename T>
void BTree<T>::InsertKey(BTreeNode<T> *p, T k, BTreeNode<T> *a, int i)
{
	for (int j = p->num; j > i; j--) //将K[i],A[i]以后的元素都往后移一个位置
	{
		p->K[j + 1] = p->K[j];
		p->A[j + 1] = p->A[j];
	}
	p->num++;        //结点的关键字个数加1
	p->K[i + 1] = k; //插入两元组在K[i],A[i]以后
	p->A[i + 1] = a;
	if (a != NULL)    //若为为空，需更新父结点指针
		a->parent = p;
}
template <typename T>
void BTree<T>::SpliteNode(BTreeNode<T> *p, T *k, BTreeNode<T> **a, int i)
{
	InsertKey(p, *k, *a, i); //先插了再说
	int mid = (m_m + 1) / 2;   //[ceil(m/2)]
	int size = (m_m & 1) ? mid : mid + 1; //奇偶性决定了分裂时拷贝的关键字个数

	BTreeNode<T> *q = new BTreeNode<T>(0, m_m, p->parent); //新结点
	//将p的K[mid+1...m]和A[mid..m]移到q的K[1...mid-1]和A[0...mid-1]
	q->A[0] = p->A[mid];
	for (int j = 1; j < size; j++)
	{
		q->K[j] = p->K[mid + j];
		q->A[j] = p->A[mid + j];
	}
	//修改q中的子女的父结点为q
	if (q->A[0] != NULL)
	{
		for (int j = 0; j < size; j++)
			q->A[j]->parent = q;
	}
	//更新结点的关键字个数
	q->num = m_m - mid;  //结点q
	p->num = mid - 1;    //结点p
	//构建新的两元组(k,a)
	*k = p->K[mid];
	*a = q;
}


template <typename T>
bool BTree<T>::Delete(const T& x)
{
	Triple<T> triple = Search(x); //检查是否已存在
	if (triple.tag == false)       //x不在B树中
		return false;
	BTreeNode<T> *p = triple.node, *q; //要删除的关键字所在结点
	int i = triple.i;

	if (p->A[i] != NULL) //非叶结点
	{
		q = p->A[i];    //找右子树的最小关键码
		while (q->A[0] != NULL)
			q = q->A[0];
		p->K[i] = q->K[1];   //用叶结点替换
		LeftCompress(q, 1);  //删除K[1]，其实只是用后面的结点覆盖一下即可
		p = q;               //转换为叶结点的删除
	}
	else
		LeftCompress(p, i);  //叶结点直接删除，其实只是用后面的结点覆盖一下即可

	int mid = (m_m + 1) / 2; //求[ceil(m/2)]
	//下面开始调整
	while (1)
	{
		if (p == m_pRoot || p->num >= mid - 1) //情形1和情形2
			break;
		else
		{
			q = p->parent; //父亲结点
			for (i = 0; i <= q->num && q->A[i] != p; i++) //找到p在父结点中的位置Ai
				;
			if (i == 0)     //p为最左指针
				RightAdjust(p, q, i);  //结点p、父结点q、p的右兄弟结点进行旋转调整
			else
				LeftAdjust(p, q, i);   //结点p、父结点q、p的左兄弟结点进行旋转调整
			p = q;         //向上调整
		}
	}
	if (m_pRoot->num == 0) //空树
	{
		p = m_pRoot->A[0];
		delete m_pRoot;
		m_pRoot = p;
		if (m_pRoot != NULL)
			m_pRoot->parent = NULL;
	}
	return true;
}

template <typename T>
void BTree<T>::RightAdjust(BTreeNode<T> *p, BTreeNode<T> *q, int i)
{
	BTreeNode<T> *pR = q->A[i + 1];  //p的右兄弟
	if (pR->num >= (m_m + 1) / 2)       //情形3，兄弟有足够多的关键字，即至少还有[ceil(m/2)]
	{
		//调整p
		p->num++;                  //p的关键字个数加1
		p->K[p->num] = q->K[i + 1];  //父结点相应关键码下移
		p->A[p->num] = pR->A[0];   //右兄弟最左指针移到p的最右
		if (p->A[p->num] != NULL)
			p->A[p->num]->parent = p;  //修改父结点，原来是pR
		//调整父结点
		q->K[i + 1] = pR->K[1];      //右兄弟的最小关键码上移到父结点
		//调整右兄弟
		pR->A[0] = pR->A[1];       //右兄弟剩余关键字与指针前移
		LeftCompress(pR, 1);       //覆盖K[1],A[1]  
	}
	else
		MergeNode(p, q, pR, i + 1);//情形4 (...p Ki+1 pR...)
}

template <typename T>
void BTree<T>::LeftAdjust(BTreeNode<T> *p, BTreeNode<T> *q, int i)
{
	BTreeNode<T> *pL = q->A[i - 1]; //p的左兄弟
	if (pL->num >= (m_m + 1) / 2)      //情形3
	{
		//调整p
		RightCompress(p, 1);     //p的关键字和指针往右移动
		p->A[1] = p->A[0];
		p->K[1] = q->K[i];        //父结点相应关键码下移
		p->A[0] = pL->A[pL->num]; //左兄弟最右指针移到p的最左
		if (p->A[0] != NULL)
			p->A[0]->parent = p;      //修改父结点，原来是pL
		//调整父结点
		q->K[i] = pL->K[pL->num]; //左兄弟的最大关键码上移到父结点
		//调整左兄弟
		pL->num--;   //左兄弟的关键字个数减1
	}
	else
	{
		//左右互换一下，以符合合并函数的参数要求
		BTreeNode<T> *pR = p;
		p = pL;
		MergeNode(p, q, pR, i);   //情形4，注意这里i，而不是i+1 (...p Ki pR...)
	}
}
template <typename T>
void BTree<T>::LeftCompress(BTreeNode<T> *p, int i)
{
	int n = p->num;   //结点关键字个数
	for (int j = i; j < n; j++)
	{
		p->K[j] = p->K[j + 1];
		p->A[j] = p->A[j + 1];
	}
	p->num--; //关键字个数减1
}

template <typename T>
void BTree<T>::RightCompress(BTreeNode<T> *p, int i)
{
	for (int j = p->num; j >= i; j--) //K[i],A[i]空出来用以放插入的二元组
	{
		p->K[j + 1] = p->K[j];
		p->A[j + 1] = p->A[j];
	}
	p->num++; //关键字个数加1
}
// 合并两个结点

template <typename T>
void BTree<T>::MergeNode(BTreeNode<T> *p, BTreeNode<T> *q, BTreeNode<T> *pR, int i)
{
	int n = p->num + 1;   //p结点下一个放关键字的位置
	p->K[n] = q->K[i];    //下降父结点的关键字
	p->A[n] = pR->A[0];   //从右兄弟左移一个指针
	for (int j = 1; j <= pR->num; j++) //将右兄弟剩余关键字和指针移到p中
	{
		p->K[n + j] = pR->K[j];
		p->A[n + j] = pR->A[j];
	}
	if (p->A[0]) //修改p中的子女的父结点为p
	{
		for (int j = 0; j <= pR->num; j++)
			p->A[n + j]->parent = p;
	}
	LeftCompress(q, i);            //父结点的关键字个数减1
	p->num = p->num + pR->num + 1; //合并后关键字的个数
	delete pR;
	pR = NULL;
}
/////////////////////////////////////////////////////
template <typename T>
QString BTree<T>::SaveBTree()
{
	QString nodes;
	if (m_pRoot != NULL)
	{
		queue<BTreeNode<T> *> nodeQueue; //利用队列
		nodeQueue.push(m_pRoot);         //放入根结点
		while (nodeQueue.size())
		{
			BTreeNode<T> * p = nodeQueue.front();
			if (p->A[0] != NULL) //非叶结点
			{
				nodeQueue.push(p->A[0]);  //将子女结点的指针放入队列中
				for (int i = 1; i <= p->num; i++)
				{
					nodeQueue.push(p->A[i]);
					nodes.push_back(QString("%1+").arg(p->K[i]));
				}
			}
			else
			{
				for (int i = 1; i <= p->num; i++)
					nodes.push_back(QString("%1+").arg(p->K[i]));
			}

			nodes .push_back('/');//换行分割节点
			nodeQueue.pop();
		}
	}
	return nodes;
}