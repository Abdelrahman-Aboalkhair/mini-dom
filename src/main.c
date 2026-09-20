#include <stdio.h>
#include <stdlib.h>

/*

basically for our mini dom every thing will be a node as defined with a tag
and a pointer to the first child so that we have an actual tree connection

*/
enum NodeType
{
   NODE_ELEMENT,
   NODE_TEXT
};

struct Node
{
   enum NodeType type;
   const char *value;
   struct Node *first_child;
   struct Node *next_sibling;
};

struct Node *create_node(enum NodeType type, const char *value)
{
   struct Node *node = malloc(sizeof *node);

   if (node == NULL)
   {
      return NULL;
   }

   node->type = type;
   node->value = value;
   node->first_child = NULL;
   node->next_sibling = NULL;

   return node;
}

void append_child(struct Node *parent, struct Node *child)
{
   if (parent->first_child == NULL)
   {
      parent->first_child = child;
      return;
   }

   struct Node *current = parent->first_child;

   while (current->next_sibling != NULL)
   {
      current = current->next_sibling;
   }

   current->next_sibling = child;
}

void print_tree(const struct Node *node, int depth)
{
   if (node == NULL)
   {
      return;
   }

   for (int level = 0; level < depth; level++)
   {
      printf("  ");
   }

   if (node->type == NODE_TEXT)
   {
      printf("%s\n", node->value);
   }
   else
   {
      printf("<%s>\n", node->value);
   }

   print_tree(node->first_child, depth + 1);
   print_tree(node->next_sibling, depth);
}

void destroy_tree(struct Node *node)
{
   if (node == NULL)
   {
      return;
   }

   destroy_tree(node->first_child);
   destroy_tree(node->next_sibling);
   free(node);
}

int main(void)
{

   struct Node *parent = create_node(NODE_ELEMENT, "div");
   struct Node *heading = create_node(NODE_ELEMENT, "h1");
   struct Node *paragraph = create_node(NODE_ELEMENT, "p");
   struct Node *text = create_node(NODE_TEXT, "Hello from C");

   if (parent == NULL || heading == NULL || paragraph == NULL || text == NULL)
   {

      fprintf(stderr, "Memory allocation failed\n");

      destroy_tree(parent);
      destroy_tree(heading);
      destroy_tree(paragraph);
      destroy_tree(text);

      return 1;
   }

   append_child(parent, heading);
   append_child(parent, paragraph);
   append_child(heading, text);

   print_tree(parent, 0);

   destroy_tree(parent);

   return 0;
}
