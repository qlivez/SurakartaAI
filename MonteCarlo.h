#pragma once

#include <iostream>

#include "agent.h"

#include "Node.h"
#include <math.h>

#include <stdlib.h>
#include <time.h>
#include <random>

class MonteCarloTree {
public:
	Node* root;
	vector<Node*> path;
	//int bsize, wsize, tsize;
	//int b_onego[BoardSize], w_onego[BoardSize], twogo[BoardSize];
	board root_board;

	const double UCB_weight = sqrt(2);

	MonteCarloTree() {}
	
	Node* UCB(Node* n) {
		srand(time(NULL));

		if(n->c_size == 0) return NULL;
		
		double eps = 1e-3;
		double max_ans = -100.0;
		double same_score[100];
		same_score[0] = 0;
		int idx = 1;
		int min_count;

		for (int i=0; i < n->c_size; i++) {
			Node* ch = n->child + i;
			
			double score = ( ch->win / ch->count ) + UCB_weight * sqrt( log(n->count) /(double)(ch->count) );
			
			if ( (score <= (max_ans+eps) ) && (score >= (max_ans-eps) ) ) {
				same_score[idx] = i;
				idx++;
			}
			else if (score > max_ans) {
				max_ans = score;
				same_score[0] = i;
				idx = 1;
			}
		}

		int ans = same_score[ rand() % idx];
		return (n->child + ans);
	}

	void select(board &b) {
		//int color = b.take_turn();

		Node* current = root;
		
		path.clear();
		path.push_back(current);

		while (current->child != NULL && current->c_size != 0) {
			current = UCB(current);
			path.push_back(current);

			//b.add(current->place, current->color);
			b.move(current->move.first, current->move.second, current->color);
		}
	}
	void backpropogate(board &b, double result) {
		for (int i=0; i<path.size(); i++) {
			path[i]->addresult(result);
		}
	}
	bool tree_policy() {
		board b;
		b = root_board;
		select(b);
		Node &last = *(path.back());
		Node *current;
		last.expand(b);

		if(last.c_size != 0) {
			current = UCB(&last);
			path.push_back(current);
			b.move(current->move.first, current->move.second, current->color);
		}
		else 
			return false;
		//b.getv(b_onego, w_onego, twogo, bsize, wsize, tsize);

		double result;
		
		result = simulate(b);

		backpropogate(b, result);
		return true;
	}
	double simulate(board b) {
		random_device rd;
		default_random_engine engine(rd());

		int cnt = 0;
		while(true) {
			cnt++;
			if (cnt > 1000) {

				// cout << "More than 1000 steps\n";
				// cout << b << '\n';
				
				return b.judge();
			}

			int color = b.take_turn();

			int tmp;
			if (color==0) 
				tmp = 2;
			else
				tmp = 1;
			vector<Pair> av = b.get_available_move(tmp);
		
			if (av.empty())
				return color;//win

			shuffle(av.begin(), av.end(), engine);

			b.move(av[0].first, av[0].second, color);

		}

	}
	void reset(board &b) {
		root_board = b;
		root = new Node;
		root->color = root_board.take_turn();
		root->move = {-1, -1};
		root->count = 1;
		root->expand(b);

	}
	void clear() {
		if (root!=NULL) delete root;
	}

};