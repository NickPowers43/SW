#pragma once

#define QT_T_BIT 1
#define QT_R_BIT 2

#define QT_BL 0
#define QT_TL 1
#define QT_BR 2
#define QT_TR 3

#define QT_ARG_TL 0
#define QT_ARG_T 1
#define QT_ARG_TR 2
#define QT_ARG_L 3
#define QT_ARG_R 4
#define QT_ARG_BL 5
#define QT_ARG_B 6
#define QT_ARG_BR 7

namespace SW
{
	class QTNode
	{
	public:
		QTNode(QTNode* parent);
		~QTNode();

		virtual void StepSurrounding(QTNode** adjacent);

		QTNode* parent;
		QTNode* children [4];
	};

}
