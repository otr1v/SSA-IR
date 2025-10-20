#include <vector>

#include "IR.h"

BasicBlock::BasicBlock(unsigned id, const std::string& name) : id_(id), name_(name) {
}

unsigned BasicBlock::getId() const {
    return id_;
}

const std::string& BasicBlock::getName() const {
    return name_;
}

const std::vector<std::unique_ptr<Inst>>& BasicBlock::getInstructions() const {
    return instructions_;
}

void BasicBlock::addPredecessor(BasicBlock* pred) {
    predecessors_.push_back(pred);
}

const std::vector<BasicBlock*>& BasicBlock::getPredecessors() const {
    return predecessors_;
}

void BasicBlock::clearPredecessors() {
    predecessors_.clear();
}

Inst* BasicBlock::getTerminator() const {
    if (instructions_.empty()) {
        return nullptr;
    }
    return instructions_.back().get();
}

std::vector<BasicBlock*> BasicBlock::getSuccessors() const {
    Inst* terminator = getTerminator();
    if (!terminator) {
        return {};
    }

    switch (terminator->getOpcode()) {
        case Opcode::JUMP:
            return {static_cast<JumpInst*>(terminator)->getTarget()};

        case Opcode::COND_JUMP: {
            auto* condJump = static_cast<CondJumpInst*>(terminator);
            return {condJump->getTrueTarget(), condJump->getFalseTarget()};
        }

        case Opcode::RETURN:
            return {};

        default:
            return {};
    }
}

void BasicBlock::dump(std::ostream& os) const {
    os << "BB" << id_ << " (" << name_ << "):";
    if (!predecessors_.empty()) {
        os << "  ; preds = ";
        for (size_t i = 0; i < predecessors_.size(); ++i) {
            os << "%BB" << predecessors_[i]->getId() << (i == predecessors_.size() - 1 ? "" : ", ");
        }
    }
    os << std::endl;

    for (const auto& inst : instructions_) {
        os << "  ";
        inst->dump(os);
        os << std::endl;
    }
}

void BasicBlock::addInstruction(std::unique_ptr<Inst> inst) {
    instructions_.push_back(std::move(inst));
}
