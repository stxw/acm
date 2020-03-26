/* http://poj.org/problem?id=3074 */
/* AC 372K	63MS */
#include <stdio.h>
#include <string.h>

const int MAX_N = 10;

/* 舞蹈链算法，用于求不重复精确覆盖问题 */
typedef struct dance_link
{
	const static int MAX_ROWS = MAX_N * MAX_N * MAX_N;
	const static int MAX_COLS = MAX_N * MAX_N * 4;

	typedef struct node
	{
		int u, d, l, r;
		int row, col;
	} node;

	int rows, cols, node_size;
	node nd[(MAX_ROWS + 1) * MAX_COLS + 1];
	int row_head[MAX_ROWS], col_nds[MAX_COLS];

	bool is_min_ans;
	int limit;
	int ans, *select_rows;

	void init(int rows, int cols)
	{
		int i;

		this -> rows = rows;
		this -> cols = cols;
		/* 初始化每一列的头节点 */
		for(i = 0; i <= cols; i++)
		{
			nd[i].u = i;
			nd[i].d = i;
			nd[i].l = i - 1;
			nd[i].r = i + 1;
			col_nds[i] = 0;
		}
		nd[0].l = cols;
		nd[cols].r = 0;
		node_size = cols + 1;

		/* 初始化每一行的行指针 */
		for(i = 0; i <= rows; i++)
		{
			row_head[i] = -1;
		}
	}

	void add_node(int row, int col)
	{
		/* nd[node_size]为新添加的节点 */
		nd[node_size].row = row;
		nd[node_size].col = col;

		/* 将新添加的节点与其所在的列连接 */
		nd[node_size].u = col;
		nd[node_size].d = nd[col].d;
		nd[nd[col].d].u = node_size;
		nd[col].d = node_size;

		/* 将新添加的节点与其所在的行连接 */
		if(row_head[row] == -1)
		{
			row_head[row] = node_size;
			nd[node_size].l = node_size;
			nd[node_size].r = node_size;
		}
		else
		{
			int row_first = row_head[row];
			nd[node_size].r = row_first;
			nd[node_size].l = nd[row_first].l;
			nd[nd[row_first].l].r = node_size;
			nd[row_first].l = node_size;
		}
		col_nds[col]++;
		node_size++;
	}

	void remove(int col)
	{
		int i, j;

		/* 将第col列从十字链表里移除 */
		nd[nd[col].l].r = nd[col].r;
		nd[nd[col].r].l = nd[col].l;

		/* 将与第col列里节点有关的行移除 */
		for(i = nd[col].d; i != col; i = nd[i].d)
		{
			for(j = nd[i].r; j != i; j = nd[j].r)
			{
				nd[nd[j].u].d = nd[j].d;
				nd[nd[j].d].u = nd[j].u;
				col_nds[nd[j].col]--;
			}
		}
	}

	void resume(int col)
	{
		int i, j;

		/* 将第col列从十字链表里恢复 */
		nd[nd[col].l].r = col;
		nd[nd[col].r].l = col;

		/* 将与第col列里节点有关的行恢复 */
		for(i = nd[col].d; i != col; i = nd[i].d)
		{
			for(j = nd[i].r; j != i; j = nd[j].r)
			{
				nd[nd[j].u].d = j;
				nd[nd[j].d].u = j;
				col_nds[nd[j].col]++;
			}
		}
	}

	int dfs(int len)
	{
		int i, j;
		int res, select_col;

		/* 判断是否超过了界限 */
		if(limit != -1 && len > limit)
		{
			return -1;
		}
		if(is_min_ans == true && ans != -1 && len > ans)
		{
			return -1;
		}
		/* 当前十字链表没有列 */
		if(nd[0].r == 0)
		{
			return len;
		}
		select_col = nd[0].r;
		for(i = nd[0].r; i != 0; i = nd[i].r)
		{
			if(nd[i].d == i)
			{
				return -1;
			}
			if(col_nds[select_col] > col_nds[i])
			{
				select_col = i;
			}
		}
		remove(select_col);
		for(i = nd[select_col].d; i != select_col; i = nd[i].d)
		{
			if(select_rows != 0)
			{
				select_rows[len] = nd[i].row;
			}
			for(j = nd[i].r; j != i; j = nd[j].r)
			{
				remove(nd[j].col);
			}
			res = dfs(len + 1);
			if(res >= 0)
			{
				if(is_min_ans == false)
				{
					return res;
				}
				else if(ans < 0 || ans > res)
				{
					ans = res;
				}
			}
			for(j = nd[i].l; j != i; j = nd[j].l)
			{
				resume(nd[j].col);
			}
		}
		resume(select_col);
		return ans;
	}

	/*
	bool is_min_ans: 是否求答案最小值，如果不是，得到一个可行解就返回，默认求最小值。
	int select_rows[]: 用于保存选择的行，取NULL时不保存，默认取NULL。
	int limit：答案的上限，取-1时无上限，默认为-1。
	*/
	int solve(bool is_min_ans = true, int select_rows[] = 0, int limit = -1)
	{
		this->is_min_ans = is_min_ans;
		this->select_rows = select_rows;
		this->limit = limit;
		ans = -1;
		ans = dfs(0);
		return ans;
	}

} dance_link;

dance_link dl;

int main()
{
	int i, j, k, len, r, c, row;
	int x[dl.MAX_ROWS], y[dl.MAX_ROWS], num[dl.MAX_ROWS], ans[dl.MAX_ROWS], ansl;
	char str[MAX_N * MAX_N];

	while(fgets(str, MAX_N * MAX_N, stdin) != NULL)
	{
		k = 0;
		for(i = 0; i < 81; i++)
		{
			if(str[i] == '.')
			{
				str[i] = 0;
			}
			else if('0' <= str[i] && str[i] <= '9')
			{
				str[i] = str[i] - '0';
			}
			else
			{
				k = 1;
				break;
			}
			// printf("%d ", str[i]);
			// if((i + 1) % 9 == 0)
			// {
			// 	printf("\n");
			// }
		}
		// printf("\n");
		if(k == 1)
		{
			break;
		}

		dl.init(9 * 9 * 9, 4 * 9 * 9);
		// printf("%d, %d\n", 9 * 9 * 9, 4 * 9 * 9);
		row = 1;
		for(i = 0; i < 81; i++)
		{
			r = i / 9;
			c = i % 9;
			if(str[i] == 0)
			{
				for(k = 0; k < 9; k++)
				{
					dl.add_node(row, r * 9 + c + 1);
					dl.add_node(row, 81 + r * 9 + k + 1);
					dl.add_node(row, 162 + c * 9 + k + 1);
					dl.add_node(row, 243 + (r / 3 * 3 + c / 3) * 9 + k + 1);
					x[row] = r;
					y[row] = c;
					num[row] = k + 1;
					// printf("(%d,%d) ", row, r * 9 + c + 1);
					// printf("(%d,%d) ", row, 81 + r * 9 + k + 1);
					// printf("(%d,%d) ", row, 162 + c * 9 + k + 1);
					// printf("(%d,%d)\n", row, 243 + (r / 3 * 3 + c / 3) * 9 + k + 1);
					row++;
				}
			}
			else
			{
				k = str[i] - 1;
				dl.add_node(row, r * 9 + c + 1);
				dl.add_node(row, 81 + r * 9 + k + 1);
				dl.add_node(row, 162 + c * 9 + k + 1);
				dl.add_node(row, 243 + (r / 3 * 3 + c / 3) * 9 + k + 1);
				x[row] = r;
				y[row] = c;
				num[row] = k + 1;
				// printf("(%d,%d) ", row, r * 9 + c + 1);
				// printf("(%d,%d) ", row, 81 + r * 9 + k + 1);
				// printf("(%d,%d) ", row, 162 + c * 9 + k + 1);
				// printf("(%d,%d)\n", row, 243 + (r / 3 * 3 + c / 3) * 9 + k + 1);
				row++;
			}
		}
		ansl = dl.solve(false, ans);
		// printf("%d\n", ansl);
		for(i = 0; i < ansl; i++)
		{
			r = x[ans[i]];
			c = y[ans[i]];
			k = num[ans[i]];
			str[r * 9 + c] = k;
		}
		// for(i = 0; i < 81; i++)
		// {
		// 	printf("%d ", str[i]);
		// 	if((i + 1) % 9 == 0)
		// 	{
		// 		printf("\n");
		// 	}
		// }
		// printf("\n");
		for(i = 0; i < 81; i++)
		{
			printf("%d", str[i]);
		}
		printf("\n");
	}
	return 0;
}

/*

.2738..1..1...6735.......293.5692.8...........6.1745.364.......9518...7..8..6534.
......52..8.4......3...9...5.1...6..2..7........3.....6...1..........7.4.......3.
end

*/