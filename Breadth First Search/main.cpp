#include <iostream>
#include <stack>
#include <queue>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <algorithm> 
#include <cstring>
#include <cmath>
#define BIG_NUM 999999

using namespace std;

int step0_row[4] = {0,0,-1,1};
int step0_col[4] = {-1,1,0,0};
int step_row[4] = {1,0,0,-1};
int step_col[4] = {0,-1,1,0};
int map[100][100];
int pre[100][100]; //record the last point
int cost[100][100];
int open_list[100][100];
int close_list[100][100];
int map_x, map_y;
int is_euclidean = 1; //check which heuristic will be used
bool rec[100][100]; //record whether it has visited or not
char output[100][100]; //output the map

struct Node
{
	int row, col, pre, cost;
}que[50000]; 

//calculate the difference between two obstacles
int calc_cost(int start_row, int start_col, int end_row, int end_col)
{
	int start = map[start_row][start_col];
	int end = map[end_row][end_col];

	//down
	if ( start >= end )
	{
		return 1;
	}
	//up
	else
	{
		return end - start + 1;
	}
}

//print out the road 
void map_print()
{
	for (int i = 1;i <= map_x; i++)
	{
		for (int j = 1; j <= map_y; j++)
		{
			cout << output[i][j] << ' ';
		}
		cout << endl;
	}
}

void getPath(int end_row, int end_col)
{
	int f = pre[end_row][end_col];
	output[end_row][end_col] = '*';

	//find the path
	while (f != -1)
	{
		output[que[f].row][que[f].col] = '*';
		f = pre[que[f].row][que[f].col];
	}
	map_print();
}

void ASTAR_getPath(int start_row, int start_col, int end_row, int end_col)
{
	int tmp_row = end_row, tmp_col = end_col, f;
	output[start_row][start_col] = '*';

	//find the path
	while (tmp_row != start_row || tmp_col != start_col)
	{
		output[tmp_row][tmp_col] = '*';
		f = pre[tmp_row][tmp_col];
		tmp_row = tmp_row - step_row[f];
		tmp_col = tmp_col - step_col[f];
	}
	map_print();
}

//calculate the H
float calc_H(int start_row, int start_col, int end_row, int end_col)
{
	//euclidean
	if (is_euclidean == 1)
	{
		return sqrt(pow(start_row - end_row,2) + pow(start_col - end_col,2));
	}
	//manhattan
	else
	{
		return abs(start_row - end_row) + abs(start_col - end_col);
	}
}

void BFS(int start_row, int start_col, int end_row, int end_col)
{
	//initial the queue
	int head = 0;
	int tail = 1;

	Node cur, last;
    int next_row, next_col, tmp_cost;

    //initialize 
	memset(rec, false, sizeof(rec)); 
	memset(cost, BIG_NUM, sizeof(cost));
	memset(pre, -1, sizeof(pre));

	//insert the start point to the queue
	que[head].row = start_row;
	que[head].col = start_col;
    que[head].pre = -1;
	cost[start_row][start_col] = 0; //the cost at the start point is zero
    rec[start_row][start_col] = true; //the cost at the start point is zero

    //it is not empty
	while (head < tail) 
	{
		cur = que[head]; 
		rec[cur.row][cur.col] = true; //it is not empty

		//not the start point 
		if(cur.pre != -1)
		{
			pre[cur.row][cur.col] = cur.pre; //record how to reach this point	
		}
			
		//the point at up down left right will be insert to the queue
		for (int i = 0; i < 4; i++)
		{
			next_row = cur.row + step0_row[i];
			next_col = cur.col + step0_col[i];
			
			//not out of range, not be visited, no barrier	
			if (next_row > 0 && next_row <= map_y && 
				next_col > 0 && next_col <= map_x && 
				map[next_row][next_col] != BIG_NUM && 
				rec[next_row][next_col] == false)
			{
				que[tail].row = next_row;
                que[tail].col = next_col;
                que[tail].pre = head; //record the pre
                tail ++;
			}
		}
        head ++;
	}
	getPath(end_row, end_col);
}

void UCS(int start_row, int start_col, int end_row, int end_col)
{
	//initial the queue
	int head = 0;
	int tail = 1;

	Node cur, last;
    int next_row, next_col, tmp_cost;

    //initial
	memset(rec, false, sizeof(rec)); 
	memset(cost, BIG_NUM, sizeof(cost));
	memset(pre, -1, sizeof(pre));

	//insert the start point to the queue
	que[head].row = start_row;
	que[head].col = start_col;
    que[head].pre = -1;
    cost[start_row][start_col] = 0; //the cost at the start point is zero
    rec[start_row][start_col] = true; //the start point has been visited

    //it is not empty
	while (head < tail) 
	{
		cur = que[head]; 
		rec[cur.row][cur.col] = true; //visited

		//not the start point 
		if (cur.pre != -1)
		{
			last = que[cur.pre]; //the last point
			tmp_cost = cost[last.row][last.col];
			tmp_cost = tmp_cost + calc_cost(last.row, last.col, cur.row, cur.col);

			//the shortest path 
			if (tmp_cost < cost[cur.row][cur.col])
			{
				cost[cur.row][cur.col] = tmp_cost;
				pre[cur.row][cur.col] = cur.pre; //record how to reach this point
			}
		}

		//the point at up down left right will be insert to the queue
		for (int i = 0; i < 4; i++)
		{
			next_row = cur.row + step_row[i];
			next_col = cur.col + step_col[i];
			
			//not out of range, not be visited, no barrier	
			if (next_row > 0 && next_row <= map_y && 
				next_col > 0 && next_col <= map_x && 
				map[next_row][next_col] != BIG_NUM && 
				rec[next_row][next_col] == false)
			{
				que[tail].row = next_row;
                que[tail].col = next_col;
                que[tail].pre = head; //record the pre point
                tail ++;
			}
		}
        head ++;
	}
	getPath(end_row, end_col);
}

void A_Star(int start_row, int start_col, int end_row, int end_col)
{
	//initialisation
	for (int i = 1; i <= map_x; i++)
	{
		for (int j = 1; j <= map_y; j++)
		{
			open_list[i][j] = 0;
			close_list[i][j] = 0;
		}
	}
	open_list[start_row][start_col] = 1; //insert the start point to the open list
	cost[start_row][start_col] = 0; //the cost at the start point is zero

	int tt = 0, tmp_x, tmp_y, tmp_cost ;

	while (true)
	{
		
		//the end point in the openlist
		if(open_list[end_row][end_col] == 1)
		{
			break;
		}

		float tmp_F, min_F = BIG_NUM; 

		//find the minimal F in the open list
		for (int i = 1;i <= map_x; i++)
		{
			for (int j = 1; j <= map_y; j++)
			{
				if ((open_list[i][j] == 1) && (close_list[i][j] == 0))
				{
					tmp_F = cost[i][j] + calc_H(i, j, end_row, end_col);
					
					if (tmp_F <= min_F)
					{
						min_F = tmp_F;
						tmp_x = i; 
						tmp_y = j;
					}
				}
			}
		}

		if (min_F == BIG_NUM)
		{
			cout << "NULL" << endl;
			return;
		}
		tt++;

		open_list[tmp_x][tmp_y] = 0;
		close_list[tmp_x][tmp_y] = 1;
	
		//the point at up down left right
		for (int i = 0; i < 4; i++)
		{
			int next_row = tmp_x + step_row[i];
			int next_col = tmp_y + step_col[i];

			//not out of range
			if (next_row > 0 && next_row <= map_y 
				&& next_col > 0 && next_col <= map_x)
			{
				//the point is in the close list then go to the next one
				if (close_list[next_row][next_col] == 1)
				{
					continue; 
				}
				//the point is not in the close list 
				else
				{
					//the point is in the open list
					if (open_list[next_row][next_col] == 1)
					{
						tmp_cost = cost[tmp_x][tmp_y] + calc_cost(tmp_x, tmp_y, next_row, next_col);
					
						//find the shorter one
						if (tmp_cost < cost[next_row][next_col])
						{
							pre[next_row][next_col] = i;
							cost[next_row][next_col] = tmp_cost;
						}
					}
					//the point is not in the open list
					else
					{
						//no barrier	
						if (map[next_row][next_col] != BIG_NUM)
						{
							open_list[next_row][next_col] = 1; //insert the point to the open list
							cost[next_row][next_col] = cost[tmp_x][tmp_y] + calc_cost(tmp_x, tmp_y, next_row, next_col);
							pre[next_row][next_col] = i;
						}
					}
				}				
			}
		}
	}
	ASTAR_getPath(start_row, start_col, end_row, end_col);
}

int main(int argc, char* argv[])
{
	int start_x, start_y, end_x, end_y;
	
	string filename(argv[1]);
	
	/*read the map file and get all values*/
	ifstream myfile(filename);

	string line;
	vector<string> vec;
 	while(getline(myfile, line))
 	{
 		vec.push_back(line);
 	}
	string str1 = vec[0], str2 = vec[1], str3 = vec[2];
	string a0, a1, b0, b1, c0, c1;
	istringstream is1(str1);
	istringstream is2(str2);
	istringstream is3(str3);

	is1 >> a0 >> a1;
	is2 >> b0 >> b1;
	is3 >> c0 >> c1;

	map_x = atoi(a0.c_str());
	map_y = atoi(a1.c_str());

	start_x = atoi(b0.c_str());
	start_y = atoi(b1.c_str());

	end_x = atoi(c0.c_str());
	end_y = atoi(c1.c_str());

	//store the map in a string
	string ss;
	for (int i = 3; i < vec.size(); ++i)
	{
		ss.append(vec[i]);
	}

	//delete the spaces
	ss.erase(remove(ss.begin(), ss.end(), ' '), ss.end());

	//change the map subscript
	for (int i = 1; i <= map_x; i++)
	{
		for (int j = 1; j <= map_y; j++)
		{
			if ( ss[(i-1) * map_y + (j-1)] == 'X') 
			{
				map[i][j] = BIG_NUM;
			}
			else
			{
				map[i][j] = ss[(i-1) * map_y + (j-1)] - '0';
			}
			output[i][j] = ss[(i-1) * map_y + (j-1)];
		}
	}

	string choice(argv[2]);
	// string heu(argv[3]);

	if (choice == "bfs")
	{
		BFS(start_x, start_y, end_x, end_y);
	}
	else if (choice == "ucs")
	{
		UCS(start_x, start_y, end_x, end_y);
	}
	else if(choice == "astar")
	{
		string heu(argv[3]);
		if ( heu == "euclidean")
		{
			is_euclidean = 1;
		}
		else
		{
			is_euclidean = 0;
		}
		A_Star(start_x, start_y, end_x, end_y);
	}
 	return 0;
}