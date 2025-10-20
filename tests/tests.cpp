#include "gtest/gtest.h"
#include "IR.h"
#include "dominators.h"
#include <map>
#include <string>

using BlockMap = std::map<char, BasicBlock*>;

BlockMap buildNewExample1(Graph& g) {
    /*
        Structure for Example 1:
        Edges: A->B, B->C, B->F, C->D, E->D, F->G, G->D, F->E
    */
    g.setStartBlock(g.createBB("A"));
    BlockMap blocks;
    blocks['A'] = g.getStartBlock();
    blocks['B'] = g.createBB("B");
    blocks['C'] = g.createBB("C");
    blocks['D'] = g.createBB("D");
    blocks['E'] = g.createBB("E");
    blocks['F'] = g.createBB("F");
    blocks['G'] = g.createBB("G");

    g.createInst<JumpInst>(blocks['A'], blocks['B']);
    auto* condB = g.createInst<ConstInst>(blocks['B'], 1);
    g.createInst<CondJumpInst>(blocks['B'], condB, blocks['C'], blocks['F']);
    g.createInst<JumpInst>(blocks['C'], blocks['D']);
    g.createInst<JumpInst>(blocks['E'], blocks['D']);
    auto* condF = g.createInst<ConstInst>(blocks['F'], 1);
    g.createInst<CondJumpInst>(blocks['F'], condF, blocks['G'], blocks['E']);
    g.createInst<JumpInst>(blocks['G'], blocks['D']);

    return blocks;
}

BlockMap buildNewExample2(Graph& g) {
    /*
        Structure for Example 2:
        Edges: A->B, B->C, B->J, C->D, D->C, D->E, 
        E->F, F->E, F->G, G->I, H->B, I->K, G->H
    */
    g.setStartBlock(g.createBB("A"));
    BlockMap blocks;
    blocks['A'] = g.getStartBlock();
    blocks['B'] = g.createBB("B");
    blocks['C'] = g.createBB("C");
    blocks['D'] = g.createBB("D");
    blocks['E'] = g.createBB("E");
    blocks['F'] = g.createBB("F");
    blocks['G'] = g.createBB("G");
    blocks['H'] = g.createBB("H");
    blocks['I'] = g.createBB("I");
    blocks['J'] = g.createBB("J");
    blocks['K'] = g.createBB("K");

    g.createInst<JumpInst>(blocks['A'], blocks['B']);
    auto* condB = g.createInst<ConstInst>(blocks['B'], 1);
    g.createInst<CondJumpInst>(blocks['B'], condB, blocks['C'], blocks['J']);
    g.createInst<JumpInst>(blocks['C'], blocks['D']);
    auto* condD = g.createInst<ConstInst>(blocks['D'], 1);
    g.createInst<CondJumpInst>(blocks['D'], condD, blocks['C'], blocks['E']);
    g.createInst<JumpInst>(blocks['E'], blocks['F']);
    auto* condF = g.createInst<ConstInst>(blocks['F'], 1);
    g.createInst<CondJumpInst>(blocks['F'], condF, blocks['E'], blocks['G']);
    auto* condG = g.createInst<ConstInst>(blocks['G'], 1);
    g.createInst<CondJumpInst>(blocks['G'], condG, blocks['I'], blocks['H']);
    g.createInst<JumpInst>(blocks['H'], blocks['B']);
    g.createInst<JumpInst>(blocks['I'], blocks['K']);
    g.createInst<ReturnInst>(blocks['J']);
    g.createInst<ReturnInst>(blocks['K']);
    
    return blocks;
}

BlockMap buildNewExample3(Graph& g) {
    /*
        Structure for Example 3:
        Edges: A->B, B->C, B->E, C->D, G->C, D->G, 
        E->F, F->H, G->I, H->G, H->I, E->D, F->B
    */
    g.setStartBlock(g.createBB("A"));
    BlockMap blocks;
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

    return blocks;
}

// =============================================================================
// GTest Test Cases
// =============================================================================

TEST(DominatorTreeSuite, CorrectedExample1) {
    Graph g("Corrected Example 1");
    BlockMap blocks = buildNewExample1(g);
    
    g.buildPredecessors();
    DominatorTree dom_tree(&g);
    dom_tree.run();

    EXPECT_EQ(dom_tree.getImmediateDominator(blocks['A']), blocks['A']);
    EXPECT_EQ(dom_tree.getImmediateDominator(blocks['B']), blocks['A']);
    EXPECT_EQ(dom_tree.getImmediateDominator(blocks['C']), blocks['B']);
    EXPECT_EQ(dom_tree.getImmediateDominator(blocks['F']), blocks['B']);
    EXPECT_EQ(dom_tree.getImmediateDominator(blocks['E']), blocks['F']);
    EXPECT_EQ(dom_tree.getImmediateDominator(blocks['G']), blocks['F']);
    EXPECT_EQ(dom_tree.getImmediateDominator(blocks['D']), blocks['B']);
}

TEST(DominatorTreeSuite, CorrectedExample2) {
    Graph g("Corrected Example 2");
    BlockMap blocks = buildNewExample2(g);
    
    g.buildPredecessors();
    DominatorTree dom_tree(&g);
    dom_tree.run();

    EXPECT_EQ(dom_tree.getImmediateDominator(blocks['A']), blocks['A']);
    EXPECT_EQ(dom_tree.getImmediateDominator(blocks['B']), blocks['A']);
    EXPECT_EQ(dom_tree.getImmediateDominator(blocks['J']), blocks['B']);
    EXPECT_EQ(dom_tree.getImmediateDominator(blocks['C']), blocks['B']);
    EXPECT_EQ(dom_tree.getImmediateDominator(blocks['D']), blocks['C']);
    EXPECT_EQ(dom_tree.getImmediateDominator(blocks['E']), blocks['D']);
    EXPECT_EQ(dom_tree.getImmediateDominator(blocks['F']), blocks['E']);
    EXPECT_EQ(dom_tree.getImmediateDominator(blocks['G']), blocks['F']);
    EXPECT_EQ(dom_tree.getImmediateDominator(blocks['H']), blocks['G']);
    EXPECT_EQ(dom_tree.getImmediateDominator(blocks['I']), blocks['G']);
    EXPECT_EQ(dom_tree.getImmediateDominator(blocks['K']), blocks['I']);
}

TEST(DominatorTreeSuite, CorrectedExample3) {
    Graph g("Corrected Example 3");
    BlockMap blocks = buildNewExample3(g);
    
    g.buildPredecessors();
    DominatorTree dom_tree(&g);
    dom_tree.run();

    EXPECT_EQ(dom_tree.getImmediateDominator(blocks['A']), blocks['A']);
    EXPECT_EQ(dom_tree.getImmediateDominator(blocks['B']), blocks['A']);
    EXPECT_EQ(dom_tree.getImmediateDominator(blocks['E']), blocks['B']);
    EXPECT_EQ(dom_tree.getImmediateDominator(blocks['F']), blocks['E']);
    EXPECT_EQ(dom_tree.getImmediateDominator(blocks['H']), blocks['F']);
    EXPECT_EQ(dom_tree.getImmediateDominator(blocks['C']), blocks['B']);
    EXPECT_EQ(dom_tree.getImmediateDominator(blocks['D']), blocks['B']);
    EXPECT_EQ(dom_tree.getImmediateDominator(blocks['G']), blocks['B']);
    EXPECT_EQ(dom_tree.getImmediateDominator(blocks['I']), blocks['B']);
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
