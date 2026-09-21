#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

int remove_child(struct Node *parent, struct Node *child)
{
   if (parent == NULL || child == NULL)
   {
      return 0;
   }

   struct Node *previous = NULL;
   struct Node *current = parent->first_child;

   while (current != NULL && current != child)
   {
      previous = current;
      current = current->next_sibling;
   }

   if (current == NULL)
   {
      return 0;
   }

   if (previous == NULL)
   {
      parent->first_child = current->next_sibling;
   }
   else
   {
      previous->next_sibling = current->next_sibling;
   }

   child->next_sibling = NULL;
   return 1;
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

void serialize_html_to(FILE *output, const struct Node *node)
{
   if (node == NULL)
   {
      return;
   }

   if (node->type == NODE_TEXT)
   {
      fprintf(output, "%s", node->value);
   }
   else
   {
      fprintf(output, "<%s>", node->value);
      serialize_html_to(output, node->first_child);
      fprintf(output, "</%s>", node->value);
   }

   serialize_html_to(output, node->next_sibling);
}

void serialize_html(const struct Node *node)
{
   serialize_html_to(stdout, node);
}

int write_html_file(const char *filename, const struct Node *root)
{
   FILE *output = fopen(filename, "w");

   if (output == NULL)
   {
      return 0;
   }

   serialize_html_to(output, root);
   fputc('\n', output);
   fclose(output);

   return 1;
}

size_t count_nodes(const struct Node *node)
{
   if (node == NULL)
   {
      return 0;
   }

   return 1 + count_nodes(node->first_child) + count_nodes(node->next_sibling);
}

const struct Node *find_first_tag(const struct Node *node, const char *tag)
{
   if (node == NULL)
   {
      return NULL;
   }

   if (node->type == NODE_ELEMENT && strcmp(node->value, tag) == 0)
   {
      return node;
   }

   const struct Node *found = find_first_tag(node->first_child, tag);

   if (found != NULL)
   {
      return found;
   }

   return find_first_tag(node->next_sibling, tag);
}

struct QueueItem
{
   const struct Node *node;
   struct QueueItem *next;
};

void enqueue(struct QueueItem **front,
             struct QueueItem **back,
             const struct Node *node)
{
   struct QueueItem *item = malloc(sizeof *item);

   if (item == NULL)
   {
      return;
   }

   item->node = node;
   item->next = NULL;

   if (*back == NULL)
   {
      *front = item;
      *back = item;
      return;
   }

   (*back)->next = item;
   *back = item;
}

const struct Node *dequeue(struct QueueItem **front,
                           struct QueueItem **back)
{
   if (*front == NULL)
   {
      return NULL;
   }

   struct QueueItem *item = *front;
   const struct Node *node = item->node;

   *front = item->next;

   if (*front == NULL)
   {
      *back = NULL;
   }

   free(item);
   return node;
}

void print_bfs(const struct Node *root)
{
   struct QueueItem *front = NULL;
   struct QueueItem *back = NULL;

   enqueue(&front, &back, root);

   const struct Node *node;

   while ((node = dequeue(&front, &back)) != NULL)
   {
      printf("%s\n", node->value);

      for (const struct Node *child = node->first_child;
           child != NULL;
           child = child->next_sibling)
      {
         enqueue(&front, &back, child);
      }
   }
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

   printf("\nHTML:\n");
   serialize_html(parent);
   printf("\n");

   printf("\nNode count: %zu\n", count_nodes(parent));

   const struct Node *found = find_first_tag(parent, "h1");
   printf("Found h1: %s\n", found == NULL ? "no" : "yes");

   printf("\nBFS:\n");
   print_bfs(parent);

   if (write_html_file("output.html", parent))
   {
      printf("\nWrote output.html\n");
   }

   if (remove_child(parent, paragraph))
   {
      printf("Removed paragraph\n");
      destroy_tree(paragraph);
   }

   destroy_tree(parent);

   return 0;
}
