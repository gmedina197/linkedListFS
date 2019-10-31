typedef struct node
{
    unsigned short int number;
    struct node *next;
} node_t;

typedef struct subdir
{
    char name[25];
    char directories[32][25]; 
    struct subdir *next;
} subdir;

void print_list(node_t *head)
{
    node_t *current = head;

    while (current != NULL)
    {
        printf("%d\n", current->number);
        current = current->next;
    }
}

void push(node_t **head, int val)
{
    node_t *new_node;
    new_node = malloc(sizeof(node_t));

    new_node->number = val;
    new_node->next = *head;
    *head = new_node;
}

int pop(node_t **head)
{
    int retval = -1;
    node_t *next_node = NULL;

    if (*head == NULL)
    {
        return -1;
    }

    next_node = (*head)->next;
    retval = (*head)->number;
    free(*head);
    *head = next_node;

    return retval;
}