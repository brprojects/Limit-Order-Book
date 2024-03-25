#include <iostream>
using namespace std;

// Definition of a node in the BST
struct Node {
    int data;
    Node* left;
    Node* right;
    Node(int val) : data(val), left(nullptr), right(nullptr) {}
};

// Insertion function for BST
Node* insert(Node* root, int val) {
    if (root == nullptr)
        return new Node(val);

    if (val < root->data)
        root->left = insert(root->left, val);
    else if (val > root->data)
        root->right = insert(root->right, val);

    return root;
}

// Searching function for BST
bool search(Node* root, int val) {
    if (root == nullptr)
        return false;

    if (root->data == val)
        return true;
    else if (val < root->data)
        return search(root->left, val);
    else
        return search(root->right, val);
}

// In-order traversal of the BST (prints the nodes in ascending order)
void inorderTraversal(Node* root) {
    if (root != nullptr) {
        inorderTraversal(root->left);
        cout << root->data << " ";
        inorderTraversal(root->right);
    }
}

int main() {
    Node* root = nullptr;
    root = insert(root, 5);
    root = insert(root, 3);
    root = insert(root, 7);
    root = insert(root, 1);
    root = insert(root, 4);

    cout << "In-order traversal of the BST: ";
    inorderTraversal(root);
    cout << endl;

    int searchVal = 4;
    if (search(root, searchVal))
        cout << searchVal << " found in the BST." << endl;
    else
        cout << searchVal << " not found in the BST." << endl;

    return 0;
}