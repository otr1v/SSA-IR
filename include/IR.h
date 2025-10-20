#ifndef IR_H
#define IR_H

#include <algorithm>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

class BasicBlock;
class Inst;

enum class Opcode {
    // Binary operations
    ADD,
    MUL,
    CMP,
    // Terminator instructions (end a basic block)
    JUMP,
    COND_JUMP,
    RETURN,
    PHI,
    // Other
    PARAM,
    CONST,
    MOV,
    CAST,
};

class Inst {
   public:
    Inst(Opcode opcode, unsigned id) : opcode_(opcode), id_(id) {
    }

    virtual ~Inst() = default;

    Opcode getOpcode() const {
        return opcode_;
    }
    unsigned getId() const {
        return id_;
    }
    const std::vector<Inst*>& getInputs() const {
        return inputs_;
    }

    virtual void dump(std::ostream& os) const {
        os << "i" << id_ << " = " << opcodeToString(opcode_);
    }

   protected:
    static std::string opcodeToString(Opcode op) {
        switch (op) {
            case Opcode::ADD:
                return "add";
            case Opcode::MUL:
                return "mul";
            case Opcode::CMP:
                return "cmp";
            case Opcode::JUMP:
                return "jmp";
            case Opcode::COND_JUMP:
                return "cond_jump";
            case Opcode::RETURN:
                return "return";
            case Opcode::PHI:
                return "phi";
            case Opcode::PARAM:
                return "param";
            case Opcode::CONST:
                return "const";
            case Opcode::MOV:
                return "mov";
            case Opcode::CAST:
                return "cast";
            default:
                return "unknown";
        }
    }

   protected:
    Opcode opcode_;
    unsigned id_;
    std::vector<Inst*> inputs_;  // Inputs: instructions whose results we use
};

class BinaryInst : public Inst {
   public:
    BinaryInst(unsigned id, Opcode opcode, Inst* lhs, Inst* rhs) : Inst(opcode, id) {
        inputs_.push_back(lhs);
        inputs_.push_back(rhs);
    }
    void dump(std::ostream& os) const override {
        Inst::dump(os);
        os << " i" << inputs_[0]->getId() << ", i" << inputs_[1]->getId();
    }
};

class ReturnInst : public Inst {
   public:
    ReturnInst(unsigned id, Inst* value = nullptr) : Inst(Opcode::RETURN, id) {
        if (value) {
            inputs_.push_back(value);
        }
    }
    void dump(std::ostream& os) const override {
        os << "  ";
        if (!inputs_.empty()) {
            Inst::dump(os);
            os << " i" << inputs_[0]->getId();
        } else {
            os << opcodeToString(opcode_);
        }
    }
};

class JumpInst : public Inst {
   public:
    JumpInst(unsigned id, BasicBlock* target) : Inst(Opcode::JUMP, id), target_(target) {
    }

    BasicBlock* getTarget() const {
        return target_;
    }

    void dump(std::ostream& os) const override;

   private:
    unsigned getTargetId() const;
    BasicBlock* target_;
};

class CondJumpInst : public Inst {
   public:
    CondJumpInst(unsigned id, Inst* cond, BasicBlock* true_target, BasicBlock* false_target)
        : Inst(Opcode::COND_JUMP, id), true_target_(true_target), false_target_(false_target) {
        inputs_.push_back(cond);
    }

    BasicBlock* getTrueTarget() const {
        return true_target_;
    }
    BasicBlock* getFalseTarget() const {
        return false_target_;
    }

    void dump(std::ostream& os) const override;

   private:
    unsigned getTrueTargetId() const;
    unsigned getFalseTargetId() const;
    BasicBlock* true_target_;
    BasicBlock* false_target_;
};

class ConstInst : public Inst {
   public:
    ConstInst(unsigned id, int64_t value) : Inst(Opcode::CONST, id), value_(value) {
    }
    void dump(std::ostream& os) const override {
        Inst::dump(os);
        os << " " << value_;
    }

   private:
    int64_t value_;
};

class ParamInst : public Inst {
   public:
    ParamInst(unsigned id, unsigned param_index) : Inst(Opcode::PARAM, id), index_(param_index) {
    }
    void dump(std::ostream& os) const override {
        Inst::dump(os);
        os << " #" << index_;
    }

   private:
    unsigned index_;
};

class PhiInst : public Inst {
   public:
    PhiInst(unsigned id) : Inst(Opcode::PHI, id) {
    }

    void addIncoming(Inst* value, BasicBlock* pred) {
        incoming_values_.push_back({value, pred});
        inputs_.push_back(value);
    }

    const std::vector<std::pair<Inst*, BasicBlock*>>& getIncoming() const {
        return incoming_values_;
    }

    void dump(std::ostream& os) const override;

   private:
    // Stores pairs of [value, predecessor_block]
    std::vector<std::pair<Inst*, BasicBlock*>> incoming_values_;
};

unsigned getBBId(const BasicBlock* bb);

class BasicBlock final {
   public:
    BasicBlock(unsigned id, const std::string& name);

    unsigned getId() const;
    const std::string& getName() const;
    const std::vector<std::unique_ptr<Inst>>& getInstructions() const;

    void addInstruction(std::unique_ptr<Inst> inst);

    void addPredecessor(BasicBlock* pred);

    const std::vector<BasicBlock*>& getPredecessors() const;

    void clearPredecessors();

    Inst* getTerminator() const;

    std::vector<BasicBlock*> getSuccessors() const;

    void dump(std::ostream& os) const;

   private:
    unsigned id_;
    std::string name_;
    std::vector<std::unique_ptr<Inst>> instructions_;

    // Control Flow Graph connections
    std::vector<BasicBlock*> predecessors_;
};

inline unsigned getBBId(const BasicBlock* bb) {
    return bb->getId();
}

inline unsigned JumpInst::getTargetId() const {
    return target_->getId();
}

inline void JumpInst::dump(std::ostream& os) const {
    os << "  " << opcodeToString(opcode_) << " -> BB" << getTargetId();
}

inline unsigned CondJumpInst::getTrueTargetId() const {
    return true_target_->getId();
}

inline unsigned CondJumpInst::getFalseTargetId() const {
    return false_target_->getId();
}

inline void CondJumpInst::dump(std::ostream& os) const {
    os << "  " << opcodeToString(opcode_) << " i" << inputs_[0]->getId() << " -> BB"
       << getTrueTargetId() << ", BB" << getFalseTargetId();
}

inline void PhiInst::dump(std::ostream& os) const {
    Inst::dump(os);
    os << " [ ";
    for (size_t i = 0; i < incoming_values_.size(); ++i) {
        os << "[ i" << incoming_values_[i].first->getId() << ", %BB"
           << getBBId(incoming_values_[i].second) << " ]";
        if (i < incoming_values_.size() - 1) {
            os << ", ";
        }
    }
    os << " ]";
}

class Graph {
   public:
    Graph(const std::string& name);

    BasicBlock* createBB(const std::string& name = "");

    template <typename InstType, typename... Args>
    InstType* createInst(BasicBlock* bb, Args&&... args) {
        unsigned id = next_inst_id_++;
        auto inst = std::make_unique<InstType>(id, std::forward<Args>(args)...);
        auto* inst_ptr = static_cast<InstType*>(inst.get());
        bb->addInstruction(std::move(inst));
        return inst_ptr;
    }

    void buildPredecessors();

    void setStartBlock(BasicBlock* bb);

    BasicBlock* getStartBlock() const;

    const std::vector<std::unique_ptr<BasicBlock>>& getBasicBlocks() const;

    void dump(std::ostream& os) const;

   private:
    std::string name_;
    std::vector<std::unique_ptr<BasicBlock>> basic_blocks_;
    std::map<unsigned, Inst*> all_insts_;  // For quick access by ID
    BasicBlock* start_block_ = nullptr;
    unsigned next_inst_id_ = 0;
};

#endif  // IR_H