#pragma once
#define NDEBUG

#include "board.h"
#include <memory>
#include <climits>
class MonteCarloTree;

class Node {
private:
	// Node* child;
	std::unique_ptr<Node[]> child;
	bool color;
	Pair move;
	std::size_t c_size;
	double count;
	double win;
	double means;

public:		
	friend class MonteCarloTree;

	Node() : child(nullptr), c_size(0), count(0), win(0), means(0.5) {}
	~Node() {}
	
	void init_Node (const Pair &m, const bool &piece) {
		this->color = piece;
		this->move = m;
	}
	
	void addresult (const WIN_STATE &result) {
		// result is draw
		if (result == DRAW)
			count++;
		else if ( (result == BLACK_WIN && color==BLACK) || (result == WHITE_WIN && color==WHITE) ) {
			count++;
			win++;
			//means = (means*count+1.00)/(count+1);
		}
		else
			count++;
	//	else
			//means = (means*count)/(count+1);

	}
	void expand(board &b) {
		
		const bool& c = b.take_turn();
		std::vector<Pair> mv = b.get_available_move(c);

		c_size = mv.size();

		if (c_size == 0)
			return;
		
		child = std::make_unique<Node[]>(c_size);
		int idx = 0;
		for (auto &m : mv) {
			child[idx].init_Node(m, c);
			idx++;
		}
		
	}

	Pair best_child() {
		std::size_t best_child_idx = -1;
		double most_visit = INT_MIN;
		double tmp;

		for (std::size_t i = 0 ; i < c_size; ++i) {
			tmp = child[i].count;
			if (tmp > most_visit) {
				most_visit = tmp;
				best_child_idx = i;
			}
		}

		auto best_child { this->child.get() + best_child_idx };
		
		return std::move( best_child->move );
	}
	void showchild() {
		std::cerr << std::setw(6) <<"from:";
		std::cerr << std::setw(10) << "to:";
		std::cerr << "  win rate     count\n";
		for(std::size_t i=0; i < c_size; ++i) {
			if (child[i].count >= 0) {
				std::cerr << "(" << child[i].move.prev/6 << ", "<< child[i].move.prev%6  << ") to (" << child[i].move.next/6 << ", " <<child[i].move.next%6 << ")  " 
				<< child[i].win/child[i].count << ' ' << child[i].count << ' ';
				std::cerr << "\n\n";
			}
		}
	}


};
