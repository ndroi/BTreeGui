#include "qtbtree.h"
#include<qmessagebox.h>
#pragma execution_character_set("utf-8")
QtBtree::QtBtree(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	connect(ui.action_NewBtree, SIGNAL(triggered()), this, SLOT(NewBtree()));
	connect(ui.action_InsertDel, SIGNAL(triggered()), this, SLOT(InsDel()));
}
void QtBtree::NewBtree()
{
	nbdlg = new NewBtreeDLG(this);
	if (!nbdlg->exec()== QDialog::Accepted) return;
	if (bt != NULL) delete bt;
	bt = new BTree<int>(nbdlg->getM(),NULL);
	vector<int> keys = nbdlg->getKeys();
	int num = nbdlg->getNum();
	for (int i = 0; i < num;i++)
	{
		bt->Insert(keys[i]);
	}
	if (da != NULL) delete da;
	da = new DrawAll;
	da->Create(bt);
    ok = true;
	update();
	ui.action_InsertDel->setEnabled(true);
}
void QtBtree::InsDel()
{
	InsAndDelDLG dlg;
    if (dlg.exec() != QDialog::Accepted) return;
    if (dlg.ui.radioButton_insert->isChecked())
	{
        int key = dlg.ui.lineEdit_keys->text().toInt();
        if (bt->Search(key).tag == true)//already exists
		{
            QMessageBox::warning(this, "warning", "the key already exists");
			return;
		}
		bt->Insert(key);
		if (da != NULL) delete da;
		da = new DrawAll;
		da->Create(bt);
        ok = true;
		update();
		return;
	}
    if (dlg.ui.radioButton_delete->isChecked())
	{
        int key = dlg.ui.lineEdit_keys->text().toInt();
        if (bt->Search(key).tag == false)//not exists
		{
            QMessageBox::warning(this, "warning", "the key does not exist");
			return;
		}
		bt->Delete(key);
		if (da != NULL) delete da;
		da = new DrawAll;
		da->Create(bt);
        ok = true;
		update();
		return;
	}
    if (dlg.ui.radioButton_multInsert->isChecked())
	{
        QStringList keys=dlg.ui.lineEdit_keys->text().split(',');
		for (int i = 0; i < keys.length(); i++)
		{
			int key =keys[i].toInt();
            if (bt->Search(key).tag == true)//already exists
			{
                QMessageBox::warning(this, "warning", "some keys already exist");
				return;
			}
		}
		for (int i = 0; i < keys.length(); i++)
		{
			bt->Insert(keys[i].toInt());
		}
		if (da != NULL) delete da;
		da = new DrawAll;
		da->Create(bt);
        ok = true;
		update();
		return;
	}
    if (dlg.ui.radioButton_multDelete->isChecked())
	{
        QStringList keys = dlg.ui.lineEdit_keys->text().split(',');
		for (int i = 0; i < keys.length(); i++)
		{
			int key = keys[i].toInt();
            if (bt->Search(key).tag == false)//not exists
			{
                QMessageBox::warning(this,"warning", "some keys do not exist");
				return;
			}
		}
		for (int i = 0; i < keys.length(); i++)
		{
			bt->Delete(keys[i].toInt());
		}
		if (da != NULL) delete da;
		da = new DrawAll;
		da->Create(bt);
        ok = true;
		update();
		return;
	}
}
void QtBtree::paintEvent(QPaintEvent* event)
{
	if (!ok) return;
	da->p = new QPainter(this);
	da->pt = QPoint(width()/2-50, 100);
	da->draw();
}
QtBtree::~QtBtree()
{
	if (bt!=NULL)
	delete bt;
	if (da != NULL)
	delete da;
}
/////////////////////////////////////////////////////////////////
InsAndDelDLG::InsAndDelDLG(QWidget *parent)
:QDialog(parent)
{
	ui.setupUi(this);
}
InsAndDelDLG::~InsAndDelDLG()
{

}
NewBtreeDLG::NewBtreeDLG(QWidget *parent) 
:QDialog(parent)
{
	ui.setupUi(this);
	ui.m_lineEdit->setValidator(new QRegExpValidator(QRegExp("[3-6]")));
	ui.lineEdit_num->setValidator(new QRegExpValidator(QRegExp("[0-9]{1,2}")));
}
int NewBtreeDLG::getM()
{

	if (ui.m_lineEdit->text().isEmpty())
		return 0;
	return ui.m_lineEdit->text().toInt();
}
int NewBtreeDLG::getNum()
{
	return ui.lineEdit_num->text().toInt();

}
vector<int> NewBtreeDLG::getKeys()
{
	QStringList sl=ui.TextEdit_v->toPlainText().split(',');
	vector<int> keys;
	for (int i = 0; i < sl.length(); i++)
		keys.push_back(sl[i].toInt());
	return keys;	

}
NewBtreeDLG::~NewBtreeDLG()
{

}
//////////////////////////--------------------------------------单节点
DrawNode::DrawNode()
{
}
int DrawNode::getKeysNum()
{
	return keys.size();
}
void DrawNode::draw()
{
	if (parent.x() != 0 && parent.y() != 0)
	p->drawLine(parent, QPoint(pt.x()+w*keys.size()/2, pt.y()));
	for (int i = 0; i < keys.size(); i++)
	{
		p->drawRect(pt.x() + i*w, pt.y(), w, h);
		p->drawText(pt.x() + i*w+8, pt.y()+15, QString("%1").arg(keys[i]));
	}	
}
QPoint DrawNode::getPoint(int i)
{
	return QPoint(pt.x() + i*w, pt.y() + h);
}
/////////////////////////////-------------------------------------排
DrawRow::DrawRow()
{
	this->pt = pt;
}
void DrawRow::draw()
{
	int xx = this->pt.x();//用来赋值各节点坐标
	for (int i = 0; i < rowNodes.size(); i++)
	{
		if (parentRow!=NULL)
		rowNodes[i].parent=parentRow->getParentPoint(i);
		else
		{
			rowNodes[i].parent = QPoint(0, 0);//标记第一个节点
		}
		rowNodes[i].p = p;
		rowNodes[i].pt.setX(xx);
		xx += rowNodes[i].getKeysNum()*DrawNode::w + d;
		rowNodes[i].pt.setY( this->pt.y());
		rowNodes[i].draw();
	}
}
void DrawRow::getNode(DrawNode dn)
{
	rowNodes.push_back(dn);
}
////////////////////////////////----------------------------------全部
DrawAll::DrawAll()
{
}
void DrawAll::getRow(DrawRow dr)
{
	rows.push_back(dr);
}
void DrawAll::draw()
{
	for (int i = 0; i < rows.size(); i++)
	{
		if (i>0) rows[i].parentRow =& rows[i - 1];
		else     rows[0].parentRow = NULL;
		int Len = rows[i].GetLen();
		rows[i].p = p;
		rows[i].pt.setX(this->pt.x()+rows[0].GetLen()/2-Len/2);//根据实际长度调整x
		rows[i].pt.setY(this->pt.y() + (DrawNode::h+d)*i);
		rows[i].draw();
	}
}
void DrawAll::Create(BTree<int>* bt)
{
	QString allNodes = bt->SaveBTree();
	QStringList oneNode = allNodes.split('/', QString::SkipEmptyParts);
	for (int i = 0; i < oneNode.size(); i++)
	{
		if (isNeedNewRow())//需要开辟新的节点排
		{
			DrawNode dn;//临时节点
			DrawRow dr;//临时排
			QStringList oneKey = oneNode[i].split('+', QString::SkipEmptyParts);
			for (int j = 0; j < oneKey.size(); j++)
			{
				dn.keys.push_back(oneKey[j].toInt());//在节点中放入key值
			}
			dr.getNode(dn);//放入排
			rows.push_back(dr);//将排放入DrawALL对象
		}
		else//不需要新的排，直接放到最后排的位置
		{
			DrawNode dn;//临时节点
			QStringList oneKey = oneNode[i].split('+', QString::SkipEmptyParts);
			for (int j = 0; j < oneKey.size(); j++)
			{
				dn.keys.push_back(oneKey[j].toInt());//在节点中放入key值
			}
			int r = rows.size() - 1;//排数
			rows[r].rowNodes.push_back(dn);
		}

	}
}
/////////////////////////////////////////////////////////
bool DrawAll::isNeedNewRow()
{
	if (rows.size() == 0 || rows.size() == 1) return true;
	int LastChildSum=0;//统计上一排的所允许的孩子数目
	int LastRowNodeNum = rows[rows.size() - 2].rowNodes.size();//上一行的节点数目
	for (int i = 0; i < LastRowNodeNum; i++)
	{
		LastChildSum +=( rows[rows.size() - 2].rowNodes[i].getKeysNum() + 1);
	}
	if (rows[rows.size() - 1].rowNodes.size() == LastChildSum)//当前排的节点数以填满，。
		return true;
	return false;
}

// 获取DrawRow在画板上的实际长度
int DrawRow::GetLen()
{
	int Len = 0;
	for (int i = 0; i < rowNodes.size(); i++)
	{
		int n = rowNodes[i].getKeysNum();
		Len += n*DrawNode::w;
	}
	Len += (rowNodes.size() - 1)*d;
	return Len;
}


// 得到第index个子节点在该排的坐标
QPoint DrawRow::getParentPoint(int index)
{
	int child = 0;
	for (int i = 0; i < rowNodes.size(); i++)
	{
		for (int j = 0; j < rowNodes[i].getKeysNum(); j++)
		{
			
			if (child == index)
			{
				QPoint pt = rowNodes[i].pt;
				pt.setX(pt.x() + DrawNode::w*j);
				pt.setY(pt.y() + DrawNode::h);
				return pt;
			}
			child++;
		}
		
		if (child == index)
		{
			QPoint pt = rowNodes[i].pt;
			pt.setX(pt.x() + DrawNode::w* rowNodes[i].getKeysNum());
			pt.setY(pt.y() + DrawNode::h);
			return pt;
		}
		child++;
	}
	return QPoint(0, 0);

}


void NewBtreeDLG::accept()
{
	QRegExp checkReg(QString("^(\\d{1,2},){%1}\\d{1,2}$").arg(getNum()-1));
	QString keys = ui.TextEdit_v->toPlainText();
	if (getM()==0||checkReg.indexIn(keys) == -1)
	{
		QMessageBox::warning(this, "警告", "输入非法");
		return;
	}
	QDialog::accept();
}

