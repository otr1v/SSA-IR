#include "IR.h"
#include "dominators.h"
int main() {
    /*
    Graph graph("factorial");
    BasicBlock* entry_bb = graph.createBB("entry");
    BasicBlock* loop_header_bb = graph.createBB("loop.header");
    BasicBlock* loop_body_bb = graph.createBB("loop.body");
    BasicBlock* exit_bb = graph.createBB("exit");

    graph.setStartBlock(entry_bb);

    // entry block:
    Inst* n = graph.createInst<ParamInst>(entry_bb, 0);
    Inst* res_init = graph.createInst<ConstInst>(entry_bb, 1);
    Inst* i_init = graph.createInst<ConstInst>(entry_bb, 2);
    graph.createInst<JumpInst>(entry_bb, loop_header_bb);
    // graph.connectBBs(entry_bb, loop_header_bb);

    // loop.header block:
    PhiInst* res_phi = graph.createInst<PhiInst>(loop_header_bb);
    PhiInst* i_phi = graph.createInst<PhiInst>(loop_header_bb);

    Inst* cmp = graph.createInst<BinaryInst>(loop_header_bb, Opcode::CMP, i_phi, n);
    graph.createInst<CondJumpInst>(loop_header_bb, cmp, loop_body_bb, exit_bb);
    // graph.connectBBs(loop_header_bb, loop_body_bb);
    // graph.connectBBs(loop_header_bb, exit_bb);
    // graph.connectBBs(loop_body_bb, loop_header_bb);

    // loop.body block:
    Inst* res_new = graph.createInst<BinaryInst>(loop_body_bb, Opcode::MUL, res_phi, i_phi);
    Inst* const_1 = graph.createInst<ConstInst>(loop_body_bb, 1);
    Inst* i_new = graph.createInst<BinaryInst>(loop_body_bb, Opcode::ADD, i_phi, const_1);
    graph.createInst<JumpInst>(loop_body_bb, loop_header_bb);

    // Fill in PHI nodes
    res_phi->addIncoming(res_init, entry_bb);
    res_phi->addIncoming(res_new, loop_body_bb);
    i_phi->addIncoming(i_init, entry_bb);
    i_phi->addIncoming(i_new, loop_body_bb);
    */
    // exit block:
    Graph g("test");
    //graph.createInst<ReturnInst>(exit_bb, res_phi);
     g.setStartBlock(g.createBB("A"));
    std::map<char, BasicBlock*> blocks;
    blocks['A'] = g.getStartBlock();
    blocks['B'] = g.createBB("B");
    blocks['C'] = g.createBB("C");
    blocks['D'] = g.createBB("D");
    blocks['E'] = g.createBB("E");
    blocks['F'] = g.createBB("F");
    blocks['G'] = g.createBB("G");
    blocks['H'] = g.createBB("H");
    blocks['I'] = g.createBB("I");
    
    g.createInst<JumpInst>(blocks['A'], blocks['B']);
    auto* condB = g.createInst<ConstInst>(blocks['B'], 1);
    g.createInst<CondJumpInst>(blocks['B'], condB, blocks['C'], blocks['E']);
    g.createInst<JumpInst>(blocks['C'], blocks['D']);
    g.createInst<JumpInst>(blocks['D'], blocks['G']);
    auto* condE = g.createInst<ConstInst>(blocks['E'], 1);
    g.createInst<CondJumpInst>(blocks['E'], condE, blocks['D'], blocks['F']);
    auto* condF = g.createInst<ConstInst>(blocks['F'], 1);
    g.createInst<CondJumpInst>(blocks['F'], condF, blocks['H'], blocks['B']);
    auto* condG = g.createInst<ConstInst>(blocks['G'], 1);
    g.createInst<CondJumpInst>(blocks['G'], condG, blocks['C'], blocks['I']);
    auto* condH = g.createInst<ConstInst>(blocks['H'], 1);
    g.createInst<CondJumpInst>(blocks['H'], condH, blocks['G'], blocks['I']);
    g.createInst<ReturnInst>(blocks['I']);

    g.dump(std::cout);
    g.buildPredecessors();
    DominatorTree dom_tree(&g);
    dom_tree.run();
    dom_tree.dump(std::cout);
    return 0;
}