/*
  Name: sdffnodes.h
  Copyright: (c) 2007
  Author: Jan krieger <jan@jkrieger.de>, http://www.jkrieger.de/
*/

/** \file sdffnodes.h
 * \ingroup hlseqgen_sdff_nodes
 *
 */

#ifndef SDFFNODES_H
#define SDFFNODES_H

#include <string>
#include <sstream>
#include <cmath>
#include <cstdio>
#include <stdint.h>




#include "tools.h"
#include "datatable.h"
#include "sequencererror.h"
#include "channelmanager.h"
#include "sequencegenerator.h"
#include "baseimporter.h"
#include "sdfftools.h"



namespace sequencer {




/**
 * \brief a virtual base class for the memory tree nodes, representing a SDFF file
 * \ingroup hlseqgen_sdff_nodes
 *
 *
 */
class SDFFnode {
  private:
  protected:
    /** \brief hidden standard constructor */
    SDFFnode() {};
    /** \brief a parser object this node belongs to */
    SDFFparser* parser;
    /** \brief the parent node or \c NULL if this is the root of a tree */
    SDFFnode* parent;
    /** \brief specifies whether or not all child nodes are destroyd by the destructor (default = \c true ) */
    bool destroy_children;

    /** \brief copy the basic propertes of this node to \a newnode */
    virtual void copy_prop(SDFFnode* newnode, SDFFnode* newparent=NULL) {
      newnode->parent=newparent;
      newnode->parser=newparent->parser;
      newnode->destroy_children=destroy_children;
    };

    /** \brief the line where this node is in the SDFF file */
    long line;
  public:
    /** \brief class constructor. The parser is adopted from the parent node */
    SDFFnode(SDFFnode* parent);

    /** \brief class constructor for a node with no parent (root node in tree). */
    SDFFnode(SDFFparser* parser);

    /** \brief class destructor.
     *
     * This destructor may also destroy (possible) child nodes. This behaviour is controled by the property
     * \c destroy_children. So if it is set \c true the destructor will call dispose() which then destroys all
     * child node. If it is set \c false the destructor will call deconstruct() which will only unregister the
     * children.
     */
    virtual ~SDFFnode();

    /** \brief set the parent node, also adopts the parser of the new parent node */
    void set_parent(SDFFnode* parent);
    /** \brief get the parent node */
    GetMacro(SDFFnode*, parent);

    /** \brief set the parser */
    SetMacro(SDFFparser*, parser);
    /** \brief get the parser */
    GetMacro(SDFFparser*, parser);

    /** \brief set \c destroy_children property */
    SetMacro(bool, destroy_children);
    /** \brief return the state of the \c destroy_children property */
    GetMacro(bool, destroy_children);

    /** \brief return the \a line property */
    GetMacro(long, line);

    /** \brief this method evaluates the node and returns the result as a sdff_result struct */
    virtual sdff_result evaluate()=0;

    /** \brief dispose the memory of all possible child nodes, using \c delete ... call this before destruction */
    virtual void dispose() {};

    /** \brief destroy the memory tree of the (possible) child nodes, but do not destroy the objects.
     *
     * This is usefull if you stick together objects that were created on the stack. This is also the method
     * that is called by the destructor. So if you wish to destroy all childs if this node is destroyed you have
     * two possibilities:
     *   -# call dispose() before the destruction of this node
     *   -# set \c destroy_children to \c true
     * .
     */
    virtual void deconstruct() {};

    /** \brief print the node (and all sub-nodes) into a string */
    virtual std::string print(int indent=0) =0;

    /** \brief return a copy of the current node and all subnodes */
    virtual SDFFnode* copy(SDFFnode* newparent=NULL)=0;
};





/**
 * \brief a virtual base class for unary nodes
 * \ingroup hlseqgen_sdff_nodes
 */
class SDFFunaryNode: public SDFFnode {
  private:
  protected:
    /** \brief hidden standard constructor */
    SDFFunaryNode() {};
    /** \brief the child node/argument node */
    SDFFnode* child;

    /** \brief make a copy of the child and store it as the child of \a newnode */
    void copy_child(SDFFunaryNode* newnode) {
      newnode->child=child->copy(newnode);
    }
  public:
    /** \brief class constructor, also sets the parent and parser property of the child node */
    SDFFunaryNode(SDFFnode* child, SDFFnode* parent);

    /** \brief class constructor for a root node (no parent), also sets the parent and parser property of the child node */
    SDFFunaryNode(SDFFparser* parser, SDFFnode* child);

    /** \brief set the parent node. This takes the parser from the parent and sets it here and
     *         also for the child node.
     */
    virtual void set_parent(SDFFnode* parent);

    /** \brief set the child node, also sets the parent and parser property of the child node */
    virtual void set_child(SDFFnode* child);

    /** \brief return a pointer to the child node */
    GetMacro(SDFFnode*, child);

    /** \brief set the parser, also for the child node */
    virtual void set_parser(SDFFparser* parser);

    /** \copydoc SDFFnode::dispose()
     */
    virtual void dispose();

    /** \copydoc SDFFnode::deconstruct()
     */
    virtual void deconstruct();
};




/**
 * \brief a virtual base class for binary nodes
 * \ingroup hlseqgen_sdff_nodes
 */
class SDFFbinaryNode: public SDFFnode {
  private:
  protected:
    /** \brief hidden standard constructor */
    SDFFbinaryNode() {};
    /** \brief the left child node/argument node */
    SDFFnode* left;
    /** \brief the left child node/argument node */
    SDFFnode* right;

    /** \brief make a copy of the children and store them as the children of \a newnode */
    void copy_children(SDFFbinaryNode* newnode) {
      newnode->left=left->copy(newnode);
      newnode->right=right->copy(newnode);
    }
  public:
    /** \brief class constructor, also sets the parent and parser property of the child nodes */
    SDFFbinaryNode(SDFFnode* left, SDFFnode* right, SDFFnode* parent);

    /** \brief class constructor for a root node (no parent), also sets the parent and parser property of the child nodes */
    SDFFbinaryNode(SDFFparser* parser, SDFFnode* left, SDFFnode* right);

    /** \brief set the parent node. This takes the parser from the parent and sets it here and
     *         also for the child node.
     */
    virtual void set_parent(SDFFnode* parent);

    /** \brief set the left child node, also sets the parent and parser property of the child node */
    virtual void set_left(SDFFnode* child);

    /** \brief set the right child node, also sets the parent and parser property of the child node */
    virtual void set_right(SDFFnode* child);

    /** \brief return a pointer to the left child node */
    GetMacro(SDFFnode*, left);

    /** \brief return a pointer to the right child node */
    GetMacro(SDFFnode*, right);

    /** \brief set the parser, also for the child nodes */
    virtual void set_parser(SDFFparser* parser);

    /** \copydoc SDFFnode::dispose()
     */
    virtual void dispose();

    /** \copydoc SDFFnode::deconstruct()
     */
    virtual void deconstruct();
};



/**
 * \brief a virtual base class for a node with a list of child nodes
 * \ingroup hlseqgen_sdff_nodes
 */
class SDFFnaryNode: public SDFFnode {
  private:
  protected:
    /** \brief hidden standard constructor */
    SDFFnaryNode() {};
    /** \brief holds the children of this node */
    std::vector<SDFFnode*> children;

    /** \brief make a copy of the children and store them as the children of \a newnode */
    void copy_children(SDFFnaryNode* newnode) {
      newnode->children.clear();
      if (children.size()>0) {
        for (size_t i=0; i<children.size(); i++) {
          newnode->children.push_back(children[i]->copy(newnode));
        }
      }
    }
  public:
    /** \brief class constructor, also sets the parent and parser property of the child nodes */
    SDFFnaryNode(SDFFnode* parent): SDFFnode(parent) {};

    /** \brief class constructor for a root node (no parent), also sets the parent and parser property of the child nodes */
    SDFFnaryNode(SDFFparser* parser): SDFFnode(parser) {};

    /** \brief set the parent node. This takes the parser from the parent and sets it here and
     *         also for the child node.
     */
    virtual void set_parent(SDFFnode* parent);

    /** \brief add a child node, also sets the parent and parser property of the child node */
    virtual void add_child(SDFFnode* child);

    /** \brief add all the children from \a children node to the true part, also sets the parent and parser
     *         property of the child node
     */
    virtual void add_children(SDFFnaryNode* children);

    /** \brief return the number of children */
    inline virtual size_t get_child_count() {
      return children.size();
    };

    /** \brief return a pointer to the n-th node */
    inline virtual SDFFnode* get_child(size_t n) {
      if (n<children.size()) return children[n];
      return NULL;
    }

    /** \brief set the parser, also for the child nodes */
    virtual void set_parser(SDFFparser* parser);


    /** \brief destroys all child nodes of this node and then clears the list of children.
     */
    virtual void dispose();

    /** clears the list of children without destroying them
     */
    virtual void deconstruct();

    /** \brief set the n-th child to the given node and returns a pointer to the old node.
     *         If \a destroy_old\c =true then the old node will be destroyed and NULL will be returned
     */
    virtual SDFFnode* replace_child(size_t n, SDFFnode* child, bool destroy_old=false);

    /** \brief delete the n-th child to the given node and returns a pointer to the old node.
     *         If \a destroy_old\c =true then the old node will be destroyed and NULL will be returned
     */
    virtual SDFFnode* delete_child(size_t n, bool destroy_old=false);

    /** \brief delete all children. If \a destroy_old\c =true then the children will be destroyed a
     */
    virtual void clear_children(bool destroy_old=false);
};




/** \brief represents a list of nodes that are evaluated one after the other. Then the result of
 *         the last node is returned. The sub nodes are executed inside a new SDFFVariableManager block.
 * \ingroup hlseqgen_sdff_nodes
 */
class SDFFlistNode: public SDFFnaryNode {
  public:
    /** \brief class constructor, also sets the parent and parser property of the child nodes */
    SDFFlistNode(SDFFnode* parent): SDFFnaryNode(parent) {};

    /** \brief class constructor for a root node (no parent), also sets the parent and parser property of the child nodes */
    SDFFlistNode(SDFFparser* parser): SDFFnaryNode(parser) {};

    /** \brief evaluate the list of nodes in this class
     *
     * this goes through all nodes and evaluates them. The last result is returned by this method (Pseudocode):
     * \verbatim
variable_manager.enter_block();
r=result_void;
forall i in childdren
  r=children[i].evaluate();
endfor
variable_manager.leave_block();
return r;
\endverbatim
     */
    virtual sdff_result evaluate();

    /** \brief print the node (and all sub-nodes) into a string */
    virtual std::string print(int indent=0);

    /** \brief return a copy of the current node and all subnodes */
    inline virtual SDFFnode* copy(SDFFnode* newparent=NULL){
      SDFFlistNode* n=new SDFFlistNode(newparent);
      copy_prop(n, newparent);
      copy_children(n);
      return n;
    };
};





/** \brief represents a constant value of an arbitrary type (represented by sdff_result).
 * \ingroup hlseqgen_sdff_nodes
 *
 * \section test program:
 * \code
// Test memory tree
SDFFparser parser(&generator, &cmanager);

cout<<"test constant node:"<<endl;
SDFFconstantNode* n1=new SDFFconstantNode(&parser);
n1->set_string("test1"); cout<<sdff_resulttostr(n1->evaluate())<<endl;
n1->set_string("test22"); cout<<sdff_resulttostr(n1->evaluate())<<endl;
n1->set_double(1.23456e-10); cout<<sdff_resulttostr(n1->evaluate())<<endl;
n1->set_integer(-1000); cout<<sdff_resulttostr(n1->evaluate())<<endl;
n1->set_boolean(false); cout<<sdff_resulttostr(n1->evaluate())<<endl;
\endcode
 * The result should look like:
 * \verbatim
test constant node:
'test1' (string)
'test22' (string)
1.23456e-010 (double)
-1000 (integer)
false (bool)
\endverbatim
 *
 * \test a test of this class is included in \link sdffseq_test.cpp .
 */
class SDFFconstantNode: public SDFFnode {
  protected:
    sdff_result value;
  public:
    /** \brief initializing class constructor, also sets the parent and parser property of the child nodes */
    SDFFconstantNode(SDFFnode* parent, sdff_result value): SDFFnode(parent) {
      this->value=value;
    };

    /** \brief initializing class constructor for a root node (no parent), also sets the parent and parser property of the child nodes */
    SDFFconstantNode(SDFFparser* parser, sdff_result value): SDFFnode(parser) {
      this->value=value;
    };

    /** \brief class constructor, also sets the parent and parser property of the child nodes.
     *         <b>Note</b> that this constructor will not initialize the constant. use one of the
     *         \c set_...() methods to set the value.
     */
    SDFFconstantNode(SDFFnode* parent): SDFFnode(parent) {};

    /** \brief class constructor for a root node (no parent), also sets the parent and parser property of the child nodes
     *         <b>Note</b> that this constructor will not initialize the constant. use one of the
     *         \c set_...() methods to set the value.
     */
    SDFFconstantNode(SDFFparser* parser): SDFFnode(parser) {};

    /** \brief set the constant value that is represented by this node */
    SetMacro(sdff_result, value);
    /** \brief get the constant value that is represented by this node */
    GetMacro(sdff_result, value);

    /** \brief set the constant value to a string constant */
    inline void set_string(std::string data) {
      value.stringValue=data;
      value.type=sdffRTString;
    };

    /** \brief set the constant value to an integer constant */
    inline void set_integer(long long data) {
      value.intValue=data;
      value.type=sdffRTInteger;
    };

    /** \brief set the constant value to a double constant */
    inline void set_double(double data) {
      value.doubleValue=data;
      value.type=sdffRTDouble;
    };

    /** \brief set the constant value to a boolean constant */
    inline void set_boolean(bool data) {
      value.boolValue=data;
      value.type=sdffRTBoolean;
    };

    /** \brief accesses the property \c vmanager from the provided \c parser to get the value of the
     *         specified variable.
     */
    inline virtual sdff_result evaluate() {
      return value;
    };

    /** \brief print the node (and all sub-nodes) into a string */
    virtual std::string print(int indent=0) {
      return asdff_resulttostr(value);
    };

    /** \brief negates the value of an integer or double constante ... and throws an exception in all other cases */
    inline void negate() {
      switch(value.type) {
        case sdffRTDouble:
          value.doubleValue=-value.doubleValue;
          break;
        case sdffRTInteger:
          value.intValue=-value.intValue;
          break;
        default:
          SEQUENCER_ERRORN(SEQUENCER_SDFFERROR_NEGNOTNUMBER_NUM, "SDFFconstantNode.negate()");
      }
    }

    /** \brief return a copy of the current node and all subnodes */
    virtual SDFFnode* copy(SDFFnode* newparent=NULL) {
      SDFFconstantNode* r=new SDFFconstantNode(newparent);
      copy_prop(r, newparent);
      r->value=value;
      return r;
    };
};




/** \brief represents a variable. This returns the value of the variable if evaluated;
 * \ingroup hlseqgen_sdff_nodes
 *
 * \section test program:
 * \code
// Test memory tree
SDFFparser parser(&generator, &cmanager);

cout<<endl<<"test variable node:"<<endl;
parser.get_vmanager()->add_variable_string("test", "testvar contents ...");
SDFFvariableNode* n2=new SDFFvariableNode(&parser, "pi");
cout<<sdff_resulttostr(n2->evaluate())<<endl;
n2->set_varname("e"); cout<<sdff_resulttostr(n2->evaluate())<<endl;
n2->set_varname("test"); cout<<sdff_resulttostr(n2->evaluate())<<endl;
\endcode
 * The result should look like:
 * \verbatim
test variable node:
3.14159 (double)
2.71828 (double)
'testvar contents ...' (string)
\endverbatim
 *
 * \test a test of this class is included in \link sdffseq_test.cpp .
 */
class SDFFvariableNode: public SDFFnode {
  protected:
    std::string varname;
  public:
    /** \brief class constructor, also sets the parent and parser property of the child nodes */
    SDFFvariableNode(SDFFnode* parent, std::string varname): SDFFnode(parent) {
      this->varname=varname;
    };

    /** \brief class constructor for a root node (no parent), also sets the parent and parser property of the child nodes */
    SDFFvariableNode(SDFFparser* parser, std::string varname): SDFFnode(parser) {
      this->varname=varname;
    };

    /** \brief set the name of the variable represented by this node */
    SetMacro(std::string, varname);

    /** \brief return the name of the variable represented by this node */
    GetMacro(std::string, varname);

    /** \brief accesses the property \c vmanager from the provided \c parser to get the value of the
     *         specified variable.
     */
    virtual sdff_result evaluate();

    /** \brief print the node (and all sub-nodes) into a string */
    virtual std::string print(int indent=0);

    /** \brief return a copy of the current node and all subnodes */
    virtual SDFFnode* copy(SDFFnode* newparent=NULL) {
      SDFFvariableNode* r=new SDFFvariableNode(newparent, varname);
      copy_prop(r, newparent);
      r->varname=varname;
      return r;
    };
};




/** \brief represents a channel_names construct. This returns void if evaluated;
 * \ingroup hlseqgen_sdff_nodes
 *
 */
class SDFFchannelnamesNode: public SDFFnode {
  protected:
    std::vector<sdff_channel_alias_data> items;
  public:
    /** \brief class constructor, also sets the parent and parser property of the child nodes */
    SDFFchannelnamesNode(SDFFnode* parent): SDFFnode(parent) {
        items.clear();
    };

    /** \brief class constructor for a root node (no parent), also sets the parent and parser property of the child nodes */
    SDFFchannelnamesNode(SDFFparser* parser): SDFFnode(parser) {
        items.clear();
    };

    /** \brief add a new channel_names item */
    inline void add_item(sdff_channel_alias_data it) { items.push_back(it); };

    /** \brief add a new channel_names item */
    inline void add_item(std::string channel, std::string alias, std::string info=std::string("")) {
        sdff_channel_alias_data it;
        it.channel=channel;
        it.alias=alias;
        it.info=info;
        items.push_back(it);
    };


    /** \brief returns void and registers the channel aliases in the parser */
    virtual sdff_result evaluate();

    /** \brief print the node (and all sub-nodes) into a string */
    virtual std::string print(int indent=0);

    /** \brief return a copy of the current node and all subnodes */
    virtual SDFFnode* copy(SDFFnode* newparent=NULL) {
      SDFFchannelnamesNode* r=new SDFFchannelnamesNode(newparent);
      copy_prop(r, newparent);
      if (items.size()>0) for (size_t i=0; i< items.size(); i++) {
        r->add_item(items[i]);
      }
      return r;
    };
};



/** \brief represents function with its parameters. If evaluated this calls the function with the according
 *         name from the \c fmanager object in the \c parser class.
 * \ingroup hlseqgen_sdff_nodes
 *
 * \section test program:
 * \code
// Test memory tree
SDFFparser parser(&generator, &cmanager);

cout<<endl<<"test function node:"<<endl;
SDFFvariableNode* n4=new SDFFvariableNode(&parser, "pi");
SDFFfunctionNode* n3=new SDFFfunctionNode(&parser, "exp", n4);
cout<<sdff_resulttostr(n3->evaluate())<<endl;
n3->set_functionname("cos"); cout<<sdff_resulttostr(n3->evaluate())<<endl;
n3->set_functionname("sin"); cout<<sdff_resulttostr(n3->evaluate())<<endl;
n3->set_functionname("tan"); cout<<sdff_resulttostr(n3->evaluate())<<endl;
\endcode
 * The result should look like:
 * \verbatim
test function node:
23.1407 (double)
-1 (double)
1.22461e-016 (double)
-1.22461e-016 (double)
\endverbatim
 *
 * \test a test of this class is included in \link sdffseq_test.cpp .
 */
class SDFFfunctionNode: public SDFFnaryNode {
  protected:
    std::string functionname;
  public:
    /** \brief class constructor, also sets the parent and parser property of the child nodes
     *         If you supply \a parameter this node will be added as the first parameter
     */
    SDFFfunctionNode(SDFFnode* parent, std::string functionname, SDFFnode* parameter=NULL): SDFFnaryNode(parent) {
      this->parent=parent;
      if (parent!=NULL) this->parser=parent->get_parser();
      this->functionname=functionname;
      if (parameter!=NULL) {
        add_child(parameter);
      }
    };

    /** \brief class constructor for a root node (no parent), also sets the parent and parser property of the child nodes
     *         If you supply \a parameter this node will be added as the first parameter
     */
    SDFFfunctionNode(SDFFparser* parser, std::string functionname, SDFFnode* parameter=NULL): SDFFnaryNode(parser) {
      this->functionname=functionname;
      if (parameter!=NULL) add_child(parameter);
    };

    /** \brief set the name of the function represented by this node */
    SetMacro(std::string, functionname);

    /** \brief return the name of the function represented by this node */
    GetMacro(std::string, functionname);

    /** \brief accesses the property \c fmanager from the provided \c parser to call the
     *         specified function after evaluating all the children.
     */
    virtual sdff_result evaluate();

    /** \brief print the node (and all sub-nodes) into a string */
    virtual std::string print(int indent=0);


    /** \brief return a copy of the current node and all subnodes */
    inline virtual SDFFnode* copy(SDFFnode* newparent=NULL){
      SDFFfunctionNode* n=new SDFFfunctionNode(newparent, functionname);
      copy_prop(n, newparent);
      copy_children(n);
      n->functionname=functionname;
      return n;
    };
};

/** \brief types negation operations in a SDFFnegateNode object
 * \ingroup hlseqgen_sdff_nodes
 */
enum sdff_negationtypes {
  sdffNTMinus,      /*!< \brief multiply value with -1 */
  sdffNTLogic,      /*!< \brief logic negation */
  sdffNTBitwise     /*!< \brief bit-wise negation */
};


/** \brief represents the negation of a double, integer or boolean variable
 * \ingroup hlseqgen_sdff_nodes
 *
 * If \c logic_negate=false then this multiplies a number with -1. This operation is not
 * possible for void and string data.
 *
 * If \c logic_negate=true then integers and strings are negated bit-wise.
 *
 * A boolean value is always simply negated (\c true->false / \c false->true ).
 *
 *
 * \section test program:
 * \code
// Test memory tree
SDFFparser parser(&generator, &cmanager);

cout<<endl<<"test negate node"<<endl;
SDFFconstantNode* c7=new SDFFconstantNode(&parser); c7->set_boolean(true);
SDFFnegateNode* n6=new SDFFnegateNode(&parser, c7, sdffNTLogic);
c7->set_boolean(true); cout<<"!true = "<<sdff_resulttostr(n6->evaluate())<<endl;
n6->set_negationtype(sdffNTMinus);
c7->set_integer(5); cout<<"-5 = "<<sdff_resulttostr(n6->evaluate())<<endl;
c7->set_double(-5.4); cout<<"-(-5.4) = "<<sdff_resulttostr(n6->evaluate())<<endl;
n6->set_negationtype(sdffNTBitwise);
c7->set_boolean(true); cout<<"~true = "<<sdff_resulttostr(n6->evaluate())<<endl;
c7->set_integer(5); cout<<"~5 = "<<sdff_resulttostr(n6->evaluate())<<endl;
c7->set_string("how"); cout<<"~'how' = "<<sdff_resulttostr(n6->evaluate())<<endl;
cout<<"ERROR TESTS:"<<endl;
try{c7->set_string("how"); n6->set_negationtype(sdffNTMinus); cout<<"-'how': "; sdff_resulttostr(n6->evaluate());} catch (sequencer::exception E) {cout<<E.get_message()<<endl; }
try{c7->set_double(5.4); n6->set_negationtype(sdffNTBitwise); cout<<"~5.4: "; sdff_resulttostr(n6->evaluate());} catch (sequencer::exception E) {cout<<E.get_message()<<endl; }
\endcode
 * The result should look like:
 * \verbatim
test negate node
!true = false (bool)
-5 = -5 (integer)
-(-5.4) = 5.4 (double)
~true = false (bool)
~5 = -6 (integer)
~'how' = 'ùÉê' (string)
ERROR TESTS:
-'how': error 66: You can not do logic or numeric negation of a string.
   [in SDFFnegateNode.evaluate()]
~5.4: error 68: You can not do logic or bit-wise negation of a double.
   [in SDFFnegateNode.evaluate()]
\endverbatim
 *
 * \test a test of this class is included in \link sdffseq_test.cpp .
 */
class SDFFnegateNode: public SDFFunaryNode {
  protected:
    /** \brief specifies which type of negation shall be executed
      */
    sdff_negationtypes negationtype;
  public:
    /** \brief class constructor, also sets the parent and parser property of the child nodes */
    SDFFnegateNode(SDFFnode* child, SDFFnode* parent, sdff_negationtypes negtype): SDFFunaryNode(child, parent) {
      negationtype=negtype;
    };

    /** \brief class constructor for a root node (no parent), also sets the parent and parser property of the child nodes */
    SDFFnegateNode(SDFFparser* parser, SDFFnode* child, sdff_negationtypes negtype): SDFFunaryNode(parser, child) {
      negationtype=negtype;
    };

    /** \brief get \c logic_negate property */
    GetMacro(sdff_negationtypes, negationtype);
    /** \brief set \c logic_negate property */
    SetMacro(sdff_negationtypes, negationtype);

    /** \brief esecute the nagetion and throw run-time exceptions if data types are not appropriate
     */
    virtual sdff_result evaluate();

    /** \brief print the node (and all sub-nodes) into a string */
    virtual std::string print(int indent=0);

    /** \brief return a copy of the current node and all subnodes */
    virtual SDFFnode* copy(SDFFnode* newparent=NULL) {
      SDFFnegateNode* n=new SDFFnegateNode(parser, NULL, negationtype);
      copy_prop(n, newparent);
      copy_child(n);
      n->negationtype=negationtype;
      return n;
    };
};


/** \brief types arithmetic operations in a SDFFarithmeticNode
 * \ingroup hlseqgen_sdff_nodes
 */
enum sdff_arithmetic_ops {
  sdffAOAdd,           /*!< \brief add/concatenate */
  sdffAOSubtract,      /*!< \brief subtract */
  sdffAOMultiply,      /*!< \brief multiply */
  sdffAODivide,        /*!< \brief division */
  sdffAOModulo,        /*!< \brief modulo division */
  sdffAOPower          /*!< \brief power operation left^right */
};


/** \brief represents an arithmetic operation (+, -, *, /, %)
 * \ingroup hlseqgen_sdff_nodes
 *
 *   - \c sdffAOAdd will add two numbers (integer/double), concatenate two strings and is
 *     not a possible operation for booleans
 *   - \c sdffAOSubtract will subtract two numbers (integer/double)
 *   - \c sdffAOMultiply will multiply two numbers (integer/double)
 *   - \c sdffAODivide will divide two numbers (integer/double)
 *   - \c sdffAOModulo will do modulo division for two integers
 *   - \c sdffAOPower will do [left] to the power of [right]. There is some hard-coded speedup for
 *     powers of 0,1,2 and 3
 * .
 *
 * \note <tt>double [+|-|*|/] integer</tt> and <tt>integer [+|-|*|/] double</tt> will always
 *       result in a double return value!
 *
 * \section test program:
 * \code
// Test memory tree
SDFFparser parser(&generator, &cmanager);

cout<<endl<<"test arithmetic node:"<<endl;
SDFFconstantNode* c1=new SDFFconstantNode(&parser); c1->set_double(1.2);
SDFFconstantNode* c2=new SDFFconstantNode(&parser); c2->set_double(-3.5);
SDFFconstantNode* c3=new SDFFconstantNode(&parser); c3->set_integer(5);
SDFFconstantNode* c4=new SDFFconstantNode(&parser); c4->set_integer(-8);
SDFFconstantNode* c5=new SDFFconstantNode(&parser); c5->set_string("hallo");
SDFFconstantNode* c6=new SDFFconstantNode(&parser); c6->set_string(" how are you");
SDFFarithmeticNode* n5=new SDFFarithmeticNode(&parser, c1, c2, sdffAOAdd);
cout<<"1.2 [op] -3.5"<<endl;
n5->set_type(sdffAOAdd); cout<<"+:  "<<sdff_resulttostr(n5->evaluate())<<endl;
n5->set_type(sdffAOSubtract); cout<<"-:  "<<sdff_resulttostr(n5->evaluate())<<endl;
n5->set_type(sdffAOMultiply); cout<<"*:  "<<sdff_resulttostr(n5->evaluate())<<endl;
n5->set_type(sdffAODivide); cout<<"/:  "<<sdff_resulttostr(n5->evaluate())<<endl;
n5->set_left(c3); cout<<"5 [op] -3.5"<<endl;
n5->set_type(sdffAOAdd); cout<<"+:  "<<sdff_resulttostr(n5->evaluate())<<endl;
n5->set_type(sdffAOSubtract); cout<<"-:  "<<sdff_resulttostr(n5->evaluate())<<endl;
n5->set_type(sdffAOMultiply); cout<<"*:  "<<sdff_resulttostr(n5->evaluate())<<endl;
n5->set_type(sdffAODivide); cout<<"/:  "<<sdff_resulttostr(n5->evaluate())<<endl;
n5->set_left(c1); n5->set_right(c4); cout<<"1.2 [op] -8"<<endl;
n5->set_type(sdffAOAdd); cout<<"+:  "<<sdff_resulttostr(n5->evaluate())<<endl;
n5->set_type(sdffAOSubtract); cout<<"-:  "<<sdff_resulttostr(n5->evaluate())<<endl;
n5->set_type(sdffAOMultiply); cout<<"*:  "<<sdff_resulttostr(n5->evaluate())<<endl;
n5->set_type(sdffAODivide); cout<<"/:  "<<sdff_resulttostr(n5->evaluate())<<endl;
n5->set_left(c3); n5->set_right(c4); cout<<"5 [op] -8"<<endl;
n5->set_type(sdffAOAdd); cout<<"+:  "<<sdff_resulttostr(n5->evaluate())<<endl;
n5->set_type(sdffAOSubtract); cout<<"-:  "<<sdff_resulttostr(n5->evaluate())<<endl;
n5->set_type(sdffAOMultiply); cout<<"*:  "<<sdff_resulttostr(n5->evaluate())<<endl;
n5->set_type(sdffAODivide); cout<<"/:  "<<sdff_resulttostr(n5->evaluate())<<endl;
n5->set_type(sdffAOModulo); cout<<"%:  "<<sdff_resulttostr(n5->evaluate())<<endl;
n5->set_left(c5); n5->set_right(c6);
n5->set_type(sdffAOAdd); cout<<"concatenate two strings: "<<sdff_resulttostr(n5->evaluate())<<endl;
cout<<"ERROR TESTS:"<<endl;
try{n5->set_type(sdffAOSubtract); cout<<"subtract two strings: "; sdff_resulttostr(n5->evaluate());} catch (sequencer::exception E) {cout<<E.get_message()<<endl; }
try{n5->set_right(c1); n5->set_type(sdffAOSubtract); cout<<"subtract string and number: "; sdff_resulttostr(n5->evaluate());} catch (sequencer::exception E) {cout<<E.get_message()<<endl; }
\endcode
 * The result should look like:
 * \verbatim
test arithmetic node:
1.2 [op] -3.5
+:  -2.3 (double)
-:  4.7 (double)
*:  -4.2 (double)
/:  -0.342857 (double)
5 [op] -3.5
+:  1.5 (double)
-:  8.5 (double)
*:  -17.5 (double)
/:  -1.42857 (double)
1.2 [op] -8
+:  -6.8 (double)
-:  9.2 (double)
*:  -9.6 (double)
/:  -0.15 (double)
5 [op] -8
+:  -3 (integer)
-:  13 (integer)
*:  -40 (integer)
/:  0 (integer)
%:  5 (integer)
concatenate two strings: 'hallo how are you' (string)
ERROR TESTS:
subtract two strings: error 71: You can not subtract string and string.
   [in SDFFarithmeticNode.evaluate()]
subtract string and number: error 71: You can not subtract string and double.
   [in SDFFarithmeticNode.evaluate()]
\endverbatim
 *
 * \test a test of this class is included in \link sdffseq_test.cpp .
 */
class SDFFarithmeticNode: public SDFFbinaryNode {
  protected:
    /** \brief specifies which type of operation shall be executed
      */
    sdff_arithmetic_ops type;
  public:
    /** \brief class constructor, also sets the parent and parser property of the child nodes */
    SDFFarithmeticNode(SDFFnode* left, SDFFnode* right, SDFFnode* parent, sdff_arithmetic_ops type): SDFFbinaryNode(left, right, parent) {
      this->type=type;
    };

    /** \brief class constructor for a root node (no parent), also sets the parent and parser property of the child nodes */
    SDFFarithmeticNode(SDFFparser* parser, SDFFnode* left, SDFFnode* right, sdff_arithmetic_ops type): SDFFbinaryNode(parser, left, right) {
      this->type=type;
    };

    /** \brief get type of operation */
    GetMacro(sdff_arithmetic_ops, type);
    /** \brief set type of operation */
    SetMacro(sdff_arithmetic_ops, type);

    /** \brief esecute the arithemtic operation and throw run-time exceptions if
     *         data types are not appropriate
     */
    virtual sdff_result evaluate();

    /** \brief print the node (and all sub-nodes) into a string */
    virtual std::string print(int indent=0);

    /** \brief return a copy of the current node and all subnodes */
    virtual SDFFnode* copy(SDFFnode* newparent=NULL) {
      SDFFarithmeticNode* n=new SDFFarithmeticNode(parser,NULL, NULL, type);
      copy_prop(n, newparent);
      copy_children(n);
      n->type=type;
      return n;
    };

};



/** \brief types comparison operations in a SDFFcompareNode
 * \ingroup hlseqgen_sdff_nodes
 */
enum sdff_compare_ops {
  sdffCOEqual,         /*!< \brief == */
  sdffCOLess,          /*!< \brief < */
  sdffCOBigger,        /*!< \brief > */
  sdffCOLessEqual,     /*!< \brief <= */
  sdffCOBiggerEqual,   /*!< \brief >= */
  sdffCOUnequal        /*!< \brief != */
};

/** \brief represents a comparison operation (==, !=, >, <, >=, <=)
 * \ingroup hlseqgen_sdff_nodes
 *
 * You can always compare two values of equal type. Additionally you can always
 * compare two number (double or integer).
 *
 * \section test program:
 * \code
// Test memory tree
SDFFparser parser(&generator, &cmanager);

cout<<endl<<"test compare node"<<endl;
SDFFconstantNode* c8=new SDFFconstantNode(&parser); c8->set_double(1.2);
SDFFconstantNode* c9=new SDFFconstantNode(&parser); c9->set_double(-3.5);
SDFFcompareNode* n7=new SDFFcompareNode(&parser, c8, c9, sdffCOEqual);
c8->set_double(1.2); c9->set_double(-1.2); cout<<sdff_resulttostr(c8->evaluate())<<" == "<<sdff_resulttostr(c9->evaluate())<<":   "<<sdff_resulttostr(n7->evaluate())<<endl;
c8->set_double(1.2); c9->set_double(1.2); cout<<sdff_resulttostr(c8->evaluate())<<" == "<<sdff_resulttostr(c9->evaluate())<<":   "<<sdff_resulttostr(n7->evaluate())<<endl;
c8->set_integer(1); c9->set_integer(-2); cout<<sdff_resulttostr(c8->evaluate())<<" == "<<sdff_resulttostr(c9->evaluate())<<":   "<<sdff_resulttostr(n7->evaluate())<<endl;
c8->set_integer(-2); c9->set_integer(-2); cout<<sdff_resulttostr(c8->evaluate())<<" == "<<sdff_resulttostr(c9->evaluate())<<":   "<<sdff_resulttostr(n7->evaluate())<<endl;
c8->set_boolean(true); c9->set_boolean(false); cout<<sdff_resulttostr(c8->evaluate())<<" == "<<sdff_resulttostr(c9->evaluate())<<":   "<<sdff_resulttostr(n7->evaluate())<<endl;
c8->set_boolean(false); c9->set_boolean(false); cout<<sdff_resulttostr(c8->evaluate())<<" == "<<sdff_resulttostr(c9->evaluate())<<":   "<<sdff_resulttostr(n7->evaluate())<<endl;
c8->set_string("hallo"); c9->set_string("hallo1"); cout<<sdff_resulttostr(c8->evaluate())<<" == "<<sdff_resulttostr(c9->evaluate())<<":   "<<sdff_resulttostr(n7->evaluate())<<endl;
c8->set_string("hallo"); c9->set_string("hallo"); cout<<sdff_resulttostr(c8->evaluate())<<" == "<<sdff_resulttostr(c9->evaluate())<<":   "<<sdff_resulttostr(n7->evaluate())<<endl;
n7->set_type(sdffCOUnequal);
c8->set_double(1.2); c9->set_double(-1.2); cout<<sdff_resulttostr(c8->evaluate())<<" != "<<sdff_resulttostr(c9->evaluate())<<":   "<<sdff_resulttostr(n7->evaluate())<<endl;
c8->set_double(1.2); c9->set_double(1.2); cout<<sdff_resulttostr(c8->evaluate())<<" != "<<sdff_resulttostr(c9->evaluate())<<":   "<<sdff_resulttostr(n7->evaluate())<<endl;
c8->set_integer(1); c9->set_integer(-2); cout<<sdff_resulttostr(c8->evaluate())<<" != "<<sdff_resulttostr(c9->evaluate())<<":   "<<sdff_resulttostr(n7->evaluate())<<endl;
c8->set_integer(-2); c9->set_integer(-2); cout<<sdff_resulttostr(c8->evaluate())<<" != "<<sdff_resulttostr(c9->evaluate())<<":   "<<sdff_resulttostr(n7->evaluate())<<endl;
c8->set_boolean(true); c9->set_boolean(false); cout<<sdff_resulttostr(c8->evaluate())<<" != "<<sdff_resulttostr(c9->evaluate())<<":   "<<sdff_resulttostr(n7->evaluate())<<endl;
c8->set_boolean(false); c9->set_boolean(false); cout<<sdff_resulttostr(c8->evaluate())<<" != "<<sdff_resulttostr(c9->evaluate())<<":   "<<sdff_resulttostr(n7->evaluate())<<endl;
c8->set_string("hallo"); c9->set_string("hallo1"); cout<<sdff_resulttostr(c8->evaluate())<<" != "<<sdff_resulttostr(c9->evaluate())<<":   "<<sdff_resulttostr(n7->evaluate())<<endl;
c8->set_string("hallo"); c9->set_string("hallo"); cout<<sdff_resulttostr(c8->evaluate())<<" != "<<sdff_resulttostr(c9->evaluate())<<":   "<<sdff_resulttostr(n7->evaluate())<<endl;
n7->set_type(sdffCOLess);
c8->set_double(1.2); c9->set_double(-1.2); cout<<sdff_resulttostr(c8->evaluate())<<" < "<<sdff_resulttostr(c9->evaluate())<<":   "<<sdff_resulttostr(n7->evaluate())<<endl;
c8->set_double(1.2); c9->set_double(1.2); cout<<sdff_resulttostr(c8->evaluate())<<" < "<<sdff_resulttostr(c9->evaluate())<<":   "<<sdff_resulttostr(n7->evaluate())<<endl;
c8->set_integer(1); c9->set_integer(-2); cout<<sdff_resulttostr(c8->evaluate())<<" < "<<sdff_resulttostr(c9->evaluate())<<":   "<<sdff_resulttostr(n7->evaluate())<<endl;
c8->set_integer(-2); c9->set_integer(-2); cout<<sdff_resulttostr(c8->evaluate())<<" < "<<sdff_resulttostr(c9->evaluate())<<":   "<<sdff_resulttostr(n7->evaluate())<<endl;
c8->set_string("hallo"); c9->set_string("hallo1"); cout<<sdff_resulttostr(c8->evaluate())<<" < "<<sdff_resulttostr(c9->evaluate())<<":   "<<sdff_resulttostr(n7->evaluate())<<endl;
c8->set_string("hallo"); c9->set_string("hallo"); cout<<sdff_resulttostr(c8->evaluate())<<" < "<<sdff_resulttostr(c9->evaluate())<<":   "<<sdff_resulttostr(n7->evaluate())<<endl;
n7->set_type(sdffCOBigger);
c8->set_double(1.2); c9->set_double(-1.2); cout<<sdff_resulttostr(c8->evaluate())<<" > "<<sdff_resulttostr(c9->evaluate())<<":   "<<sdff_resulttostr(n7->evaluate())<<endl;
c8->set_double(1.2); c9->set_double(1.2); cout<<sdff_resulttostr(c8->evaluate())<<" > "<<sdff_resulttostr(c9->evaluate())<<":   "<<sdff_resulttostr(n7->evaluate())<<endl;
c8->set_integer(1); c9->set_integer(-2); cout<<sdff_resulttostr(c8->evaluate())<<" > "<<sdff_resulttostr(c9->evaluate())<<":   "<<sdff_resulttostr(n7->evaluate())<<endl;
c8->set_integer(-2); c9->set_integer(-2); cout<<sdff_resulttostr(c8->evaluate())<<" > "<<sdff_resulttostr(c9->evaluate())<<":   "<<sdff_resulttostr(n7->evaluate())<<endl;
c8->set_string("hallo"); c9->set_string("hallo1"); cout<<sdff_resulttostr(c8->evaluate())<<" > "<<sdff_resulttostr(c9->evaluate())<<":   "<<sdff_resulttostr(n7->evaluate())<<endl;
c8->set_string("hallo"); c9->set_string("hallo"); cout<<sdff_resulttostr(c8->evaluate())<<" > "<<sdff_resulttostr(c9->evaluate())<<":   "<<sdff_resulttostr(n7->evaluate())<<endl;
n7->set_type(sdffCOLessEqual);
c8->set_double(1.2); c9->set_double(-1.2); cout<<sdff_resulttostr(c8->evaluate())<<" <= "<<sdff_resulttostr(c9->evaluate())<<":   "<<sdff_resulttostr(n7->evaluate())<<endl;
c8->set_double(1.2); c9->set_double(1.2); cout<<sdff_resulttostr(c8->evaluate())<<" <= "<<sdff_resulttostr(c9->evaluate())<<":   "<<sdff_resulttostr(n7->evaluate())<<endl;
c8->set_integer(1); c9->set_integer(-2); cout<<sdff_resulttostr(c8->evaluate())<<" <= "<<sdff_resulttostr(c9->evaluate())<<":   "<<sdff_resulttostr(n7->evaluate())<<endl;
c8->set_integer(-2); c9->set_integer(-2); cout<<sdff_resulttostr(c8->evaluate())<<" <= "<<sdff_resulttostr(c9->evaluate())<<":   "<<sdff_resulttostr(n7->evaluate())<<endl;
c8->set_string("hallo"); c9->set_string("hallo1"); cout<<sdff_resulttostr(c8->evaluate())<<" <= "<<sdff_resulttostr(c9->evaluate())<<":   "<<sdff_resulttostr(n7->evaluate())<<endl;
c8->set_string("hallo"); c9->set_string("hallo"); cout<<sdff_resulttostr(c8->evaluate())<<" <= "<<sdff_resulttostr(c9->evaluate())<<":   "<<sdff_resulttostr(n7->evaluate())<<endl;
n7->set_type(sdffCOBiggerEqual);
c8->set_double(1.2); c9->set_double(-1.2); cout<<sdff_resulttostr(c8->evaluate())<<" >= "<<sdff_resulttostr(c9->evaluate())<<":   "<<sdff_resulttostr(n7->evaluate())<<endl;
c8->set_double(1.2); c9->set_double(1.2); cout<<sdff_resulttostr(c8->evaluate())<<" >= "<<sdff_resulttostr(c9->evaluate())<<":   "<<sdff_resulttostr(n7->evaluate())<<endl;
c8->set_integer(1); c9->set_integer(-2); cout<<sdff_resulttostr(c8->evaluate())<<" >= "<<sdff_resulttostr(c9->evaluate())<<":   "<<sdff_resulttostr(n7->evaluate())<<endl;
c8->set_integer(-2); c9->set_integer(-2); cout<<sdff_resulttostr(c8->evaluate())<<" >= "<<sdff_resulttostr(c9->evaluate())<<":   "<<sdff_resulttostr(n7->evaluate())<<endl;
c8->set_string("hallo"); c9->set_string("hallo1"); cout<<sdff_resulttostr(c8->evaluate())<<" >= "<<sdff_resulttostr(c9->evaluate())<<":   "<<sdff_resulttostr(n7->evaluate())<<endl;
c8->set_string("hallo"); c9->set_string("hallo"); cout<<sdff_resulttostr(c8->evaluate())<<" >= "<<sdff_resulttostr(c9->evaluate())<<":   "<<sdff_resulttostr(n7->evaluate())<<endl;
\endcode
 * The result should look like:
 * \verbatim
test compare node
1.2 (double) == -1.2 (double):   false (bool)
1.2 (double) == 1.2 (double):   true (bool)
1 (integer) == -2 (integer):   false (bool)
-2 (integer) == -2 (integer):   true (bool)
true (bool) == false (bool):   false (bool)
false (bool) == false (bool):   true (bool)
'hallo' (string) == 'hallo1' (string):   false (bool)
'hallo' (string) == 'hallo' (string):   true (bool)
1.2 (double) != -1.2 (double):   true (bool)
1.2 (double) != 1.2 (double):   false (bool)
1 (integer) != -2 (integer):   true (bool)
-2 (integer) != -2 (integer):   false (bool)
true (bool) != false (bool):   true (bool)
false (bool) != false (bool):   false (bool)
'hallo' (string) != 'hallo1' (string):   true (bool)
'hallo' (string) != 'hallo' (string):   false (bool)
1.2 (double) < -1.2 (double):   false (bool)
1.2 (double) < 1.2 (double):   false (bool)
1 (integer) < -2 (integer):   false (bool)
-2 (integer) < -2 (integer):   false (bool)
'hallo' (string) < 'hallo1' (string):   true (bool)
'hallo' (string) < 'hallo' (string):   false (bool)
1.2 (double) > -1.2 (double):   true (bool)
1.2 (double) > 1.2 (double):   false (bool)
1 (integer) > -2 (integer):   true (bool)
-2 (integer) > -2 (integer):   false (bool)
'hallo' (string) > 'hallo1' (string):   false (bool)
'hallo' (string) > 'hallo' (string):   false (bool)
1.2 (double) <= -1.2 (double):   false (bool)
1.2 (double) <= 1.2 (double):   true (bool)
1 (integer) <= -2 (integer):   false (bool)
-2 (integer) <= -2 (integer):   true (bool)
'hallo' (string) <= 'hallo1' (string):   true (bool)
'hallo' (string) <= 'hallo' (string):   true (bool)
1.2 (double) >= -1.2 (double):   true (bool)
1.2 (double) >= 1.2 (double):   true (bool)
1 (integer) >= -2 (integer):   true (bool)
-2 (integer) >= -2 (integer):   true (bool)
'hallo' (string) >= 'hallo1' (string):   false (bool)
'hallo' (string) >= 'hallo' (string):   true (bool)
\endverbatim
 *
 * \test a test of this class is included in \link sdffseq_test.cpp .
 */
class SDFFcompareNode: public SDFFbinaryNode {
  protected:
    /** \brief specifies which type of operation shall be executed
      */
    sdff_compare_ops type;
  public:
    /** \brief class constructor, also sets the parent and parser property of the child nodes */
    SDFFcompareNode(SDFFnode* left, SDFFnode* right, SDFFnode* parent, sdff_compare_ops type): SDFFbinaryNode(left, right, parent) {
      this->type=type;
    };

    /** \brief class constructor for a root node (no parent), also sets the parent and parser property of the child nodes */
    SDFFcompareNode(SDFFparser* parser, SDFFnode* left, SDFFnode* right, sdff_compare_ops type): SDFFbinaryNode(parser, left, right) {
      this->type=type;
    };

    /** \brief get type of operation */
    GetMacro(sdff_compare_ops, type);
    /** \brief set type of operation */
    SetMacro(sdff_compare_ops, type);

    /** \brief esecute the comparison and throw run-time exceptions if data types
     *         are not appropriate
     */
    virtual sdff_result evaluate();

    /** \brief print the node (and all sub-nodes) into a string */
    virtual std::string print(int indent=0);

    /** \brief return a copy of the current node and all subnodes */
    virtual SDFFnode* copy(SDFFnode* newparent=NULL) {
      SDFFcompareNode* n=new SDFFcompareNode(parser, NULL, NULL, type);
      copy_prop(n, newparent);
      copy_children(n);
      n->type=type;
      return n;
    };
};







/** \brief types arithmetic operations in a SDFFarithmeticNode
 * \ingroup hlseqgen_sdff_nodes
 */
enum sdff_logic_ops {
  sdffLOAND,           /*!< \brief and operation */
  sdffLOOR,            /*!< \brief or operation */
  sdffLOXOR,           /*!< \brief exclusive or (xor) operation */
  sdffLONOR,           /*!< \brief negatet or (nor) operation */
  sdffLONAND,          /*!< \brief negatet and (nand) operation */
  sdffLOSHR,           /*!< \brief shift right operation */
  sdffLOSHL            /*!< \brief shift left operation */
};


/** \brief represents a logic (also bit-wise) operation (and or xor, nand, nor, shr, shl)
 * \ingroup hlseqgen_sdff_nodes
 *
 * If these operations are applied to two boolean values, the corresponding logical operation is
 * executed. If they are applied to two integers or two strings (of equal length) the operation
 * is executed bit-wise.
 *
 * \section test program:
 * \code
// Test memory tree
SDFFparser parser(&generator, &cmanager);

cout<<endl<<"test logic node:"<<endl;
SDFFconstantNode* c10=new SDFFconstantNode(&parser); c10->set_boolean(true);
SDFFconstantNode* c11=new SDFFconstantNode(&parser); c11->set_boolean(false);
SDFFlogicNode* n8=new SDFFlogicNode(&parser, c10, c11, sdffLOAND);
n8->set_type(sdffLOAND);
c10->set_boolean(false); c11->set_boolean(false); cout<<sdff_resulttostr(c10->evaluate())<<" AND "<<sdff_resulttostr(c11->evaluate())<<"  \t  =   "<<sdff_resulttostr(n8->evaluate())<<endl;
c10->set_boolean(true); c11->set_boolean(false); cout<<sdff_resulttostr(c10->evaluate())<<" AND "<<sdff_resulttostr(c11->evaluate())<<"  \t  =   "<<sdff_resulttostr(n8->evaluate())<<endl;
c10->set_boolean(false); c11->set_boolean(true); cout<<sdff_resulttostr(c10->evaluate())<<" AND "<<sdff_resulttostr(c11->evaluate())<<"  \t  =   "<<sdff_resulttostr(n8->evaluate())<<endl;
c10->set_boolean(true); c11->set_boolean(true); cout<<sdff_resulttostr(c10->evaluate())<<" AND "<<sdff_resulttostr(c11->evaluate())<<"  \t  =   "<<sdff_resulttostr(n8->evaluate())<<endl;
c10->set_integer(0x0F); c11->set_integer(0x05); cout<<sdff_resulttostr(c10->evaluate())<<" AND "<<sdff_resulttostr(c11->evaluate())<<"  \t  =   "<<sdff_resulttostr(n8->evaluate())<<endl;
n8->set_type(sdffLOOR);
c10->set_boolean(false); c11->set_boolean(false); cout<<sdff_resulttostr(c10->evaluate())<<" OR "<<sdff_resulttostr(c11->evaluate())<<"  \t  =   "<<sdff_resulttostr(n8->evaluate())<<endl;
c10->set_boolean(true); c11->set_boolean(false); cout<<sdff_resulttostr(c10->evaluate())<<" OR "<<sdff_resulttostr(c11->evaluate())<<"  \t  =   "<<sdff_resulttostr(n8->evaluate())<<endl;
c10->set_boolean(false); c11->set_boolean(true); cout<<sdff_resulttostr(c10->evaluate())<<" OR "<<sdff_resulttostr(c11->evaluate())<<"  \t  =   "<<sdff_resulttostr(n8->evaluate())<<endl;
c10->set_boolean(true); c11->set_boolean(true); cout<<sdff_resulttostr(c10->evaluate())<<" OR "<<sdff_resulttostr(c11->evaluate())<<"  \t  =   "<<sdff_resulttostr(n8->evaluate())<<endl;
c10->set_integer(0x0F); c11->set_integer(0x05); cout<<sdff_resulttostr(c10->evaluate())<<" OR "<<sdff_resulttostr(c11->evaluate())<<"  \t  =   "<<sdff_resulttostr(n8->evaluate())<<endl;
n8->set_type(sdffLONOR);
c10->set_boolean(false); c11->set_boolean(false); cout<<sdff_resulttostr(c10->evaluate())<<" NOR "<<sdff_resulttostr(c11->evaluate())<<"  \t  =   "<<sdff_resulttostr(n8->evaluate())<<endl;
c10->set_boolean(true); c11->set_boolean(false); cout<<sdff_resulttostr(c10->evaluate())<<" NOR "<<sdff_resulttostr(c11->evaluate())<<"  \t  =   "<<sdff_resulttostr(n8->evaluate())<<endl;
c10->set_boolean(false); c11->set_boolean(true); cout<<sdff_resulttostr(c10->evaluate())<<" NOR "<<sdff_resulttostr(c11->evaluate())<<"  \t  =   "<<sdff_resulttostr(n8->evaluate())<<endl;
c10->set_boolean(true); c11->set_boolean(true); cout<<sdff_resulttostr(c10->evaluate())<<" NOR "<<sdff_resulttostr(c11->evaluate())<<"  \t  =   "<<sdff_resulttostr(n8->evaluate())<<endl;
c10->set_integer(0x0F); c11->set_integer(0x05); cout<<sdff_resulttostr(c10->evaluate())<<" NOR "<<sdff_resulttostr(c11->evaluate())<<"  \t  =   "<<sdff_resulttostr(n8->evaluate())<<endl;
n8->set_type(sdffLOXOR);
c10->set_boolean(false); c11->set_boolean(false); cout<<sdff_resulttostr(c10->evaluate())<<" XOR "<<sdff_resulttostr(c11->evaluate())<<"  \t  =   "<<sdff_resulttostr(n8->evaluate())<<endl;
c10->set_boolean(true); c11->set_boolean(false); cout<<sdff_resulttostr(c10->evaluate())<<" XOR "<<sdff_resulttostr(c11->evaluate())<<"  \t  =   "<<sdff_resulttostr(n8->evaluate())<<endl;
c10->set_boolean(false); c11->set_boolean(true); cout<<sdff_resulttostr(c10->evaluate())<<" XOR "<<sdff_resulttostr(c11->evaluate())<<"  \t  =   "<<sdff_resulttostr(n8->evaluate())<<endl;
c10->set_boolean(true); c11->set_boolean(true); cout<<sdff_resulttostr(c10->evaluate())<<" XOR "<<sdff_resulttostr(c11->evaluate())<<"  \t  =   "<<sdff_resulttostr(n8->evaluate())<<endl;
c10->set_integer(0x0F); c11->set_integer(0x05); cout<<sdff_resulttostr(c10->evaluate())<<" XOR "<<sdff_resulttostr(c11->evaluate())<<"  \t  =   "<<sdff_resulttostr(n8->evaluate())<<endl;
n8->set_type(sdffLONAND);
c10->set_boolean(false); c11->set_boolean(false); cout<<sdff_resulttostr(c10->evaluate())<<" NAND "<<sdff_resulttostr(c11->evaluate())<<"  \t  =   "<<sdff_resulttostr(n8->evaluate())<<endl;
c10->set_boolean(true); c11->set_boolean(false); cout<<sdff_resulttostr(c10->evaluate())<<" NAND "<<sdff_resulttostr(c11->evaluate())<<"  \t  =   "<<sdff_resulttostr(n8->evaluate())<<endl;
c10->set_boolean(false); c11->set_boolean(true); cout<<sdff_resulttostr(c10->evaluate())<<" NAND "<<sdff_resulttostr(c11->evaluate())<<"  \t  =   "<<sdff_resulttostr(n8->evaluate())<<endl;
c10->set_boolean(true); c11->set_boolean(true); cout<<sdff_resulttostr(c10->evaluate())<<" NAND "<<sdff_resulttostr(c11->evaluate())<<"  \t  =   "<<sdff_resulttostr(n8->evaluate())<<endl;
c10->set_integer(0x0F); c11->set_integer(0x05); cout<<sdff_resulttostr(c10->evaluate())<<" NAND "<<sdff_resulttostr(c11->evaluate())<<"  \t  =   "<<sdff_resulttostr(n8->evaluate())<<endl;
n8->set_type(sdffLOXOR);
c10->set_string("message"); c11->set_string("12endg&"); cout<<sdff_resulttostr(c10->evaluate())<<" XOR "<<sdff_resulttostr(c11->evaluate())<<"  \t  =   "<<sdff_resulttostr(n8->evaluate())<<endl;
c10->set_string(n8->evaluate().stringValue); c11->set_string("12endg&"); cout<<sdff_resulttostr(c10->evaluate())<<" XOR "<<sdff_resulttostr(c11->evaluate())<<"  \t  =   "<<sdff_resulttostr(n8->evaluate())<<endl;
\endcode
 * The result should look like:
 * \verbatim
test logic node:
false (bool) AND false (bool)     =   false (bool)
true (bool) AND false (bool)      =   false (bool)
false (bool) AND true (bool)      =   false (bool)
true (bool) AND true (bool)       =   true (bool)
15 (integer) AND 5 (integer)      =   5 (integer)
false (bool) OR false (bool)      =   false (bool)
true (bool) OR false (bool)       =   true (bool)
false (bool) OR true (bool)       =   true (bool)
true (bool) OR true (bool)        =   true (bool)
15 (integer) OR 5 (integer)       =   15 (integer)
false (bool) NOR false (bool)     =   true (bool)
true (bool) NOR false (bool)      =   false (bool)
false (bool) NOR true (bool)      =   false (bool)
true (bool) NOR true (bool)       =   false (bool)
15 (integer) NOR 5 (integer)      =   -16 (integer)
false (bool) XOR false (bool)     =   false (bool)
true (bool) XOR false (bool)      =   true (bool)
false (bool) XOR true (bool)      =   true (bool)
true (bool) XOR true (bool)       =   false (bool)
15 (integer) XOR 5 (integer)      =   10 (integer)
false (bool) NAND false (bool)            =   true (bool)
true (bool) NAND false (bool)     =   true (bool)
false (bool) NAND true (bool)     =   true (bool)
true (bool) NAND true (bool)      =   false (bool)
15 (integer) NAND 5 (integer)     =   -6 (integer)
'message' (string) XOR '12endg&' (string)         =   '\W??? C' (string)
'\W??? C' (string) XOR '12endg&' (string)         =   'message' (string)
\endverbatim
 *
 * \test a test of this class is included in \link sdffseq_test.cpp .
 */
class SDFFlogicNode: public SDFFbinaryNode {
  protected:
    /** \brief specifies which type of operation shall be executed
      */
    sdff_logic_ops type;
  public:
    /** \brief class constructor, also sets the parent and parser property of the child nodes */
    SDFFlogicNode(SDFFnode* left, SDFFnode* right, SDFFnode* parent, sdff_logic_ops type): SDFFbinaryNode(left, right, parent) {
      this->type=type;
    };

    /** \brief class constructor for a root node (no parent), also sets the parent and parser property of the child nodes */
    SDFFlogicNode(SDFFparser* parser, SDFFnode* left, SDFFnode* right, sdff_logic_ops type): SDFFbinaryNode(parser, left, right) {
      this->type=type;
    };

    /** \brief get type of operation */
    GetMacro(sdff_logic_ops, type);
    /** \brief set type of operation */
    SetMacro(sdff_logic_ops, type);

    /** \brief esecute the arithemtic operation and throw run-time exceptions if
     *         data types are not appropriate
     */
    virtual sdff_result evaluate();

    /** \brief print the node (and all sub-nodes) into a string */
    virtual std::string print(int indent=0);

    /** \brief return a copy of the current node and all subnodes */
    virtual SDFFnode* copy(SDFFnode* newparent=NULL) {
      SDFFlogicNode* n=new SDFFlogicNode(parser, NULL, NULL, type);
      copy_prop(n, newparent);
      copy_children(n);
      n->type=type;
      return n;
    };
};





/** \brief represents a variable assignment
 * \ingroup hlseqgen_sdff_nodes
 *
 * If \c constant=true then the variable will be constant after the assign. If the variable is already
 * constant we can not assign a value. If the variable does already exist in the current or a lower variable cope
 * its value will be set. If it does not exist or \c create==true the variable will be created inside the
 * current scope. See SDFFVariableManager class for details on finding a variable. All variables are generated as
 * internally managed variables, i.e. the dedicated memory will be manager by the variable manager.
 *
 * If the variable name is a valid channel name (tested against the channelManager inside the parser object)
 * the evaluate() method generates a SDFFSEset sequence element and adds it to the sequence element manager
 * of parser. So this corresponds to a set value operation on the channel. If an optional parameter in brackes []
 * is provided this is interpreted as absolute (in channel- or outside block) or a relative (inside event- or
 * reltive_event-block) time. This means that the statements <tt>AO1[-5u]=2.34;</tt> and <tt>set("AO1", 2.34, -5u)</tt>
 * are equal, where set is the standard channel set method registered to SDFFparser.
 *
 *
 * \section test program:
 * \code
// Test memory tree
SDFFparser parser(&generator, &cmanager);

cout<<endl<<"test variable assign node:"<<endl;
SDFFconstantNode* c12=new SDFFconstantNode(&parser); c11->set_string("variable value");
SDFFvariableassignNode* n9=new SDFFvariableassignNode(&parser, "testvar", c12);
try{c12->set_string("variable value"); n9->set_variable_name("testvar"); n9->evaluate(); cout<<"testvar= "<<sdff_resulttostr(parser.get_vmanager()->get("testvar"))<<endl; } catch (sequencer::exception E) {cout<<E.get_message()<<endl; }
try{c12->set_double(1.2345); n9->set_variable_name("testvar"); n9->set_constant(true); n9->evaluate(); cout<<"testvar= "<<sdff_resulttostr(parser.get_vmanager()->get("testvar"))<<endl; } catch (sequencer::exception E) {cout<<E.get_message()<<endl; }
try{c12->set_double(2345); n9->set_variable_name("testvar"); n9->evaluate(); cout<<"testvar= "<<sdff_resulttostr(parser.get_vmanager()->get("testvar"))<<endl; } catch (sequencer::exception E) {cout<<E.get_message()<<endl; }
\endcode
 * The result should look like:
 * \verbatim
test variable assign node:
testvar= 'variable value' (string)
testvar= 1.2345 (double)
testvar= 1.2345 (double)
\endverbatim
 *
 * \test a test of this class is included in \link sdffseq_test.cpp .
 */
class SDFFvariableassignNode: public SDFFunaryNode {
  protected:
    /** \brief specifies the name of the variable to assign to */
    std::string variable_name;
    /** \brief specifies whether the variable should be set constant */
    bool constant;
    /** \brief If this is set \c true there will be a new variable in the current block in ANY case. If this
     *         is set \c false a new variable will be created only if no variable with the given name exists.
     */
    bool create;
    /** \brief this represents an optional parameter, written in [ ] after the variable name */
    SDFFnode* parameter;
  public:
    /** \brief class constructor, also sets the parent and parser property of the child nodes */
    SDFFvariableassignNode(std::string varname, SDFFnode* child, SDFFnode* parent): SDFFunaryNode(child, parent) {
      variable_name=varname;
      constant=false;
      create=false;
      parameter=NULL;
    };

    /** \brief class constructor for a root node (no parent), also sets the parent and parser property of the child nodes */
    SDFFvariableassignNode(SDFFparser* parser, std::string varname, SDFFnode* child): SDFFunaryNode(parser, child) {
      variable_name=varname;
      constant=false;
      create=false;
      parameter=NULL;
    };

    /** \brief get \c variable_name property */
    GetMacro(std::string, variable_name);
    /** \brief set \c variable_name property */
    SetMacro(std::string, variable_name);

    /** \brief get the \a parameter node */
    GetMacro(SDFFnode*, parameter);
    /** \brief set the \a parameter node */
    SetMacro(SDFFnode*, parameter);

    /** \brief get \c constant property */
    GetMacro(bool, constant);
    /** \brief set \c constant property */
    SetMacro(bool, constant);

    /** \brief get \c create property */
    GetMacro(bool, create);
    /** \brief set \c create property */
    SetMacro(bool, create);

    /** \brief esecute the variable assignment and throw a run-time exception if an error occurs
     */
    virtual sdff_result evaluate();

    /** \brief print the node (and all sub-nodes) into a string */
    virtual std::string print(int indent=0);

    /** \brief return a copy of the current node and all subnodes */
    virtual SDFFnode* copy(SDFFnode* newparent=NULL) {
      SDFFvariableassignNode* n=new SDFFvariableassignNode(variable_name, NULL, newparent);
      copy_prop(n, newparent);
      copy_child(n);
      n->variable_name=variable_name;
      n->constant=constant;
      n->create=create;
      n->parameter=parameter->copy(n);
      return n;
    };
};






/** \brief represents a function definition
 * \ingroup hlseqgen_sdff_nodes
 *
 * if \c list==true then we expect a SDFFlistNode as child and have to add a local variable 'result'
 * (initialized as void) which will be returned by the function. Thsi corresponds to a procedure that
 * contains a list of instructions.
 *
 * \section test program:
 * \code
// Test memory tree
SDFFparser parser(&generator, &cmanager);

cout<<endl<<"test function assign node:"<<endl;
SDFFconstantNode* c13=new SDFFconstantNode(&parser); c13->set_integer(2);
SDFFvariableNode* c14=new SDFFvariableNode(&parser, "x");
SDFFconstantNode* c15=new SDFFconstantNode(&parser); c15->set_double(5);
SDFFarithmeticNode* n10=new SDFFarithmeticNode(&parser, c14, c13, sdffAOPower);
SDFFfunctionassignNode* n11=new SDFFfunctionassignNode(&parser, "sqr", n10);
SDFFfunctionNode* n12=new SDFFfunctionNode(&parser, "sqr", c15);
n11->add_parameter("x");
n11->evaluate();
std::cout<<"sqr(5) = "<<sdff_resulttostr(n12->evaluate())<<endl;
c15->set_integer(25);
std::cout<<"sqr(25) = "<<sdff_resulttostr(n12->evaluate())<<endl;
cout<<"ERROR TESTS:"<<endl;
c15->set_string("bla");
try{std::cout<<"sqr('bla') = "<<sdff_resulttostr(n12->evaluate())<<endl;} catch (sequencer::exception E) {cout<<E.get_message()<<endl; }
\endcode
 * The result should look like:
 * \verbatim
test function assign node:
sqr(5) = 25 (double)
sqr(25) = 625 (integer)
ERROR TESTS:
error 71: You can not exponentiate string and integer.
   [in SDFFarithmeticNode.evaluate()]
\endverbatim
 *
 * \test a test of this class is included in \link sdffseq_test.cpp .
 */
class SDFFfunctionassignNode: public SDFFunaryNode {
  protected:
    /** \brief specifies the name of the function to be created/defined */
    std::string function_name;
    /** \brief specifies the names of the parameters of the function */
    std::vector<std::string> parameters;
  public:
    /** \brief class constructor, also sets the parent and parser property of the child nodes */
    SDFFfunctionassignNode(std::string fname, SDFFnode* child, SDFFnode* parent): SDFFunaryNode(child, parent) {
      function_name=fname;
      parameters.clear();
    };

    /** \brief class constructor for a root node (no parent), also sets the parent and parser property of the child nodes */
    SDFFfunctionassignNode(SDFFparser* parser, std::string fname, SDFFnode* child): SDFFunaryNode(parser, child) {
      function_name=fname;
      parameters.clear();
    };

    /** \brief get \c function_name property */
    GetMacro(std::string, function_name);
    /** \brief set \c function_name property */
    SetMacro(std::string, function_name);

    /** \brief add the name of a new parameter */
    inline void add_parameter(std::string name) {
      parameters.push_back(name);
    };

    /** \brief remove all parameters, previously added */
    inline void clear_parameters() {
      parameters.clear();
    };

    /** \brief esecute the function assignment and throw a run-time exception if an error occurs
     *
     * This simply creates a new function in the function manager object of parser and then assigns the
     * given child node as the defining node.
     */
    virtual sdff_result evaluate();

    /** \brief print the node (and all sub-nodes) into a string */
    virtual std::string print(int indent=0);

    /** \brief return a copy of the current node and all subnodes */
    virtual SDFFnode* copy(SDFFnode* newparent=NULL) {
      SDFFfunctionassignNode* n=new SDFFfunctionassignNode(function_name, NULL, newparent);
      copy_prop(n, newparent);
      copy_child(n);
      n->parameters=parameters;
      n->function_name=function_name;
      return n;
    };
};




/** \brief virtual base class for loops that include a test expression
 * \ingroup hlseqgen_sdff_nodes
 */
class SDFFtestloopNode: public SDFFnaryNode {
  protected:
    SDFFnode* test;
  public:
    /** \brief class constructor, also sets the parent and parser property of the child nodes */
    SDFFtestloopNode(SDFFnode* parent, SDFFnode* test): SDFFnaryNode(parent) {
      this->test=test;
      if (test!=NULL) test->set_parent(this);
    };

    /** \brief class constructor for a root node (no parent), also sets the parent and parser property of the child nodes */
    SDFFtestloopNode(SDFFparser* parser, SDFFnode* test): SDFFnaryNode(parser) {
      this->test=test;
      if (test!=NULL) test->set_parent(this);
    };

    /** \brief set the test node */
    inline void set_test(SDFFnode* value) {
      this->test=value;
      if (value!=NULL) value->set_parent(this);
    };
    /** \brief get the test node */
    GetMacro(SDFFnode*, test);

};




/** \brief represents a <tt>while(test) do {...}</tt> loop with a test expression and a list of nodes inside the loop
 * \ingroup hlseqgen_sdff_nodes
 */
class SDFFwhileNode: public SDFFtestloopNode {
  public:
    /** \brief class constructor, also sets the parent and parser property of the child nodes */
    SDFFwhileNode(SDFFnode* parent, SDFFnode* test): SDFFtestloopNode(parent, test) { };

    /** \brief class constructor for a root node (no parent), also sets the parent and parser property of the child nodes */
    SDFFwhileNode(SDFFparser* parser, SDFFnode* test): SDFFtestloopNode(parser, test) { };

    /** \brief evaluate the while loop, i.e. evaluate the sub-nodes as long as the node \c test is true. The return value is always \c void.
     *
     *  \verbatim
variable_manager.eneter_block();
t=test.evaluate();
while (t) {
  subnode1.evaluate();
  subnode2.evaluate();
  ...
  subnoden.evaluate();
  t=test.evaluate();
}
variable_manager.leave_block();
\endverbatim
     */
    virtual sdff_result evaluate();

    /** \brief print the node (and all sub-nodes) into a string */
    virtual std::string print(int indent=0);

    /** \brief return a copy of the current node and all subnodes */
    virtual SDFFnode* copy(SDFFnode* newparent=NULL) {
      SDFFwhileNode* n=new SDFFwhileNode(newparent, NULL);
      copy_prop(n, newparent);
      copy_children(n);
      n->test=test->copy(n);
      return n;
    };
};




/** \brief represents a <tt>do {...} while(test)</tt> loop with a test expression and a list of nodes inside the loop
 * \ingroup hlseqgen_sdff_nodes
 */
class SDFFdowhileNode: public SDFFtestloopNode {
  public:
    /** \brief class constructor, also sets the parent and parser property of the child nodes */
    SDFFdowhileNode(SDFFnode* parent, SDFFnode* test): SDFFtestloopNode(parent, test) { };

    /** \brief class constructor for a root node (no parent), also sets the parent and parser property of the child nodes */
    SDFFdowhileNode(SDFFparser* parser, SDFFnode* test): SDFFtestloopNode(parser, test) { };

    /** \brief evaluate the while loop, i.e. evaluate the sub-nodes as long as the node \c test is true. The return value is always \c void.
     *
     *  \verbatim
variable_manager.eneter_block();
do {
  subnode1.evaluate();
  subnode2.evaluate();
  ...
  subnoden.evaluate();
  t=test.evaluate();
} while (t);
variable_manager.leave_block();
\endverbatim
     */
    virtual sdff_result evaluate();

    /** \brief print the node (and all sub-nodes) into a string */
    virtual std::string print(int indent=0);

    /** \brief return a copy of the current node and all subnodes */
    virtual SDFFnode* copy(SDFFnode* newparent=NULL) {
      SDFFdowhileNode* n=new SDFFdowhileNode(newparent, NULL);
      copy_prop(n, newparent);
      copy_children(n);
      n->test=test->copy(n);
      return n;
    };
};




/** \brief represents a <tt>repeat {...} until(test)</tt> loop with a test expression and a list of nodes inside the loop
 * \ingroup hlseqgen_sdff_nodes
 */
class SDFFrepeatuntilNode: public SDFFtestloopNode {
  public:
    /** \brief class constructor, also sets the parent and parser property of the child nodes */
    SDFFrepeatuntilNode(SDFFnode* parent, SDFFnode* test): SDFFtestloopNode(parent, test) { };

    /** \brief class constructor for a root node (no parent), also sets the parent and parser property of the child nodes */
    SDFFrepeatuntilNode(SDFFparser* parser, SDFFnode* test): SDFFtestloopNode(parser, test) { };

    /** \brief evaluate the while loop, i.e. evaluate the sub-nodes as long as the node \c test is true. The return value is always \c void.
     *
     *  \verbatim
variable_manager.eneter_block();
repeat {
  subnode1.evaluate();
  subnode2.evaluate();
  ...
  subnoden.evaluate();
  t=test.evaluate();
} until (t);
variable_manager.leave_block();
\endverbatim
     */
    virtual sdff_result evaluate();

    /** \brief print the node (and all sub-nodes) into a string */
    virtual std::string print(int indent=0);

    /** \brief return a copy of the current node and all subnodes */
    virtual SDFFnode* copy(SDFFnode* newparent=NULL) {
      SDFFrepeatuntilNode* n=new SDFFrepeatuntilNode(newparent, NULL);
      copy_prop(n, newparent);
      copy_children(n);
      n->test=test->copy(n);
      return n;
    };
};





/**
 * \brief represenst an <tt>if(test) {...} else {...}</tt>-expression
 * \ingroup hlseqgen_sdff_nodes
 */
class SDFFifNode: public SDFFnode {
  private:
  protected:
    /** \brief hidden standard constructor */
    SDFFifNode() {};
    /** \brief holds the nodes of the true part of this node */
    std::vector<SDFFnode*> truechildren;
    /** \brief holds the nodes of the false part of this node */
    std::vector<SDFFnode*> falsechildren;
    /** \brief holds the test expression */
    SDFFnode* test;
  public:
    /** \brief class constructor, also sets the parent and parser property of the child nodes */
    SDFFifNode(SDFFnode* parent, SDFFnode* test): SDFFnode(parent) {
      this->test=test;
      if (test!=NULL) {
        test->set_parent(this);
        test->set_parser(parser);
      }
    };

    /** \brief class constructor for a root node (no parent), also sets the parent and parser property of the child nodes */
    SDFFifNode(SDFFparser* parser, SDFFnode* test): SDFFnode(parser) {
      this->test=test;
      if (test!=NULL) {
        test->set_parent(this);
        test->set_parser(parser);
      }
    };

    /** \brief set the test node */
    SetMacro(SDFFnode*, test);
    /** \brief get the test node */
    GetMacro(SDFFnode*, test);

    /** \brief set the parent node. This takes the parser from the parent and sets it here and
     *         also for the child node.
     */
    virtual void set_parent(SDFFnode* parent);

    /** \brief add a child node to the true part, also sets the parent and parser property of the child node */
    virtual void add_true_child(SDFFnode* child);

    /** \brief add a child node to the false part, also sets the parent and parser property of the child node */
    virtual void add_false_child(SDFFnode* child);

    /** \brief add all the children from \a children node to the true part, also sets the parent and parser
     *         property of the child node
     */
    virtual void add_true_children(SDFFnaryNode* children);

    /** \brief add all the children from \a children node to the false part, also sets the parent and parser
     *         property of the child node
     */
    virtual void add_false_children(SDFFnaryNode* children);

    /** \brief return the number of true children */
    inline virtual size_t get_true_child_count() {
      return truechildren.size();
    };

    /** \brief return the number of false children */
    inline virtual size_t get_false_child_count() {
      return falsechildren.size();
    };

    /** \brief return a pointer to the n-th true node */
    inline virtual SDFFnode* get_true_child(size_t n) {
      if ((n>=0)&&(n<truechildren.size())) return truechildren[n];
      return NULL;
    }

    /** \brief return a pointer to the n-th false node */
    inline virtual SDFFnode* get_false_child(size_t n) {
      if ((n>=0)&&(n<falsechildren.size())) return falsechildren[n];
      return NULL;
    }

    /** \brief set the parser, also for the child nodes */
    virtual void set_parser(SDFFparser* parser);


    /** \brief destroys all child nodes of this node and then clears the list of children.
     */
    virtual void dispose();

    /** clears the list of children without destroying them
     */
    inline virtual void deconstruct();

    /** \brief set the n-th child to the given node and returns a pointer to the old node.
     *         If \a destroy_old\c =true then the old node will be destroyed and NULL will be returned
     */
    virtual SDFFnode* replace_true_child(size_t n, SDFFnode* child, bool destroy_old=false);

    /** \brief set the n-th child to the given node and returns a pointer to the old node.
     *         If \a destroy_old\c =true then the old node will be destroyed and NULL will be returned
     */
    virtual SDFFnode* replace_false_child(size_t n, SDFFnode* child, bool destroy_old=false);

    /** \brief delete the n-th child to the given node and returns a pointer to the old node.
     *         If \a destroy_old\c =true then the old node will be destroyed and NULL will be returned
     */
    virtual SDFFnode* delete_true_child(size_t n, bool destroy_old=false);

    /** \brief delete the n-th child to the given node and returns a pointer to the old node.
     *         If \a destroy_old\c =true then the old node will be destroyed and NULL will be returned
     */
    virtual SDFFnode* delete_false_child(size_t n, bool destroy_old=false);
    /** \brief evaluate the if node. if \c test is true then execute truechildren otherwise execute falsechildren
     *
     *  \verbatim
t=test.evaluate()
if (t) {
  variable_manager.eneter_block();
  execute true children
  variable_manager.leave_block();
} else {
  variable_manager.eneter_block();
  execute false children
  variable_manager.leave_block();
}
\endverbatim
     */
    virtual sdff_result evaluate();

    /** \brief print the node (and all sub-nodes) into a string */
    virtual std::string print(int indent=0);

    /** \brief return a copy of the current node and all subnodes */
    virtual SDFFnode* copy(SDFFnode* newparent=NULL) {
      SDFFifNode* n=new SDFFifNode(newparent, NULL);
      copy_prop(n, newparent);
      n->truechildren.clear();
      n->falsechildren.clear();
      if (truechildren.size()>0) {
        for (size_t i=0; i<truechildren.size(); i++) {
          n->truechildren.push_back(truechildren[i]->copy(n));
        }
      }
      if (falsechildren.size()>0) {
        for (size_t i=0; i<falsechildren.size(); i++) {
          n->falsechildren.push_back(falsechildren[i]->copy(n));
        }
      }
      n->test=test->copy(n);
      return n;
    };
};




/** \brief represents an <tt>event</tt>- or <tt>relative_event</tt>-block with all ist sub-nodes and its time parameter
 * \ingroup hlseqgen_sdff_nodes
 */
class SDFFeventNode: public SDFFnaryNode {
  protected:
    /** \brief represents the time parameter of the <tt>event</tt>-block */
    SDFFnode* time;
    /** \brief specifies whether this is a <tt>relative_event</tt>-block */
    bool relative;
  public:
    /** \brief class constructor, also sets the parent and parser property of the child nodes */
    SDFFeventNode(SDFFnode* parent, SDFFnode* time, bool relative=false): SDFFnaryNode(parent) {
      this->time=time;
      this->relative=relative;
    };

    /** \brief class constructor for a root node (no parent), also sets the parent and parser property of the child nodes */
    SDFFeventNode(SDFFparser* parser, SDFFnode* time, bool relative=false): SDFFnaryNode(parser) {
      this->time=time;
      this->relative=relative;
    };

    /** \brief set the time node */
    SetMacro(SDFFnode*, time);
    /** \brief get the time node */
    GetMacro(SDFFnode*, time);
    /** \brief set the relative property */
    SetMacro(bool, relative);
    /** \brief get the relative property */
    GetMacro(bool, relative);

    /** \brief evaluate the list of nodes in this class
     *
     * this goes through all nodes and evaluates them. The last result is returned by this method (Pseudocode):
     * \verbatim
if (relative) {
  t=parser.get_current_time()+time.evaluate();
} else {
  t=time.evaluate();
}
parser.set_current_time(t);
variable_manager.enter_block();
r=result_void;
forall i in childdren
  r=children[i].evaluate();
endfor
variable_manager.leave_block();
return r;
\endverbatim
     */
    virtual sdff_result evaluate();

    /** \brief print the node (and all sub-nodes) into a string */
    virtual std::string print(int indent=0);

    /** \brief return a copy of the current node and all subnodes */
    virtual SDFFnode* copy(SDFFnode* newparent=NULL) {
      SDFFeventNode* n=new SDFFeventNode(newparent, NULL);
      copy_prop(n, newparent);
      copy_children(n);
      n->time=time->copy(n);
      n->relative=relative;
      return n;
    };
};



/** \brief represents an <tt>pre_event</tt>-block with all ist sub-nodes and its time parameter
 * \ingroup hlseqgen_sdff_nodes
 */
class SDFFpreEventNode: public SDFFnaryNode {
  protected:
  public:
    /** \brief class constructor, also sets the parent and parser property of the child nodes */
    SDFFpreEventNode(SDFFnode* parent): SDFFnaryNode(parent) {
    };

    /** \brief class constructor for a root node (no parent), also sets the parent and parser property of the child nodes */
    SDFFpreEventNode(SDFFparser* parser): SDFFnaryNode(parser) {
    };

    /** \brief evaluate the list of nodes in this class
     *
     * this goes through all nodes and evaluates them. The last result is returned by this method (Pseudocode):
     * \verbatim
if (relative) {
  t=parser.get_current_time()+time.evaluate();
} else {
  t=time.evaluate();
}
parser.set_current_time(t);
variable_manager.enter_block();
r=result_void;
forall i in childdren
  r=children[i].evaluate();
endfor
variable_manager.leave_block();
return r;
\endverbatim
     */
    virtual sdff_result evaluate();

    /** \brief print the node (and all sub-nodes) into a string */
    virtual std::string print(int indent=0);

    /** \brief return a copy of the current node and all subnodes */
    virtual SDFFnode* copy(SDFFnode* newparent=NULL) {
      SDFFpreEventNode* n=new SDFFpreEventNode(newparent);
      copy_prop(n, newparent);
      copy_children(n);
      return n;
    };
};


/** \brief represents an <tt>last_event</tt>-block with all ist sub-nodes and its time parameter
 * \ingroup hlseqgen_sdff_nodes
 */
class SDFFlastEventNode: public SDFFnaryNode {
  protected:
  public:
    /** \brief class constructor, also sets the parent and parser property of the child nodes */
    SDFFlastEventNode(SDFFnode* parent): SDFFnaryNode(parent) {
    };

    /** \brief class constructor for a root node (no parent), also sets the parent and parser property of the child nodes */
    SDFFlastEventNode(SDFFparser* parser): SDFFnaryNode(parser) {
    };

    /** \brief evaluate the list of nodes in this class
     *
     * this goes through all nodes and evaluates them. The last result is returned by this method (Pseudocode):
     * \verbatim
if (relative) {
  t=parser.get_current_time()+time.evaluate();
} else {
  t=time.evaluate();
}
parser.set_current_time(t);
variable_manager.enter_block();
r=result_void;
forall i in childdren
  r=children[i].evaluate();
endfor
variable_manager.leave_block();
return r;
\endverbatim
     */
    virtual sdff_result evaluate();

    /** \brief print the node (and all sub-nodes) into a string */
    virtual std::string print(int indent=0);

    /** \brief return a copy of the current node and all subnodes */
    virtual SDFFnode* copy(SDFFnode* newparent=NULL) {
      SDFFlastEventNode* n=new SDFFlastEventNode(newparent);
      copy_prop(n, newparent);
      copy_children(n);
      return n;
    };
};


/** \brief represents an <tt>channel</tt>--block with all ist sub-nodes and its parameter
 * \ingroup hlseqgen_sdff_nodes
 */
class SDFFchannelNode: public SDFFnaryNode {
  protected:
    /** \brief represents the time parameter of the <tt>event</tt>-block */
    SDFFnode* channel;
  public:
    /** \brief class constructor, also sets the parent and parser property of the child nodes */
    SDFFchannelNode(SDFFnode* parent, SDFFnode* chanel): SDFFnaryNode(parent) {
      this->channel=channel;
    };

    /** \brief class constructor for a root node (no parent), also sets the parent and parser property of the child nodes */
    SDFFchannelNode(SDFFparser* parser, SDFFnode* channel): SDFFnaryNode(parser) {
      this->channel=channel;
    };

    /** \brief set the channel node */
    SetMacro(SDFFnode*, channel);
    /** \brief get the channel node */
    GetMacro(SDFFnode*, channel);

    /** \brief evaluate the list of nodes in this class
     *
     * this goes through all nodes and evaluates them. The last result is returned by this method (Pseudocode):
     * \verbatim
current_channel=channel.evaluate();
parser.set_current_time(t);
variable_manager.enter_block();
r=result_void;
forall i in childdren
  r=children[i].evaluate();
endfor
variable_manager.leave_block();
return r;
\endverbatim
     */
    virtual sdff_result evaluate();

    /** \brief print the node (and all sub-nodes) into a string */
    virtual std::string print(int indent=0);

    /** \brief return a copy of the current node and all subnodes */
    virtual SDFFnode* copy(SDFFnode* newparent=NULL) {
      SDFFchannelNode* n=new SDFFchannelNode(newparent, NULL);
      copy_prop(n, newparent);
      copy_children(n);
      n->channel=channel->copy(n);
      return n;
    };
};




}
#endif // SDFFNODES_H


/*@}*/
