#include "IR.h"

Graph::Graph(const std::string& name) : name_(name) {
}

BasicBlock* Graph::createBB(const std::string& name) {
    unsigned id = basic_blocks_.size();
    basic_blocks_.push_back(std::make_unique<BasicBlock>(id, name));
    return basic_blocks_.back().get();
}

void Graph::buildPredecessors() {
    // clear old connections
    for (const auto& bb : basic_blocks_) {
        bb->clearPredecessors();
    }

    for (const auto& bb : basic_blocks_) {
        auto successors = bb->getSuccessors();
        for (auto* succ : successors) {
            succ->addPredecessor(bb.get());
        }
    }
}

void Graph::setStartBlock(BasicBlock* bb) {
    start_block_ = bb;
}

BasicBlock* Graph::getStartBlock() const {
    return start_block_;
}

const std::vector<std::unique_ptr<BasicBlock>>& Graph::getBasicBlocks() const {
    return basic_blocks_;
}

void Graph::dump(std::ostream& os) const {
    os << "Function Graph: " << name_ << std::endl;
    os << "----------------------\n";
    for (const auto& bb : basic_blocks_) {
        bb->dump(os);
    }
    os << "----------------------\n";
}
