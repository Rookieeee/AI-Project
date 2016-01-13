#include "GoBoard.h"
#include <queue>

bool GoBoard::is_self_atari(int point, int color) //  is_self_atari that is , add this point, the group of the move will be in atari(only one liberty)
{
	for (int i = 0; i < 4; ++i)
	{
		int ai = I(point) + deltai[i];
		int aj = J(point) + deltaj[i];
		if (!on_board(ai, aj))
			continue;
		if (board[POS(ai, aj)] && board[POS(ai, aj)]->get_color() == OTHER_COLOR(color) && board[POS(ai, aj)]->get_liberties_number() == 1)
			return false;
	}

	return total_liberties(point, color, 0, 1, 0) == 1;
}

bool GoBoard::heavy_policy(int point, int color)
{
	if (is_virtual_eye(point, color)) return false;
	if (!available(I(point), J(point), color)) return false;
	if (is_self_atari(point, color)) return false;
	//if (fast_ladder(point,color)) return false;  I don't understand what is fast_ladder, the implementation is below.
	//if (bad_self_atari(point,color)) return false; do not understand.... refer the source code.
	return true;
}

int GoBoard::is_kakari_available(int color, int last_point)
{
	int star[4] = { 3 * board_size + 3, 3 * board_size + 9, 9 * board_size + 3, 9 * board_size + 9 }; //stars position
	int other = OTHER_COLOR(color);
	int kakari1, kakari2;

	if (board[star[0]] && board[star[0]]->get_color() != color)
	{
		kakari1 = relative_point(star[0], -1, 2);
		if (!board[kakari1] && heavy_policy(kakari1, color)) return kakari1;
		kakari2 = relative_point(star[0], 2, -1);
		if (board[kakari1]->get_color() != color && !board[kakari2] && heavy_policy(kakari2, color)) return kakari2;
	}

	if (board[star[1]] && board[star[1]]->get_color() != color)
	{
		kakari1 = relative_point(star[1], 2, 1);
		if (!board[kakari1] && heavy_policy(kakari1, color)) return kakari1;
		kakari2 = relative_point(star[1], -1, -2);
		if (board[kakari1]->get_color() != color && !board[kakari2] && heavy_policy(kakari2, color)) return kakari2;
	}

	if (board[star[2]] && board[star[2]]->get_color() != color)
	{
		kakari1 = relative_point(star[2], 1, 2);
		if (!board[kakari1] && heavy_policy(kakari1, color)) return kakari1;
		kakari2 = relative_point(star[2], -2, -1);
		if (board[kakari1]->get_color() != color && !board[kakari2] && heavy_policy(kakari2, color)) return kakari2;
	}

	if (board[star[3]] && board[star[3]]->get_color() != color)
	{
		kakari1 = relative_point(star[3], -2, 1);
		if (!board[kakari1] && heavy_policy(kakari1, color)) return kakari1;
		kakari2 = relative_point(star[3], 1, -2);
		if (board[kakari1]->get_color() != color && !board[kakari2] && heavy_policy(kakari2, color)) return kakari2;
	}

	return -1;
}

int GoBoard::is_anti_dian33_available(int color, int last_point)
{
	int star[4] = { 3 * board_size + 3, 3 * board_size + 9, 9 * board_size + 3, 9 * board_size + 9 }; //stars position
	int other = OTHER_COLOR(color);
	int dian, anti;

	dian = relative_point(star[0], -1, -1);
	anti = relative_point(star[0], 0, -1);
	if (board[star[0]] && board[star[0]]->get_color() == color
		&& board[dian] && board[dian]->get_color() != color
		&& !board[anti] && heavy_policy(anti, color))
		return anti;

	dian = relative_point(star[1], -1, 1);
	anti = relative_point(star[1], 0, 1);
	if (board[star[1]] && board[star[1]]->get_color() == color
		&& board[dian] && board[dian]->get_color() != color
		&& !board[anti] && heavy_policy(anti, color))
		return anti;

	dian = relative_point(star[2], 1, -1);
	anti = relative_point(star[2], 0, -1);
	if (board[star[2]] && board[star[2]]->get_color() == color
		&& board[dian] && board[dian]->get_color() != color
		&& !board[anti] && heavy_policy(anti, color))
		return anti;

	dian = relative_point(star[3], 1, 1);
	anti = relative_point(star[3], 0, 1);
	if (board[star[3]] && board[star[3]]->get_color() == color
		&& board[dian] && board[dian]->get_color() != color
		&& !board[anti] && heavy_policy(anti, color))
		return anti;

	return -1;
}

int GoBoard::is_anti_yijianjia_available(int color, int last_point)
{
	int star[4] = { 3 * board_size + 3, 3 * board_size + 9, 9 * board_size + 3, 9 * board_size + 9 }; //stars position
	int yijianjia1, yijianjia2;
	int other = OTHER_COLOR(color);

	yijianjia1 = relative_point(star[0], 0, 2);
	yijianjia2 = relative_point(star[0], 2, 0);
	if (board[star[0]] && board[star[0]]->get_color() == color){
		if (board[yijianjia1] && board[yijianjia1]->get_color() != color
			&& !board[yijianjia2] && heavy_policy(yijianjia2, color))
			return yijianjia2;
		if (board[yijianjia2] && board[yijianjia2]->get_color() != color
			&& !board[yijianjia1] && heavy_policy(yijianjia1, color))
			return yijianjia1;
	}

	yijianjia1 = relative_point(star[1], 0, -2);
	yijianjia2 = relative_point(star[1], 2, 0);
	if (board[star[1]] && board[star[1]]->get_color() == color){
		if (board[yijianjia1] && board[yijianjia1]->get_color() != color
			&& !board[yijianjia2] && heavy_policy(yijianjia2, color))
			return yijianjia2;
		if (board[yijianjia2] && board[yijianjia2]->get_color() != color
			&& !board[yijianjia1] && heavy_policy(yijianjia1, color))
			return yijianjia1;
	}

	yijianjia1 = relative_point(star[2], 0, 2);
	yijianjia2 = relative_point(star[2], -2, 0);
	if (board[star[2]] && board[star[2]]->get_color() == color){
		if (board[yijianjia1] && board[yijianjia1]->get_color() != color
			&& !board[yijianjia2] && heavy_policy(yijianjia2, color))
			return yijianjia2;
		if (board[yijianjia2] && board[yijianjia2]->get_color() != color
			&& !board[yijianjia1] && heavy_policy(yijianjia1, color))
			return yijianjia1;
	}

	yijianjia1 = relative_point(star[3], 0, -2);
	yijianjia2 = relative_point(star[3], -2, 0);
	if (board[star[3]] && board[star[3]]->get_color() == color){
		if (board[yijianjia1] && board[yijianjia1]->get_color() != color
			&& !board[yijianjia2] && heavy_policy(yijianjia2, color))
			return yijianjia2;
		if (board[yijianjia2] && board[yijianjia2]->get_color() != color
			&& !board[yijianjia1] && heavy_policy(yijianjia1, color))
			return yijianjia1;
	}

	return -1;
}

int GoBoard::is_anti_kakari_available(int color, int last_point)
{
	int star[4] = { 3 * board_size + 3, 3 * board_size + 9, 9 * board_size + 3, 9 * board_size + 9 }; //stars position
	int kakari1, kakari2;
	int other = OTHER_COLOR(color);

	kakari1 = relative_point(star[0], -1, 2);
	kakari2 = relative_point(star[0], 2, -1);
	if (board[star[0]] && board[star[0]]->get_color() == color){
		if (board[kakari1] && board[kakari1]->get_color() != color
			&& !board[kakari2] && heavy_policy(kakari2, color))
			return kakari2;
		if (board[kakari2] && board[kakari2]->get_color() != color
			&& !board[kakari1] && heavy_policy(kakari1, color))
			return kakari1;
	}

	kakari1 = relative_point(star[1], 2, 1);
	kakari2 = relative_point(star[1], -1, -2);
	if (board[star[1]] && board[star[1]]->get_color() == color){
		if (board[kakari1] && board[kakari1]->get_color() != color
			&& !board[kakari2] && heavy_policy(kakari2, color))
			return kakari2;
		if (board[kakari2] && board[kakari2]->get_color() != color
			&& !board[kakari1] && heavy_policy(kakari1, color))
			return kakari1;
	}

	kakari1 = relative_point(star[2], 1, 2);
	kakari2 = relative_point(star[2], -2, -1);
	if (board[star[2]] && board[star[2]]->get_color() == color){
		if (board[kakari1] && board[kakari1]->get_color() != color
			&& !board[kakari2] && heavy_policy(kakari2, color))
			return kakari2;
		if (board[kakari2] && board[kakari2]->get_color() != color
			&& !board[kakari1] && heavy_policy(kakari1, color))
			return kakari1;
	}

	kakari1 = relative_point(star[3], -2, 1);
	kakari2 = relative_point(star[3], 1, -2);
	if (board[star[3]] && board[star[3]]->get_color() == color){
		if (board[kakari1] && board[kakari1]->get_color() != color
			&& !board[kakari2] && heavy_policy(kakari2, color))
			return kakari2;
		if (board[kakari2] && board[kakari2]->get_color() != color
			&& !board[kakari1] && heavy_policy(kakari1, color))
			return kakari1;
	}

	return -1;
}

int GoBoard::relative_point(int pos, int di, int dj)
{
	return POS(I(pos) + di, J(pos) + dj);
}

int GoBoard::is_xiaomu_available(int color, int last_point)
{
	int star[4] = { 3 * board_size + 3, 3 * board_size + 9, 9 * board_size + 3, 9 * board_size + 9 }; //stars position
	int anti, xiaomu;

	if (!(board[star[0]] && board[star[0]]->get_color() == color)){
		anti = relative_point(star[0], 1, -1);
		xiaomu = relative_point(star[0], -1, 0);
		if (board[xiaomu] && board[xiaomu]->get_color() != color
			&& !board[anti] && heavy_policy(anti, color))
			return anti;
		anti = relative_point(star[0], -1, 1);
		xiaomu = relative_point(star[0], 0, -1);
		if (board[xiaomu] && board[xiaomu]->get_color() != color
			&& !board[anti] && heavy_policy(anti, color))
			return anti;
	}

	if (!(board[star[1]] && board[star[1]]->get_color() == color)){
		anti = relative_point(star[1], 1, 1);
		xiaomu = relative_point(star[1], -1, 0);
		if (board[xiaomu] && board[xiaomu]->get_color() != color
			&& !board[anti] && heavy_policy(anti, color))
			return anti;
		anti = relative_point(star[1], -1, -1);
		xiaomu = relative_point(star[1], 0, 1);
		if (board[xiaomu] && board[xiaomu]->get_color() != color
			&& !board[anti] && heavy_policy(anti, color))
			return anti;
	}

	if (!(board[star[2]] && board[star[2]]->get_color() == color)){
		anti = relative_point(star[2], -1, -1);
		xiaomu = relative_point(star[2], 1, 0);
		if (board[xiaomu] && board[xiaomu]->get_color() != color
			&& !board[anti] && heavy_policy(anti, color))
			return anti;
		anti = relative_point(star[2], 1, 1);
		xiaomu = relative_point(star[2], 0, -1);
		if (board[xiaomu] && board[xiaomu]->get_color() != color
			&& !board[anti] && heavy_policy(anti, color))
			return anti;
	}

	if (!(board[star[3]] && board[star[3]]->get_color() == color)){
		anti = relative_point(star[3], 1, -1);
		xiaomu = relative_point(star[3], 0, 1);
		if (board[xiaomu] && board[xiaomu]->get_color() != color
			&& !board[anti] && heavy_policy(anti, color))
			return anti;
		anti = relative_point(star[3], -1, 1);
		xiaomu = relative_point(star[3], 1, 0);
		if (board[xiaomu] && board[xiaomu]->get_color() != color
			&& !board[anti] && heavy_policy(anti, color))
			return anti;
	}
	return -1;
}
//
int GoBoard::is_star_available(int color, int last_moves)
{
	int star[4] = { 3 * board_size + 3, 3 * board_size + 9, 9 * board_size + 3, 9 * board_size + 9 }; //stars position
	int mid[4] = { 3 * board_size + 6, 6 * board_size + 3, 6 * board_size + 9, 9 * board_size + 6 }; //middle of stars

	//Occupy Stars
	if (!board[star[0]] && heavy_policy(star[0], color)) return star[0];
	if (!board[star[1]] && heavy_policy(star[1], color)) return star[1];
	if (!board[star[2]] && heavy_policy(star[2], color)) return star[2];
	if (!board[star[3]] && heavy_policy(star[3], color)) return star[3];

	//Occupy Mids
	if (board[star[0]] && board[star[1]])
	{
		if (board[star[0]]->get_color() == color && board[star[1]]->get_color() == color
			&& !board[mid[0]] && heavy_policy(mid[0], color))
			return mid[0];
	}

	if (board[star[0]] && board[star[2]])
	{
		if (board[star[0]]->get_color() == color && board[star[2]]->get_color() == color
			&& !board[mid[1]] && heavy_policy(mid[1], color))
			return mid[1];
	}

	if (board[star[1]] && board[star[3]])
	{
		if (board[star[1]]->get_color() == color && board[star[3]]->get_color() == color
			&& !board[mid[2]] && heavy_policy(mid[2], color))
			return mid[2];
	}

	if (board[star[2]] && board[star[3]])
	{
		if (board[star[2]]->get_color() == color && board[star[3]]->get_color() == color
			&& !board[mid[3]] && heavy_policy(mid[3], color))
			return mid[3];
	}

	return -1;
}
