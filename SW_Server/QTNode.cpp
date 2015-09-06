#include "stdafx.h"
#include "QTNode.h"


namespace SW_Server
{
	QTNode::QTNode()
	{
		children[0] = NULL;
		children[1] = NULL;
		children[2] = NULL;
		children[3] = NULL;
	}


	QTNode::~QTNode()
	{
	}

	void QTNode::UpdateSurrounding(QTNode* tl, QTNode* t, QTNode* tr, QTNode* l, QTNode* r, QTNode* bl, QTNode* b, QTNode* br)
	{
		//QTNode* child_adj[8];
		QTNode* child_tl;
		QTNode* child_t;
		QTNode* child_tr;
		QTNode* child_l;
		QTNode* child_r;
		QTNode* child_bl;
		QTNode* child_b;
		QTNode* child_br;

		//update child 0
		if (children[QT_BL])
		{
			child_tl = (l) ? l->children[QT_TR] : NULL;
			child_t = children[QT_TL];
			child_tr = children[QT_TR];
			child_l = (l) ? l->children[QT_BR] : NULL;
			child_r = children[QT_BR];
			child_bl = (bl) ? bl->children[QT_TR] : NULL;
			child_b = (b) ? b->children[QT_TL] : NULL;
			child_br = (b) ? b->children[QT_TR] : NULL;
			children[QT_BL]->UpdateSurrounding(child_tl, child_t, child_tr, child_l, child_r, child_bl, child_b, child_br);
		}
		//update child 1
		if (children[QT_TL])
		{
			child_tl = (tl) ? tl->children[QT_BR] : NULL;
			child_t = (t) ? t->children[QT_BL] : NULL;
			child_tr = (t) ? t->children[QT_BR] : NULL;
			child_l = (l) ? l->children[QT_TR] : NULL;
			child_r = children[QT_TR];
			child_bl = (l) ? l->children[QT_BR] : NULL;
			child_b = children[QT_BL];
			child_br = children[QT_BR];
			children[QT_TL]->UpdateSurrounding(child_tl, child_t, child_tr, child_l, child_r, child_bl, child_b, child_br);
		}
		//update child 2
		if (children[QT_BR])
		{
			child_tl = children[QT_TL];
			child_t = children[QT_TR];
			child_tr = (r) ? r->children[QT_TL] : NULL;
			child_l = children[QT_BL];
			child_r = (r) ? r->children[QT_BL] : NULL;
			child_bl = (b) ? b->children[QT_TL] : NULL;
			child_b = (b) ? b->children[QT_TR] : NULL;
			child_br = (br) ? br->children[QT_TL] : NULL;
			children[QT_BR]->UpdateSurrounding(child_tl, child_t, child_tr, child_l, child_r, child_bl, child_b, child_br);
		}
		//update child 3
		if (children[QT_TR])
		{
			child_tl = (t) ? t->children[QT_BL] : NULL;
			child_t = (t) ? t->children[QT_BR] : NULL;
			child_tr = (tr) ? tr->children[QT_BL] : NULL;
			child_l = children[QT_TL];
			child_r = (r) ? r->children[QT_TL] : NULL;
			child_bl = children[QT_BL];
			child_b = children[QT_BR];
			child_br = (r) ? r->children[QT_BL] : NULL;
			children[QT_TR]->UpdateSurrounding(child_tl, child_t, child_tr, child_l, child_r, child_bl, child_b, child_br);
		}
	}
}
