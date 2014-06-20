#ifndef ELEPHANTS_RBTREE_H_
#define ELEPHANTS_RBTREE_H_
#define BLACK 1
#define RED 0

#ifdef TEST_ALGORITHM
#include <iostream>
#endif


namespace Elephants
{
    template<class TK, class TV>
    class rbpair
    {
    public:
        TK  key;
        TV  value;

        rbpair(TK&  k, TV& val)
        {
            key = k;
            value = val;
        }
        
        rbpair(const rbpair&  val)
        {
            key = val.key;
            value = val.value;
        }
    }

    template<class TK, class TV>
    class Node 
    {
    public:
        TK  key;
        TV  value;
        bool color;
        Node* leftTree;
        Node* rightTree;
        Node* parent;

        Node() 
        {
            color = RED;
            leftTree = NULL;
            rightTree = NULL;
            parent = NULL;
        }

        Node* grandparent() 
        {
            if (parent == NULL) 
            {
                return NULL;
            }
            return parent->parent;
        }

        Node* uncle() 
        {
            if (grandparent() == NULL) 
            {
                return NULL;
            }

            if (parent == grandparent()->rightTree)
            {
                return grandparent()->leftTree;
            }
            else
            {
                return grandparent()->rightTree;
            }
        }

        Node* sibling() 
        {
            if (parent->leftTree == this)
            {
                return parent->rightTree;
            }
            else
            {
                return parent->leftTree;
            }
        }
    };

    template<class TK,  class TV>
    class  rbtree
    {
    private:
        Node *root;
        const Node* NIL;

    public:

        rbtree() 
        {
            Node*  tmp = new (std::nothrow) Node();
            tmp->color = BLACK;

            NIL = tmp;
            root = NULL;
        }

        ~rbtree() 
        {
            if (root)
            {
                DeleteTree(root);
            }
            delete NIL;
        }

        void inorder() 
        {
            if (root == NULL)
            {
                return;
            }
            inorder(root);
        }

        void insert(TK& x, TV& y) 
        {
            if (root == NULL) 
            {
                root = new Node();
                root->color = BLACK;
                root->leftTree = root->rightTree = NIL;
                root->key = x;
                root->value = y;
            } 
            else 
            {
                insert(root, rbpair(x, y));
            }
        }

        bool delete_value(const TK& data) 
        {
            return delete_child(root, data);
        }


    private:
        void rotate_right(Node *p)
        {
            Node *gp = p->grandparent();
            Node *fa = p->parent;
            Node *y = p->rightTree;

            fa->leftTree = y;

            if (y != NIL)
            {
                y->parent = fa;
            }
            p->rightTree = fa;
            fa->parent = p;

            if (root == fa)
            {
                root = p;
            }
            p->parent = gp;

            if (gp != NULL) 
            {
                if (gp->leftTree == fa)
                {
                    gp->leftTree = p;
                }
                else
                {
                    gp->rightTree = p;
                }
            }

        }

        void rotate_left(Node *p) 
        {
            if (p->parent == NULL) 
            {
                root = p;
                return;
            }
            Node *gp = p->grandparent();
            Node *fa = p->parent;
            Node *y = p->leftTree;

            fa->rightTree = y;

            if (y != NIL)
            {
                y->parent = fa;
            }
            p->leftTree = fa;
            fa->parent = p;

            if (root == fa)
            {
                root = p;
            }
            p->parent = gp;

            if (gp != NULL) 
            {
                if (gp->leftTree == fa)
                {
                    gp->leftTree = p;
                }
                else
                {
                    gp->rightTree = p;
                }
            }
        }

        void inorder(Node *p) 
        {
            if (p == NIL)
            {
                return;
            }

            if (p->leftTree)
            {
                inorder(p->leftTree);
            }
#ifdef TEST_ALGORITHM
            std::cout << p->key << ":" << p->value << " ";
#endif

            if (p->rightTree)
            {
                inorder(p->rightTree);
            }
        }

        string outputColor(bool color) 
        {
            return color ? "BLACK" : "RED";
        }

        Node* getSmallestChild(Node *p) 
        {
            if (p->leftTree == NIL)
                return p;
            return getSmallestChild(p->leftTree);
        }

        bool delete_child(Node *p, const TK& data) {
            if (p->key > data) 
            {
                if (p->leftTree == NIL) 
                {
                    return false;
                }
                return delete_child(p->leftTree, data);
            } 
            else if (p->key < data) 
            {
                if (p->rightTree == NIL) 
                {
                    return false;
                }
                return delete_child(p->rightTree, data);
            } 
            else if (p->key == data) 
            {
                if (p->rightTree == NIL) 
                {
                    delete_one_child(p);
                    return true;
                }
                Node *smallest = getSmallestChild(p->rightTree);
                swap(p->value, smallest->value);
                delete_one_child(smallest);

                return true;
            }
        }

        void delete_one_child(Node *p) 
        {
            Node *child = p->leftTree == NIL ? p->rightTree : p->leftTree;
            if (p->parent == NULL && p->leftTree == NIL && p->rightTree == NIL) 
            {
                delete p;
                p = NULL;
                root = NULL;
                return;
            }

            if (p->parent == NULL) 
            {
                delete  p;
                child->parent = NULL;
                root = child;
                return;
            }

            if (p->parent->leftTree == p) 
            {
                p->parent->leftTree = child;
            } 
            else 
            {
                p->parent->rightTree = child;
            }
            child->parent = p->parent;

            if (p->color == BLACK) 
            {
                if (child->color == RED) 
                {
                    child->color = BLACK;
                } 
                else
                {
                    delete_case(child);
                }
            }

            delete p;
        }

        void delete_case(Node *p) 
        {
            if (p->parent == NULL) 
            {
                p->color = BLACK;
                return;
            }

            if (p->sibling()->color == RED) 
            {
                p->parent->color = RED;
                p->sibling()->color = BLACK;
                if (p == p->parent->leftTree)
                {
                    rotate_left(p->sibling());
                }
                else
                {
                    rotate_right(p->sibling());
                }
            }

            if (p->parent->color == BLACK && p->sibling()->color == BLACK
                && p->sibling()->leftTree->color == BLACK && p->sibling()->rightTree->color == BLACK) 
            {
                    p->sibling()->color = RED;
                    delete_case(p->parent);
            } 
            else if (p->parent->color == RED && p->sibling()->color == BLACK
                && p->sibling()->leftTree->color == BLACK && p->sibling()->rightTree->color == BLACK) 
            {
                    p->sibling()->color = RED;
                    p->parent->color = BLACK;
            } 
            else 
            {
                if (p->sibling()->color == BLACK) 
                {
                    if (p == p->parent->leftTree && p->sibling()->leftTree->color == RED
                        && p->sibling()->rightTree->color == BLACK) 
                    {
                            p->sibling()->color = RED;
                            p->sibling()->leftTree->color = BLACK;
                            rotate_right(p->sibling()->leftTree);
                    } 
                    else if (p == p->parent->rightTree && p->sibling()->leftTree->color == BLACK
                        && p->sibling()->rightTree->color == RED) 
                    {
                            p->sibling()->color = RED;
                            p->sibling()->rightTree->color = BLACK;
                            rotate_left(p->sibling()->rightTree);
                    }
                }
                p->sibling()->color = p->parent->color;
                p->parent->color = BLACK;
                if (p == p->parent->leftTree) 
                {
                    p->sibling()->rightTree->color = BLACK;
                    rotate_left(p->sibling());
                } 
                else 
                {
                    p->sibling()->leftTree->color = BLACK;
                    rotate_right(p->sibling());
                }
            }
        }

        void insert(Node *p, const rbpair& data) 
        {
            if (p->key > data.key) 
            {
                if (p->leftTree != NIL)
                {
                    insert(p->leftTree, data);
                }
                else 
                {
                    Node *tmp = new Node();
                    tmp->key = data.key;
                    tmp->value = data.value;
                    tmp->leftTree = tmp->rightTree = NIL;
                    tmp->parent = p;
                    p->leftTree = tmp;
                    insert_case(tmp);
                }
            } 
            else if (p->key < data.key)
            {
                if (p->rightTree != NIL)
                {
                    insert(p->rightTree, data);
                }
                else 
                {
                    Node *tmp = new Node();
                    tmp->key = data.key;
                    tmp->value = data.value;
                    tmp->leftTree = tmp->rightTree = NIL;
                    tmp->parent = p;
                    p->rightTree = tmp;
                    insert_case(tmp);
                }
            }
            else
            {
                p->value = data.value;   // if key is equal, and rbtree will set new value to this node
            }
        }

        void insert_case(Node *p) 
        {
            if (p->parent == NULL) 
            {
                root = p;
                p->color = BLACK;
                return;
            }

            if (p->parent->color == RED) 
            {
                if (p->uncle()->color == RED) 
                {
                    p->parent->color = p->uncle()->color = BLACK;
                    p->grandparent()->color = RED;
                    insert_case(p->grandparent());
                } 
                else 
                {
                    if (p->parent->rightTree == p && p->grandparent()->leftTree == p->parent) 
                    {
                        rotate_left(p);
                        rotate_right(p);
                        p->color = BLACK;
                        p->leftTree->color = p->rightTree->color = RED;
                    } 
                    else if (p->parent->leftTree == p && p->grandparent()->rightTree == p->parent) 
                    {
                        rotate_right(p);
                        rotate_left(p);
                        p->color = BLACK;
                        p->leftTree->color = p->rightTree->color = RED;
                    } 
                    else if (p->parent->leftTree == p && p->grandparent()->leftTree == p->parent) 
                    {
                        p->parent->color = BLACK;
                        p->grandparent()->color = RED;
                        rotate_right(p->parent);
                    } 
                    else if (p->parent->rightTree == p && p->grandparent()->rightTree == p->parent) 
                    {
                        p->parent->color = BLACK;
                        p->grandparent()->color = RED;
                        rotate_left(p->parent);
                    }
                }
            }
        }

        void DeleteTree(Node *p) 
        {
            if (!p || p == NIL) 
            {
                return;
            }
            DeleteTree(p->leftTree);
            DeleteTree(p->rightTree);
            delete p;
        }
    };
}



#endif



