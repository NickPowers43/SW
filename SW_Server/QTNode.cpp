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

	void QTNode::UpdateSurrounding(QTNode** adjacent)
	{
		QTNode* child_adj[8] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};

		//update child 0
		if (children[QT_BL])
		{
			child_adj[QT_ARG_TL] = (adjacent[QT_ARG_L]) ? adjacent[QT_ARG_L]->children[QT_TR] : NULL;
			child_adj[QT_ARG_T] = children[QT_TL];
			child_adj[QT_ARG_TR] = children[QT_TR];
			child_adj[QT_ARG_L] = (adjacent[QT_ARG_L]) ? adjacent[QT_ARG_L]->children[QT_BR] : NULL;
			child_adj[QT_ARG_R] = children[QT_BR];
			child_adj[QT_ARG_BL] = (adjacent[QT_ARG_BL]) ? adjacent[QT_ARG_BL]->children[QT_TR] : NULL;
			child_adj[QT_ARG_B] = (adjacent[QT_ARG_B]) ? adjacent[QT_ARG_B]->children[QT_TL] : NULL;
			child_adj[QT_ARG_BR] = (adjacent[QT_ARG_B]) ? adjacent[QT_ARG_B]->children[QT_TR] : NULL;
			children[QT_BL]->UpdateSurrounding((QTNode**)&child_adj);
		}
		//update child 1
		if (children[QT_TL])
		{
			child_adj[QT_ARG_TL] = (adjacent[QT_ARG_TL]) ? adjacent[QT_ARG_TL]->children[QT_BR] : NULL;
			child_adj[QT_ARG_T] = (adjacent[QT_ARG_T]) ? adjacent[QT_ARG_T]->children[QT_BL] : NULL;
			child_adj[QT_ARG_TR] = (adjacent[QT_ARG_T]) ? adjacent[QT_ARG_T]->children[QT_BR] : NULL;
			child_adj[QT_ARG_L] = (adjacent[QT_ARG_L]) ? adjacent[QT_ARG_L]->children[QT_TR] : NULL;
			child_adj[QT_ARG_R] = children[QT_TR];
			child_adj[QT_ARG_BL] = (adjacent[QT_ARG_L]) ? adjacent[QT_ARG_L]->children[QT_BR] : NULL;
			child_adj[QT_ARG_B] = children[QT_BL];
			child_adj[QT_ARG_BR] = children[QT_BR];
			children[QT_TL]->UpdateSurrounding((QTNode**)&child_adj);
		}
		//update child 2
		if (children[QT_BR])
		{
			child_adj[QT_ARG_TL] = children[QT_TL];
			child_adj[QT_ARG_T] = children[QT_TR];
			child_adj[QT_ARG_TR] = (adjacent[QT_ARG_R]) ? adjacent[QT_ARG_R]->children[QT_TL] : NULL;
			child_adj[QT_ARG_L] = children[QT_BL];
			child_adj[QT_ARG_R] = (adjacent[QT_ARG_R]) ? adjacent[QT_ARG_R]->children[QT_BL] : NULL;
			child_adj[QT_ARG_BL] = (adjacent[QT_ARG_B]) ? adjacent[QT_ARG_B]->children[QT_TL] : NULL;
			child_adj[QT_ARG_B] = (adjacent[QT_ARG_B]) ? adjacent[QT_ARG_B]->children[QT_TR] : NULL;
			child_adj[QT_ARG_BR] = (adjacent[QT_ARG_BR]) ? adjacent[QT_ARG_BR]->children[QT_TL] : NULL;
			children[QT_BR]->UpdateSurrounding((QTNode**)&child_adj);
		}
		//update child 3
		if (children[QT_TR])
		{
			child_adj[QT_ARG_TL] = (adjacent[QT_ARG_T]) ? adjacent[QT_ARG_T]->children[QT_BL] : NULL;
			child_adj[QT_ARG_T] = (adjacent[QT_ARG_T]) ? adjacent[QT_ARG_T]->children[QT_BR] : NULL;
			child_adj[QT_ARG_TR] = (adjacent[QT_ARG_TR]) ? adjacent[QT_ARG_TR]->children[QT_BL] : NULL;
			child_adj[QT_ARG_L] = children[QT_TL];
			child_adj[QT_ARG_R] = (adjacent[QT_ARG_R]) ? adjacent[QT_ARG_R]->children[QT_TL] : NULL;
			child_adj[QT_ARG_BL] = children[QT_BL];
			child_adj[QT_ARG_B] = children[QT_BR];
			child_adj[QT_ARG_BR] = (adjacent[QT_ARG_R]) ? adjacent[QT_ARG_R]->children[QT_BL] : NULL;
			children[QT_TR]->UpdateSurrounding((QTNode**)&child_adj);
		}
	}
}
