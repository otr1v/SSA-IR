#ifndef DOMINATORS_H
#define DOMINATORS_H

#include <algorithm>
#include <iostream>
#include <map>
#include <set>
#include <vector>

#include "IR.h"

class DominatorTree {
   public:
    explicit DominatorTree(Graph* g) : graph_(g) {
    }

    // Main function to run the analysis
    void run() {
        computeRPO();
        computeIDom();
        buildDomTree();
    }

    BasicBlock* getImmediateDominator(BasicBlock* bb) const {
        auto it = idom_.find(bb);
        if (it != idom_.end()) {
            return it->second;
        }
        return nullptr;
    }

    // Returns the children of a block in the dominator tree
    const std::vector<BasicBlock*>& getChildren(BasicBlock* bb) const {
        static const std::vector<BasicBlock*> empty_children;
        auto it = dom_tree_.find(bb);
        if (it != dom_tree_.end()) {
            return it->second;
        }
        return empty_children;
    }

    bool dominates(BasicBlock* A, BasicBlock* B) const {
        if (A == B) {
            return true;
        }
        BasicBlock* current = B;
        while (current != nullptr && current != graph_->getStartBlock()) {
            current = getImmediateDominator(current);
            if (current == A) {
                return true;
            }
        }
        return false;
    }

    void dump(std::ostream& os) const {
        os << "Reverse Post-Order (RPO):\n";
        for (const auto& bb : rpo_order_) {
            os << "  BB" << bb->getId() << " (" << bb->getName() << ")" << std::endl;
        }
        os << std::endl;

        os << "Dominator Tree (Child -> Parent):\n";
        for (const auto& bb : rpo_order_) {
            BasicBlock* idom = getImmediateDominator(bb);
            if (idom) {
                os << "  BB" << bb->getId() << " -> BB" << idom->getId() << "\n";
            } else {
                os << "  BB" << bb->getId() << " -> (no idom)\n";
            }
        }
        os << "\n";

        os << "Dominator Tree (Parent -> Children):\n";
        for (const auto& bb : rpo_order_) {
            os << "  BB" << bb->getId() << " dominates { ";
            const auto& children = getChildren(bb);
            for (size_t i = 0; i < children.size(); ++i) {
                os << "BB" << children[i]->getId() << (i == children.size() - 1 ? "" : ", ");
            }
            os << " }\n";
        }
    }

   private:
    void computeRPO() {
        std::set<BasicBlock*> visited;
        std::vector<BasicBlock*> post_order;
        dfsVisit(graph_->getStartBlock(), visited, post_order);

        rpo_order_.assign(post_order.rbegin(), post_order.rend());

        for (size_t i = 0; i < rpo_order_.size(); ++i) {
            rpo_map_[rpo_order_[i]] = i;
        }
    }

    void dfsVisit(BasicBlock* u, std::set<BasicBlock*>& visited,
                  std::vector<BasicBlock*>& post_order) {
        visited.insert(u);
        for (auto* v : u->getSuccessors()) {
            if (visited.find(v) == visited.end()) {
                dfsVisit(v, visited, post_order);
            }
        }
        post_order.push_back(u);
    }

    // Based on "A Simple, Fast Dominator Algorithm" by Cooper
    void computeIDom() {
        BasicBlock* start_node = graph_->getStartBlock();
        idom_[start_node] = start_node;

        bool changed = true;
        while (changed) {
            changed = false;

            for (auto* b : rpo_order_) {
                if (b == start_node) continue;

                BasicBlock* new_idom = nullptr;

                // Find the first processed predecessor in the RPO
                for (auto* p : b->getPredecessors()) {
                    if (idom_.count(p)) {
                        new_idom = p;
                        break;
                    }
                }

                // For all other predecessors, find the common dominator
                for (auto* p : b->getPredecessors()) {
                    if (p != new_idom && idom_.count(p)) {
                        new_idom = intersect(p, new_idom);
                    }
                }

                if (!idom_.count(b) || idom_[b] != new_idom) {
                    idom_[b] = new_idom;
                    changed = true;
                }
            }
        }
    }

    // Helper to find the common dominator of two blocks
    BasicBlock* intersect(BasicBlock* b1, BasicBlock* b2) {
        BasicBlock* finger1 = b1;
        BasicBlock* finger2 = b2;
        while (finger1 != finger2) {
            while (rpo_map_[finger1] < rpo_map_[finger2]) {
                finger2 = idom_[finger2];
            }
            while (rpo_map_[finger2] < rpo_map_[finger1]) {
                finger1 = idom_[finger1];
            }
        }
        return finger1;
    }

    void buildDomTree() {
        for (auto const& [node, idom] : idom_) {
            if (node != idom) {
                dom_tree_[idom].push_back(node);
            }
        }
    }

    Graph* graph_;
    std::vector<BasicBlock*> rpo_order_;
    std::map<BasicBlock*, size_t> rpo_map_;
    std::map<BasicBlock*, BasicBlock*> idom_;
    std::map<BasicBlock*, std::vector<BasicBlock*>> dom_tree_;
};

#endif  // DOMINATORS_H
