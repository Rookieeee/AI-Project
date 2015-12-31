#include "uctNode.h"
#include "constants.h"
#include <fstream>
#include <iostream>
#include <iomanip>

uctNode* uctNode::copy()
{
	uctNode* tmp = new uctNode(pos, color, lastMove);
	tmp->play = play;
	tmp->playResult = playResult;
	tmp->score = score;
	tmp->opened = opened;
	for (int i = 0; i < nextMove.size(); ++i)
	{
		tmp->addPos(nextMove[i]);
	}
	return tmp;
}

uctNode::uctNode(int p, int c, uctNode* last, int total)
{
	play = 0;
	playResult = 0;
	pos = p;
	color = c;
	score = 0.0;
	amafPlay = 0;
	amafPlayResult = 0;
	amafScore = 0.0;
	lastMove = last;
	opened = false;
	//total_num_moves = total;
}

void uctNode::addPos(uctNode* p)
{
	nextMove.push_back(p);
}

uctNode::~uctNode()
{
	for (int i = 0; i < nextMove.size(); ++i)
	{
		delete nextMove[i];
	}
}

//void Node::set_amaf(int result, const AmafBoard &amaf, bool side, int depth)
//{
//	const double discount = 0.0001; //0.0001;
//	for (Node *next = child; next; next = next->sibling) {
//		next->rave_results += result * amaf.value(next->move, depth, side, discount);
//		next->rave_visits += amaf.value(next->move, depth, side, discount);
//	}
//}

//playresult only +
void uctNode::result(int r, bool* blackExist, bool* whiteExist, int simul, AmafBoard* tamaf)
{
	//uctNode *p = this;
	//uctNode * tmp;
	//int color = p->color;

	//int depth = simul;
	//int side;
	//if (color == BLACK)
	//	side = 1;
	//else
	//	side = 0;
	//while (p)
	//{
	//	tmp = p;
	//	++(p->play);
	//	if (p->color == color)
	//		p->playResult += r;
	//	else p->playResult += (1 - r);
	//	p = p->lastMove;
	//	
	//	//new
	//	if (!p)
	//		break;
	//	if (p->color == 1) //white
	//	{
	//		//child is black
	//		for (int i = 0; i < p->nextMove.size(); ++i)
	//		{
	//			if (blackExist[p->nextMove[i]->pos] && p->nextMove[i] != tmp)
	//			{
	//				//++(p->nextMove[i]->amafPlay);
	//				p->nextMove[i]->amafPlay += tamaf->value(p->nextMove[i]->pos, depth, 1, 0.0001);
	//				if(p->nextMove[i]->color == color)
	//					//p->nextMove[i]->amafPlayResult += r;
	//					p->nextMove[i]->amafPlayResult += r * tamaf->value(p->nextMove[i]->pos, depth, 1, 0.0001);
	//				else
	//					p->nextMove[i]->amafPlayResult += (1 - r) * tamaf->value(p->nextMove[i]->pos, depth, 1, 0.0001);
	//			}
	//		}
	//	}
	//	else //black
	//	{
	//		//child is white
	//		for (int i = 0; i < p->nextMove.size(); ++i)
	//		{
	//			if (whiteExist[p->nextMove[i]->pos] && p->nextMove[i] != tmp)
	//			{
	//				//++(p->nextMove[i]->amafPlay);
	//				p->nextMove[i]->amafPlay += tamaf->value(p->nextMove[i]->pos, depth, 0, 0.0001);
	//				if (p->nextMove[i]->color == color)
	//					//p->nextMove[i]->amafPlayResult += r;
	//					p->nextMove[i]->amafPlayResult += r * tamaf->value(p->nextMove[i]->pos, depth, 0, 0.0001);
	//				else 
	//					p->nextMove[i]->amafPlayResult += (1 - r) * tamaf->value(p->nextMove[i]->pos, depth, 0, 0.0001);
	//			}
	//		}
	//	}
	//	--depth;

	//}
}

void uctNode::set_results(int result)
{
	play++;
	playResult += result;
}

void uctNode::set_amaf(int result, const AmafBoard &amaf, bool side, int depth)
{
	const double discount = 0.0001; //0.0001;
	for (int i = 0; i < nextMove.size(); ++i) 
	{
		uctNode *next = nextMove[i];
		if (next->color == BLACK && side == 0)
			std::cerr << "error";
		if (next->color == WHITE && side == 1)
			std::cerr << "error";
		next->amafPlayResult += result * amaf.value(next->pos, depth, side, discount);
		//std::cout << "??" << amaf.value(next->pos, depth, side, discount) << std::endl;
		next->amafPlay += amaf.value(next->pos, depth, side, discount);
		//std::cout << next->amafPlay << "amafplay ";
	}
}

void uctNode::show_node(uctNode *node, int threshold,int depth)
{
	if (depth > 4)
		return;
	for (int i = 0; i < depth; ++i) std::cerr << " ";
	std::cerr << "|->";
	std::cerr.precision(4);
	std::cerr << std::right << std::setw(4) << node->pos <<
		": " << std::right << std::setw(6) << node->playResult*1.0 / node->play
		<< "/" << std::left << std::setw(5) << node->play
		<< "[ RAVE: " << std::right << std::setw(6) << node->amafPlayResult*1.0 / node->amafPlay <<
		"/" << std::left << std::setw(5) << node->amafPlay << "]\n";
	
	uctNode *children[MAX_BOARD2] = { 0 };
	int len = 0;

	for (int i = 0; i < node->nextMove.size(); ++i)
	{
		if (node->nextMove[i]->play > threshold)
			children[len++] = node->nextMove[i];
	}
	while (1)
	{
		int best = -1, visits = 0;
		for (int i = 0; i < len; ++i)
		{
			if (children[i] && children[i]->play>visits)
			{
				best = i;
				visits = children[i]->play;
			}
		}
		if (best < 0)break;
		if (children[best]->pos>0)
			show_node(children[best], threshold, depth + 1);
		children[best] = 0;
	}
}
void uctNode::show_node()
{
	int threshold = play / 300;
	show_node(this, threshold, 0);
}

//playResult can be -/+
//void uctNode::result(int r, bool* blackExist, bool* whiteExist)
//{
//	uctNode *p = this;
//	uctNode * tmp;
//	while (p)
//	{
//		tmp = p;
//		++(p->play);
//		p->playResult += r;
//		p = p->lastMove;
//		//new
//		if (!p)
//			break;
//		if (p->color == 1) //white
//		{
//			//child is black
//			for (int i = 0; i < p->nextMove.size(); ++i)
//			{
//				if (blackExist[p->nextMove[i]->pos] && p->nextMove[i] != tmp)
//				{
//					++(p->nextMove[i]->amafPlay);
//					p->nextMove[i]->amafPlayResult += r;
//				}
//			}
//		}
//		else //black
//		{
//			//child is white
//			for (int i = 0; i < p->nextMove.size(); ++i)
//			{
//				if (whiteExist[p->nextMove[i]->pos] && p->nextMove[i] != tmp)
//				{
//					++(p->nextMove[i]->amafPlay);
//					p->nextMove[i]->amafPlayResult += r;
//				}
//			}
//		}
//
//	}
//}