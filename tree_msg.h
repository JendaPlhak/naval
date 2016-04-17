#pragma once
#include <algorithm>
#include <vector>
#include <iostream>
#include <limits>
#include <utility> // std::pair

/// A Node class that represents the tree structure of the network.
/// Each node holds a pointer to all its children. The pointer is non-owning
/// (the Node class does not deal with its children's destruction).
/// You may extend the class if you wish.
class Node {
public:
	Node( int id ) : _id( id ) {}
	int getID() const { return _id; }
	bool hasMsg() const { return _has_msg; }
	void setMsg( bool m ) { _has_msg = m; }

	void addChild( Node & child ) {
		children.push_back( &child );
	}

    void setIt(uint n_iterations) {
        _n_iterations = n_iterations;
    }
    uint getIt() const {
        assert(_n_iterations != None);
        return _n_iterations;
    }
    // Sort children according to number of iterations needed by their subtree
    // to be visited. Order is descending.
    void sort_children() {
        std::sort(children.begin(), children.end(),
            [](const Node *lhs, const Node *rhs) {
                return lhs->getIt() > rhs->getIt();
            });
    }

	const std::vector< Node * > & getChildren() const { return children; }
private:
    // Number of iterations needed to process all children
    const uint None    = std::numeric_limits<uint>::max();
    uint _n_iterations = None;
    int _id;
	bool _has_msg = false;
	std::vector< Node * > children;
};

using ScheduleRound = std::vector< std::pair< Node *, Node * > >;
using Schedule = std::vector< ScheduleRound >;

template <typename Visitor>
void
post_order(Node *node, Visitor visitor)
{
    for (Node *child : node->getChildren()) {
        post_order(child, visitor);
    }
    visitor(node);
}

uint
extra(const std::vector<Node*>& children)
{
    for (const auto child : children) {
        std::cout << child->getIt() << " ";
    }
    std::cout << std::endl;

    uint extra  = 0;
    uint buffer = 0;
    for (size_t i = 1; i < children.size(); ++i) {
        buffer += children[i - 1]->getIt() - children[i]->getIt();
        if (buffer == 0) {
            extra += 1;
        } else {
            buffer -= 1;
        }
    }
    return extra;
}

Schedule
createSchedule( Node & root )
{
    post_order(&root, [&](Node *node) {
        node->sort_children();
        const auto& children = node->getChildren();

        if (children.empty()) {
            node->setIt(0);
        } else {
            node->setIt(children.front()->getIt() + extra(children) + 1);
        }
    });
    std::cout << root.getIt() << std::endl;
    Schedule schedule(root.getIt());

    std::function<void(Node*, uint)> create_schedule =
    [&](Node *node, uint i) {
        for (Node *child : node->getChildren()) {
            schedule.at(i).push_back({node, child});
            create_schedule(child, i + 1);
            ++i;
        }
    };

    create_schedule(&root, 0);
    return schedule;
}
