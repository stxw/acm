/* http://acm.fzu.edu.cn/problem.php?pid=1686 */
#include <stdio.h>
#include <string.h>

const int MAX_N = 40;

typedef struct dance_link
{
	const static int MAX_ROWS = MAX_N * MAX_N;
	const static int MAX_COLS = MAX_N * MAX_N;

	typedef struct node
	{
		int u, d, l, r;
		int row, col;
	} node;

	int rows, cols, node_size;
	node nd[MAX_ROWS * MAX_COLS];
	int row_head[MAX_ROWS], col_nds[MAX_COLS];

	int ans;
	
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

	inline void add_node(int row, int col)
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

	inline void remove(int col)
	{
		int i;
		for(i = nd[col].d; i != col; i = nd[i].d)
		{
			nd[nd[i].l].r = nd[i].r;
			nd[nd[i].r].l = nd[i].l;
		}
	}

	inline void resume(int col)
	{
		int i;
		for(i = nd[col].d; i != col; i = nd[i].d)
		{
			nd[nd[i].l].r = i;
			nd[nd[i].r].l = i;
		}
	}

	/* 计算取得答案最少需要的行数 */
	inline int get_min_rows()
	{
		int i, j, k, num = 0;
		bool v[MAX_COLS];
		
		for(i = nd[0].r; i != 0; i = nd[i].r)
		{
			v[i] = true;
		}
		for(i = nd[0].r; i != 0; i = nd[i].r)
		{
			if(v[i] == false)
			{
				continue;
			}
			num++;
			for(j = nd[i].d; j != i; j = nd[j].d)
			{
				for(k = nd[j].r; k != j; k = nd[k].r)
				{
					v[nd[k].col] = false;
				}
			}
		}
		return num;
	}

	int dfs(int len)
	{
		int i, j;
		int res, select_col;

		/* 判断是否超过了界限 */
		int mr = get_min_rows();
		if(ans > 0 && len + mr >= ans)
		{
			return -1;
		}
		/* 当前十字链表没有列 */
		if(nd[0].r == 0)
		{
			ans = len;
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
		for(i = nd[select_col].d; i != select_col; i = nd[i].d)
		{
			remove(i);
			for(j = nd[i].r; j != i; j = nd[j].r)
			{
				remove(j);
			}
			dfs(len + 1);
			for(j = nd[i].l; j != i; j = nd[j].l)
			{
				resume(j);
			}
			resume(i);
		}
		return ans;
	}
} dance_link;

dance_link dl;

int main()
{
	int n, m, n1, m1;
	int i, j, k, x, y, ans;
	int map[MAX_N][MAX_N];

	while(scanf("%d %d", &n, &m) != EOF)
	{
		k = 0;
		for(i = 0; i < n; i++)
		{
			for(j = 0; j < m; j++)
			{
				scanf("%d", &map[i][j]);
				if(map[i][j] != 0)
				{
					k++;
					map[i][j] = k;
				}
			}
		}
		scanf("%d %d", &n1, &m1);

		dl.init((n - n1 + 1) * (m - m1 + 1), k);
		k = 0;
		for(x = 0; x + n1 <= n; x++)
		{
			for(y = 0; y + m1 <= m; y++)
			{
				for(i = x; i < x + n1; i++)
				{
					for(j = y; j < y + m1; j++)
					{
						if(map[i][j] != 0)
						{
							dl.add_node(k + 1, map[i][j]);
						}
					}
				}
				k++;
			}
		}

		dl.ans = -1;
		ans = dl.dfs(0);
		printf("%d\n", ans);
	}
	return 0;
}

/*
4 4
1 0 0 1
0 1 1 0
0 1 1 0
1 0 0 1
2 2
4 4 
0 0 0 0
0 1 1 0
0 1 1 0
0 0 0 0
2 2

4
1
*/