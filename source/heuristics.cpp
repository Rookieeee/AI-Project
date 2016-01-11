#include "GoBoard.h"
#include <random>


int GoBoard::capture_move(int bi, int bj, int color)
{
	if (!on_board(bi, bj))
		return -1;
	if (get_board(bi, bj) != OTHER_COLOR(color))
		return -1;
	if (board[POS(bi, bj)]->get_liberties_number() != 1)
		return -1;
	int move = board[POS(bi,bj)]->liberties[0];

	return move;
}
int GoBoard::capture_heuristic(int color)// sometimes check  the same string
{
	int capture_moves[18];
	int captures_moves_number = 0;
	for (int i = 0; i < 8; ++i)
	{
		int bi = I(last_point) + around_i[i];
		int bj = J(last_point) + around_j[i];
		int move = capture_move(bi, bj, color);
		if (move != -1)
			capture_moves[captures_moves_number++] = move;
	}
	for (int i = 0; i < 8; ++i)
	{
		int bi = I(last_point2) + around_i[i];
		int bj = J(last_point2) + around_j[i];
		int move = capture_move(bi, bj, color);
		if (move != -1)
			capture_moves[captures_moves_number++] = move;
	}
	int bi = I(last_point);
	int bj = J(last_point);
	int move = capture_move(bi, bj, color);
	if (move != -1)
		capture_moves[captures_moves_number++] = move;
	
	move = random_choose_move(capture_moves, captures_moves_number, color);
	if(move<0)
		return -1;
	else return move;
}
int GoBoard::find_escape_point(String *string, int escape_points[],int index)
{
	if (!string)
		return 0;
	for (int i = 0; i < string->get_liberties_number(); ++i)
	{
		if (gains_liberty(string->liberties[i], string))
			index += add_point(escape_points, index, string->liberties[i]);
	}
	String * neighbours[MAX_BOARD2];
	int nneigh = neighbour_strings(string, OTHER_COLOR(string->get_color()), string->get_liberties_number(), neighbours);
	for (int i = 0; i < nneigh; ++i)
	{
		for (int j = 0; j < neighbours[i]->get_liberties_number(); ++j)
		{
			if (gains_liberty(neighbours[i]->liberties[j], string))
				index += add_point(escape_points, index, neighbours[i]->liberties[j]);
		}
	}
	return index;
}


int GoBoard::neighbour_strings( String *string, int color, int max_liberties,String * neighbours[]) 
{
	int nlibs = 0;
	if (!string)
		return 0;
	for (int i = 0; i < string->get_stones_number(); ++i)
	{
		nlibs = neighbour_strings(string->stones[i], color, max_liberties, neighbours, nlibs);
	}
	return nlibs;
}

int GoBoard::neighbour_strings(int point, int color, int max_liberties, String *  neighbours[],int index)
{
	for (int i = 0; i < 4; ++i)
	{
		int ai = I(point) + deltai[i];
		int aj = J(point) + deltaj[i];
		if (!on_board(ai, aj))
			continue;
		String * current_string = board[POS(ai, aj)];
		if (current_string &&current_string->get_color() == color&&current_string->get_liberties_number() <= max_liberties)
			index += add_string(neighbours, index, current_string);
	}
	return index;
}



int GoBoard::save_heuristic(int color)
{
	if (last_point < 0)
		return -1;
	int save_moves[MAX_BOARD2];
	int save_moves_number = 0;
	if (board[last_point] && board[last_point]->get_liberties_number() == 2)
	{
		for (int i = 0; i < 2; ++i)
		{
			int lib = board[last_point]->get_liberty(i);
			save_moves[save_moves_number++] = lib;
			//if ( gains_liberty(lib, board[last_point]))
			//{
			//	save_moves[save_moves_number++] = lib;
			//}
		}
	}
	String * neighbours[4];
	int neighbour_number = 0;
	int ai, aj;
	for (int k = 0; k < 4; ++k)
	{
		ai = I(last_point) + deltai[k];
		aj = J(last_point) + deltaj[k];
		if (!on_board(ai, aj))
			continue;
		if (!board[POS(ai, aj)])
			continue;
		neighbour_number += add_string(neighbours, neighbour_number, board[POS(ai, aj)]);
	}
	for (int k = 0; k < neighbour_number; ++k)
	{
		if (neighbours[k]->get_color() == color &&neighbours[k]->get_liberties_number() == 2)
		{
			save_moves_number = find_escape_point(neighbours[k], save_moves, save_moves_number);
		}
	}

	int move = random_choose_move(save_moves, save_moves_number, color);
	if (move >= 0)
		return move;
	else return -1;
	//for (int k = 0; k < 4; ++k)
	//{
	//	ai = I(last_point) + deltai[k];
	//	aj = J(last_point) + deltaj[k];
	//	pos = POS(ai, aj);
	//	if (on_board(ai, aj) && get_board(ai, aj) == color && board[pos]->get_liberties_number() <= 2)
	//	{
	//		for (int i = 0; i < board[pos]->get_liberties_number(); ++i)
	//		{
	//			int lib = board[pos]->get_liberty(i);
	//			if (available(I(lib), J(lib), color) && gains_liberty(lib, board[pos]))
	//			{
	//				save_moves[save_moves_number++] = lib;
	//			}
	//		}
	//	}
	//}
	//if (save_moves_number)
	//{
	//	return save_moves[rand()*save_moves_number / (RAND_MAX + 1)];
	//}
	//return -1;
}




int GoBoard::last_atari_heuristic(int color)
{
	int last_atari_point;
	last_atari_point = last_atari[OTHER_COLOR(color)];
	if (last_atari_point>=0&& board[last_atari_point] && board[last_atari_point]->get_color() == OTHER_COLOR(color) && board[last_atari_point]->get_liberties_number() == 1)
	{
		int move = board[last_atari_point]->liberties[0];
		if (available(I(move), J(move), color) && !is_self_atari(move, color) && gains_liberty(move, board[last_atari_point])) //
		{
			return move;
		}

	}
	else
	{
		last_atari[OTHER_COLOR(color)] = -1;
	}
	last_atari_point = last_atari[color];

	if (last_atari_point>=0 &&  board[last_atari_point] &&board[last_atari_point]->get_color() == color && board[last_atari_point]->get_liberties_number() == 1  )
	{
		int points[MAXSIZE2];

		int number_points = find_escape_point(board[last_atari_point], points,0);
		int move = random_choose_move(points, number_points, color);
		if (move >= 0)
		{
			return move;
		}
	}
	else
	{
		last_atari[color] = -1;
	}
	return -1;
	//int last_atari_moves[4];
	//int last_atari_moves_number = 0;

	//if (on_board(I(last_point), J(last_point)) && board[last_point] && board[last_point]->get_liberties_number() == 1)
	//{
	//	int lib = board[last_point]->get_liberty(0);
	//	if (available(I(lib), J(lib), color) && gains_liberty(lib, board[last_point]))
	//		last_atari_moves[last_atari_moves_number++] = lib;
	//}
	//if (on_board(I(last_point2), J(last_point2)) && board[last_point2] && board[last_point2]->get_liberties_number() == 1)
	//{
	//	int lib = board[last_point2]->get_liberty(0);
	//	if (available(I(lib), J(lib), color) && gains_liberty(lib, board[last_point2]))
	//		last_atari_moves[last_atari_moves_number++] = lib;
	//}
	//if (last_atari_moves_number)
	//{
	//	return last_atari_moves[rand()*last_atari_moves_number / (RAND_MAX + 1)];
	//}
	//return -1;
}

//bool GoBoard::match_mid(int i, int j, int my_color)
//{
//	int pos = POS(i, j);
//	for (int start = 0; start < 8; start += 2)
//	{
//		String * around_points[8];
//		for (int m = 0; m < 8; ++m)
//		{
//			around_points[m] =board[ POS(i + around_i[(start + m) % 8], j + around_j[(start + m) % 8])];
//		}
//		if (!around_points[1])
//			continue;
//		int this_color =around_points[1]->get_color();
//		if (around_points[0] && around_points[0]->get_color() == OTHER_COLOR(this_color))
//		{
//			if (!around_points[3] && !around_points[7])
//			{
//				if (around_points[2] && around_points[2]->get_color() == OTHER_COLOR(this_color))
//					return true;
//				if (!around_points[2] && !around_points[5])
//					return true;
//			}
//			if (!around_points[3] && !around_points[5] && around_points[7]
//				&& around_points[7]->get_color() == OTHER_COLOR(this_color))
//				return true;
//			if (around_points[7] && around_points[7]->get_color() == this_color)
//			{
//				if (!(!around_points[5] && around_points[5] && around_points[5]->get_color() == this_color)
//					&& !(!around_points[3] && around_points[5] && around_points[5]->get_color() == this_color))
//					return true;
//			}
//
//
//		}
//	}
//
//}

bool GoBoard::match_hane(int i, int j, int my_color)
{
	int pos = POS(i, j);
	for (int start = 0; start < 8; start += 2)
	{

		int around_points[8];
		for (int m = 0; m < 8; ++m)
		{
			around_points[m] = POS(i + around_i[(start + m) % 8], j + around_j[(start + m) % 8]);
		}
		if (!board[around_points[0]])
			continue;
		int color = board[around_points[0]]->get_color();
		if (!board[around_points[1]])
			continue;
		if (board[around_points[1]]->get_color() != OTHER_COLOR(color))
			continue;
		// cut1 
		if (get_board(I(around_points[7]), J(around_points[7])) == OTHER_COLOR(color))
		{
			if (!(get_board(I(around_points[5]), J(around_points[5])) == EMPTY
				&& get_board(I(around_points[3]), J(around_points[3])) == OTHER_COLOR(color)
				)
				&& !(
					get_board(I(around_points[3]), J(around_points[3])) == EMPTY
					&& get_board(I(around_points[5]), J(around_points[5])) == OTHER_COLOR(color)
					)
				)
			{
				return true;
			}
		}
		if (board[around_points[7]])
		{
			if (board[around_points[7]]->get_color() == color &&!board[around_points[3]] && !board[around_points[5]])
			{
				return true;
			}
			continue;
		}
		if (get_board(I(around_points[2]), J(around_points[2])) == color && get_board(I(around_points[3]), J(around_points[3])) == EMPTY)
		{
			return true;
		}
		if (!board[around_points[2]] && !board[around_points[3]] && !board[around_points[5]])
		{
			return true;
		}
		if (!board[around_points[3]] && !board[around_points[5]] && color == my_color  &&get_board(I(around_points[2]), J(around_points[2])) == OTHER_COLOR(color))
		{
			return true;
		}
	}
	for (int start = 0; start < 8; start += 2)
	{

		int around_points[8];
		for (int m = 0; m < 8; ++m)
		{
			around_points[m] = POS(i + around_i[(start - m+8) % 8], j + around_j[(start - m+8) % 8]);
		}
		if (!board[around_points[0]])
			continue;
		int color = board[around_points[0]]->get_color();
		if (!board[around_points[1]])
			continue;
		if (board[around_points[1]]->get_color() != OTHER_COLOR(color))
			continue;
		if (board[around_points[7]])
		{
			if (board[around_points[7]]->get_color() == color &&!board[around_points[3]] && !board[around_points[5]])
			{
				return true;
			}
			continue;
		}
		if (!board[around_points[2]] && !board[around_points[3]] && !board[around_points[5]])
		{
			return true;
		}
		if (!board[around_points[3]] && !board[around_points[5]] && color == my_color  &&get_board(I(around_points[2]), J(around_points[2])) == OTHER_COLOR(color))
		{
			return true;
		}
	}


	return false; //?
}
//bool GoBoard::match_cut1(int i, int j, int color) //symmetric
//{
//	for (int start = 0; start < 8; start += 2) //clock wise
//	{
//		bool color_same = false;
//		if (!board[  POS(i+around_i[start],j+around_j[start] ) ])
//			continue;
//		if (board[POS(i + around_i[start], j + around_j[start])]->get_color() == BLACK)
//			color_same = true;
//		int bis[8];
//		int bjs[8];
//		for (int m = 0; m < 8; ++m) {
//			bis[m] = i + around_i[(start + m) % 8];
//			bjs[m] = j + around_j[(start + m) % 8];
//		}
//		if (color_same) {
//			if (get_board(bis[3], bjs[3]) == WHITE &&
//				get_board(bis[1], bjs[5]) == EMPTY)
//				continue;
//			if (get_board(bis[3], bjs[3]) == EMPTY &&
//				get_board(bis[1], bjs[5]) == WHITE)
//				continue;
//
//			if (get_board(bis[0], bjs[0]) == BLACK &&
//				get_board(bis[1], bjs[1]) == WHITE &&
//				get_board(bis[7], bjs[7]) == WHITE)
//				return true;
//		}
//		else
//		{
//			if (get_board(bis[3], bjs[3]) == BLACK &&
//				get_board(bis[1], bjs[5]) == EMPTY)
//				continue;
//			if (get_board(bis[3], bjs[3]) == EMPTY &&
//				get_board(bis[1], bjs[5]) == BLACK)
//				continue;
//
//			if (get_board(bis[0], bjs[0]) == WHITE &&
//				get_board(bis[1], bjs[1]) == BLACK &&
//				get_board(bis[7], bjs[7]) == BLACK)
//				return true;
//		}
//	}
//	return false;
//
//}
bool GoBoard::match_cut2(int i, int j, int color) //symmetric
{
	for (int start = 0; start < 8; start += 2) //clock wise
	{
		int around_points[8];
		for (int m = 0; m < 8; ++m)
		{
			around_points[m] = POS(i + around_i[(start + m) % 8], j + around_j[(start + m) % 8]);
		}
		if (!board[around_points[1]])
			continue;
		int color = board[around_points[1]]->get_color();
		if (get_board(I(around_points[7]), J(around_points[7])) != OTHER_COLOR(color))
			continue;
		if (get_board(I(around_points[3]), J(around_points[3])) != OTHER_COLOR(color))
			continue;
		if (get_board(I(around_points[4]), J(around_points[4])) == OTHER_COLOR(color))
			continue;
		if (get_board(I(around_points[5]), J(around_points[5])) == OTHER_COLOR(color))
			continue;
		if (get_board(I(around_points[6]), J(around_points[6])) == OTHER_COLOR(color))
			continue;
		{
			return true;
		}
	}
	return false;
}
bool GoBoard::match_board_side(int i, int j, int color)
{
	int start = 0;
	if (i == 0)
		start = 2;
	if (i == board_size - 1)
		start = 6;
	if (j == 0)
		start = 4;
	if (j == board_size - 1)
		start = 0;
	int bis[8];
	int bjs[8];
	for (int m = 0; m < 8; ++m) {
		bis[m] = i + around_i[(start + m) % 8];
		bjs[m] = j + around_j[(start + m) % 8];
	}
	//8.1
	if (get_board(bis[0], bjs[0]) == BLACK&&
		get_board(bis[7], bjs[7]) == WHITE&&
		get_board(bis[1], bjs[1]) == EMPTY
		)
	{
		return true;
	}
	if (get_board(bis[0], bjs[0]) == WHITE&&
		get_board(bis[7], bjs[7]) == BLACK&&
		get_board(bis[1], bjs[1]) == EMPTY
		)
	{
		return true;
	}
	if (get_board(bis[2], bjs[2]) == BLACK&&
		get_board(bis[3], bjs[3]) == WHITE&&
		get_board(bis[1], bjs[1]) == EMPTY
		)
	{
		return true;
	}
	if (get_board(bis[2], bjs[2]) == WHITE&&
		get_board(bis[3], bjs[3]) == BLACK&&
		get_board(bis[1], bjs[1]) == EMPTY
		)
	{
		return true;
	}
	//8.2
	if (get_board(bis[7], bjs[7]) != BLACK&&
		get_board(bis[1], bjs[1]) == BLACK&&
		get_board(bis[3], bjs[3]) == WHITE
		)
	{
		return true;
	}
	if (get_board(bis[7], bjs[7]) != WHITE&&
		get_board(bis[1], bjs[1]) == WHITE&&
		get_board(bis[3], bjs[3]) == BLACK
		)
	{
		return true;
	}
	if (get_board(bis[3], bjs[3]) != BLACK&&
		get_board(bis[1], bjs[1]) == BLACK&&
		get_board(bis[7], bjs[7]) == WHITE
		)
	{
		return true;
	}
	if (get_board(bis[3], bjs[3]) != WHITE&&
		get_board(bis[1], bjs[1]) == WHITE&&
		get_board(bis[7], bjs[7]) == BLACK
		)
	{
		return true;
	}
	//8.3
	if (get_board(bis[1], bjs[1]) == BLACK&&
		get_board(bis[2], bjs[2]) == WHITE&&
		color == BLACK
		)
	{
		return true;
	}
	if (get_board(bis[1], bjs[1]) == WHITE&&
		get_board(bis[2], bjs[2]) == BLACK&&
		color == WHITE
		)
	{
		return true;
	}
	if (get_board(bis[1], bjs[1]) == WHITE&&
		get_board(bis[0], bjs[0]) == BLACK&&
		color == WHITE
		)
	{
		return true;
	}
	if (get_board(bis[1], bjs[1]) == BLACK&&
		get_board(bis[0], bjs[0]) == WHITE&&
		color == BLACK
		)
	{
		return true;
	}
	//8.4
	if (get_board(bis[1], bjs[1]) == BLACK&&
		get_board(bis[2], bjs[2]) == WHITE&&
		get_board(bis[3], bjs[3]) != BLACK&&
		color == WHITE
		)
	{
		return true;
	}
	if (get_board(bis[1], bjs[1]) == WHITE&&
		get_board(bis[2], bjs[2]) == BLACK&&
		get_board(bis[3], bjs[3]) != WHITE&&
		color == BLACK
		)
	{
		return true;
	}
	if (get_board(bis[1], bjs[1]) == WHITE&&
		get_board(bis[0], bjs[0]) == BLACK&&
		get_board(bis[7], bjs[7]) != WHITE&&
		color == BLACK
		)
	{
		return true;
	}
	if (get_board(bis[1], bjs[1]) == BLACK&&
		get_board(bis[0], bjs[0]) == WHITE&&
		get_board(bis[7], bjs[7]) != BLACK&&
		color == WHITE
		)
	{
		return true;
	}
	//8.5
	if (get_board(bis[1], bjs[1]) == BLACK&&
		get_board(bis[2], bjs[2]) == WHITE&&
		get_board(bis[3], bjs[3]) == BLACK&&
		get_board(bis[7], bjs[7]) == WHITE&&
		color == WHITE
		)
	{
		return true;
	}
	if (get_board(bis[1], bjs[1]) == WHITE&&
		get_board(bis[2], bjs[2]) == BLACK&&
		get_board(bis[3], bjs[3]) == WHITE&&
		get_board(bis[7], bjs[7]) == BLACK&&
		color == BLACK
		)
	{
		return true;
	}
	if (get_board(bis[1], bjs[1]) == BLACK&&
		get_board(bis[0], bjs[0]) == WHITE&&
		get_board(bis[7], bjs[7]) == BLACK&&
		get_board(bis[3], bjs[3]) == WHITE&&
		color == WHITE
		)
	{
		return true;
	}
	if (get_board(bis[1], bjs[1]) == WHITE&&
		get_board(bis[0], bjs[0]) == BLACK&&
		get_board(bis[7], bjs[7]) == WHITE&&
		get_board(bis[3], bjs[3]) == BLACK&&
		color == BLACK
		)
	{
		return true;
	}
	return false;
}
bool GoBoard::match_mogo_pattern(int bi, int bj, int color)
{
	if ((bi == 0 && bj == 0) || (bi == board_size - 1 && bj == 0) || (bi == board_size - 1 && bj == board_size - 1) || (bi == 0 && bj == board_size - 1)) // at corner
		return false;

	if (bi > 0 && bi < board_size - 1 && bj>0 && bj < board_size - 1)
	{
		if (match_hane(bi, bj, color))
			return true;
		//if (match_cut1(bi, bj, color))
		//	return true;
		
		if (match_cut2(bi, bj, color))
			return true;
	}
	else
	{
		if (match_board_side(bi, bj, color))
			return true;
	}
	return false;

}
int GoBoard::mogo_pattern_heuristic(int color)//require not self_atari!!!
{
	int matches[8];
	int matches_number = 0;
	for (int i = 0; i < 8; ++i)
	{
		int bi = I(last_point) + around_i[i];
		int bj = J(last_point) + around_j[i];
		if (!on_board(bi, bj))
			continue;
		if (get_board(bi, bj) != EMPTY)
			continue;
		if (match_mogo_pattern(bi, bj, color))
		{
			matches[matches_number++] = POS(bi, bj);
		}
	}
	int move = random_choose_move(matches, matches_number, color);
	if (move < 0)
		return -1;
	else
	{
		return move;
	}
}